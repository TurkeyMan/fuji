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
const char* MFRenderer_D3D11_GetSemanticName(MFVertexElementType type);
MF_API MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount);

//---------------------------------------------------------------------------------------------------------------------
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;
//---------------------------------------------------------------------------------------------------------------------

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

	//MFRenderer_SetMatrices(pAnimMats, pAnimMats ? pModel->pAnimation->numBones : 0);
	MFRenderer_D3D11_SetWorldToScreenMatrix(wts);
	//MFRenderer_D3D11_SetModelColour(pModel->modelColour);

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pSubobjects[a].pMeshChunks;

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

				MFVertex_SetVertexDeclaration(pMC->pDecl);
				MFVertex_SetVertexStreamSource(0, pMC->pVertexBuffers[0]);
				if(pMC->pVertexBuffers[1])
					MFVertex_SetVertexStreamSource(1, pMC->pVertexBuffers[1]);
				MFVertex_SetIndexBuffer(pMC->pIndexBuffer);
				MFVertex_RenderIndexedVertices(MFPT_TriangleList, 0, 0, pMC[b].numVertices, pMC[b].numIndices);
			}
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMC, pBase, load);
}
//---------------------------------------------------------------------------------------------------------------------
#endif // MF_RENDERER