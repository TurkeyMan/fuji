#include "Fuji.h"
#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFModel_Internal.h"
#include "MFAnimation_Internal.h"
#include "MFFileSystem.h"
#include "MFView.h"

#include "Display_Internal.h"
#include "MFRenderer.h"
#include "MFRenderer_PC.h"

extern IDirect3DDevice9 *pd3dDevice;

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

	MFMatrix *pAnimMats = NULL;

	MFMatrix wts;

	if(pModel->pAnimation)
	{
		pAnimMats = MFAnimation_CalculateMatrices(pModel->pAnimation, &pModel->worldMatrix);
		wts = MFView_GetWorldToScreenMatrix();
	}
	else
	{
		MFView_GetLocalToScreen(pModel->worldMatrix, &wts);
	}

	MFRenderer_SetMatrices(pAnimMats, pAnimMats ? pModel->pAnimation->numBones : 0);
	MFRendererPC_SetWorldToScreenMatrix(wts);

	MFMaterial *pMatOverride = (MFMaterial*)MFRenderer_GetRenderStateOverride(MFRS_MaterialOverride);

	if(pMatOverride)
		MFMaterial_SetMaterial(pMatOverride);

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pSubobjects[a].pMeshChunks;

				if(!pMatOverride)
					MFMaterial_SetMaterial(pMC[b].pMaterial);

				if(pModel->pAnimation)
				{
					MFRendererPC_SetNumWeights(pMC[b].maxWeights);
					MFRenderer_SetBatch(pMC[b].pBatchIndices, pMC[b].matrixBatchSize);
				}
				else
					MFRendererPC_SetNumWeights(0);

				MFRenderer_Begin();

				pd3dDevice->SetVertexDeclaration(pMC[b].pVertexDeclaration);
				pd3dDevice->SetStreamSource(0, pMC[b].pVertexBuffer, 0, pMC[b].vertexStride);
				if(pMC[b].pAnimBuffer)
					pd3dDevice->SetStreamSource(1, pMC[b].pAnimBuffer, 0, pMC[b].animVertexStride);
				pd3dDevice->SetIndices(pMC[b].pIndexBuffer);

				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMC[b].numVertices, 0, pMC[b].numIndices/3);
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
	hr = pd3dDevice->CreateVertexBuffer(pMC->vertexDataSize, 0, 0, D3DPOOL_MANAGED, &pMC->pVertexBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex buffer..");
	if(pMC->pAnimData)
	{
		hr = pd3dDevice->CreateVertexBuffer(pMC->animDataSize, 0, 0, D3DPOOL_MANAGED, &pMC->pAnimBuffer, NULL);
		MFDebug_Assert(SUCCEEDED(hr), "Failed to create animation buffer..");
	}
	hr = pd3dDevice->CreateIndexBuffer(pMC->indexDataSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pMC->pIndexBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create index buffer..");

	void *pData;

	// fill vertex buffer
	hr = pMC->pVertexBuffer->Lock(0, 0, &pData, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock vertex buffer..");

	MFCopyMemory(pData, pMC->pVertexData, pMC->vertexDataSize);

	pMC->pVertexBuffer->Unlock();

	// fill animation buffer
	if(pMC->pAnimData)
	{
		hr = pMC->pAnimBuffer->Lock(0, 0, &pData, 0);
		MFDebug_Assert(SUCCEEDED(hr), "Failed to lock animation buffer..");

		MFCopyMemory(pData, pMC->pAnimData, pMC->animDataSize);

		pMC->pAnimBuffer->Unlock();
	}

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

	if(pMC->pAnimBuffer)
	{
		pMC->pAnimBuffer->Release();
		pMC->pAnimBuffer = NULL;
	}

	pMC->pVertexDeclaration->Release();
	pMC->pVertexDeclaration = NULL;
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFCALLSTACK;

	MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pMeshChunk;

	MFFixUp(pMC->pMaterial, pBase, load);
	MFFixUp(pMC->pVertexData, pBase, load);
	MFFixUp(pMC->pAnimData, pBase, load);
	MFFixUp(pMC->pIndexData, pBase, load);
	MFFixUp(pMC->pVertexElements, pBase, load);
	MFFixUp(pMC->pBatchIndices, pBase, load);
}
