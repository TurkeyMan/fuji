#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFModel_InitModulePlatformSpecific MFModel_InitModulePlatformSpecific_D3D11
	#define MFModel_DeinitModulePlatformSpecific MFModel_DeinitModulePlatformSpecific_D3D11
	#define MFModel_Draw MFModel_Draw_D3D11
	#define MFModel_CreateMeshChunk MFModel_CreateMeshChunk_D3D11
	#define MFModel_DestroyMeshChunk MFModel_DestroyMeshChunk_D3D11
	#define MFModel_FixUpMeshChunk MFModel_FixUpMeshChunk_D3D11
#endif

#include "MFModel_Internal.h"
#include "MFMesh_Internal.h"

#include "MFVertex.h"
#include "MFRenderer.h"
#include "MFRenderer_D3D11.h"
#include "MFDebug.h"
#include "MFView.h"


//---------------------------------------------------------------------------------------------------------------------
DXGI_FORMAT MFRenderer_D3D11_GetFormat(MFVertexDataFormat format);
DXGI_FORMAT MFRenderer_D3D11_GetFormat(MFMeshVertexDataType format);
const char* MFRenderer_D3D11_GetSemanticName(MFVertexElementType type);
MFVertexDataFormat MFMesh_ConvertMeshVertexDataType(MFMeshVertexDataType type);
MFVertexDeclaration *MFVertex_CreateVertexDeclaration2(MFMeshVertexFormat *pMVF);
MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount);

//---------------------------------------------------------------------------------------------------------------------
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
struct MeshChunkRuntimeDataD3D11
{
	//ID3D11Buffer *vertexBuffer;
	//ID3D11Buffer *animBuffer;
	//ID3D11Buffer *indexBuffer;
	//ID3D11InputLayout *vertexDecl;

	MFVertexDeclaration *pVertexDeclaration;
	MFVertexBuffer		*pVertexBuffer;
	MFVertexBuffer		*pAnimBuffer;
	MFIndexBuffer		*pIndexBuffer;
};
//---------------------------------------------------------------------------------------------------------------------
void MFModel_InitModulePlatformSpecific()
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
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

	//MFRenderer_SetMatrices(pAnimMats, pAnimMats ? pModel->pAnimation->numBones : 0);
	MFRenderer_D3D11_SetWorldToScreenMatrix(wts);
	//MFRenderer_D3D11_SetModelColour(pModel->modelColour);

	
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
					//MFRendererPC_SetNumWeights(pMC[b].maxBlendWeights);
					MFRenderer_SetBatch(pMC[b].pBatchIndices, pMC[b].matrixBatchSize);
				}
				else
				{
					//MFRendererPC_SetNumWeights(0);
				}

				MFRenderer_Begin();

				MeshChunkRuntimeDataD3D11 &runtimeData = (MeshChunkRuntimeDataD3D11&)pMC[b].runtimeData;

				MFVertex_SetVertexDeclaration(runtimeData.pVertexDeclaration);
				MFVertex_SetVertexStreamSource(0, runtimeData.pVertexBuffer);
				if (runtimeData.pAnimBuffer)
				{
					MFVertex_SetVertexStreamSource(1, runtimeData.pAnimBuffer);
				}
				MFVertex_RenderIndexedVertices(MFVPT_TriangleList, pMC[b].numVertices, pMC[b].numIndices, runtimeData.pIndexBuffer);
			}
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------
HRESULT MFModel_D3D1_CreateVertexDeclaration(MFMeshVertexFormat *pMVF, ID3D11InputLayout** ppInputLayout)
{
	D3D11_INPUT_ELEMENT_DESC elements[32];
	int element = 0;

	for (int a = 0; a < pMVF->numVertexStreams; ++a)
	{
		for (int b = 0; b < pMVF->pStreams[a].numVertexElements; ++b)
		{
			MFMeshVertexElement &rElement = pMVF->pStreams[a].pElements[b];

			elements[element].SemanticName = MFRenderer_D3D11_GetSemanticName(rElement.usage);
			elements[element].SemanticIndex = rElement.usageIndex;
			elements[element].Format = MFRenderer_D3D11_GetFormat(rElement.type);
			elements[element].InputSlot = a;
			elements[element].AlignedByteOffset = pMVF->pStreams[a].pElements[b].offset;
			elements[element].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elements[element].InstanceDataStepRate = 0;
			++element;

			MFDebug_Assert(element < 32, "whoops");
		}
	}
	
	return g_pd3dDevice->CreateInputLayout(elements, element, NULL, 0, ppInputLayout);
}
//---------------------------------------------------------------------------------------------------------------------
MFVertexDeclaration *MFModel_D3D1_CreateVertexDeclaration(MFMeshVertexFormat *pMVF)
{
	MFVertexElement elements[32];
	int element = 0;

	for (int a = 0; a < pMVF->numVertexStreams; ++a)
	{
		for (int b = 0; b < pMVF->pStreams[a].numVertexElements; ++b)
		{
			MFMeshVertexElement &rElement = pMVF->pStreams[a].pElements[b];

			//elements[element].SemanticName = MFRenderer_D3D11_GetSemanticName(rElement.usage);
			//elements[element].SemanticIndex = rElement.usageIndex;
			//elements[element].Format = MFRenderer_D3D11_GetFormat(rElement.type);
			//elements[element].InputSlot = a;
			//elements[element].AlignedByteOffset = pMVF->pStreams[a].pElements[b].offset;
			//elements[element].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			//elements[element].InstanceDataStepRate = 0;

			elements[element].stream = a;
			elements[element].elementType = rElement.usage;
			elements[element].elementIndex = rElement.usageIndex;
			//elements[element].componentCount = rElement.type;

			++element;

			MFDebug_Assert(element < 32, "whoops");
		}
	}
	
	return MFVertex_CreateVertexDeclaration(elements, element);
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
	
	MFDebug_Assert(sizeof(MeshChunkRuntimeDataD3D11) <= sizeof(pMC->runtimeData), "MeshChunkRuntimeDataD3D11 is larger than runtimeData!");
	MeshChunkRuntimeDataD3D11 &runtimeData = (MeshChunkRuntimeDataD3D11&)pMC->runtimeData;

	runtimeData.pVertexDeclaration = MFVertex_CreateVertexDeclaration2(pMC->pVertexFormat);

	runtimeData.pVertexBuffer = MFVertex_CreateVertexBuffer(runtimeData.pVertexDeclaration, pMC->numVertices, MFVBType_Static, pMC->ppVertexStreams[0]);
	
	if (pMC->pVertexFormat->numVertexStreams > 1)
	{
		runtimeData.pVertexBuffer = MFVertex_CreateVertexBuffer(runtimeData.pVertexDeclaration, pMC->numVertices, MFVBType_Static, pMC->ppVertexStreams[1]);
	}

	runtimeData.pIndexBuffer = MFVertex_CreateIndexBuffer(pMC->numIndices, const_cast<uint16*>(pMC->pIndexData));
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);

	MeshChunkRuntimeDataD3D11 &runtimeData = (MeshChunkRuntimeDataD3D11&)pMC->runtimeData;

	MFVertex_DestroyIndexBuffer(runtimeData.pIndexBuffer);
	MFVertex_DestroyVertexBuffer(runtimeData.pAnimBuffer);
	MFVertex_DestroyVertexBuffer(runtimeData.pVertexBuffer);
	MFVertex_DestroyVertexDeclaration(runtimeData.pVertexDeclaration);
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMC, pBase, load);
}
//---------------------------------------------------------------------------------------------------------------------
#endif // MF_RENDERER