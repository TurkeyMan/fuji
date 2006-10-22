#include "Fuji.h"
#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFModel_Internal.h"
#include "MFFileSystem.h"
#include "MFView.h"

#include "Display_Internal.h"
#include "MFRenderer.h"

extern IDirect3DDevice9 *pd3dDevice;

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&pModel->worldMatrix);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&MFView_GetViewToScreenMatrix());

	if(MFView_IsOrtho())
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&MFMatrix::identity);
	else
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&MFView_GetWorldToViewMatrix());

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pSubobjects[a].pMeshChunks;

				MFMaterial_SetMaterial(pMC[b].pMaterial);
				MFRenderer_Begin();

				pd3dDevice->SetVertexDeclaration(pMC[b].pVertexDeclaration);
				pd3dDevice->SetStreamSource(0, pMC[b].pVertexBuffer, 0, pMC[b].vertexStride);
				pd3dDevice->SetIndices(pMC[b].pIndexBuffer);
				pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1);
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMC[b].numVertices, 0, pMC[b].numIndices);
			}
		}
	}
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);

	HRESULT hr;

	// create D3D interfaces
	hr = pd3dDevice->CreateVertexDeclaration(pMC->pVertexElements, &pMC->pVertexDeclaration);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex declaration..");
	hr = pd3dDevice->CreateVertexBuffer(pMC->vertexDataSize, 0, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1, D3DPOOL_MANAGED, &pMC->pVertexBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex buffer..");
	hr = pd3dDevice->CreateIndexBuffer(pMC->indexDataSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pMC->pIndexBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create index buffer..");

	void *pData;

	// fill vertex buffer
	hr = pMC->pVertexBuffer->Lock(0, 0, &pData, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock vettex buffer..");

	MFCopyMemory(pData, pMC->pVertexData, pMC->vertexDataSize);

	pMC->pVertexBuffer->Unlock();

	// fill index buffer
	hr = pMC->pIndexBuffer->Lock(0, 0, &pData, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock index buffer..");

	MFCopyMemory(pData, pMC->pIndexData, pMC->indexDataSize);

	pMC->pIndexBuffer->Unlock();
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

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
	MFCALLSTACK;

	MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pMeshChunk;

	MFFixUp(pMC->pMaterial, (void*)base, load);
	MFFixUp(pMC->pVertexData, (void*)base, load);
	MFFixUp(pMC->pIndexData, (void*)base, load);
	MFFixUp(pMC->pVertexElements, (void*)base, load);
}

MFMeshChunk* MFModel_GetMeshChunkInternal(MFModelTemplate *pModelTemplate, int subobjectIndex, int meshChunkIndex)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModelTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFDebug_Assert(subobjectIndex < pChunk->count, "Subobject index out of bounds.");
		SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

		MFDebug_Assert(meshChunkIndex < pSubobjects->numMeshChunks, "Mesh chunk index out of bounds.");
		MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pSubobjects[subobjectIndex].pMeshChunks;
		return &pMC[meshChunkIndex];
	}

	return NULL;
}
