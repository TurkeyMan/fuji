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

#include "MFMatrix.h"

struct MFTexture;
struct MFMaterial;
struct MFModel;
struct MFStateBlock;

/**
 * @struct MFMeshChunk
 * Represents a mesh chunk.
 */
struct MFMeshChunk;

/**
 * Matrix Types.
 * Various matrix types that the renderer needs to know about.
 */
enum MFMatrixType
{
	MFMT_Unknown, /**< Unknown transform matrix */

	// these can be set directly
	MFMT_WorldMatrix,		/**< World transform matrix */
	MFMT_CameraMatrix,		/**< Camera transform matrix */
	MFMT_ProjectionMatrix,	/**< Projection transform matrix */

	// these can not be set directly, only read
	MFMT_ViewMatrix,				/**< View matrix (inverse camera matrix) */
	MFMT_WorldViewMatrix,			/**< Concatenated World-View matrix */
	MFMT_ViewProjectionMatrix,		/**< Concatenated View-Projection matrix */
	MFMT_WorldViewProjectionMatrix,	/**< Concatenated World-View-Projection matrix */

	MFMT_Max,					/**< Maximum matrix type */
	MFMT_ForceInt = 0x7FFFFFFF	/**< Force MatrixType to an int type */
};

/**
 * Primitive Types.
 * Primitive types that can be submitted to the renderer.
 */
enum MFPrimType
{
	MFPT_PointList,	/**< Point list */
	MFPT_LineList,	/**< Line list */
	MFPT_LineStrip,	/**< Line strip */
	MFPT_TriList,	/**< Triangle list */
	MFPT_TriStrip,	/**< Triangle strip */
	MFPT_TriFan,	/**< Triangle fan */
	MFPT_QuadList,	/**< Quad list */

	MFPT_Max,		/**< Maximum prim type */
	MFPT_ForceInt = 0x7FFFFFFF	/**< Force PrimType to an int type */
};

/**
 * Render Flags.
 * Render flags to control the way geometry is rendered.
 */
enum MFRenderFlags
{
	MFRF_Prelit = 1,	/**< Vertices should not be lit */
	MFRF_Untextured = 2	/**< Geometry should not be textured */
};

/**
 * Vertex Components.
 * Various Vertex Components.
 */
enum MFVertexComponent
{
	MFVC_Invalid = -1,	/**< Invalid or unknown vertex component */
	MFVC_Position = 0,	/**< Vertex position */
	MFVC_Colour,		/**< Vertex colour */
	MFVC_Normal,		/**< Vertex normal */
	MFVC_TexCoord1,		/**< Vertex texture coordinate 1 */
	MFVC_TexCoord2,		/**< Vertex texture coordinate 2 */

	MFVC_Binormal,		/**< Vertex binormal */
	MFVC_Tangent,		/**< Vertex tangent */

	MFVC_Indices,		/**< Vertex bone indices */
	MFVC_Weights,		/**< Vertex blend normal */

	MFVC_Max,			/**< Maximum vertex component */
	MFVC_ForceInt = 0x7FFFFFFF	/**< Force VertexComponent to an int type */
};

// new from display...
enum MFRenderClearFlags
{
	MFRCF_Colour		= 1,
	MFRCF_ZBuffer		= 2,
	MFRCF_Stencil		= 4,

	MFRCF_All			= MFRCF_Colour | MFRCF_ZBuffer | MFRCF_Stencil,
	MFRCF_DepthStencil	= MFRCF_ZBuffer | MFRCF_Stencil,
	MFRCF_ForceInt		= 0x7FFFFFFF
};

MF_API void MFRenderer_SetClearColour(float r, float g, float b, float a);
MF_API void MFRenderer_ClearScreen(uint32 flags = MFRCF_All);

MF_API void MFRenderer_GetViewport(MFRect *pRect);
MF_API void MFRenderer_SetViewport(MFRect *pRect);
MF_API void MFRenderer_ResetViewport();

MF_API void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget);
MF_API void MFRenderer_SetDeviceRenderTarget();

MF_API float MFRenderer_GetTexelCenterOffset();

/**
 * Begin a render block.
 * Configures the renderer to render geometry with the current settings.
 * @return Remaining number of passes.
 * @remarks MFRenderer_Begin() should be called in a while loop to perform multipass effects. The same geometry should be submitted for each renderer pass.
 */
MF_API int MFRenderer_Begin(); // returns number of passes remaining..

/**
 * Get a transform matrix.
 * Gets a transform matrix.
 * @param type The matrix type to retrieve.
 * @return Returns a const reference to the specified matrix.
 */
MF_API const MFMatrix& MFRenderer_GetMatrix(MFMatrixType type);

/**
 * Set a transform matrix.
 * Sets a geometry transform matrix.
 * @param type The matrix type to set.
 * @param matrix A matrix to set as the specified matrix type.
 * @return None.
 */
MF_API void MFRenderer_SetMatrix(MFMatrixType type, const MFMatrix &matrix);

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
 * Flush the renderer settings to the hardware.
 * Flushes the renderer settings to the hardware.
 * @return None.
 */
MF_API void MFRenderer_FlushSettings();

/**
 * Render a mesh chunk.
 * Submits a mesh chunk for rendering.
 * @param pMeshChunk Pointer to the mesh chunk to be rendered.
 * @return None.
 */
MF_API void MFRenderer_RenderMeshChunk(MFMeshChunk *pMeshChunk);

/**
 * Begin an immediate mode block.
 * Begins an immediate mode block.
 * @param primType Type of primitives to be rendered.
 * @param renderFlags Flags to be passed to the renderer. Should be a combination of flags from the MFRenderFlags enumerated type.
 * @return Ummmm? Some kind of error?
 */
MF_API int MFRenderer_BeginImmediate(MFPrimType primType, uint32 renderFlags);

/**
 * Begin submiting vertices to the renderer.
 * Configures the renderer to begin submitting vertices to the hardware.
 * @param numVertices Number of vertices to be submitted.
 * @return Ummmm? Some kind of error?
 */
MF_API int MFRenderer_RenderVertices(int numVertices);

/**
 * Submit vertex data.
 * Submits a piece of vertex data to the renderer.
 * @param vertexComponent Value from the MFVertexComponent enumerated type specifying the vertex component that should be set.
 * @param x X component of data.
 * @param y Y component of data.
 * @param z Z component of data.
 * @param w W component of data.
 * @return None.
 */
MF_API void MFRenderer_SetVertexData4f(MFVertexComponent vertexComponent, float x, float y, float z, float w);

/**
 * Submit vertex data.
 * Submits a piece of vertex data to the renderer.
 * @param vertexComponent Value from the MFVertexComponent enumerated type specifying the vertex component that should be set.
 * @param data Vector specifying data for the vertex component.
 * @return None.
 */
MF_API void MFRenderer_SetVertexData4v(MFVertexComponent vertexComponent, const MFVector &data);

/**
 * End submitting vertices.
 * Ends submition of vertices.
 * @return None.
 */
MF_API void MFRenderer_EndVertices();

/**
 * End immediate mode rendering.
 * Ends immediate mode rendering.
 * @return None.
 */
MF_API void MFRenderer_EndImmediate();


enum MFRenderState
{
	MFRS_MaterialOverride,
	MFRS_NoZRead,
	MFRS_NoZWrite,
	MFRS_Untextured,
	MFRS_PreLit,
	MFRS_PrimType,

	MFRS_ShowZBuffer,
	MFRS_ShowOverDraw,

	MFRS_Max,
	MFRS_ForceInt = 0x7FFFFFFF
};

/**
 * Set a global render state override.
 * Sets a global render state override.
 * @param renderState The global renderstate to set.
 * @param value Value to set.
 * @return Returns the old value.
 */
MF_API uintp MFRenderer_SetRenderStateOverride(uint32 renderState, uintp value);

/**
 * Get the value of a global renderstate override state.
 * Gets the value of a global renderstate override state.
 * @param renderState The global renderstate to get.
 * @return Returns the current value of the global renderstate.
 */
MF_API uintp MFRenderer_GetRenderStateOverride(uint32 renderState);




// new renderer interface...

enum MFRenderLayerSortMode
{
	MFRL_SM_Unknown = -1,

	MFRL_SM_Default = 0,
	MFRL_SM_FrontToBack,
	MFRL_SM_BackToFront,

	MFRL_SM_Max,
	MFRL_SM_ForceInt = 0x7FFFFFFF,
};


struct MFRenderer;

struct MFRenderLayer;

MF_API MFRenderer* MFRenderer_Create(int numLayers, MFStateBlock *pGlobal, MFStateBlock *pOverride);
MF_API void MFRenderer_Destroy(MFRenderer *pRenderer);
MF_API MFRenderLayer* MFRenderer_GetLayer(MFRenderer *pRenderer, int layer);

MF_API void MFRenderer_BuildCommandBuffers(MFRenderer *pRenderer);
MF_API void MFRenderer_Kick(MFRenderer *pRenderer);

MF_API MFStateBlock* MFRenderer_SetGlobalStateBlock(MFRenderer *pRenderer, MFStateBlock *pGlobal);
MF_API MFStateBlock* MFRenderer_SetOverrideStateBlock(MFRenderer *pRenderer, MFStateBlock *pOverride);

MF_API void MFRenderer_CloneLayer(MFRenderer *pRenderer, int sourceLayer, int destLayer);

MF_API void MFRenderLayer_SetLayerStateBlock(MFRenderLayer *pLayer, MFStateBlock *pState);
MF_API void MFRenderLayer_SetLayerSortMode(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode);
//MF_API void MFRenderLayer_SetLayerRenderTarget(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode);

//MF_API void MFRenderLayer_Clear(MFRenderLayer *pLayer, MFRenderClearFlags clearFlags = MFRCF_All, const MFVector &colour = MFVector::zero, float z = 1.f, int stencil = 0);

MF_API void MFRenderLayer_AddModel(MFRenderLayer *pLayer, MFModel *pModel, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride);
MF_API void MFRenderLayer_AddVertices(MFRenderLayer *pLayer, MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride);
MF_API void MFRenderLayer_AddIndexedVertices(MFRenderLayer *pLayer, MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, MFStateBlock *pEntity, MFStateBlock *pMaterialOverride);

MF_API void MFRenderLayer_AddFence(MFRenderLayer *pLayer);


// helpers...
__forceinline MFRenderer* MFRenderer_SetLayerStateBlock(MFRenderer *pRenderer, int layer, MFStateBlock *pState)
{
	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, layer);
	MFRenderLayer_SetLayerStateBlock(pLayer, pState);
}

#endif // _MFRENDERER_H

/** @} */
