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

	MFModelDataChunk *pChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

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
					MFRendererPC_SetNumWeights(pMC[b].maxBlendWeights);
					MFRenderer_SetBatch(pMC[b].pBatchIndices, pMC[b].matrixBatchSize);
				}
				else
					MFRendererPC_SetNumWeights(0);

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

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	MFMaterial_Release(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMC, pBase, load);
}

#endif // MF_RENDERER
