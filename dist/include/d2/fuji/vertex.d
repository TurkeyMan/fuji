module fuji.vertex;

import fuji.vector;

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

	Float4 = 0,
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
	Float16_2
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

enum MFVertexPrimType
{
	Points,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	TriangleFan
}

struct MFVertexElement
{
	int stream;
	MFVertexElementType elementType;
	int elementIndex;
	int componentCount;
}

extern (C) MFVertexDeclaration* MFVertex_CreateVertexDeclaration(MFVertexElement* pElementArray, int elementCount);
extern (C) void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration* pDeclaration);

extern (C) MFVertexBuffer* MFVertex_CreateVertexBuffer(MFVertexDeclaration* pVertexFormat, int numVerts, MFVertexBufferType type, void* pVertexBufferMemory = null);
extern (C) void MFVertex_DestroyVertexBuffer(MFVertexBuffer* pVertexBuffer);
extern (C) void MFVertex_LockVertexBuffer(MFVertexBuffer* pVertexBuffer);
extern (C) void MFVertex_CopyVertexData(MFVertexBuffer* pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const(void*) pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices);
extern (C) void MFVertex_SetVertexData4v(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, ref const(MFVector) data);
extern (C) void MFVertex_ReadVertexData4v(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, MFVector* pData);
extern (C) void MFVertex_SetVertexData4ub(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, uint data);
extern (C) void MFVertex_ReadVertexData4ub(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, uint* pData);
extern (C) void MFVertex_UnlockVertexBuffer(MFVertexBuffer* pVertexBuffer);

extern (C) MFIndexBuffer* MFVertex_CreateIndexBuffer(int numIndices, ushort* pIndexBufferMemory = null);
extern (C) void MFVertex_DestroyIndexBuffer(MFIndexBuffer* pIndexBuffer);
extern (C) void MFVertex_LockIndexBuffer(MFIndexBuffer* pIndexBuffer, ushort** ppIndices);
extern (C) void MFVertex_UnlockIndexBuffer(MFIndexBuffer* pIndexBuffer);

extern (C) void MFVertex_SetVertexDeclaration(MFVertexDeclaration* pVertexDeclaration);
extern (C) void MFVertex_SetVertexStreamSource(int stream, MFVertexBuffer* pVertexBuffer);
extern (C) void MFVertex_RenderVertices(MFVertexPrimType primType, int firstVertex, int numVertices);
extern (C) void MFVertex_RenderIndexedVertices(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer* pIndexBuffer);

