#if !defined(_MODEL_H)
#define _MODEL_H

#include "Display.h"
#include "SceneGraph.h"
#include "Matrix.h"

class Animation;
class Material;

enum CustomDataType
{
	CUST_VertexBuffer,
	CUST_Cloth
};

enum VertexFormat
{
	VF_Position = (1<<0),	// position

	VF_Colour	= (1<<1),	// diffuse colour
	VF_Illum	= (1<<3),	// illumination colour
	VF_Specular	= (1<<6),	// specular colour

	VF_Normal	= (1<<2),	// normal vectors
	VF_Tengent	= (1<<4),	// tangent vectors
	VF_BiNormal	= (1<<5),	// binormal vectors

	VF_TexMask	= (15<<12)	// bits 12, 13, 14 and 15: 0x0000F000 // number of tex coords
};

class MeshChunk;
class ModelData;

class Model : public Renderable
{
public:
	static Model* Create(char *pFilename);
	void Release();

	virtual void Draw(Matrix *pLocalToWorld = NULL);

	Matrix worldMatrix; 

	ModelData *pModelData;
	Animation *pAnimation;
};

class MeshChunk : public Renderable
{
public:
	virtual void Draw(Matrix *pLocalToWorld = NULL);

	uint16 vertexFormat;
	uint16 vertexSize;
	uint32 vertexCount;
	uint32 vertexOffset;

	uint32 indexCount;
	uint32 indexOffset;

	uint32 materialIndex;

#if defined(_WINDOWS)
	IDirect3DVertexBuffer9 *pVertexBuffer;
	IDirect3DIndexBuffer9 *pIndexBuffer;
#elif defined(_XBOX)
	IDirect3DVertexBuffer8 *pVertexBuffer;
	IDirect3DIndexBuffer8 *pIndexBuffer;
#else
	uint32 reserved[2];
#endif
};

class ModelData
{
public:
	uint32 IDtag;
	char *pName;

	uint16 meshChunkCount;
	uint16 materialCount;
	uint16 boneCount;
	uint16 customDataCount;
	uint32 flags;

	MeshChunk *pMeshChunks;

	struct MaterialData
	{
		char *pName;
		char *pMaterialDescription;
		Material *pMaterial;
		uint32 reserved;
	} *pMaterials;

	struct CustomData
	{
		uint16 customDataType;
		uint16 count;
		void *pData;
		uint32 res;
		uint32 res2;
	} *pCustomData;

	void FixUpPointers();
	void CollapsePointers();
};

extern PtrList<ModelData> gModelBank;

////////////////////////////////////////
// code beyond here is obsolete....
////////////////////////////////////////
/*
class Subobject
{
public:

};

class ModelData
{
public:
	uint32 IDtag;
	char *pName;

	uint16 subobjectCount;
	uint16 materialCount;
	uint16 boneCount;
	uint16 customDataCount;
	uint32 flags;

	struct MaterialData
	{
		char *pName;
		char *pMaterialDescription;
		Material *pMaterial;
		uint32 reserved;
	} *pMaterials;

	uint32 vertexCount;
	char *pVertexData;
	uint32 indexCount;
	char *pIndexData;

	struct Subobject
	{
		uint16 vertexFormat;
		uint16 vertexSize;
		uint32 vertexCount;
		uint32 vertexOffset;

		uint32 indexCount;
		uint32 indexOffset;

		uint32 materialIndex;

		uint32 reserved[2];
	} *pSubobjects;

	struct CustomData
	{
		uint16 customDataType;
		uint16 count;
		void *pData;
		uint32 res;
		uint32 res2;
	} *pCustomData;

	void FixUpPointers();
	void CollapsePointers();
};

class Model
{
public:
	static Model* Create(char *pFilename);

	void Draw();

	Matrix worldMatrix;

	ModelData *pModelData;
	Animation *pAnimation;
	Matrix *pAnimMatrices;

#if defined(_WINDOWS)
	IDirect3DVertexBuffer9 *pVertexBuffer;
	IDirect3DIndexBuffer9 *pIndexBuffer;
#endif
};
*/
#endif
