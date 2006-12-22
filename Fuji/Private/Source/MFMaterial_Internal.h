#if !defined(_MATERIAL_INTERNAL_H)
#define _MATERIAL_INTERNAL_H

#include "MFMaterial.h"
#include "MFVector.h"
#include "MFTexture.h"
#include "MFMatrix.h"

#include "DebugMenu_Internal.h"

struct MFMaterialType;

// material functions
void MFMaterial_InitModule();
void MFMaterial_DeinitModule();

void MFMaterial_Update();

MFMaterialType *MaterialInternal_GetMaterialType(const char *pTypeName);

// MFMaterial structure
struct MFMaterialType
{
	MFMaterialCallbacks materialCallbacks;
	char *pTypeName;
};

struct MFMaterial
{
	char *pName;

	MFMaterialType *pType;
	void *pInstanceData;

	int refCount;
};

struct MFMeshChunk
{
	MFMaterial *pMaterial;
};

#if defined(_WINDOWS) || (defined(_FUJI_UTIL) && !defined(_LINUX) && !defined(_OSX))

#include <d3d9.h>

struct MFMeshChunk_PC : public MFMeshChunk
{
	// interface pointers
	IDirect3DVertexBuffer9 *pVertexBuffer;
	IDirect3DVertexBuffer9 *pAnimBuffer;
	IDirect3DIndexBuffer9 *pIndexBuffer;
	IDirect3DVertexDeclaration9 *pVertexDeclaration;

	// model data
	const char *pVertexData;
	uint32 vertexDataSize;

	const char *pAnimData;
	uint32 animDataSize;

	const char *pIndexData;
	uint32 indexDataSize;

	// vertex format declaration
	D3DVERTEXELEMENT9 *pVertexElements;

	// matrix batching data
	int matrixBatchSize;
	uint16 *pBatchIndices;

	uint32 numVertices;
	uint32 numIndices;
	uint32 vertexStride;
	uint32 animVertexStride;
	uint32 maxWeights;
};
#endif

#if defined(_MFXBOX) || defined(_FUJI_UTIL)
struct MFMeshChunk_XB : public MFMeshChunk
{
	// some vertex buffer type thing...

	uint32 numVertices;
	uint32 numIndices;
	uint32 vertexStride;
};
#endif

#if defined(_PSP) || defined(_FUJI_UTIL)
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
#endif

#if defined(_PS2) || defined(_FUJI_UTIL)
struct MFMeshChunk_PS2 : public MFMeshChunk
{
	uint32 numVertices;

	const char *pDMAList;
	uint32 listSize;
};
#endif

#if defined(_LINUX) || defined(_OSX) || defined(_FUJI_UTIL)
struct MFMeshChunk_Linux : public MFMeshChunk
{
	uint32 numVertices;
	uint32 numIndices;

	const char *pVertexData;
	uint32 vertexDataSize;
	const char *pNormalData;
	uint32 normalDataSize;
	const char *pColourData;
	uint32 colourDataSize;
	const char *pUVData;
	uint32 uvDataSize;

	const char *pIndexData;
	uint32 indexDataSize;
};
#endif

#if defined(_GC) || defined(_FUJI_UTIL)
struct MFMeshChunk_GC : public MFMeshChunk
{
	// GC can use indexed streams..
};
#endif

// define MFMeshChunk_Current
#if defined(_WINDOWS)
typedef MFMeshChunk_PC MFMeshChunk_Current;
#elif defined(_MFXBOX)
typedef MFMeshChunk_XB MFMeshChunk_Current;
#elif defined(_PSP)
typedef MFMeshChunk_PSP MFMeshChunk_Current;
#elif defined(_PS2)
typedef MFMeshChunk_PS2 MFMeshChunk_Current;
#elif defined(_LINUX)
typedef MFMeshChunk_Linux MFMeshChunk_Current;
#elif defined(_GC)
typedef MFMeshChunk_GC MFMeshChunk_Current;
#else
#error Platform not yet supported...
#endif

// a debug menu material information display object
class MaterialBrowser : public MenuObject
{
public:
	MaterialBrowser();

	virtual void Draw();
	virtual void Update();

	virtual float ListDraw(bool selected, const MFVector &pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual MFVector GetDimensions(float maxWidth);

	int selection;
};

#endif
