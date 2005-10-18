#include "Fuji.h"
#include "MFPtrList.h"
#include "System.h"
#include "MFModel_Internal.h"
#include "MFFileSystem.h"
#include "View.h"

#include "Display_Internal.h"
#include "Renderer.h"

void MFModel_Draw(MFModel *pModel)
{
	CALLSTACK;

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&pModel->worldMatrix);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&View_GetViewToScreenMatrix());

	if(View_IsOrtho())
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&MFMatrix::identity);
	else
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&View_GetWorldToViewMatrix());

	DataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, CT_SubObjects);

	if(pChunk)
	{
		SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)&pSubobjects[a].pMeshChunks[b];

				MFMaterial_SetMaterial(pMC->pMaterial);
				Renderer_Begin();

				pd3dDevice->SetVertexDeclaration(pMC->pVertexDeclaration);
				pd3dDevice->SetStreamSource(0, pMC->pVertexBuffer, 0, pMC->vertexStride);
				pd3dDevice->SetIndices(pMC->pIndexBuffer);
				pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1);
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMC->numVertices, 0, pMC->numVertices/3);
			}
		}
	}
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);

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
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);

	pMC->pIndexBuffer->Release();
	pMC->pIndexBuffer = NULL;

	pMC->pVertexBuffer->Release();
	pMC->pVertexBuffer = NULL;

	pMC->pVertexDeclaration->Release();
	pMC->pVertexDeclaration = NULL;
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, uint32 base, bool load)
{
	MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pMeshChunk;

	if(load)
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial + base);
		pMC->pVertexData += base;
		pMC->pIndexData += base;
		(char*&)pMC->pVertexElements += base;
	}
	else
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial - base);
		pMC->pVertexData -= base;
		pMC->pIndexData -= base;
		(char*&)pMC->pVertexElements -= base;
	}
}
