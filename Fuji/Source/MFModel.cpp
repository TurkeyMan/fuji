#include "Fuji.h"
#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFModel_Internal.h"
#include "MFFileSystem.h"
#include "MFView.h"
#include "MFCollision_Internal.h"
#include "MFAnimation.h"
#include "MFMesh_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFRenderer.h"
#include "MFRenderState.h"
#include "MFVertex.h"
#include "Asset/MFIntModel.h"

#define ALLOW_LOAD_FROM_SOURCE_DATA

MFInitStatus MFModel_InitModule()
{
	MFModel_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFModel_DeinitModule()
{
	bool bShowHeader = true;

	// list all non-freed textures...
	MFResourceIterator *pI = MFResource_EnumerateFirst(MFRT_Model);
	while(pI)
	{
		if(bShowHeader)
		{
			bShowHeader = false;
			MFDebug_Message("\nUn-freed models:\n----------------------------------------------------------");
		}

		MFModelTemplate *pModel = (MFModelTemplate*)MFResource_Get(pI);
		MFDebug_Message(MFStr("'%s' - x%d", pModel->pName, pModel->refCount));

		// Destroy template...

		MFResource_EnumerateNext(pI, MFRT_Model);
	}

	MFModel_DeinitModulePlatformSpecific();
}

MFModelDataChunk *MFModel_GetDataChunk(MFModelTemplate *pModelTemplate, MFModelDataChunkType chunkID)
{
	MFModelDataChunk *pChunk = NULL;

	for(int a=0; a<pModelTemplate->numDataChunks; a++)
	{
		if(pModelTemplate->pDataChunks[a].chunkType == chunkID)
			return &pModelTemplate->pDataChunks[a];
	}

	return pChunk;
}

void MFModel_FixUp(MFModelTemplate *pTemplate, bool load)
{
	int a, b, c;

	if(load)
	{
		MFFixUp(pTemplate->pDataChunks, pTemplate, 1);
		MFFixUp(pTemplate->pName, pTemplate, 1);
	}

	for(a=0; a<pTemplate->numDataChunks; a++)
	{
		if(load)
			MFFixUp(pTemplate->pDataChunks[a].pData, pTemplate, 1);

		switch(pTemplate->pDataChunks[a].chunkType)
		{
			case MFChunkType_SubObjects:
			{
				MFModelSubObject *pSubobjects = (MFModelSubObject*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					if(load)
					{
						MFFixUp(pSubobjects[b].pMeshChunks, pTemplate, 1);

						MFFixUp(pSubobjects[b].pSubObjectName, pTemplate, 1);
//						MFFixUp(pSubobjects[b].pMaterial, pTemplate, 1);
					}

					for(c=0; c<pSubobjects[b].numMeshChunks; c++)
					{
						MFModel_FixUpMeshChunk(MFModel_GetMeshChunkInternal(pTemplate, b, c), pTemplate, load);
					}

					if(!load)
					{
						MFFixUp(pSubobjects[b].pSubObjectName, pTemplate, 0);
//						MFFixUp(pSubobjects[b].pMaterial, pTemplate, 0);

						MFFixUp(pSubobjects[b].pMeshChunks, pTemplate, 0);
					}
				}
				break;
			}

			case MFChunkType_Bones:
			{
				MFModelBone *pBones = (MFModelBone*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					MFFixUp(pBones[b].pBoneName, pTemplate, load);
					MFFixUp(pBones[b].pParentName, pTemplate, load);
					MFFixUp(pBones[b].pChildren, pTemplate, load);
				}
				break;
			}

			case MFChunkType_Collision:
			{
				MFCollisionTemplate *pCollisionChunk = (MFCollisionTemplate*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					MFFixUp(pCollisionChunk[b].pName, pTemplate, load);

					if(load)
						MFFixUp(pCollisionChunk[b].pCollisionTemplateData, pTemplate, 1);

					if(pCollisionChunk[b].type == MFCT_Mesh)
					{
						MFCollisionMesh *pMesh = (MFCollisionMesh*)pCollisionChunk[b].pCollisionTemplateData;
						MFFixUp(pMesh->pTriangles, pTemplate, load);
					}

					if(!load)
						MFFixUp(pCollisionChunk[b].pCollisionTemplateData, pTemplate, 0);
				}
				break;
			}

			case MFChunkType_Tags:
			{
				MFModelTag *pTags = (MFModelTag*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					MFFixUp(pTags[b].pTagName, pTemplate, load);
				}
				break;
			}

			default:
				break;
		}

		if(!load)
			MFFixUp(pTemplate->pDataChunks[a].pData, pTemplate, 0);
	}

	if(!load)
	{
		MFFixUp(pTemplate->pDataChunks, pTemplate, 0);
		MFFixUp(pTemplate->pName, pTemplate, 0);
	}
}

MFMeshChunk* MFModel_GetMeshChunkInternal(MFModelTemplate *pModelTemplate, int subobjectIndex, int meshChunkIndex)
{
	MFModelDataChunk *pChunk = MFModel_GetDataChunk(pModelTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFDebug_Assert(subobjectIndex < pChunk->count, "Subobject index out of bounds.");
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		MFDebug_Assert(meshChunkIndex < pSubobjects->numMeshChunks, "Mesh chunk index out of bounds.");
		MFMeshChunk_Current *pMC = (MFMeshChunk_Current*)pSubobjects[subobjectIndex].pMeshChunks;
		return &pMC[meshChunkIndex];
	}

	return NULL;
}

MF_API MFModel* MFModel_Create(const char *pFilename)
{
	const char* pOriginalFilename = pFilename;

	// see if it's already loaded
	MFModelTemplate *pTemplate = (MFModelTemplate*)MFResource_FindResource(MFUtil_HashString(pOriginalFilename) ^ 0x0DE10DE1);

	if(!pTemplate)
	{
		char *pTemplateData = NULL;

		MFFile *hFile = MFFileSystem_Open(MFStr("%s.mdl", pFilename), MFOF_Read|MFOF_Binary);
		if(hFile)
		{
			int64 size = MFFile_GetSize(hFile);

			if(size > 0)
			{
				// allocate memory and load file
				pTemplateData = (char*)MFHeap_Alloc((size_t)size + MFString_Length(pFilename) + 1);
				MFFile_Read(hFile, pTemplateData, (size_t)size);
				MFFile_Close(hFile);

				MFString_Copy(&pTemplateData[size], pFilename);
				pFilename = &pTemplateData[size];
			}
		}
		else
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
			// try to load from source data
			const char * const pExt[] = { ".f3d", ".dae", ".x", ".ase", ".obj", ".md2", ".md3", ".me2", NULL };
			const char * const *ppExt = pExt;
			MFIntModel *pIM = NULL;
			while(!pIM && *ppExt)
			{
				MFString tempFilename = MFString::Format("%s%s", pFilename, *ppExt);
				pIM = MFIntModel_CreateFromFile(tempFilename.CStr());
				if(pIM)
				{
					pFilename = MFString_Copy((char*)MFHeap_Alloc(tempFilename.NumBytes()+1), tempFilename.CStr());
					break;
				}
				++ppExt;
			}

			if(pIM)
			{
				MFIntModel_Optimise(pIM);

				size_t size;
				MFIntModel_CreateRuntimeData(pIM, (void**)&pTemplateData, &size, MFSystem_GetCurrentPlatform());

				MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.mdl", pOriginalFilename), MFOF_Write | MFOF_Binary);
				if(pFile)
				{
					MFFile_Write(pFile, pTemplateData, size, false);
					MFFile_Close(pFile);
				}

				MFIntModel_Destroy(pIM);
			}
#endif
		}

		if(!pTemplateData)
			return NULL;

		pTemplate = (MFModelTemplate*)pTemplateData;

		// check ID string
		MFDebug_Assert(pTemplate->hash == MFMAKEFOURCC('M', 'D', 'L', '2'), "Incorrect MFModel version.");

		pTemplate->hash = MFUtil_HashString(pOriginalFilename) ^ 0x0DE10DE1;

		MFResource_AddResource(pTemplate);

		// store filename for later reference
		pTemplate->pFilename = pFilename;

		MFModel_FixUp(pTemplate, true);

		MFModelDataChunk *pChunk = MFModel_GetDataChunk(pTemplate, MFChunkType_SubObjects);

		if(pChunk)
		{
			MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

			for(int a=0; a<pChunk->count; a++)
			{
//				pSubobjects[a].pMaterial = MFMaterial_Create((char*)pSubobjects[a].pMaterial);

				for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
				{
					MFMeshChunk *pMC = MFModel_GetMeshChunkInternal(pTemplate, a, b);
					pMC->pGeomState = MFStateBlock_Create(128);

					if(pMC->type == MFMCT_Generic)
					{
						MFMeshChunk_Generic *pMCG = (MFMeshChunk_Generic*)pMC;
						pMC->pDecl = MFVertex_CreateVertexDeclaration(pMCG->pElements, pMCG->elementCount);

						MFStateBlock_SetRenderState(pMC->pGeomState, MFSCRS_VertexDeclaration, pMC->pDecl);

						for(int s=0; s<pMCG->numVertexStreams; ++s)
						{
							MFVertexDeclaration *pVBDecl = MFVertex_GetStreamDeclaration(pMC->pDecl, s);
							if(pVBDecl)
							{
								const char *pVBName = MFStr("%s|%s[%d:%d]", pTemplate->pFilename, pSubobjects[a].pSubObjectName, b, s);
								pMC->pVertexBuffers[s] = MFVertex_CreateVertexBuffer(pVBDecl, pMC->numVertices, MFVBType_Static, pMCG->ppVertexStreams[s], pVBName);

								MFStateBlock_SetRenderState(pMC->pGeomState, MFSCRS_VertexBuffer(s), pMC->pVertexBuffers[s]);
							}
						}

						const char *pIBName = MFStr("%s|%s[%d]", pTemplate->pFilename, pSubobjects[a].pSubObjectName, b);
						pMC->pIndexBuffer = MFVertex_CreateIndexBuffer(pMC->numIndices, pMCG->pIndexData, pIBName);

						MFStateBlock_SetRenderState(pMC->pGeomState, MFSCRS_IndexBuffer, pMC->pIndexBuffer);
					}

					MFModel_CreateMeshChunk(pMC);
				}
			}
		}
	}

	MFModel *pModel;
	pModel = (MFModel*)MFHeap_Alloc(sizeof(MFModel));

	pModel->worldMatrix = MFMatrix::identity;
	pModel->modelColour = MFVector::one;
	pModel->pTemplate = pTemplate;
	pModel->pAnimation = NULL;

	++pTemplate->refCount;

	return pModel;
}

MF_API MFModel* MFModel_CreateWithAnimation(const char *pFilename, const char *pAnimationFilename)
{
	MFModel *pModel = MFModel_Create(pFilename);

	if(pAnimationFilename)
		MFAnimation_Create(pAnimationFilename, pModel);

	return pModel;
}

MF_API int MFModel_Destroy(MFModel *pModel)
{
	// free instance data
	if(pModel->pAnimation)
		MFAnimation_Destroy(pModel->pAnimation);

	// decrement and possibly free template
	--pModel->pTemplate->refCount;
	int refCount = pModel->pTemplate->refCount;

	if(!pModel->pTemplate->refCount)
	{
		MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

		if(pChunk)
		{
			MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

			for(int a=0; a<pChunk->count; a++)
			{
//				MFMaterial_Destroy(pSubobjects[a].pMaterial);

				for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
				{
					MFMeshChunk *pMC = MFModel_GetMeshChunkInternal(pModel->pTemplate, a, b);

					MFModel_DestroyMeshChunk(pMC);

					MFStateBlock_Destroy(pMC->pGeomState);

					if(pMC->pDecl)
						MFVertex_DestroyVertexDeclaration(pMC->pDecl);

					for(int s=0; s<8; ++s)
					{
						if(pMC->pVertexBuffers[s])
							MFVertex_DestroyVertexBuffer(pMC->pVertexBuffers[s]);
					}

					if(pMC->pIndexBuffer)
						MFVertex_DestroyIndexBuffer(pMC->pIndexBuffer);
				}
			}
		}

		// remove it from the registry
		MFResource_RemoveResource(pModel->pTemplate);

		MFHeap_Free(pModel->pTemplate);
	}

	//free instance
	MFHeap_Free(pModel);

	return refCount;
}


MF_API void MFModel_SubmitGeometry(MFModel *pModel, MFRenderLayerSet *pLayerSet, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, MFStateBlock *pView)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);
	if(!pChunk)
		return;

	MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;
	for(int s = 0; s < pChunk->count; s++)
	{
		MFModelSubObject &sub = pSubobjects[s];

		for(int m = 0; m < sub.numMeshChunks; ++m)
		{
			MFMeshChunk &mc = sub.pMeshChunks[m];

			// set world matrix and colour
//			if(sub.subobjectAnimMatrix != -1 && pModel->pAnimation)
//				//TODO!
//			else
				MFStateBlock_SetMatrix(mc.pGeomState, MFSCM_World, pModel->worldMatrix);
//			MFStateBlock_SetVector(mc.pGeomState, MFSCV_DiffuseColour, pModel->modelColour);

			if(mc.pIndexBuffer)
				MFRenderLayer_AddIndexedVertices(pLayerSet->pSolidLayer, mc.pGeomState, 0, mc.numIndices, MFPT_TriangleList, mc.pMaterial, pEntity, pMaterialOverride, pView);
			else
				MFRenderLayer_AddVertices(pLayerSet->pSolidLayer, mc.pGeomState, 0, mc.numVertices, MFPT_TriangleList, mc.pMaterial, pEntity, pMaterialOverride, pView);
		}
	}
}

MF_API void MFModel_SetWorldMatrix(MFModel *pModel, const MFMatrix &worldMatrix)
{
	pModel->worldMatrix = worldMatrix;
}

MF_API void MFModel_SetColour(MFModel *pModel, const MFVector &colour)
{
	pModel->modelColour = colour;
}

MF_API const char* MFModel_GetName(MFModel *pModel)
{
	return pModel->pTemplate->pName;
}

MF_API int MFModel_GetNumSubObjects(MFModel *pModel)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
		return pChunk->count;

	return 0;
}

MF_API int MFModel_GetSubObjectIndex(MFModel *pModel, const char *pSubobjectName)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		for(int a=0; a<pChunk->count; ++a)
		{
			if(!MFString_Compare(pSubobjects->pSubObjectName, pSubobjectName))
				return a;
		}
	}

	return -1;
}

MF_API const char* MFModel_GetSubObjectName(MFModel *pModel, int index)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFDebug_Assert(index < pChunk->count, "Subobject index out of bounds.");

		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;
		return pSubobjects[index].pSubObjectName;
	}

	return NULL;
}

MF_API void MFModel_EnableSubobject(MFModel *pModel, int index, bool enable)
{
	MFDebug_Assert(false, "Not written...");
}

MF_API bool MFModel_IsSubobjectEnabed(MFModel *pModel, int index)
{
	return true;
}

MF_API MFBoundingVolume* MFModel_GetBoundingVolume(MFModel *pModel)
{
	return &pModel->pTemplate->boundingVolume;
}

MF_API MFMeshChunk* MFModel_GetMeshChunk(MFModel *pModel, int subobjectIndex, int meshChunkIndex)
{
	return MFModel_GetMeshChunkInternal(pModel->pTemplate, subobjectIndex, meshChunkIndex);
}

MF_API MFAnimation *MFModel_GetAnimation(MFModel *pModel)
{
	return pModel->pAnimation;
}

MF_API int MFModel_GetNumBones(MFModel *pModel)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Bones);

	if(pChunk)
		return pChunk->count;

	return 0;
}

MF_API const char* MFModel_GetBoneName(MFModel *pModel, int boneIndex)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Bones);

	if(pChunk)
	{
		MFDebug_Assert(boneIndex < pChunk->count, "boneIndex is out of bounds.");

		MFModelBone *pBones = (MFModelBone*)pChunk->pData;
		return pBones[boneIndex].pBoneName;
	}

	return 0;
}

MF_API const MFMatrix& MFModel_GetBoneOrigin(MFModel *pModel, int boneIndex)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Bones);

	if(pChunk)
	{
		MFDebug_Assert(boneIndex < pChunk->count, "boneIndex is out of bounds.");

		MFModelBone *pBones = (MFModelBone*)pChunk->pData;
		return pBones[boneIndex].worldMatrix;
	}

	return MFMatrix::identity;
}

MF_API int MFModel_GetBoneIndex(MFModel *pModel, const char *pName)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Bones);

	if(pChunk)
	{
		MFModelBone *pBones = (MFModelBone*)pChunk->pData;

		for(int a=0; a<pChunk->count; ++a)
		{
			if(!MFString_Compare(pBones->pBoneName, pName))
				return a;
		}
	}

	return -1;
}

MF_API int MFModel_GetNumTags(MFModel *pModel)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
		return pChunk->count;

	return 0;
}

MF_API const char* MFModel_GetTagName(MFModel *pModel, int tagIndex)
{
	MFDebug_Assert(tagIndex >= 0, "Invalid Tag index");

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
	{
		MFModelTag *pTags = (MFModelTag*)pChunk->pData;

		return pTags[tagIndex].pTagName;
	}

	return NULL;
}

MF_API const MFMatrix& MFModel_GetTagMatrix(MFModel *pModel, int tagIndex)
{
	MFDebug_Assert(tagIndex >= 0, "Invalid Tag index");

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
	{
		MFModelTag *pTags = (MFModelTag*)pChunk->pData;

		return pTags[tagIndex].tagMatrix;
	}

	return MFMatrix::identity;
}

MF_API int MFModel_GetTagIndex(MFModel *pModel, const char *pName)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
	{
		MFModelTag *pTags = (MFModelTag*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			if(!MFString_CaseCmp(pTags[a].pTagName, pName))
				return a;
		}
	}

	return -1;
}
