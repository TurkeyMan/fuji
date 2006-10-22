#include "Fuji.h"
#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFAnimation_Internal.h"
#include "MFFileSystem.h"
#include "MFModel_Internal.h"

MFPtrList<MFAnimationTemplate> gAnimationBank;

void MFAnimation_InitModule()
{
	gAnimationBank.Init("Animation Bank", gDefaults.animation.maxAnimations);
}

void MFAnimation_DeinitModule()
{
	gAnimationBank.Deinit();
}

MFAnimationTemplate* MFAnimation_FindTemplate(const char *pName)
{
	MFCALLSTACK;

	MFAnimationTemplate **ppIterator = gAnimationBank.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pName, (*ppIterator)->pFilename)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

void MFAnimation_FixUp(MFAnimationTemplate *pTemplate, bool load)
{
	int a;

	if(load)
	{
		MFFixUp(pTemplate->pBones, pTemplate, 1);
		MFFixUp(pTemplate->pName, pTemplate, 1);
	}

	for(a=0; a<pTemplate->numBones; a++)
	{
		MFFixUp(pTemplate->pBones[a].pBoneName, pTemplate, load);
		MFFixUp(pTemplate->pBones[a].pTime, pTemplate, load);
		MFFixUp(pTemplate->pBones[a].pFrames, pTemplate, load);
	}

	if(!load)
	{
		MFFixUp(pTemplate->pBones, pTemplate, 0);
		MFFixUp(pTemplate->pName, pTemplate, 0);
	}
}

MFAnimation* MFAnimation_Create(const char *pFilename, MFModel *pModel)
{
	MFAnimationTemplate *pTemplate = MFAnimation_FindTemplate(pFilename);

	if(!pTemplate)
	{
		MFFileHandle hFile = MFFileSystem_Open(MFStr("%s.anm", pFilename), MFOF_Read|MFOF_Binary);

		if(hFile)
		{
			int size = MFFile_GetSize(hFile);

			if(size > 0)
			{
				char *pTemplateData;

				// allocate memory and load file
				pTemplateData = (char*)MFHeap_Alloc(size + MFString_Length(pFilename) + 1);
				MFFile_Read(hFile, pTemplateData, size);

				// check ID string
				MFDebug_Assert(*(uint32*)pTemplateData == MFMAKEFOURCC('A', 'N', 'M', '2'), "Incorrect MFAnimation version.");

				// store filename for later reference
				pTemplate = (MFAnimationTemplate*)pTemplateData;

				MFString_Copy(&pTemplateData[size], pFilename);
				pTemplate->pFilename = &pTemplateData[size];

				gAnimationBank.Create(pTemplate);

				MFAnimation_FixUp(pTemplate, true);
			}

			MFFile_Close(hFile);
		}
	}

	if(!pTemplate)
		return NULL;

	++pTemplate->refCount;

	// get the model bone chunk
	MFModelDataChunk *pBoneChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Bones);
	MFDebug_Assert(pBoneChunk, "Cant apply animation to a model with no skeleton!");

	// create and init instance
	MFAnimation *pAnimation;
	pAnimation = (MFAnimation*)MFHeap_Alloc(MFALIGN16(sizeof(MFAnimation)) + MFALIGN16(sizeof(MFMatrix) * pBoneChunk->count) + sizeof(int)*pBoneChunk->count);
	pAnimation->pModel = pModel;
	pAnimation->pTemplate = pTemplate;

	// add animation to model animation list
	pModel->pAnimation = pAnimation;

	pAnimation->pMatrices = (MFMatrix*)MFALIGN16(&pAnimation[1]);
	for(int a=0; a<pBoneChunk->count; a++)
	{
		pAnimation->pMatrices[a] = MFMatrix::identity;
	}

	pAnimation->pBoneMap = (int*)MFALIGN16(&pAnimation->pMatrices[pBoneChunk->count]);
	for(int a=0; a<pBoneChunk->count; a++)
	{
		const char *pBoneName = MFModel_GetBoneName(pModel, a);

		// find bone in animation
		for(int b=0; b<pTemplate->numBones; b++)
		{
			if(!MFString_CaseCmp(pBoneName, pTemplate->pBones[b].pBoneName))
			{
				pAnimation->pBoneMap[a] = b;
				break;
			}
		}

		if(a == pTemplate->numBones)
			pAnimation->pBoneMap[a] = -1;
	}

	pAnimation->pCustomMatrices = NULL;
	pAnimation->curFrame = 0.0f;
	pAnimation->targetFrame = 0.0f;
	pAnimation->tween = 0.0f;

	return pAnimation;
}

void MFAnimation_Destroy(MFAnimation *pAnimation)
{
	--pAnimation->pTemplate->refCount;

	if(!pAnimation->pTemplate->refCount)
	{
		gAnimationBank.Destroy(pAnimation->pTemplate);
		MFHeap_Free(pAnimation->pTemplate);
	}

	MFHeap_Free(pAnimation);
}

int gFrame = 0;

MFMatrix *MFAnimation_CalculateMatrices(MFAnimation *pAnimation)
{
	MFMatrix *pMats = pAnimation->pMatrices;
	MFModel *pModel = pAnimation->pModel;
	MFAnimationBone *pAnims = pAnimation->pTemplate->pBones;

	// get the model bone chunk
	MFModelDataChunk *pBoneChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Bones);
	BoneChunk *pBones = (BoneChunk*)pBoneChunk->pData;

	int numBones = pBoneChunk->count;

	for(int a=0; a<numBones; a++)
	{
		MFMatrix boneMat = MFMatrix::identity;

		int b = a;
		do
		{
			int map = pAnimation->pBoneMap[b];

			MFMatrix *pMat;

			if(map != -1)
				pMat = &pAnims[map].pFrames[gFrame].key;
			else
				pMat = &pBones[b].boneMatrix;

			boneMat.Multiply(*pMat);

			b = pBones[b].parent;
		}
		while(b != -1);

		pMats[a].Multiply(pBones[a].invWorldMatrix, boneMat);
	}

	return pAnimation->pMatrices;
}

int MFAnimation_GetNumFrames(MFAnimation *pAnimation)
{
	return pAnimation->pTemplate->pBones[0].numFrames;
}

void MFAnimation_SetFrame(MFAnimation *, int frame)
{
	gFrame = frame;
}
