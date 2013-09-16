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

/**
 * Clear flags.
 * Flags that control which buffers are affected during clear operations.
 */
enum MFRenderClearFlags
{
	MFRCF_Colour		= 1,											/**< Clear the colour buffer */
	MFRCF_ZBuffer		= 2,											/**< Clear the depth buffer */
	MFRCF_Stencil		= 4,											/**< Clear the stencil buffer */

	MFRCF_None			= 0,											/**< Don't clear any buffers */
	MFRCF_All			= MFRCF_Colour | MFRCF_ZBuffer | MFRCF_Stencil,	/**< Clear the colour, depth and stencil buffers */
	MFRCF_DepthStencil	= MFRCF_ZBuffer | MFRCF_Stencil,				/**< Clear the depth and stencil buffers */

	MFRCF_ForceInt		= 0x7FFFFFFF									/**< Force MFRenderClearFlags to a uint type */
};

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags = MFRCF_All, const MFVector &colour = MFVector::zero, float z = 1.f, int stencil = 0);

MF_API void MFRenderer_SetViewport(MFRect *pRect);
MF_API void MFRenderer_ResetViewport();

/**
 * Get the device render target.
 * Get's the device render target. This is usually the display's back buffer.
 * @return An \a MFTexture representing the device render target.
 * @see MFRenderer_GetDeviceDepthStencil()
 */
MF_API MFTexture* MFRenderer_GetDeviceRenderTarget();

/**
 * Get the device depth+stencil target.
 * Get's the device depth+stencil target.
 * @return An \a MFTexture representing the device depth+stencil target.
 * @see MFRenderer_GetDeviceRenderTarget()
 */
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


/**
 * Allocate command buffer memory.
 * Allocates memory from the render command buffer.
 * @param bytes Number of bytes to allocate.
 * @param alignment Alignment of the allocation.
 * @return A pointer to a block of memory allocated from the command buffer.
 * @see MFRenderer_AllocateScratchMemory(), MFRenderer_GetMemoryStats()
 * @remarks Memory allocated in the device command buffer will persist until the GPU has finished rendering.
 */
MF_API void* MFRenderer_AllocateCommandBufferMemory(size_t bytes, size_t alignment = 16);

/**
 * Allocate scratch memory.
 * Allocates memory from the frame scratch buffer.
 * @param bytes Number of bytes to allocate.
 * @param alignment Alignment of the allocation.
 * @return A pointer to a block of memory allocated from the frame scratch buffer.
 * @see MFRenderer_AllocateCommandBufferMemory(), MFRenderer_GetMemoryStats()
 * @remarks Memory allocated in the frame scratch buffer will persist until the end of the current frame.
 */
MF_API void* MFRenderer_AllocateScratchMemory(size_t bytes, size_t alignment = 16);

/**
 * Get render memory statistics.
 * Get's statistics about the renderer memory usage.
 * @param pCommandBuffer Pointer to a size_t that receives the number of bytes allocated in the command buffer.
 * @param pCommandBufferPeak Pointer to a size_t that receives the largest number of bytes ever allocated in the command buffer.
 * @param pScratch Pointer to a size_t that receives the number of bytes allocated in the frame scratch buffer.
 * @param pScratchPeak Pointer to a size_t that receives the largest number of bytes ever allocated in the frame scratch buffer.
 * @return None.
 * @see MFRenderer_AllocateCommandBufferMemory(), MFRenderer_AllocateScratchMemory()
 */
MF_API void MFRenderer_GetMemoryStats(size_t *pCommandBuffer, size_t *pCommandBufferPeak, size_t *pScratch, size_t *pScratchPeak);

/**
 * Render layer sort mode.
 * Sort mode for render elements within a render layer.
 */
enum MFRenderLayerSortMode
{
	MFRL_SM_Unknown = -1,			/**< Unknown sort mode. */

	MFRL_SM_Default = 0,			/**< Default sort mode. Elements will be sorted to maximise efficiency with respect to GPU state changes. */
	MFRL_SM_FrontToBack,			/**< Sort elements from front to back. Often used for opaque geometry to reduce overdraw. */
	MFRL_SM_BackToFront,			/**< Sort elements from back to front. Often used when rendering transparent geometry to reduce occlusion. */
	MFRL_SM_None,					/**< Don't sort the layer. Elements are rendered in the order they are submitted. */

	MFRL_SM_Max,					/**< Number of sort modes */
	MFRL_SM_ForceInt = 0x7FFFFFFF,	/**< Force MFRenderLayerSortMode to an int type */
};

/**
 * @struct MFRenderer
 * Represents a Fuji renderer.
 */
struct MFRenderer;

/**
 * @struct MFRenderLayer
 * Represents a Fuji render layer.
 */
struct MFRenderLayer;

/**
 * Render layer description
 * Describes an \a MFRenderLayer.
 */
struct MFRenderLayerDescription
{
	const char *pName;	//**< Name of the render layer. */
};

/**
 * Render layer set
 * A set of render layers to be used while submitting geometry.
 */
struct MFRenderLayerSet
{
	MFRenderLayer *pSolidLayer;		//**< Layer for solid geometry. */
	MFRenderLayer *pAlphaLayer;		//**< Optional layer for transparent geometry. If NULL, transparent meshes are submitted to the solid layer. */
	MFRenderLayer *pZPrimeLayer;	//**< Optional layer for z-prime geometry. */
	MFRenderLayer *pShadowLayer;	//**< Optional layer for shadow casting geometry. */
};

/**
 * Create a renderer.
 * Creates an \a MFRenderer.
 * @param pLayers Pointer to an array of layer descriptions.
 * @param numLayers Number of layer descriptions pointed to by \a pLayers.
 * @param pGlobal Optional pointer to a global stateblock, which may be used to declare default values for any render states.
 * @param pOverride Optional pointer to an override stateblock, which may be used to override values for any render states.
 * @return A new \a MFRenderer instance.
 */
MF_API MFRenderer* MFRenderer_Create(MFRenderLayerDescription *pLayers, int numLayers, MFStateBlock *pGlobal, MFStateBlock *pOverride = NULL);

/**
 * Destroy a renderer.
 * Destroys an \a MFRenderer.
 * @param pRenderer A renderer to be destroyed.
 * @return None.
 */
MF_API void MFRenderer_Destroy(MFRenderer *pRenderer);

MF_API const MFStateBlock* MFRenderer_SetGlobalStateBlock(MFRenderer *pRenderer, const MFStateBlock *pGlobal);
MF_API const MFStateBlock* MFRenderer_SetOverrideStateBlock(MFRenderer *pRenderer, const MFStateBlock *pOverride);

MF_API void MFRenderer_SetRenderLayerSet(MFRenderer *pRenderer, MFRenderLayerSet *pLayerSet);

MF_API MFRenderLayer* MFRenderer_GetLayer(MFRenderer *pRenderer, int layer);
MF_API MFRenderLayer* MFRenderer_GetDebugLayer(MFRenderer *pRenderer);

MF_API void MFRenderer_CloneLayer(MFRenderer *pRenderer, int sourceLayer, int destLayer);

MF_API void MFRenderer_BuildCommandBuffers(MFRenderer *pRenderer);
MF_API void MFRenderer_Kick(MFRenderer *pRenderer);

MF_API MFRenderer* MFRenderer_SetCurrent(MFRenderer *pRenderer);
MF_API MFRenderer* MFRenderer_GetCurrent();

MF_API void MFRenderer_AddMesh(MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderer_AddModel(MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderer_AddVertices(const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderer_AddIndexedVertices(const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

MF_API void MFRenderLayer_SetLayerStateBlock(MFRenderLayer *pLayer, const MFStateBlock *pState);
MF_API void MFRenderLayer_SetLayerSortMode(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode);

MF_API void MFRenderLayer_SetLayerRenderTarget(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture);
MF_API void MFRenderLayer_SetLayerDepthTarget(MFRenderLayer *pLayer, MFTexture *pTexture);
MF_API void MFRenderLayer_SetLayerColorCapture(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture);
MF_API void MFRenderLayer_SetLayerDepthCapture(MFRenderLayer *pLayer, MFTexture *pTexture);

MF_API void MFRenderLayer_SetClear(MFRenderLayer *pLayer, MFRenderClearFlags clearFlags, const MFVector &colour = MFVector::zero, float z = 1.f, int stencil = 0);

MF_API void MFRenderLayer_AddVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderLayer_AddIndexedVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderLayer_AddMesh(MFRenderLayer *pLayer, MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
MF_API void MFRenderLayer_AddModel(MFRenderLayerSet *pLayerSet, MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

MF_API void MFRenderLayer_AddFence(MFRenderLayer *pLayer);


// helpers...
__forceinline void MFRenderer_SetLayerStateBlock(MFRenderer *pRenderer, int layer, const MFStateBlock *pState)
{
	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, layer);
	MFRenderLayer_SetLayerStateBlock(pLayer, pState);
}

#endif // _MFRENDERER_H

/** @} */
