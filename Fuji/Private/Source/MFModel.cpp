#include "Fuji.h"
#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFModel_Internal.h"
#include "MFFileSystem.h"
#include "MFView.h"
#include "MFCollision_Internal.h"

#include "Display_Internal.h"
#include "MFRenderer.h"

MFPtrList<MFModelTemplate> gModelBank;

void MFModel_InitModule()
{
	gModelBank.Init("Model Bank", gDefaults.model.maxModels);
}

void MFModel_DeinitModule()
{
	gModelBank.Deinit();
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

MFModelTemplate* MFModel_FindTemplate(const char *pName)
{
	MFCALLSTACK;

	MFModelTemplate **ppIterator = gModelBank.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pName, (*ppIterator)->pFilename)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

void MFModel_FixUp(MFModelTemplate *pTemplate, bool load)
{
	int a, b, c;
	uint32 base = (uint32)pTemplate;

	if(load)
	{
		(char*&)pTemplate->pDataChunks += base;
		pTemplate->pName += base;
	}

	for(a=0; a<pTemplate->numDataChunks; a++)
	{
		if(load)
			(char*&)pTemplate->pDataChunks[a].pData += base;

		switch(pTemplate->pDataChunks[a].chunkType)
		{
			case MFChunkType_SubObjects:
			{
				SubObjectChunk *pSubobjectChunk = (SubObjectChunk*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					if(load)
					{
						(char*&)pSubobjectChunk[b].pMeshChunks += base;

						pSubobjectChunk[b].pSubObjectName += base;
//						pSubobjectChunk[b].pMaterial = (MFMaterial*)((char*)pSubobjectChunk[b].pMaterial + base);
					}

					for(c=0; c<pSubobjectChunk[b].numMeshChunks; c++)
					{
						MFModel_FixUpMeshChunk(&pSubobjectChunk[b].pMeshChunks[c], base, load);
					}

					if(!load)
					{
						pSubobjectChunk[b].pSubObjectName -= base;
//						pSubobjectChunk[b].pMaterial = (MFMaterial*)((char*)pSubobjectChunk[b].pMaterial - base);

						(char*&)pSubobjectChunk[b].pMeshChunks -= base;
					}
				}
				break;
			}

			case MFChunkType_Bones:
			{
				BoneChunk *pBoneChunk = (BoneChunk*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					if(load)
					{
						pBoneChunk[b].pBoneName += base;
						pBoneChunk[b].pParentName += base;
					}
					else
					{
						pBoneChunk[b].pBoneName -= base;
						pBoneChunk[b].pParentName -= base;
					}
				}
				break;
			}

			case MFChunkType_Collision:
			{
				MFCollisionTemplate *pCollisionChunk = (MFCollisionTemplate*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					if(load)
					{
						pCollisionChunk[b].pName += base;
						(char*&)pCollisionChunk[b].pCollisionTemplateData += base;

						if(pCollisionChunk[b].type == MFCT_Mesh)
						{
							MFCollisionMesh *pMesh = (MFCollisionMesh*)pCollisionChunk[b].pCollisionTemplateData;
							(char*&)pMesh->pTriangles += base;
						}
					}
					else
					{
						if(pCollisionChunk[b].type == MFCT_Mesh)
						{
							MFCollisionMesh *pMesh = (MFCollisionMesh*)pCollisionChunk[b].pCollisionTemplateData;
							(char*&)pMesh->pTriangles -= base;
						}

						(char*&)pCollisionChunk[b].pCollisionTemplateData -= base;
						pCollisionChunk[b].pName -= base;
					}
				}
				break;
			}

			case MFChunkType_Tags:
			{
				TagChunk *pTags = (TagChunk*)pTemplate->pDataChunks[a].pData;

				for(b=0; b<pTemplate->pDataChunks[a].count; b++)
				{
					if(load)
					{
						pTags[b].pTagName += base;
					}
					else
					{
						pTags[b].pTagName -= base;
					}
				}
				break;
			}

			default:
				break;
		}

		if(!load)
			(char*&)pTemplate->pDataChunks[a].pData -= base;
	}

	if(!load)
	{
		(char*&)pTemplate->pDataChunks -= base;
		pTemplate->pName -= base;
	}
}

MFModel* MFModel_Create(const char *pFilename)
{
	MFModelTemplate *pTemplate = MFModel_FindTemplate(pFilename);

	if(!pTemplate)
	{
		MFFileHandle hFile = MFFileSystem_Open(MFStr("%s.mdl", pFilename), MFOF_Read|MFOF_Binary);

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
				MFDebug_Assert(*(uint32*)pTemplateData == MFMAKEFOURCC('M', 'D', 'L', '2'), "Incorrect MFModel version.");

				// store filename for later reference
				pTemplate = (MFModelTemplate*)pTemplateData;

				MFString_Copy(&pTemplateData[size], pFilename);
				pTemplate->pFilename = &pTemplateData[size];

				gModelBank.Create(pTemplate);

				MFModel_FixUp(pTemplate, true);

				MFModelDataChunk *pChunk = MFModel_GetDataChunk(pTemplate, MFChunkType_SubObjects);

				if(pChunk)
				{
					SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

					for(int a=0; a<pChunk->count; a++)
					{
//						pSubobjects[a].pMaterial = MFMaterial_Create((char*)pSubobjects[a].pMaterial);

						for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
						{
							MFModel_CreateMeshChunk(&pSubobjects[a].pMeshChunks[b]);
						}
					}
				}
			}

			MFFile_Close(hFile);
		}
	}

	if(!pTemplate)
		return NULL;

	MFModel *pModel;
	pModel = (MFModel*)MFHeap_Alloc(sizeof(MFModel));

	pModel->worldMatrix = MFMatrix::identity;
	pModel->modelColour = MFVector::one;
	pModel->pTemplate = pTemplate;

	++pTemplate->refCount;

	return pModel;
}

void MFModel_Destroy(MFModel *pModel)
{
	--pModel->pTemplate->refCount;

	if(!pModel->pTemplate->refCount)
	{
		MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

		if(pChunk)
		{
			SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

			for(int a=0; a<pChunk->count; a++)
			{
//				MFMaterial_Destroy(pSubobjects[a].pMaterial);

				for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
				{
					MFModel_DestroyMeshChunk(&pSubobjects[a].pMeshChunks[b]);
				}
			}
		}

		gModelBank.Destroy(pModel->pTemplate);
		MFHeap_Free(pModel->pTemplate);
	}

	MFHeap_Free(pModel);
}


void MFModel_SetWorldMatrix(MFModel *pModel, const MFMatrix &worldMatrix)
{
	pModel->worldMatrix = worldMatrix;
}

void MFModel_SetColour(MFModel *pModel, const MFVector &colour)
{
	pModel->modelColour = colour;
}

MFBoundingVolume* MFModel_GetBoundingVolume(MFModel *pModel)
{
	return &pModel->pTemplate->boundingVolume;
}


int MFModel_GetNumTags(MFModel *pModel)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
		return pChunk->count;

	return 0;
}

const char* MFModel_GetTagName(MFModel *pModel, int tagIndex)
{
	MFDebug_Assert(tagIndex >= 0, "Invalid Tag index");

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
	{
		TagChunk *pTags = (TagChunk*)pChunk->pData;

		return pTags[tagIndex].pTagName;
	}

	return NULL;
}

const MFMatrix& MFModel_GetTagMatrix(MFModel *pModel, int tagIndex)
{
	MFDebug_Assert(tagIndex >= 0, "Invalid Tag index");

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
	{
		TagChunk *pTags = (TagChunk*)pChunk->pData;

		return pTags[tagIndex].tagMatrix;
	}

	return MFMatrix::identity;
}

int MFModel_GetTagIndex(MFModel *pModel, const char *pName)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Tags);

	if(pChunk)
	{
		TagChunk *pTags = (TagChunk*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			if(!MFString_CaseCmp(pTags[a].pTagName, pName))
				return a;
		}
	}

	return -1;
}
