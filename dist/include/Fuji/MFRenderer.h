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
#include "MFRenderState.h"

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

MF_API void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pDepthStencil);
MF_API void MFRenderer_SetDeviceRenderTarget();


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

/**
 * Get the current texel center offset.
 * Gets the offset to the center of a texel.
 * @return The texel center offset.
 */
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
	const char *pName;	/**< Name of the render layer. */
};

/**
 * Render layer set
 * A set of render layers to be used while submitting geometry.
 */
struct MFRenderLayerSet
{
	MFRenderLayer *pSolidLayer;		/**< Layer for solid geometry. */
	MFRenderLayer *pAlphaLayer;		/**< Optional layer for transparent geometry. If NULL, transparent meshes are submitted to the solid layer. */
	MFRenderLayer *pZPrimeLayer;	/**< Optional layer for z-prime geometry. */
	MFRenderLayer *pShadowLayer;	/**< Optional layer for shadow casting geometry. */
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

/**
 * Set a global stateblock.
 * Assign a global stateblock to a renderer.
 * @param pRenderer MFRenderer instance.
 * @param pGlobal A stateblock to be assigned as the global stateblock.
 * @return The previously set global stateblock.
 * @see MFRenderer_SetOverrideStateBlock()
 */
MF_API const MFStateBlock* MFRenderer_SetGlobalStateBlock(MFRenderer *pRenderer, const MFStateBlock *pGlobal);

/**
 * Set an override stateblock.
 * Assign an override stateblock to a renderer.
 * @param pRenderer MFRenderer instance.
 * @param pOverride A stateblock to be assigned as the override stateblock.
 * @return The previously set override stateblock.
 * @see MFRenderer_SetGlobalStateBlock()
 */
MF_API const MFStateBlock* MFRenderer_SetOverrideStateBlock(MFRenderer *pRenderer, const MFStateBlock *pOverride);

/**
 * Assign a render layer set.
 * Assigns a render layer set to a renderer.
 * @param pRenderer MFRenderer instance.
 * @param pLayerSet The layer set to use in following rendering calls.
 * @return None.
 */
MF_API void MFRenderer_SetRenderLayerSet(MFRenderer *pRenderer, MFRenderLayerSet *pLayerSet);

/**
 * Get a render layer.
 * Gets a render layer.
 * @param pRenderer MFRenderer instance.
 * @param layer Index of the render layer.
 * @return The requested render layer.
 */
MF_API MFRenderLayer* MFRenderer_GetLayer(MFRenderer *pRenderer, int layer);

/**
 * Get the debug render layer.
 * Gets the debug render layer.
 * @param pRenderer MFRenderer instance.
 * @return The debug render layer.
 * @remarks The debug layer is the final render layer, which may be used to render any debug information to the screen.
 */
MF_API MFRenderLayer* MFRenderer_GetDebugLayer(MFRenderer *pRenderer);

/**
 * Clone a render layer.
 * Clones a render layer into another render layer. This operation includes all render elements submitted for rendering.
 * @param pRenderer MFRenderer instance.
 * @param sourceLayer Index of the layer to copy from.
 * @param destLayer Index of the layer to write to. Existing contents will be lost.
 * @return None.
 */
MF_API void MFRenderer_CloneLayer(MFRenderer *pRenderer, int sourceLayer, int destLayer);

/**
 * Build the GPU command buffers.
 * Builds the GPU command buffers from the current renderer state.
 * @param pRenderer MFRenderer instance.
 * @return None.
 */
MF_API void MFRenderer_BuildCommandBuffers(MFRenderer *pRenderer);

/**
 * Submit renderer to the GPU for rendering.
 * Submits the renderer's command buffers to the GPU for rendering.
 * @param pRenderer MFRenderer instance.
 * @return None.
 */
MF_API void MFRenderer_Kick(MFRenderer *pRenderer);

/**
 * Make a renderer current.
 * Makes a renderer 'current'. It will be used in any following render calls.
 * @param pRenderer MFRenderer instance.
 * @return The previous 'current' renderer.
 */
MF_API MFRenderer* MFRenderer_SetCurrent(MFRenderer *pRenderer);

/**
 * Get the current renderer.
 * Returns the current renderer.
 * @return The current renderer.
 */
MF_API MFRenderer* MFRenderer_GetCurrent();

/**
 * Submit a mesh for rendering.
 * Submits a mesh for rendering.
 * @param pMesh Pointer to an \a MFMesh to render.
 * @param pMaterial Material to use when rendering the mesh.
 * @param pEntity Optional pointer to an entity stateblock.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @remarks The current layer set will be used to choose a render layer depending on mesh and material properties.
 * @see MFRenderer_AddModel(), MFRenderer_AddVertices(), MFRenderer_AddIndexedVertices(), MFRenderer_SetRenderLayerSet()
 */
MF_API void MFRenderer_AddMesh(MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Submit a model for rendering.
 * Submits a model for rendering.
 * @param pModel Model to render.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @remarks The current layer set will be used to choose render layers for each subobject depending on mesh and material properties.
 * @see MFRenderer_AddMesh(), MFRenderer_AddVertices(), MFRenderer_AddIndexedVertices(), MFRenderer_SetRenderLayerSet()
 */
MF_API void MFRenderer_AddModel(MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Submit vertices for rendering.
 * Submits vertices for rendering.
 * @param pMeshStateBlock Pointer to a mesh stateblock. This should contain at least a VertexDeclaration and VertexBuffer states.
 * @param firstVertex Offset of the first vertex to render.
 * @param numVertices Number of vertices to render.
 * @param primType Type of primitives to render.
 * @param pMaterial Material to use when rendering the mesh.
 * @param pEntity Optional pointer to an entity stateblock.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @remarks The current layer set will be used to choose a render layer depending on mesh and material properties.
 * @see MFRenderer_AddMesh(), MFRenderer_AddModel(), MFRenderer_AddIndexedVertices(), MFRenderer_SetRenderLayerSet(), MFStateBlock_SetMiscState()
 */
MF_API void MFRenderer_AddVertices(const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Submit indexed vertices for rendering.
 * Submits indexed vertices for rendering.
 * @param pMeshStateBlock Pointer to a mesh stateblock. This should contain at least a VertexDeclaration, an IndexBuffer, and VertexBuffer states.
 * @param firstIndex Offset of the first index to render.
 * @param numVertices Number of vertices to render.
 * @param primType Type of primitives to render.
 * @param pMaterial Material to use when rendering the mesh.
 * @param pEntity Optional pointer to an entity stateblock.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @remarks The current layer set will be used to choose a render layer depending on mesh and material properties.
 * @see MFRenderer_AddMesh(), MFRenderer_AddModel(), MFRenderer_AddVertices(), MFRenderer_SetRenderLayerSet(), MFStateBlock_SetMiscState()
 */
MF_API void MFRenderer_AddIndexedVertices(const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Set a render layer stateblock.
 * Assign a render layer stateblock.
 * @param pLayer An MFRenderLayer.
 * @param pState A stateblock to be assigned as the layer stateblock.
 * @return None.
 */
MF_API void MFRenderLayer_SetLayerStateBlock(MFRenderLayer *pLayer, const MFStateBlock *pState);

/**
 * Set the render layer sort mode.
 * Sets the render layer sort mode, which controls sorting of render elements within the layer.
 * @param pLayer An MFRenderLayer.
 * @param sortMode The sort more for the layer.
 * @return None.
 */
MF_API void MFRenderLayer_SetLayerSortMode(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode);

/**
 * Set a render layer's render target.
 * Sets a render layer's render target.
 * @param pLayer An MFRenderLayer.
 * @param targetIndex Render target index.
 * @param pTexture A texture created with the \a TEX_RenderTarget flag to be assigned as the render target.
 * @return None.
 */
MF_API void MFRenderLayer_SetLayerRenderTarget(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture);

/**
 * Set a render layer's depth target.
 * Sets a render layer's depth target.
 * @param pLayer An MFRenderLayer.
 * @param pTexture A texture created with the \a TEX_RenderTarget flag to be assigned as the depth target.
 * @return None.
 * @remarks \a pTexture should be a compatible z-buffer format. Use ImgFmt_SelectDepth or ImgFmt_SelectDepthStencil to select a format automatically.
 */
MF_API void MFRenderLayer_SetLayerDepthTarget(MFRenderLayer *pLayer, MFTexture *pTexture);

/**
 * Set a render layer's render target capture surface.
 * Sets a texture that will receive a copy of the render target when the layer has finished rendering.
 * @param pLayer An MFRenderLayer.
 * @param targetIndex Render target index.
 * @param pTexture A texture created with the \a TEX_RenderTarget flag to be assigned as the capture target.
 * @return None.
 */
MF_API void MFRenderLayer_SetLayerColourCapture(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture);

/**
 * Set a render layer's depth target capture surface.
 * Sets a texture that will receive a copy of the depth target when the layer has finished rendering.
 * @param pLayer An MFRenderLayer.
 * @param pTexture A texture created with the \a TEX_RenderTarget flag to be assigned as the capture target.
 * @return None.
 */
MF_API void MFRenderLayer_SetLayerDepthCapture(MFRenderLayer *pLayer, MFTexture *pTexture);

/**
 * Set the clear mode for the layer.
 * Sets the clear mode for a render layer.
 * @param pLayer An MFRenderLayer.
 * @param clearFlags A combination of flags from the \a MFRenderClearFlags enumerated type specifying which buffers to clear.
 * @param colour Colour to be written.
 * @param z Z value to be written.
 * @param stencil Stencil value to be written.
 * @return None.
 * @remarks Render layers are cleared according to the clear mode immediately before rendering.
 */
MF_API void MFRenderLayer_SetClear(MFRenderLayer *pLayer, MFRenderClearFlags clearFlags, const MFVector &colour = MFVector::zero, float z = 1.f, int stencil = 0);

/**
 * Submit vertices for rendering.
 * Submits vertices to a layer for rendering.
 * @param pLayer Layer to receive the geometry.
 * @param pMeshStateBlock Pointer to a mesh stateblock. This should contain at least a VertexDeclaration and VertexBuffer states.
 * @param firstVertex Offset of the first vertex to render.
 * @param numVertices Number of vertices to render.
 * @param primType Type of primitives to render.
 * @param pMaterial Material to use when rendering the mesh.
 * @param pEntity Optional pointer to an entity stateblock.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @see MFRenderLayer_AddIndexedVertices(), MFRenderLayer_AddMesh(), MFRenderLayer_AddModel(), MFStateBlock_SetMiscState()
 */
MF_API void MFRenderLayer_AddVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Submit indexed vertices for rendering.
 * Submits indexed vertices to a layer for rendering.
 * @param pLayer Layer to receive the geometry.
 * @param pMeshStateBlock Pointer to a mesh stateblock. This should contain at least a VertexDeclaration, an IndexBuffer, and VertexBuffer states.
 * @param firstIndex Offset of the first index to render.
 * @param numVertices Number of vertices to render.
 * @param primType Type of primitives to render.
 * @param pMaterial Material to use when rendering the mesh.
 * @param pEntity Optional pointer to an entity stateblock.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @see MFRenderLayer_AddVertices(), MFRenderLayer_AddMesh(), MFRenderLayer_AddModel(), MFStateBlock_SetMiscState()
 */
MF_API void MFRenderLayer_AddIndexedVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Submit a mesh for rendering.
 * Submits a mesh to a layer for rendering.
 * @param pLayer Layer to receive the geometry.
 * @param pMesh Pointer to an \a MFMesh to render.
 * @param pMaterial Material to use when rendering the mesh.
 * @param pEntity Optional pointer to an entity stateblock.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @see MFRendererLayer_AddVertices(), MFRendererLayer_AddIndexedVertices(), MFRenderLayer_AddModel()
 */
MF_API void MFRenderLayer_AddMesh(MFRenderLayer *pLayer, MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Submit a model for rendering.
 * Submits a model to the specified layers for rendering.
 * @param pLayerSet Layer set specifying which layers should receive the geometry.
 * @param pModel Model to render.
 * @param pMaterialOverride Optional pointer to a material override stateblock.
 * @param pView Optional pointer to a view stateblock.
 * @return None.
 * @remarks Render layers will be chosen from \a pLayerSet for each subobject depending on mesh and material properties.
 * @see MFRendererLayer_AddVertices(), MFRendererLayer_AddIndexedVertices(), MFRenderLayer_AddMesh()
 */
MF_API void MFRenderLayer_AddModel(MFRenderLayerSet *pLayerSet, MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Add a fence to the render layer
 * Adds a fence to the render layer.
 * @param pLayer Layer to receive the fence.
 * @return None.
 * @remarks Adding a fence to a render layer will force all geometry submitted prior to the fence be rendered before any geometry after.
 */
MF_API void MFRenderLayer_AddFence(MFRenderLayer *pLayer);


/**
 * Set a render layer stateblock.
 * Assign a render layer stateblock.
 * @param pRenderer MFRenderer instance.
 * @param layer Layer index.
 * @param pState A stateblock to be assigned as the layer stateblock.
 * @return None.
 */
__forceinline void MFRenderer_SetLayerStateBlock(MFRenderer *pRenderer, int layer, const MFStateBlock *pState)
{
	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, layer);
	MFRenderLayer_SetLayerStateBlock(pLayer, pState);
}

#endif // _MFRENDERER_H

/** @} */
