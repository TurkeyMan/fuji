#include "Fuji_Internal.h"
#include "MFAnimation_Internal.h"
#include "MFModel_Internal.h"
#include "MFFileSystem.h"
#include "MFSystem.h"
#include "MFHeap.h"
#include "MFResource.h"
#include "MFRenderState.h"
#include "Util.h"


// 128 temp matrices for intermediate calculation data
MFMatrix gWorkingMats[128];

static void MFAnimationTemplate_Destroy(MFResource *pRes)
{
	MFAnimationTemplate *pTemplate = (MFAnimationTemplate*)pRes;
	MFHeap_Free(pTemplate);
}

MFInitStatus MFAnimation_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFRT_AnimationTemplate = MFResource_Register("MFAnimationTemplate", &MFAnimationTemplate_Destroy);

	return MFIS_Succeeded;
}

void MFAnimation_DeinitModule()
{
}

MFAnimationTemplate* MFAnimation_FindTemplate(const char *pName)
{
	return (MFAnimationTemplate*)MFResource_Find(MFUtil_HashString(pName) ^ 0xA010A010);
}

void MFAnimation_FixUp(MFAnimationTemplate *pTemplate, bool load)
{
	if(load)
	{
		MFFixUp(pTemplate->pBones, pTemplate, 1);
		MFFixUp(pTemplate->pAnimName, pTemplate, 1);
	}

	for(uint32 a=0; a<pTemplate->numBones; a++)
	{
		MFFixUp(pTemplate->pBones[a].pBoneName, pTemplate, load);
		MFFixUp(pTemplate->pBones[a].pTime, pTemplate, load);
		MFFixUp(pTemplate->pBones[a].pFrames, pTemplate, load);
	}

	if(!load)
	{
		MFFixUp(pTemplate->pBones, pTemplate, 0);
		MFFixUp(pTemplate->pAnimName, pTemplate, 0);
	}
}

MF_API MFAnimation* MFAnimation_Create(const char *pFilename, MFModel *pModel)
{
	MFAnimationTemplate *pTemplate = MFAnimation_FindTemplate(pFilename);

	if(!pTemplate)
	{
		size_t size = 0;
		pTemplate = (MFAnimationTemplate*)MFModelInternal_PendingAnimationTemplate(&size);

		if(!pTemplate)
		{
			MFFile *hFile = MFFileSystem_Open(MFStr("%s.anm", pFilename), MFOF_Read|MFOF_Binary);

			if(hFile)
			{
				size = (size_t)MFFile_GetSize(hFile);

				if(size > 0)
				{
					char *pTemplateData;

					// allocate memory and load file
					pTemplateData = (char*)MFHeap_Alloc(size + MFString_Length(pFilename) + 1);
					MFFile_Read(hFile, pTemplateData, size);

					pTemplate = (MFAnimationTemplate*)pTemplateData;

					// check ID string
					MFDebug_Assert(pTemplate->hash == MFMAKEFOURCC('A', 'N', 'M', '2'), "Incorrect MFAnimation version.");
				}

				MFFile_Close(hFile);
			}
		}

		if(!pTemplate)
			return NULL;

		pFilename = MFString_Copy((char*)pTemplate + size, pFilename);

		MFAnimation_FixUp(pTemplate, true);
		MFResource_AddResource(pTemplate, MFRT_AnimationTemplate, MFUtil_HashString(pFilename) ^ 0xA010A010, pFilename);
	}

	// get the model bone chunk
	MFModelDataChunk *pBoneChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Bones);
	MFDebug_Assert(pBoneChunk, "Cant apply animation to a model with no skeleton!");

	// create and init instance
	MFAnimation *pAnimation;
	size_t bytes = MFALIGN16(sizeof(MFAnimation)) + sizeof(MFMatrix)*pBoneChunk->count + sizeof(int)*pBoneChunk->count + sizeof(MFAnimationCurrentFrame)*pBoneChunk->count;
	pAnimation = (MFAnimation*)MFHeap_Alloc(bytes);
	pAnimation->pModel = pModel;
	pAnimation->pTemplate = pTemplate;

	// add animation to model animation list
	pModel->pAnimation = pAnimation;

	// get bones pointer from model (for convenience)
	pAnimation->pBones = (MFModelBone*)pBoneChunk->pData;
	pAnimation->numBones = pBoneChunk->count;

	// set matrices to identity
	pAnimation->pMatrices = (MFMatrix*)MFALIGN16(&pAnimation[1]);
	for(int a=0; a<pBoneChunk->count; a++)
	{
		pAnimation->pMatrices[a] = MFMatrix::identity;
	}

	MFStateConstant_AnimationMatrices animMats;
	animMats.pMatrices = pAnimation->pMatrices;
	animMats.numMatrices = pAnimation->numBones;
	MFStateBlock_SetAnimMatrices(pModel->pEntityState, animMats);

	// build bone to animation stream mapping
	pAnimation->pBoneMap = (int*)MFALIGN16(&pAnimation->pMatrices[pBoneChunk->count]);
	for(int a=0; a<pBoneChunk->count; a++)
	{
		const char *pBoneName = MFModel_GetBoneName(pModel, a);

		// find bone in animation
		pAnimation->pBoneMap[a] = -1;
		for(uint32 b=0; b<pTemplate->numBones; b++)
		{
			if(!MFString_CaseCmp(pBoneName, pTemplate->pBones[b].pBoneName))
			{
				pAnimation->pBoneMap[a] = b;
				break;
			}
		}
	}

	pAnimation->pCustomMatrices = NULL;
	pAnimation->blendLayer.frameTime = pAnimation->pTemplate->startTime;

	pAnimation->blendLayer.pCurFrames = (MFAnimationCurrentFrame*)&pAnimation->pBoneMap[pBoneChunk->count];
	MFZeroMemory(pAnimation->blendLayer.pCurFrames, sizeof(MFAnimationCurrentFrame)*pBoneChunk->count);

	return pAnimation;
}

MF_API int MFAnimation_Release(MFAnimation *pAnimation)
{
	// release the template
	int rc = MFResource_Release(pAnimation->pTemplate);

	// free the instance
	MFHeap_Free(pAnimation);

	return rc;
}

MF_API MFMatrix *MFAnimation_CalculateMatrices(MFAnimation *pAnimation, MFMatrix *pLocalToWorld)
{
	MFAnimationBone *pAnims = pAnimation->pTemplate->pBones;
	MFModelBone *pBones = pAnimation->pBones;
	MFMatrix *pMats = pAnimation->pMatrices;

	float t = pAnimation->blendLayer.frameTime;
	MFDebug_Assert(t >= pAnimation->pTemplate->startTime && t <= pAnimation->pTemplate->endTime, "Frame time outside animation range...");

	// find the frame number for each bone
	for(uint32 a=0; a<pAnimation->numBones; a++)
	{
		int map = pAnimation->pBoneMap[a];

		if(map != -1)
		{
			float *pTimes = pAnims[map].pTime;
			int lastFrames = pAnims[map].numFrames-1;

			if(t == pTimes[lastFrames])
			{
				pAnimation->blendLayer.pCurFrames[a].tweenStart = lastFrames;
				pAnimation->blendLayer.pCurFrames[a].tweenEnd = lastFrames;
				pAnimation->blendLayer.pCurFrames[a].tween = 0;
			}
			else
			{
				// TODO: change this to a binary search...
				for(int b=0; b<lastFrames; b++)
				{
					float t1 = pTimes[b];
					float t2 = pTimes[b+1];

					if(t >= pTimes[b] && t < pTimes[b+1])
					{
						pAnimation->blendLayer.pCurFrames[a].tweenStart = b;
						pAnimation->blendLayer.pCurFrames[a].tweenEnd = b+1;
						pAnimation->blendLayer.pCurFrames[a].tween = (t-t1) / (t2-t1);
						break;
					}
				}
			}
		}
	}

	// calculate the matrix for each bone
	for(uint32 a=0; a<pAnimation->numBones; a++)
	{
		int map = pAnimation->pBoneMap[a];

		if(map != -1)
		{
			MFMatrix &m1 = pAnims[map].pFrames[pAnimation->blendLayer.pCurFrames[a].tweenStart].key;
			MFMatrix &m2 = pAnims[map].pFrames[pAnimation->blendLayer.pCurFrames[a].tweenEnd].key;

			gWorkingMats[a].Tween(m1, m2, pAnimation->blendLayer.pCurFrames[a].tween);
		}
		else
		{
			gWorkingMats[a] = pBones[a].boneMatrix;
		}
	}

	// build the animation matrix for each bone...
	// TODO: this could be much faster
	for(uint32 a=0; a<pAnimation->numBones; a++)
	{
		MFMatrix boneMat = MFMatrix::identity;

		int b = (int)a;
		do
		{
			boneMat.Multiply(gWorkingMats[b]);
			b = pBones[b].parent;
		}
		while(b != -1);

//		pMats[a].Multiply(boneMat, pBones[a].invWorldMatrix);
		pMats[a].Multiply(pBones[a].invWorldMatrix, boneMat);

		if(pLocalToWorld)
			pMats[a].Multiply(*pLocalToWorld);
	}

	return pAnimation->pMatrices;
}

MF_API void MFAnimation_GetFrameRange(const MFAnimation *pAnimation, float *pStartTime, float *pEndTime)
{
	if(pStartTime)
		*pStartTime = pAnimation->pTemplate->startTime;
	if(pEndTime)
		*pEndTime = pAnimation->pTemplate->endTime;
}

MF_API void MFAnimation_SetFrame(MFAnimation *pAnimation, float frameTime)
{
	pAnimation->blendLayer.frameTime = frameTime;
}
