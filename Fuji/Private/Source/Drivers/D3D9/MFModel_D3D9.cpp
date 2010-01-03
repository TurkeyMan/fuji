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

static BYTE gUsageTable[] =
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

static BYTE gTypeTable[] =
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
	MFRendererPC_SetModelColour(pModel->modelColour);

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

				pd3dDevice->SetVertexDeclaration((IDirect3DVertexDeclaration9*)pMC[b].userData[3]);
				pd3dDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)pMC[b].userData[0], 0, pMC[b].pVertexFormat->pStreams[0].streamStride);
				if(pMC[b].userData[1])
					pd3dDevice->SetStreamSource(1, (IDirect3DVertexBuffer9*)pMC[b].userData[1], 0, pMC[b].pVertexFormat->pStreams[1].streamStride);
				pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)pMC[b].userData[2]);

				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMC[b].numVertices, 0, pMC[b].numIndices/3);
			}
		}
	}
}

HRESULT MFModelD3D9_CreateVertexDeclaration(MFVertexFormat *pVF, IDirect3DVertexDeclaration9** ppDecl)
{
	D3DVERTEXELEMENT9 elements[16];
	int element = 0;

	for(int a=0; a<pVF->numVertexStreams; ++a)
	{
		for(int b=0; b<pVF->pStreams[a].numVertexElements; ++b)
		{
			elements[element].Stream = a;
			elements[element].Offset = pVF->pStreams[a].pElements[b].offset;
			elements[element].Type = gTypeTable[pVF->pStreams[a].pElements[b].type];
			elements[element].Method = D3DDECLMETHOD_DEFAULT;
			elements[element].Usage = gUsageTable[pVF->pStreams[a].pElements[b].usage];
			elements[element].UsageIndex = pVF->pStreams[a].pElements[b].usageIndex;
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

	IDirect3DVertexBuffer9* &vb = (IDirect3DVertexBuffer9*&)pMC->userData[0];
	IDirect3DVertexBuffer9* &ab = (IDirect3DVertexBuffer9*&)pMC->userData[1];
	IDirect3DIndexBuffer9* &ib = (IDirect3DIndexBuffer9*&)pMC->userData[2];
	IDirect3DVertexDeclaration9* &vd = (IDirect3DVertexDeclaration9*&)pMC->userData[3];

	// create D3D interfaces
	hr = MFModelD3D9_CreateVertexDeclaration(pMC->pVertexFormat, &vd);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex declaration..");

	hr = pd3dDevice->CreateVertexBuffer(pMC->pVertexFormat->pStreams[0].streamStride*pMC->numVertices, 0, 0, D3DPOOL_MANAGED, &vb, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex buffer..");
	if(pMC->pVertexFormat->numVertexStreams > 1)
	{
		hr = pd3dDevice->CreateVertexBuffer(pMC->pVertexFormat->pStreams[1].streamStride*pMC->numVertices, 0, 0, D3DPOOL_MANAGED, &ab, NULL);
		MFDebug_Assert(SUCCEEDED(hr), "Failed to create animation buffer..");
	}
	hr = pd3dDevice->CreateIndexBuffer(sizeof(uint16)*pMC->numIndices, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create index buffer..");

	void *pData;

	// fill vertex buffer
	hr = vb->Lock(0, 0, &pData, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock vertex buffer..");

	MFCopyMemory(pData, pMC->ppVertexStreams[0], pMC->pVertexFormat->pStreams[0].streamStride*pMC->numVertices);

	vb->Unlock();

	// fill animation buffer
	if(pMC->pVertexFormat->numVertexStreams > 1)
	{
		hr = ab->Lock(0, 0, &pData, 0);
		MFDebug_Assert(SUCCEEDED(hr), "Failed to lock animation buffer..");

		MFCopyMemory(pData, pMC->ppVertexStreams[1], pMC->pVertexFormat->pStreams[1].streamStride*pMC->numVertices);

		ab->Unlock();
	}

	// fill index buffer
	hr = ib->Lock(0, 0, &pData, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock index buffer..");

	MFCopyMemory(pData, pMC->pIndexData, sizeof(uint16)*pMC->numIndices);

	ib->Unlock();
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);

	IDirect3DVertexBuffer9* &vb = (IDirect3DVertexBuffer9*&)pMC->userData[0];
	IDirect3DVertexBuffer9* &ab = (IDirect3DVertexBuffer9*&)pMC->userData[1];
	IDirect3DIndexBuffer9* &ib = (IDirect3DIndexBuffer9*&)pMC->userData[2];
	IDirect3DVertexDeclaration9* &vd = (IDirect3DVertexDeclaration9*&)pMC->userData[3];

	ib->Release();
	ib = NULL;

	vb->Release();
	vb = NULL;

	if(ab)
	{
		ab->Release();
		ab = NULL;
	}

	vd->Release();
	vd = NULL;
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMC, pBase, load);
}

#endif // MF_RENDERER
