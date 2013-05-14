#if !defined(_MFMESH_INTERNAL_H)
#define _MFMESH_INTERNAL_H

#include "MFVertex.h"

struct MFMaterial;
struct MFStateBlock;

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

struct MFMeshChunk
{
	MFMeshChunkType type;

	MFMaterial *pMaterial;

	MFStateBlock *pGeomState;

	MFVertexDeclaration *pDecl;
	MFVertexBuffer *pVertexBuffers[8];
	MFIndexBuffer *pIndexBuffer;

	int numVertices;
	int numIndices;

	// number of blend weights present in the mesh
	int maxBlendWeights;

	// matrix batching data
	int matrixBatchSize;
	uint16 *pBatchIndices;
};

struct MFMeshChunk_Generic : public MFMeshChunk
{
	// vertex format description
	MFVertexElement *pElements;

	void **ppVertexStreams;
	uint16 *pIndexData;

	int elementCount;
	int numVertexStreams;

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
