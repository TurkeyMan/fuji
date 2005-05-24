#if !defined(_MFMODEL_INTERNAL_H)
#define _MFMODEL_INTERNAL_H

#include "MFModel.h"
#include "Material_Internal.h"

struct MFModelTemplate;

void MFModel_InitModule();
void MFModel_DeinitModule();

enum DataChunkType
{
	CT_SubObjects,
	CT_Bones,
	CT_Tags,
	CT_Materials,
	CT_EmbeddedTextures,

	CT_Max,
	CT_ForceInt = 0x7FFFFFFF
};

struct DataChunk
{
	DataChunkType chunkType;
	int count;
	void *pData;
	uint32 reserved;
};

struct MFModel
{
	Matrix worldMatrix;
	Vector4 modelColour;

	// subobject flags
	// * enabled flags
	// * subobject colours

	MFModelTemplate *pTemplate;

	int refCount;
};

struct MFModelTemplate
{
	uint32 IDtag;

	char *pName;

	int numDataChunks;
	DataChunk *pDataChunks;
};

struct SubObjectChunk
{
	char *pSubObjectName;
	Material *pMaterial;

	int numMeshChunks;
	MFMeshChunk *pMeshChunk;
};

struct BoneChunk
{
	Vector3 boneOrigin;
	char *pBoneName;

	uint32 reserved[3];
};

struct MaterialsChunk
{
	char *pIniName;
	char *pIniBuffer;
};

struct ImageChunk
{
	char *pImageName;

	char *pImageData;
	int dataSize;

	int width, height;
	int format;
};


#endif