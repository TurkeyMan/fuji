#if !defined(_MESH_H)
#define _MESH_H

struct MFMaterial;

enum MFVertexElementType
{
	MFVET_Position,
	MFVET_Normal,
	MFVET_Colour,
	MFVET_TexCoord,
	MFVET_Binormal,
	MFVET_Tangent,
	MFVET_Indices,
	MFVET_Weights
};

enum MFVertexDataType
{
	MFVDT_Float1,
	MFVDT_Float2,
	MFVDT_Float3,
	MFVDT_Float4,
	MFVDT_ColourBGRA,
	MFVDT_UByte4,
	MFVDT_UByte4N,
	MFVDT_Short2,
	MFVDT_Short4,
	MFVDT_Short2N,
	MFVDT_Short4N,
	MFVDT_UShort2N,
	MFVDT_UShort4N,
	MFVDT_UDec3,
	MFVDT_Dec3N,
	MFVDT_Float16_2,
	MFVDT_Float16_4
};

enum MFMeshChunkType
{
	MFMCT_Generic,	// generic non-platform-specific mesh chunk data in little endian format
	MFMCT_PSP,
	MFMCT_PS2,
	MFMCT_PS3,
	MFMCT_XB,
	MFMCT_X360,
	MFMCT_GC
};


struct MFVertexElement
{
	MFVertexElementType usage;
	int usageIndex;
	MFVertexDataType type;
	int offset;
};

struct MFVertexStream
{
	const char *pStreamName;
	MFVertexElement *pElements;
	int numVertexElements;
	int streamStride;
};

struct MFVertexFormat
{
	MFVertexStream *pStreams;
	int numVertexStreams;
};


struct MFMeshChunk
{
	MFMeshChunkType type;
	MFMaterial *pMaterial;
};

struct MFMeshChunk_Generic : public MFMeshChunk
{
	// vertex format description
	MFVertexFormat *pVertexFormat;

	// vertex data pointers;
	void **ppVertexStreams;
	int numVertices;

	// index data
	const uint16 *pIndexData;
	int numIndices;

	// matrix batching data
	int matrixBatchSize;
	uint16 *pBatchIndices;

	// number of blend weights present in the mesh
	int maxBlendWeights;

	uint32 userData[8];	// this can be used to store pointers or handles to platform specific things at runtime...
};

struct MFMeshChunk_XB : public MFMeshChunk
{
	// some vertex buffer type thing...

	uint32 numVertices;
	uint32 numIndices;
	uint32 vertexStride;
};

struct MFMeshChunk_PSP : public MFMeshChunk
{
	const char *pVertexData;
	uint32 vertexDataSize;

	// matrix batching data
	int matrixBatchSize;
	uint16 *pBatchIndices;

	uint32 numVertices;
	uint32 vertexFormat;
	uint32 vertexStride;
	uint32 maxWeights;
};

struct MFMeshChunk_PS2 : public MFMeshChunk
{
	uint32 numVertices;

	const char *pDMAList;
	uint32 listSize;
};

struct MFMeshChunk_GC : public MFMeshChunk
{
	// GC can use indexed streams..
};

// define MFMeshChunk_Current
#if MF_RENDERER == MF_DRIVER_D3D9 || MF_RENDERER == MF_DRIVER_OPENGL || MF_RENDERER == MF_DRIVER_PLUGIN
typedef MFMeshChunk_Generic MFMeshChunk_Current;
#elif MF_RENDERER == MF_DRIVER_XBOX
typedef MFMeshChunk_XB MFMeshChunk_Current;
#elif MF_RENDERER == MF_DRIVER_PSP
typedef MFMeshChunk_PSP MFMeshChunk_Current;
#elif MF_RENDERER == MF_DRIVER_PS2
typedef MFMeshChunk_PS2 MFMeshChunk_Current;
#elif MF_RENDERER == MF_DRIVER_GC
typedef MFMeshChunk_GC MFMeshChunk_Current;
#else
#error Platform not yet supported...
#endif

void MFMesh_FixUpMeshChunkGeneric(MFMeshChunk *pMeshChunk, void *pBase, bool load);

#endif
