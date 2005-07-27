#include "Common.h"
#include "PtrList.h"
#include "System.h"
#include "MFModel_Internal.h"
#include "MFFileSystem.h"
#include "View.h"

#include "Display_Internal.h"
#include "Renderer.h"

PtrList<MFModelTemplate> gModelBank;

void MFModel_InitModule()
{
	gModelBank.Init("Model Bank", gDefaults.model.maxModels);
}

void MFModel_DeinitModule()
{
	gModelBank.Deinit();
}

void MFModel_Draw(MFModel *pModel)
{
	CALLSTACK;

#if defined(_WINDOWS)
	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&pModel->worldMatrix);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&View_GetViewToScreenMatrix());

	if(View_IsOrtho())
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&Matrix::identity);
	else
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&View_GetWorldToViewMatrix());

	DataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, CT_SubObjects);

	if(pChunk)
	{
		SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			MFMaterial_SetMaterial(pSubobjects[a].pMaterial);
			Renderer_Begin();

			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)&pSubobjects[a].pMeshChunks[b];

				pd3dDevice->SetVertexDeclaration(pMC->pVertexDeclaration);
				pd3dDevice->SetStreamSource(0, pMC->pVertexBuffer, 0, pMC->vertexStride);
				pd3dDevice->SetIndices(pMC->pIndexBuffer);
				pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1);
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMC->numVertices, 0, pMC->numVertices/3);
			}
		}
	}
#endif
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
		if(!StrCaseCmp(pName, (char*)(*ppIterator)->IDtag)) return *ppIterator;

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
						pSubobjectChunk[b].pMaterial = (MFMaterial*)((char*)pSubobjectChunk[b].pMaterial + base);
					}

					for(c=0; c<pSubobjectChunk[b].numMeshChunks; c++)
					{
#if defined(_WINDOWS)
						MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)&pSubobjectChunk[b].pMeshChunks[c];

						if(load)
						{
							pMC->pVertexData += base;
							pMC->pIndexData += base;
							(char*&)pMC->pVertexElements += base;
						}
						else
						{
							pMC->pVertexData -= base;
							pMC->pIndexData -= base;
							(char*&)pMC->pVertexElements -= base;
						}
#endif
					}

					if(!load)
					{
						pSubobjectChunk[b].pSubObjectName -= base;
						pSubobjectChunk[b].pMaterial = (MFMaterial*)((char*)pSubobjectChunk[b].pMaterial - base);

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
				strcpy(&pTemplateData[size], pFilename);
				(char*&)(*pTemplateData) = &pTemplateData[size];

				pTemplate = (MFModelTemplate*)pTemplateData;

				gModelBank.Create(pTemplate);

				MFModel_FixUp(pTemplate, true);

				DataChunk *pChunk =	MFModel_GetDataChunk(pTemplate, CT_SubObjects);

				if(pChunk)
				{
					SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

					for(int a=0; a<pChunk->count; a++)
					{
						pSubobjects[a].pMaterial = MFMaterial_Create((char*)pSubobjects[a].pMaterial);

						for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
						{
#if defined(_WINDOWS)
							MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)&pSubobjects[a].pMeshChunks[b];

							pd3dDevice->CreateVertexDeclaration(pMC->pVertexElements, &pMC->pVertexDeclaration);
							pd3dDevice->CreateVertexBuffer(pMC->vertexDataSize, 0, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1, D3DPOOL_MANAGED, &pMC->pVertexBuffer, NULL);
							pd3dDevice->CreateIndexBuffer(pMC->indexDataSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pMC->pIndexBuffer, NULL);

							void *pData;
							pMC->pVertexBuffer->Lock(0, 0, &pData, 0);
							memcpy(pData, pMC->pVertexData, pMC->vertexDataSize);
							pMC->pVertexBuffer->Unlock();

							pMC->pIndexBuffer->Lock(0, 0, &pData, 0);
							memcpy(pData, pMC->pIndexData, pMC->indexDataSize);
							pMC->pIndexBuffer->Unlock();
#endif
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

	pModel->worldMatrix = Matrix::identity;
	pModel->modelColour = Vector4::one;
	pModel->pTemplate = pTemplate;

	++pTemplate->refCount;

	return pModel;
}

void MFModel_Destroy(MFModel *pModel)
{
	--pModel->pTemplate->refCount;

	if(!pModel->pTemplate->refCount)
	{
		// destroy d3d data...

		Heap_Free(pModel->pTemplate);
	}

	Heap_Free(pModel);
}


void MFModel_SetWorldMatrix(MFModel *pModel, const Matrix &worldMatrix)
{
	pModel->worldMatrix = worldMatrix;
}

void MFModel_SetColour(MFModel *pModel, const Vector4 &colour)
{
	pModel->modelColour = colour;
}
