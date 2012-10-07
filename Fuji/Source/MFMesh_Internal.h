#if !defined(_MFMESH_INTERNAL_H)
#define _MFMESH_INTERNAL_H

#include "MFVertex.h"

struct MFMaterial;

enum MFMeshVertexDataType
{
	MFMVDT_Float1,
	MFMVDT_Float2,
	MFMVDT_Float3,
	MFMVDT_Float4,
	MFMVDT_ColourBGRA,
	MFMVDT_UByte4,
	MFMVDT_UByte4N,
	MFMVDT_Short2,
	MFMVDT_Short4,
	MFMVDT_Short2N,
	MFMVDT_Short4N,
	MFMVDT_UShort2N,
	MFMVDT_UShort4N,
	MFMVDT_UDec3,
	MFMVDT_Dec3N,
	MFMVDT_Float16_2,
	MFMVDT_Float16_4,

	MFMVDT_Max,
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


struct MFMeshVertexElement
{
	MFVertexElementType usage;
	int usageIndex;
	MFMeshVertexDataType type;
	int offset;
};

struct MFMeshVertexStream
{
	const char *pStreamName;
	MFMeshVertexElement *pElements;
	int numVertexElements;
	int streamStride;
};

struct MFMeshVertexFormat
{
	MFMeshVertexStream *pStreams;
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
	MFMeshVertexFormat *pVertexFormat;

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

	// this 32 bytes can be used to store pointers or handles to platform specific things at runtime...
	char runtimeData[32];
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
#if MF_RENDERER == MF_DRIVER_D3D9 || MF_RENDERER == MF_DRIVER_D3D11 || MF_RENDERER == MF_DRIVER_OPENGL || MF_RENDERER == MF_DRIVER_PLUGIN
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

#endif // _MFMESH_INTERNAL_H
