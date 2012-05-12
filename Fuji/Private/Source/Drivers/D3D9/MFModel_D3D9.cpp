#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFModel_InitModulePlatformSpecific MFModel_InitModulePlatformSpecific_D3D9
	#define MFModel_DeinitModulePlatformSpecific MFModel_DeinitModulePlatformSpecific_D3D9
	#define MFModel_Draw MFModel_Draw_D3D9
	#define MFModel_CreateMeshChunk MFModel_CreateMeshChunk_D3D9
	#define MFModel_DestroyMeshChunk MFModel_DestroyMeshChunk_D3D9
	#define MFModel_FixUpMeshChunk MFModel_FixUpMeshChunk_D3D9
#endif


#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFModel_Internal.h"
#include "MFAnimation_Internal.h"
#include "MFFileSystem.h"
#include "MFView.h"

#include "MFMesh_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFRenderer.h"
#include "MFRenderer_D3D9.h"

extern IDirect3DDevice9 *pd3dDevice;

struct MeshChunkD3DRuntimeData
{
	IDirect3DVertexBuffer9 *vertexBuffer;
	IDirect3DVertexBuffer9 *animBuffer;
	IDirect3DIndexBuffer9 *indexBuffer;
	IDirect3DVertexDeclaration9 *vertexDecl;
};

static BYTE gUsageTable[MFVET_Max] =
{
	D3DDECLUSAGE_POSITION,
	D3DDECLUSAGE_NORMAL,
	D3DDECLUSAGE_COLOR,
	D3DDECLUSAGE_TEXCOORD,
	D3DDECLUSAGE_BINORMAL,
	D3DDECLUSAGE_TANGENT,
	D3DDECLUSAGE_BLENDINDICES,
	D3DDECLUSAGE_BLENDWEIGHT
};

static BYTE gTypeTable[MFMVDT_Max] =
{
	D3DDECLTYPE_FLOAT1,
	D3DDECLTYPE_FLOAT2,
	D3DDECLTYPE_FLOAT3,
	D3DDECLTYPE_FLOAT4,
	D3DDECLTYPE_D3DCOLOR,
	D3DDECLTYPE_UBYTE4,
	D3DDECLTYPE_UBYTE4N,
	D3DDECLTYPE_SHORT2,
	D3DDECLTYPE_SHORT4,
	D3DDECLTYPE_SHORT2N,
	D3DDECLTYPE_SHORT4N,
	D3DDECLTYPE_USHORT2N,
	D3DDECLTYPE_USHORT4N,
	D3DDECLTYPE_UDEC3,
	D3DDECLTYPE_DEC3N,
	D3DDECLTYPE_FLOAT16_2,
	D3DDECLTYPE_FLOAT16_4
};

void MFModel_InitModulePlatformSpecific()
{
}

void MFModel_DeinitModulePlatformSpecific()
{
}

MF_API void MFModel_Draw(MFModel *pModel)
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
	MFRendererPC_SetModelColour(pModel->modelColour);

	MFMaterial *pMatOverride = (MFMaterial*)MFRenderer_GetRenderStateOverride(MFRS_MaterialOverride);

	if(pMatOverride)
		MFMaterial_SetMaterial(pMatOverride);

	MFModelDataChunk *pChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pSubobjects[a].pMeshChunks;

				if(!pMatOverride)
					MFMaterial_SetMaterial(pMC[b].pMaterial);

				if(pModel->pAnimation)
				{
					MFRendererPC_SetNumWeights(pMC[b].maxBlendWeights);
					MFRenderer_SetBatch(pMC[b].pBatchIndices, pMC[b].matrixBatchSize);
				}
				else
					MFRendererPC_SetNumWeights(0);

				MFRenderer_Begin();

				MeshChunkD3DRuntimeData &runtimeData = (MeshChunkD3DRuntimeData&)pMC[b].runtimeData;

				pd3dDevice->SetVertexDeclaration(runtimeData.vertexDecl);
				pd3dDevice->SetStreamSource(0, runtimeData.vertexBuffer, 0, pMC[b].pVertexFormat->pStreams[0].streamStride);
				if(runtimeData.animBuffer)
					pd3dDevice->SetStreamSource(1, runtimeData.animBuffer, 0, pMC[b].pVertexFormat->pStreams[1].streamStride);
				pd3dDevice->SetIndices(runtimeData.indexBuffer);

				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMC[b].numVertices, 0, pMC[b].numIndices/3);
			}
		}
	}
}

HRESULT MFModelD3D9_CreateVertexDeclaration(MFMeshVertexFormat *pMVF, IDirect3DVertexDeclaration9** ppDecl)
{
	D3DVERTEXELEMENT9 elements[16];
	int element = 0;

	for(int a=0; a<pMVF->numVertexStreams; ++a)
	{
		for(int b=0; b<pMVF->pStreams[a].numVertexElements; ++b)
		{
			elements[element].Stream = (uint16)a;
			elements[element].Offset = (uint16)pMVF->pStreams[a].pElements[b].offset;
			elements[element].Type = gTypeTable[pMVF->pStreams[a].pElements[b].type];
			elements[element].Method = D3DDECLMETHOD_DEFAULT;
			elements[element].Usage = gUsageTable[pMVF->pStreams[a].pElements[b].usage];
			elements[element].UsageIndex = (uint8)pMVF->pStreams[a].pElements[b].usageIndex;
			++element;
		}
	}
	D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
	elements[element] = endMacro;

	return pd3dDevice->CreateVertexDeclaration(elements, ppDecl);
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);

	HRESULT hr;

	MFDebug_Assert(sizeof(MeshChunkD3DRuntimeData) <= sizeof(pMC->runtimeData), "MeshChunkD3DRuntimeData is larger than runtimeData!");
	MeshChunkD3DRuntimeData &runtimeData = (MeshChunkD3DRuntimeData&)pMC->runtimeData;

	// create D3D interfaces
	hr = MFModelD3D9_CreateVertexDeclaration(pMC->pVertexFormat, &runtimeData.vertexDecl);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex declaration..");

	hr = pd3dDevice->CreateVertexBuffer(pMC->pVertexFormat->pStreams[0].streamStride*pMC->numVertices, 0, 0, D3DPOOL_MANAGED, &runtimeData.vertexBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex buffer..");
	if(pMC->pVertexFormat->numVertexStreams > 1)
	{
		hr = pd3dDevice->CreateVertexBuffer(pMC->pVertexFormat->pStreams[1].streamStride*pMC->numVertices, 0, 0, D3DPOOL_MANAGED, &runtimeData.animBuffer, NULL);
		MFDebug_Assert(SUCCEEDED(hr), "Failed to create animation buffer..");
	}
	hr = pd3dDevice->CreateIndexBuffer(sizeof(uint16)*pMC->numIndices, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &runtimeData.indexBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create index buffer..");

	void *pData;

	// fill vertex buffer
	hr = runtimeData.vertexBuffer->Lock(0, 0, &pData, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock vertex buffer..");

	MFCopyMemory(pData, pMC->ppVertexStreams[0], pMC->pVertexFormat->pStreams[0].streamStride*pMC->numVertices);

	runtimeData.vertexBuffer->Unlock();

	// fill animation buffer
	if(pMC->pVertexFormat->numVertexStreams > 1)
	{
		hr = runtimeData.animBuffer->Lock(0, 0, &pData, 0);
		MFDebug_Assert(SUCCEEDED(hr), "Failed to lock animation buffer..");

		MFCopyMemory(pData, pMC->ppVertexStreams[1], pMC->pVertexFormat->pStreams[1].streamStride*pMC->numVertices);

		runtimeData.animBuffer->Unlock();
	}

	// fill index buffer
	hr = runtimeData.indexBuffer->Lock(0, 0, &pData, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock index buffer..");

	MFCopyMemory(pData, pMC->pIndexData, sizeof(uint16)*pMC->numIndices);

	runtimeData.indexBuffer->Unlock();
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);

	MeshChunkD3DRuntimeData &runtimeData = (MeshChunkD3DRuntimeData&)pMC->runtimeData;

	runtimeData.indexBuffer->Release();
	runtimeData.indexBuffer = NULL;

	runtimeData.vertexBuffer->Release();
	runtimeData.vertexBuffer = NULL;

	if(runtimeData.animBuffer)
	{
		runtimeData.animBuffer->Release();
		runtimeData.animBuffer = NULL;
	}

	runtimeData.vertexDecl->Release();
	runtimeData.vertexDecl = NULL;
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMC, pBase, load);
}

#endif // MF_RENDERER
