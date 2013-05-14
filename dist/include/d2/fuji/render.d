module fuji.render;

public import fuji.fuji;
import fuji.matrix;
import fuji.texture;

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
	Unknown, /**< Unknown transform matrix */

	// these can be set directly
	WorldMatrix,		/**< World transform matrix */
	CameraMatrix,		/**< Camera transform matrix */
	ProjectionMatrix,	/**< Projection transform matrix */

	// these can not be set directly, only read
	ViewMatrix,					/**< View matrix (inverse camera matrix) */
	WorldViewMatrix,			/**< Concatenated World-View matrix */
	ViewProjectionMatrix,		/**< Concatenated View-Projection matrix */
	WorldViewProjectionMatrix	/**< Concatenated World-View-Projection matrix */
}

/**
* Primitive Types.
* Primitive types that can be submitted to the renderer.
*/
enum MFPrimType
{
	PointList,	/**< Point list */
	LineList,	/**< Line list */
	LineStrip,	/**< Line strip */
	TriList,	/**< Triangle list */
	TriStrip,	/**< Triangle strip */
	TriFan,	/**< Triangle fan */
	QuadList	/**< Quad list */
}

/**
* Render Flags.
* Render flags to control the way geometry is rendered.
*/
enum MFRenderFlags
{
	Prelit = 1,		/**< Vertices should not be lit */
	Untextured = 2	/**< Geometry should not be textured */
}

/**
* Vertex Components.
* Various Vertex Components.
*/
enum MFVertexComponent
{
	Invalid = -1,	/**< Invalid or unknown vertex component */

	Position = 0,	/**< Vertex position */
	Colour,			/**< Vertex colour */
	Normal,			/**< Vertex normal */
	TexCoord1,		/**< Vertex texture coordinate 1 */
	TexCoord2,		/**< Vertex texture coordinate 2 */

	Binormal,		/**< Vertex binormal */
	Tangent,		/**< Vertex tangent */

	Indices,		/**< Vertex bone indices */
	Weights			/**< Vertex blend normal */
}

// new from display...
enum MFClearScreenFlags
{
	Colour	= 1,
	ZBuffer	= 2,
	Stencil	= 4,

	All = Colour | ZBuffer | Stencil
}

extern (C) void MFRenderer_SetClearColour(float r, float g, float b, float a);
extern (C) void MFRenderer_ClearScreen(uint flags = MFClearScreenFlags.All);

extern (C) void MFRenderer_SetViewport(MFRect* pRect);
extern (C) void MFRenderer_ResetViewport();

extern (C) MFTexture* MFRenderer_GetDeviceRenderTarget();
extern (C) MFTexture* MFRenderer_GetDeviceDepthStencil();
extern (C) void MFRenderer_SetRenderTarget(MFTexture* pRenderTarget, MFTexture* pZTarget);
extern (C) void MFRenderer_SetDeviceRenderTarget();

extern (C) float MFRenderer_GetTexelCenterOffset();

/**
* Begin a render block.
* Configures the renderer to render geometry with the current settings.
* @return Remaining number of passes.
* @remarks MFRenderer_Begin() should be called in a while loop to perform multipass effects. The same geometry should be submitted for each renderer pass.
*/
extern (C) int MFRenderer_Begin(); // returns number of passes remaining..

/**
* Get a transform matrix.
* Gets a transform matrix.
* @param type The matrix type to retrieve.
* @return Returns a const reference to the specified matrix.
*/
extern (C) const(MFMatrix)* MFRenderer_GetMatrix(MFMatrixType type);

/**
* Set a transform matrix.
* Sets a geometry transform matrix.
* @param type The matrix type to set.
* @param matrix A matrix to set as the specified matrix type.
* @return None.
*/
extern (C) void MFRenderer_SetMatrix(MFMatrixType type, const ref MFMatrix matrix);

/**
* Set the animation matrices.
* Sets the current set of animation matrices.
* @param pMatrices Pointer to an array of matrices to be used for animation.
* @param numMatrices The number of matrices that \a pMatrices points to.
* @return None.
*/
extern (C) void MFRenderer_SetMatrices(const(MFMatrix)* pMatrices, int numMatrices);

/**
* Set the current animation bone batch.
* Sets the current animation bone batch.
* @param pBatch Pointer to the matrix batch.
* @param numBonesInBatch The number of bones in the batch.
* @return None.
*/
extern (C) void MFRenderer_SetBatch(const(ushort)* pBatch, int numBonesInBatch);

/**
* Flush the renderer settings to the hardware.
* Flushes the renderer settings to the hardware.
* @return None.
*/
extern (C) void MFRenderer_FlushSettings();

/**
* Render a mesh chunk.
* Submits a mesh chunk for rendering.
* @param pMeshChunk Pointer to the mesh chunk to be rendered.
* @return None.
*/
extern (C) void MFRenderer_RenderMeshChunk(MFMeshChunk* pMeshChunk);

/**
* Begin an immediate mode block.
* Begins an immediate mode block.
* @param primType Type of primitives to be rendered.
* @param renderFlags Flags to be passed to the renderer. Should be a combination of flags from the MFRenderFlags enumerated type.
* @return Ummmm? Some kind of error?
*/
extern (C) int MFRenderer_BeginImmediate(MFPrimType primType, uint renderFlags);

/**
* Begin submiting vertices to the renderer.
* Configures the renderer to begin submitting vertices to the hardware.
* @param numVertices Number of vertices to be submitted.
* @return Ummmm? Some kind of error?
*/
extern (C) int MFRenderer_RenderVertices(int numVertices);

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
extern (C) void MFRenderer_SetVertexData4f(MFVertexComponent vertexComponent, float x, float y, float z, float w);

/**
* Submit vertex data.
* Submits a piece of vertex data to the renderer.
* @param vertexComponent Value from the MFVertexComponent enumerated type specifying the vertex component that should be set.
* @param data Vector specifying data for the vertex component.
* @return None.
*/
extern (C) void MFRenderer_SetVertexData4v(MFVertexComponent vertexComponent, const ref MFVector data);

/**
* End submitting vertices.
* Ends submition of vertices.
* @return None.
*/
extern (C) void MFRenderer_EndVertices();

/**
* End immediate mode rendering.
* Ends immediate mode rendering.
* @return None.
*/
extern (C) void MFRenderer_EndImmediate();


enum MFRenderState
{
	MaterialOverride,
	NoZRead,
	NoZWrite,
	Untextured,
	PreLit,
	PrimType,

	ShowZBuffer,
	ShowOverDraw
}

/**
* Set a global render state override.
* Sets a global render state override.
* @param renderState The global renderstate to set.
* @param value Value to set.
* @return Returns the old value.
*/
extern (C) size_t MFRenderer_SetRenderStateOverride(uint renderState, size_t value);

/**
* Get the value of a global renderstate override state.
* Gets the value of a global renderstate override state.
* @param renderState The global renderstate to get.
* @return Returns the current value of the global renderstate.
*/
extern (C) size_t MFRenderer_GetRenderStateOverride(uint renderState);

