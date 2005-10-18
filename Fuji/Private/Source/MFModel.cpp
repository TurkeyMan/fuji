#include "Fuji.h"
#include "MFPtrList.h"
#include "System.h"
#include "MFModel_Internal.h"
#include "MFFileSystem.h"
#include "View.h"

#include "Display_Internal.h"
#include "Renderer.h"

MFPtrList<MFModelTemplate> gModelBank;

void MFModel_InitModule()
{
	gModelBank.Init("Model Bank", gDefaults.model.maxModels);
}

void MFModel_DeinitModule()
{
	gModelBank.Deinit();
}

DataChunk *MFModel_GetDataChunk(MFModelTemplate *pModelTemplate, DataChunkType chunkID)
{
	DataChunk *pChunk = NULL;

	for(int a=0; a<pModelTemplate->numDataChunks; a++)
	{
		if(pModelTemplate->pDataChunks[a].chunkType == chunkID)
			return &pModelTemplate->pDataChunks[a];
	}

	return pChunk;
}

MFModelTemplate* MFModel_FindTemplate(const char *pName)
{
	CALLSTACK;

	MFModelTemplate **ppIterator = gModelBank.Begin();

	while(*ppIterator)
	{
		if(!StrCaseCmp(pName, (*ppIterator)->pFilename)) return *ppIterator;

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
			case CT_SubObjects:
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

			case CT_Bones:
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

			case CT_Tags:
			{
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
		MFFileHandle hFile = MFFileSystem_Open(pFilename, MFOF_Read|MFOF_Binary);

		if(hFile)
		{
			int size = MFFile_GetSize(hFile);

			if(size > 0)
			{
				char *pTemplateData;

				// allocate memory and load file
				pTemplateData = (char*)Heap_Alloc(size + strlen(pFilename) + 1);
				MFFile_Read(hFile, pTemplateData, size);

				// check ID string
				DBGASSERT(*(uint32*)pTemplateData == 0x324c444d, "Incorrect MFModel version.");

				// store filename for later reference
				pTemplate = (MFModelTemplate*)pTemplateData;

				strcpy(&pTemplateData[size], pFilename);
				pTemplate->pFilename = &pTemplateData[size];

				gModelBank.Create(pTemplate);

				MFModel_FixUp(pTemplate, true);

				DataChunk *pChunk =	MFModel_GetDataChunk(pTemplate, CT_SubObjects);

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

	MFModel *pModel = (MFModel*)Heap_Alloc(sizeof(MFModel));

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
		DataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, CT_SubObjects);

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

		Heap_Free(pModel->pTemplate);
	}

	Heap_Free(pModel);
}


void MFModel_SetWorldMatrix(MFModel *pModel, const MFMatrix &worldMatrix)
{
	pModel->worldMatrix = worldMatrix;
}

void MFModel_SetColour(MFModel *pModel, const MFVector &colour)
{
	pModel->modelColour = colour;
}
