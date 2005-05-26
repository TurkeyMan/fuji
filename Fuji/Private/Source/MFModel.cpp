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
			Material_SetMaterial(pSubobjects[a].pMaterial);

			Renderer_SetRenderer(0, 0, RS_MeshChunk);

			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				pd3dDevice->SetVertexDeclaration(pSubobjects[a].pMeshChunks[b].pVertexDeclaration);
				pd3dDevice->SetStreamSource(0, pSubobjects[a].pMeshChunks[b].pVertexBuffer, 0, pSubobjects[a].pMeshChunks[b].vertexStride);
				pd3dDevice->SetIndices(pSubobjects[a].pMeshChunks[b].pIndexBuffer);
				pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX0);
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pSubobjects[a].pMeshChunks[b].numVertices, 0, pSubobjects[a].pMeshChunks[b].numVertices/3);
			}
		}
	}
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
						pSubobjectChunk[b].pMaterial = (Material*)((char*)pSubobjectChunk[b].pMaterial + base);
					}

					for(c=0; c<pSubobjectChunk[b].numMeshChunks; c++)
					{
						if(load)
						{
							pSubobjectChunk[b].pMeshChunks[c].pVertexData += base;
							pSubobjectChunk[b].pMeshChunks[c].pIndexData += base;
							(char*&)pSubobjectChunk[b].pMeshChunks[c].pVertexElements += base;
						}
						else
						{
							pSubobjectChunk[b].pMeshChunks[c].pVertexData -= base;
							pSubobjectChunk[b].pMeshChunks[c].pIndexData -= base;
							(char*&)pSubobjectChunk[b].pMeshChunks[c].pVertexElements -= base;
						}
					}

					if(!load)
					{
						pSubobjectChunk[b].pSubObjectName -= base;
						pSubobjectChunk[b].pMaterial = (Material*)((char*)pSubobjectChunk[b].pMaterial - base);

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
						pSubobjects[a].pMaterial = Material_Create((char*)pSubobjects[a].pMaterial);

						for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
						{
							pd3dDevice->CreateVertexDeclaration(pSubobjects[a].pMeshChunks[b].pVertexElements, &pSubobjects[a].pMeshChunks[b].pVertexDeclaration);
							pd3dDevice->CreateVertexBuffer(pSubobjects[a].pMeshChunks[b].vertexDataSize, 0, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX0, D3DPOOL_MANAGED, &pSubobjects[a].pMeshChunks[b].pVertexBuffer, NULL);
							pd3dDevice->CreateIndexBuffer(pSubobjects[a].pMeshChunks[b].indexDataSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pSubobjects[a].pMeshChunks[b].pIndexBuffer, NULL);

							void *pData;
							pSubobjects[a].pMeshChunks[b].pVertexBuffer->Lock(0, 0, &pData, 0);
							memcpy(pData, pSubobjects[a].pMeshChunks[b].pVertexData, pSubobjects[a].pMeshChunks[b].vertexDataSize);
							pSubobjects[a].pMeshChunks[b].pVertexBuffer->Unlock();

							pSubobjects[a].pMeshChunks[b].pIndexBuffer->Lock(0, 0, &pData, 0);
							memcpy(pData, pSubobjects[a].pMeshChunks[b].pIndexData, pSubobjects[a].pMeshChunks[b].indexDataSize);
							pSubobjects[a].pMeshChunks[b].pIndexBuffer->Unlock();
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
