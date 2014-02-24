/**
 * @file MFVertex.h
 * @brief Provides low level access to vertex and index buffers.
 * @author Manu Evans
 * @defgroup MFVertex Vertex data access
 * @{
 */

#if !defined(_MFVERTEX_H)
#define _MFVERTEX_H

struct MFStateBlock;
struct MFEffectTechnique;

/**
 * @struct MFVertexDeclaration
 * Represents the layout of a vertex buffer.
 */
struct MFVertexDeclaration;

/**
 * @struct MFVertexBuffer
 * Represents a Fuji vertex buffer.
 */
struct MFVertexBuffer;

/**
 * @struct MFIndexBuffer
 * Represents a Fuji index buffer.
 */
struct MFIndexBuffer;

/**
 * Vertex data format.
 * Vertex data format identifiers.
 */
enum MFVertexDataFormat
{
	MFVDF_Unknown = -1,			/**< Unknown vertex data format. */

	MFVDF_Auto = 0,				/**< Choose format automatically. */

	MFVDF_Float4,				/**< A 4 dimensional float vector. */
	MFVDF_Float3,				/**< A 3 dimensional float vector. */
	MFVDF_Float2,				/**< A 2 dimensional float vector. */
	MFVDF_Float1,				/**< A single float. */
	MFVDF_UByte4_RGBA,			/**< 4 non-normalised unsigned bytes in RGBA order. */
	MFVDF_UByte4N_RGBA,			/**< 4 normalised unsigned bytes in RGBA order. */
	MFVDF_UByte4N_BGRA,			/**< 4 normalised unsigned bytes in BGRA order. */
	MFVDF_SShort4,				/**< 4 signed shorts. */
	MFVDF_SShort2,				/**< 2 signed shorts. */
	MFVDF_SShort4N,				/**< 4 normalised signed shorts. */
	MFVDF_SShort2N,				/**< 2 normalised signed shorts. */
	MFVDF_UShort4,				/**< 4 unsigned shorts. */
	MFVDF_UShort2,				/**< 2 unsigned shorts. */
	MFVDF_UShort4N,				/**< 4 normalised unsigned shorts. */
	MFVDF_UShort2N,				/**< 2 normalised unsigned shorts. */
	MFVDF_Float16_4,			/**< 4 16bit 'half' floats. */
	MFVDF_Float16_2,			/**< 2 16bit 'half' floats. */
	MFVDF_UDec3,				/**< 3d vector of unsigned 10-bit integers. */
	MFVDF_Dec3N,				/**< 3d vector of normalised signed 10-bit integers. */

	MFVDF_Max,					/**< Maximum vertex data format. */
	MFVDF_ForceInt = 0x7FFFFFFF	/**< Force MFVertexDataFormat to an int type. */
};

/**
 * Vertex buffer type.
 * Vertex buffer type.
 */
enum MFVertexBufferType
{
	MFVBType_Static,				/**< A static vertex buffer is created and never changes. */
	MFVBType_Dynamic,				/**< A dynamic buffer may be locked and updated. */
	MFVBType_Scratch,				/**< A scratch buffer is for single frame use, it is destroyed and invalidated at the end of the frame. */

	MFVBType_Max,					/**< Maximum vertex data format. */
	MFVBType_ForceInt = 0x7FFFFFFF	/**< Force MFVertexBufferType to an int type. */
};

/**
 * Vertex element type.
 * Vertex element type.
 */
enum MFVertexElementType
{
	MFVET_Position,				/**< Position element. */
	MFVET_Normal,				/**< Normal element. */
	MFVET_Colour,				/**< Colour element. */
	MFVET_TexCoord,				/**< Texture coordinate element. */
	MFVET_Binormal,				/**< Binormal element. */
	MFVET_Tangent,				/**< Tangent element. */
	MFVET_Indices,				/**< Matrix indices. */
	MFVET_Weights,				/**< Matrix weights. */

	MFVET_Max,					/**< Maximum vertex data format. */
	MFVET_ForceInt = 0x7FFFFFFF	/**< Force MFVertexElementType to an int type. */
};

/**
 * Primitive Types.
 * Primitive types that can be submitted to the renderer.
 */
enum MFPrimType
{
	MFPT_Points,				/**< Point list */
	MFPT_LineList,				/**< Line list */
	MFPT_LineStrip,				/**< Line strip */
	MFPT_TriangleList,			/**< Triangle list */
	MFPT_TriangleStrip,			/**< Triangle strip */
	MFPT_TriangleFan,			/**< Triangle fan */
	MFPT_QuadList,				/**< Quad list */

	MFPT_Max,					/**< Maximum prim type */
	MFPT_ForceInt = 0x7FFFFFFF	/**< Force MFPrimType to an int type */
};

/**
 * Describes a vertex element.
 * Describes an element of a vertex format.
 */
struct MFVertexElement
{
	int stream;					/**< Vertex stream index. */
	MFVertexElementType type;	/**< Vertex element type. */
	int index;					/**< Element index - for multiple elements of the same type. */
	int componentCount;			/**< Number of vector components. */
	MFVertexDataFormat format;	/**< Element data format. */
};

/**
 * Describe a renderable mesh.
 * Describes a renderable mesh.
 */
struct MFMesh
{
	MFStateBlock *pMeshState;	/**< A mesh stateblock that should contain at least a vertex declaration, vertex buffer(/s), and an index buffer if rendering indexed vertices. */
	MFPrimType primType;		/**< Type of primitives to render. */
	int vertexOffset;			/**< Offset to start of vertices. */
	int numVertices;			/**< Number of non-indexed vertices to render. Note: If numIndices is > 0, indexed vertices will be rendered, and this value is ignored. */
	int indexOffset;			/**< Offset of first index to render. */
	int numIndices;				/**< Number of indices to render, or 0 to render non-indexed. */
};

/**
 * Create a vertex declaration.
 * Creates a vertex declaration which represents the format of vertex data.
 * @param pElementArray Pointer to an array of vertex element descriptions.
 * @param elementCount Number of vertex elements in the array.
 * @return An MFVertexDeclaration representing the specified vertex format.
 */
MF_API MFVertexDeclaration *MFVertex_CreateVertexDeclaration(const MFVertexElement *pElementArray, int elementCount);

/**
 * Release a vertex declaration.
 * Release a reference to a vertex declaration.
 * @param pDeclaration Vertex declaration to release.
 * @return The new reference count of the vertex declaration.
 */
MF_API int MFVertex_ReleaseVertexDeclaration(MFVertexDeclaration *pDeclaration);

/**
 * Get the declaration for a vertex stream.
 * Gets the declaration for a single vertex stream.
 * @param pDeclaration A vertex declaration.
 * @param stream Vertex stream index.
 * @return A vertex declaration for the specified vertex stream.
 */
MF_API MFVertexDeclaration *MFVertex_GetStreamDeclaration(MFVertexDeclaration *pDeclaration, int stream);

/**
 * Create a vertex buffer.
 * Creates a new vertex buffer.
 * @param pVertexFormat Pointer to a vertex declaration that describes the buffer format.
 * @param numVerts Number of vertices to allocate.
 * @param type The vertex buffer type.
 * @param pVertexBufferMemory (Optional) Pointer to existing vertex data for the buffer.
 * @param pName (Optional) Name for the vertex buffer.
 * @return The created vertrex buffer.
 * @see MFVertex_ReleaseVertexBuffer()
 */
MF_API MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory = NULL, const char *pName = NULL);

/**
 * Release a vertex buffer.
 * Releases a reference to a vertex buffer.
 * @param pVertexBuffer Vertex buffer to release.
 * @return The new reference count of the vertex buffer.
 * @see MFVertex_CreateVertexBuffer()
 */
MF_API int MFVertex_ReleaseVertexBuffer(MFVertexBuffer *pVertexBuffer);

/**
 * Lock a vertex buffer.
 * Locks a vertex buffer for writing.
 * @param pVertexBuffer The vertex buffer to lock.
 * @param ppVertices Receives a pointer to the locked vertex buffer memory.
 * @return None.
 * @see MFVertex_UnlockVertexBuffer(), MFVertex_CreateVertexBuffer()
 */
MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer, void **ppVertices);

/**
 * Unlock a vertex buffer.
 * Unlocks a previously locked vertex buffer.
 * @param pVertexBuffer Vertex buffer to unlock.
 * @return None.
 * @see MFVertex_LockVertexBuffer()
 */
MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer);

/**
 * Copy a vertex element stream into a vertex buffer.
 * Copies a stream of data for a single vertex element into a locked vertex buffer.
 * @param pVertexBuffer Vertex buffer to write to.
 * @param targetElement Target element within the vertex buffer.
 * @param targetElementIndex Target element index.
 * @param pSourceData Pointer to the source data.
 * @param sourceDataFormat Format of the source data.
 * @param sourceDataStride Stride of the source data, in bytes.
 * @param numVertices Number of vertices to write.
 * @return None.
 * @see MFVertex_LockVertexBuffer()
 */
MF_API void MFVertex_CopyVertexData(MFVertexBuffer *pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const void *pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices);

// TODO:
MF_API void MFVertex_SetVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, const MFVector &data);
MF_API void MFVertex_ReadVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, MFVector *pData);
MF_API void MFVertex_SetVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 data);
MF_API void MFVertex_ReadVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 *pData);

/**
 * Create an index buffer.
 * Creates a new index buffer.
 * @param numIndices Number of indices to allocate.
 * @param pIndexBufferMemory (Optional) Pointer to existing index data for the buffer.
 * @param pName (Optional) Name for the index buffer.
 * @return The created index buffer.
 * @see MFVertex_ReleaseIndexBuffer()
 */
MF_API MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory = NULL, const char *pName = NULL);

/**
 * Release an index buffer.
 * Releases a reference to an index buffer.
 * @param pIndexBuffer Index buffer to release.
 * @return The new reference count of the index buffer.
 * @see MFVertex_CreateIndexBuffer()
 */
MF_API int MFVertex_ReleaseIndexBuffer(MFIndexBuffer *pIndexBuffer);

/**
 * Lock an index buffer.
 * Locks an index buffer for writing.
 * @param pIndexBuffer The index buffer to lock.
 * @param ppIndices Receives a pointer to the locked index buffer memory.
 * @return None.
 * @see MFVertex_UnlockIndexBuffer(), MFVertex_CreateIndexBuffer()
 */
MF_API void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices);

/**
 * Unlock an index buffer.
 * Unlocks a previously locked index buffer.
 * @param pIndexBuffer Index buffer to unlock.
 * @return None.
 * @see MFVertex_LockIndexBuffer()
 */
MF_API void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer);

MF_API void MFVertex_SetVertexDeclaration(const MFVertexDeclaration *pVertexDeclaration);
MF_API void MFVertex_SetVertexStreamSource(int stream, const MFVertexBuffer *pVertexBuffer);
MF_API void MFVertex_SetIndexBuffer(const MFIndexBuffer *pIndexBuffer);
MF_API void MFVertex_RenderVertices(MFEffectTechnique *pTechnique, MFPrimType primType, int firstVertex, int numVertices);
MF_API void MFVertex_RenderIndexedVertices(MFEffectTechnique *pTechnique, MFPrimType primType, int vertexOffset, int indexOffset, int numVertices, int numIndices);

#endif

/** @} */
