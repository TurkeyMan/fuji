module fuji.vertex;

import fuji.vector;
import fuji.renderstate;

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

enum MFVertexDataFormat
{
	Unknown = -1,

	Auto = 0,

	Float4,
	Float3,
	Float2,
	Float1,
	UByte4_RGBA,
	UByte4N_RGBA,
	UByte4N_BGRA,
	SShort4,
	SShort2,
	SShort4N,
	SShort2N,
	UShort4,
	UShort2,
	UShort4N,
	UShort2N,
	Float16_4,
	Float16_2,
	UDec3,
	Dec3N
};

enum MFVertexBufferType
{
	Static,
	Dynamic,
	Scratch
};

enum MFVertexElementType
{
	Position,
	Normal,
	Colour,
	TexCoord,
	Binormal,
	Tangent,
	Indices,
	Weights
}

enum MFPrimType
{
	Points,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	TriangleFan,
	QuadList
}

struct MFVertexElement
{
	int stream;
	MFVertexElementType elementType;
	int elementIndex;
	int componentCount;
	MFVertexDataFormat format;
}

struct MFMesh
{
	MFStateBlock *pMeshState;
	MFPrimType primType;
	int vertexOffset;
	int numVertices;
	int indexOffset;
	int numIndices;
};

extern (C) MFVertexDeclaration* MFVertex_CreateVertexDeclaration(MFVertexElement* pElementArray, int elementCount);
extern (C) int MFVertex_ReleaseVertexDeclaration(MFVertexDeclaration* pDeclaration);
extern (C) const(MFVertexDeclaration)* MFVertex_GetStreamDeclaration(const(MFVertexDeclaration)* pDeclaration, int stream) pure;

extern (C) MFVertexBuffer* MFVertex_CreateVertexBuffer(MFVertexDeclaration* pVertexFormat, int numVerts, MFVertexBufferType type, void* pVertexBufferMemory = null, const(char)* pName = null);
extern (C) int MFVertex_ReleaseVertexBuffer(MFVertexBuffer* pVertexBuffer);
extern (C) void MFVertex_LockVertexBuffer(MFVertexBuffer* pVertexBuffer);
extern (C) void MFVertex_CopyVertexData(MFVertexBuffer* pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const(void)* pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices);
extern (C) void MFVertex_SetVertexData4v(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, ref const(MFVector) data);
extern (C) void MFVertex_ReadVertexData4v(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, MFVector* pData);
extern (C) void MFVertex_SetVertexData4ub(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, uint data);
extern (C) void MFVertex_ReadVertexData4ub(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, uint* pData);
extern (C) void MFVertex_UnlockVertexBuffer(MFVertexBuffer* pVertexBuffer);

extern (C) MFIndexBuffer* MFVertex_CreateIndexBuffer(int numIndices, ushort* pIndexBufferMemory = null, const(char)* pName = null);
extern (C) int MFVertex_ReleaseIndexBuffer(MFIndexBuffer* pIndexBuffer);
extern (C) void MFVertex_LockIndexBuffer(MFIndexBuffer* pIndexBuffer, ushort** ppIndices);
extern (C) void MFVertex_UnlockIndexBuffer(MFIndexBuffer* pIndexBuffer);

extern (C) void MFVertex_SetVertexDeclaration(const(MFVertexDeclaration)* pVertexDeclaration);
extern (C) void MFVertex_SetVertexStreamSource(int stream, const(MFVertexBuffer)* pVertexBuffer);
extern (C) void MFVertex_SetIndexBuffer(const(MFIndexBuffer)* pIndexBuffer);
extern (C) void MFVertex_RenderVertices(MFPrimType primType, int firstVertex, int numVertices);
extern (C) void MFVertex_RenderIndexedVertices(MFPrimType primType, int vertexOffset, int indexOffset, int numVertices, int numIndices);

