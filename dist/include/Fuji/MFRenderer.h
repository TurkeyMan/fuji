/**
 * @file MFRenderer.h
 * @brief Provides access to low level renderer calls.
 * @author Manu Evans
 * @defgroup MFRenderer Renderer Interface
 * @{
 */

#pragma once
#if !defined(_MFRENDERER_H)
#define _MFRENDERER_H

#include "MFVertex.h"
#include "MFMatrix.h"

struct MFTexture;
struct MFMaterial;
struct MFModel;

// new from display...
enum MFRenderClearFlags
{
	MFRCF_Colour		= 1,
	MFRCF_ZBuffer		= 2,
	MFRCF_Stencil		= 4,

	MFRCF_None			= 0,
	MFRCF_All			= MFRCF_Colour | MFRCF_ZBuffer | MFRCF_Stencil,
	MFRCF_DepthStencil	= MFRCF_ZBuffer | MFRCF_Stencil,
	MFRCF_ForceInt		= 0x7FFFFFFF
};

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags = MFRCF_All, const MFVector &colour = MFVector::zero, float z = 1.f, int stencil = 0);

MF_API void MFRenderer_SetViewport(MFRect *pRect);
MF_API void MFRenderer_ResetViewport();

MF_API MFTexture* MFRenderer_GetDeviceRenderTarget();
MF_API MFTexture* MFRenderer_GetDeviceDepthStencil();
MF_API void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pDepthStencil);
MF_API void MFRenderer_SetDeviceRenderTarget();

MF_API float MFRenderer_GetTexelCenterOffset();

/**
 * Begin a render block.
 * Configures the renderer to render geometry with the current settings.
 * @return Remaining number of passes.
 * @remarks MFRenderer_Begin() should be called in a while loop to perform multipass effects. The same geometry should be submitted for each renderer pass.
 */
MF_API int MFRenderer_Begin();

/**
 * Set the animation matrices.
 * Sets the current set of animation matrices.
 * @param pMatrices Pointer to an array of matrices to be used for animation.
 * @param numMatrices The number of matrices that \a pMatrices points to.
 * @return None.
 */
MF_API void MFRenderer_SetMatrices(const MFMatrix *pMatrices, int numMatrices);

/**
 * Set the current animation bone batch.
 * Sets the current animation bone batch.
 * @param pBatch Pointer to the matrix batch.
 * @param numBonesInBatch The number of bones in the batch.
 * @return None.
 */
MF_API void MFRenderer_SetBatch(const uint16 *pBatch, int numBonesInBatch);

MF_API void* MFRenderer_AllocateCommandBufferMemory(size_t bytes, size_t alignment = 16);

MF_API void* MFRenderer_AllocateScratchMemory(size_t bytes, size_t alignment = 16);

MF_API void MFRenderer_GetMemoryStats(size_t *pCommandBuffer, size_t *pCommandBufferPeak, size_t *pScratch, size_t *pScratchPeak);

// new renderer interface...

enum MFRenderLayerSortMode
{
	MFRL_SM_Unknown = -1,

	MFRL_SM_Default = 0,
	MFRL_SM_FrontToBack,
	MFRL_SM_BackToFront,
	MFRL_SM_None,

	MFRL_SM_Max,
	MFRL_SM_ForceInt = 0x7FFFFFFF,
};


struct MFRenderer;

struct MFRenderLayer;

struct MFRenderLayerDescription
{
	const char *pName;
};

struct MFRenderLayerSet
{
	MFRenderLayer *pSolidLayer;
	MFRenderLayer *pAlphaLayer;
	MFRenderLayer *pZPrimeLayer;
};

MF_API MFRenderer* MFRenderer_Create(MFRenderLayerDescription *pLayers, int numLayers, MFStateBlock *pGlobal, MFStateBlock *pOverride);
MF_API void MFRenderer_Destroy(MFRenderer *pRenderer);

MF_API MFStateBlock* MFRenderer_SetGlobalStateBlock(MFRenderer *pRenderer, MFStateBlock *pGlobal);
MF_API MFStateBlock* MFRenderer_SetOverrideStateBlock(MFRenderer *pRenderer, MFStateBlock *pOverride);

MF_API void MFRenderer_SetRenderLayerSet(MFRenderer *pRenderer, MFRenderLayerSet *pLayerSet);

MF_API MFRenderLayer* MFRenderer_GetLayer(MFRenderer *pRenderer, int layer);
MF_API MFRenderLayer* MFRenderer_GetDebugLayer(MFRenderer *pRenderer);

MF_API void MFRenderer_CloneLayer(MFRenderer *pRenderer, int sourceLayer, int destLayer);

MF_API void MFRenderer_BuildCommandBuffers(MFRenderer *pRenderer);
MF_API void MFRenderer_Kick(MFRenderer *pRenderer);

MF_API MFRenderer* MFRenderer_SetCurrent(MFRenderer *pRenderer);
MF_API MFRenderer* MFRenderer_GetCurrent();

MF_API void MFRenderer_AddMesh(MFMesh *pMesh, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderer_AddModel(MFModel *pModel, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderer_AddVertices(MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderer_AddIndexedVertices(MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

MF_API void MFRenderLayer_SetLayerStateBlock(MFRenderLayer *pLayer, MFStateBlock *pState);
MF_API void MFRenderLayer_SetLayerSortMode(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode);

MF_API void MFRenderLayer_SetLayerRenderTarget(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture);
MF_API void MFRenderLayer_SetLayerDepthTarget(MFRenderLayer *pLayer, MFTexture *pTexture);
MF_API void MFRenderLayer_SetLayerColorCapture(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture);
MF_API void MFRenderLayer_SetLayerDepthCapture(MFRenderLayer *pLayer, MFTexture *pTexture);

MF_API void MFRenderLayer_SetClear(MFRenderLayer *pLayer, MFRenderClearFlags clearFlags, const MFVector &colour = MFVector::zero, float z = 1.f, int stencil = 0);

MF_API void MFRenderLayer_AddVertices(MFRenderLayer *pLayer, MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderLayer_AddIndexedVertices(MFRenderLayer *pLayer, MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderLayer_AddMesh(MFRenderLayer *pLayer, MFMesh *pMesh, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderLayer_AddModel(MFRenderLayerSet *pLayerSet, MFModel *pModel, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

MF_API void MFRenderLayer_AddFence(MFRenderLayer *pLayer);


// helpers...
__forceinline MFRenderer* MFRenderer_SetLayerStateBlock(MFRenderer *pRenderer, int layer, MFStateBlock *pState)
{
	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, layer);
	MFRenderLayer_SetLayerStateBlock(pLayer, pState);
}

#endif // _MFRENDERER_H

/** @} */
