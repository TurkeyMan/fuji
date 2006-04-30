#if !defined(_MATERIAL_INTERNAL_H)
#define _MATERIAL_INTERNAL_H

#if defined(_WINDOWS)
#include <d3d9.h>
#endif

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

#if defined(_WINDOWS) || (defined(_FUJI_UTIL) && !defined(_LINUX))
struct MFMeshChunk_PC : public MFMeshChunk
{
	// interface pointers
	IDirect3DVertexBuffer9 *pVertexBuffer;
	IDirect3DIndexBuffer9 *pIndexBuffer;
	IDirect3DVertexDeclaration9 *pVertexDeclaration;

	// model data
	const char *pVertexData;
	uint32 vertexDataSize;

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
};
#endif

#if defined(_XBOX) || defined(_FUJI_UTIL)
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
	// not done yet
};
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
