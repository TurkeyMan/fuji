#include "Fuji.h"
#include "MFRenderer_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFView.h"
#include "MFHeap.h"

uintp gRenderStates[MFRS_Max];
uintp gCurrentRenderStates[MFRS_Max];

extern MFMaterial *pCurrentMaterial;

MFMatrix gTransformationMatrices[MFMT_Max];

const MFMatrix *pAnimMats = NULL;
int gNumAnimMats = 0;

const uint16 *pCurrentBatch = NULL;
int gNumBonesInBatch = 0;

MFInitStatus MFRenderer_InitModule()
{
	MFCALLSTACK;

	MFZeroMemory(gRenderStates, sizeof(gRenderStates));
	MFMemSet(gCurrentRenderStates, -1, sizeof(gCurrentRenderStates));

	for(int a=0; a<MFMT_Max; a++)
	{
		gTransformationMatrices[a] = MFMatrix::identity;
	}

	MFRenderer_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFRenderer_DeinitModule()
{
	MFCALLSTACK;

	MFRenderer_DeinitModulePlatformSpecific();
}

MF_API int MFRenderer_Begin()
{
	MFCALLSTACK;

	MFDebug_Assert(pCurrentMaterial, "No material set!");
	return pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial);
}

MF_API const MFMatrix& MFRenderer_GetMatrix(MFMatrixType type)
{
	MFCALLSTACK;

	return gTransformationMatrices[type];
}

MF_API void MFRenderer_SetMatrix(MFMatrixType type, const MFMatrix &matrix)
{
	MFCALLSTACK;

	gTransformationMatrices[type] = matrix;
}

MF_API void MFRenderer_SetMatrices(const MFMatrix *pMatrices, int numMatrices)
{
	pAnimMats = pMatrices;
	gNumAnimMats = numMatrices;
}

MF_API void MFRenderer_SetBatch(const uint16 *pBatch, int numBonesInBatch)
{
	pCurrentBatch = pBatch;
	gNumBonesInBatch = numBonesInBatch;
}

MF_API uintp MFRenderer_SetRenderStateOverride(uint32 renderState, uintp value)
{
	MFCALLSTACK;

	uintp t = gRenderStates[renderState];
	gRenderStates[renderState] = value;

	return t;
}

MF_API uintp MFRenderer_GetRenderStateOverride(uint32 renderState)
{
	MFCALLSTACK;

	return gRenderStates[renderState];
}


//////////////////

MF_API MFRenderer* MFRenderer_Create(int numLayers, MFStateBlock *pGlobal, MFStateBlock *pOverride)
{
	MFRenderer *pRenderer = (MFRenderer*)MFHeap_AllocAndZero(sizeof(MFRenderer) + sizeof(MFRenderLayer)*numLayers);

	pRenderer->pLayers = (MFRenderLayer*)&pRenderer[1];
	pRenderer->numLayers = numLayers;
	pRenderer->pGlobal = pGlobal;
	pRenderer->pOverride = pOverride;

	return pRenderer;
}

MF_API void MFRenderer_Destroy(MFRenderer *pRenderer)
{
	MFHeap_Free(pRenderer);
}

MF_API MFRenderLayer* MFRenderer_GetLayer(MFRenderer *pRenderer, int layer)
{
	return &pRenderer->pLayers[layer];
}

MF_API void MFRenderer_BuildCommandBuffers(MFRenderer *pRenderer)
{
	//...
}

MF_API void MFRenderer_Kick(MFRenderer *pRenderer)
{
	//...
}

MF_API MFStateBlock* MFRenderer_SetGlobalStateBlock(MFRenderer *pRenderer, MFStateBlock *pGlobal)
{
	MFStateBlock *pOld = pRenderer->pGlobal;
	pRenderer->pGlobal = pGlobal;
	return pOld;
}

MF_API MFStateBlock* MFRenderer_SetOverrideStateBlock(MFRenderer *pRenderer, MFStateBlock *pOverride)
{
	MFStateBlock *pOld = pRenderer->pOverride;
	pRenderer->pOverride = pOverride;
	return pOld;
}

MF_API void MFRenderer_CloneLayer(MFRenderer *pRenderer, int sourceLayer, int destLayer)
{
}

MF_API void MFRenderLayer_SetLayerStateBlock(MFRenderLayer *pLayer, MFStateBlock *pState)
{
	pLayer->pLayer = pState;
}

MF_API void MFRenderLayer_SetLayerSortMode(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode)
{
	pLayer->sortMode = sortMode;
}

//MF_API void MFRenderLayer_SetLayerRenderTarget(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode);

//MF_API void MFRenderLayer_Clear(MFRenderLayer *pLayer, MFRenderClearFlags clearFlags = MFRCF_All, const MFVector &colour = MFVector::zero, float z = 1.f, int stencil = 0);

MF_API void MFRenderLayer_AddModel(MFRenderLayer *pLayer, MFModel *pModel, MFStateBlock *pEntity, MFMaterial *pMaterial)
{
/*
	MFRenderElement &e = pLayer->elements.push();

	e.type = 0; // TODO: geometry?
	e.primarySortKey = pLayer->primaryKey;
	e.zSort = 0;

//	e.pShaderTechnique = NULL;

//	e.pViewState = NULL;
	e.pGeometryState = pMeshStateBlock;
	e.pEntityState = pEntity;
//	e.pMaterialState = pMaterial->
	e.pMaterialOverrideState = pMaterialOverride;

	e.vertexBufferOffset = firstVertex;
	e.indexBufferOffset = 0;
	e.vertexCount = numVertices;
	e.primType = primType;
	e.renderIndexed = 0;

//	e.pInstances = NULL;
//	e.numInstances = 0;
//	e.animBatch = 0;
*/
}

MF_API void MFRenderLayer_AddVertices(MFRenderLayer *pLayer, MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride)
{
	MFRenderElement &e = pLayer->elements.push();

	e.type = 0; // TODO: geometry?
	e.primarySortKey = (uint8)pLayer->primaryKey;
	e.zSort = 0;

//	e.pShaderTechnique = NULL;

	e.pViewState = NULL;
	e.pGeometryState = pMeshStateBlock;
	e.pEntityState = pEntity;
	e.pMaterialState = MFMaterial_GetMaterialStateBlock(pMaterial);
	e.pMaterialOverrideState = pMaterialOverride;

	e.vertexBufferOffset = firstVertex;
	e.indexBufferOffset = 0;
	e.vertexCount = numVertices;
	e.primType = primType;
	e.renderIndexed = 0;

//	e.pInstances = NULL;
//	e.numInstances = 0;
//	e.animBatch = 0;
}

MF_API void MFRenderLayer_AddIndexedVertices(MFRenderLayer *pLayer, MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride)
{
	MFRenderElement &e = pLayer->elements.push();

	e.type = 0; // TODO: geometry?
	e.primarySortKey = (uint8)pLayer->primaryKey;
	e.zSort = 0;

//	e.pShaderTechnique = NULL;

	e.pViewState = NULL;
	e.pGeometryState = pMeshStateBlock;
	e.pEntityState = pEntity;
	e.pMaterialState = MFMaterial_GetMaterialStateBlock(pMaterial);
	e.pMaterialOverrideState = pMaterialOverride;

	e.vertexBufferOffset = 0;
	e.indexBufferOffset = firstIndex;
	e.vertexCount = numVertices;
	e.primType = primType;
	e.renderIndexed = 1;

//	e.pInstances = NULL;
//	e.numInstances = 0;
//	e.animBatch = 0;
}

MF_API void MFRenderLayer_AddFence(MFRenderLayer *pLayer)
{
	++pLayer->primaryKey;
}
