#if !defined(_MFMODEL_INTERNAL_H)
#define _MFMODEL_INTERNAL_H

#include "MFBoundingVolume.h"
#include "MFModel.h"
#include "MFMaterial_Internal.h"

struct MFModelTemplate;

void MFModel_InitModule();
void MFModel_DeinitModule();

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk);
void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk);
void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, uint32 base, bool load);

enum MFModelDataChunkType
{
	MFChunkType_Unknown = -1,

	MFChunkType_SubObjects = 0,
	MFChunkType_Bones,
	MFChunkType_Collision,
	MFChunkType_Tags,
	MFChunkType_Materials,
	MFChunkType_BinaryFilesystem,

	MFChunkType_Max,
	MFChunkType_ForceInt = 0x7FFFFFFF
};

struct MFModelDataChunk
{
	MFModelDataChunkType chunkType;
	int count;
	void *pData;
	uint32 reserved;
};

MFModelDataChunk *MFModel_GetDataChunk(MFModelTemplate *pModelTemplate, MFModelDataChunkType chunkID);
MFMeshChunk* MFModel_GetMeshChunkInternal(MFModelTemplate *pModelTemplate, int subobjectIndex, int meshChunkIndex);

struct MFModel
{
	MFMatrix worldMatrix;
	MFVector modelColour;

	// subobject flags
	// * enabled flags
	// * subobject colours

	MFModelTemplate *pTemplate;
};

struct MFModelTemplate
{
	union
	{
		uint32 IDtag;
		const char *pFilename;
	};

	const char *pName;

	MFModelDataChunk *pDataChunks;
	int numDataChunks;

	MFBoundingVolume boundingVolume;

	int refCount;
};

struct SubObjectChunk
{
	const char *pSubObjectName;
//	MFMaterial *pMaterial;

	MFMeshChunk *pMeshChunks;
	int numMeshChunks;
};

struct BoneChunk
{
	MFMatrix boneMatrix;
	MFMatrix worldMatrix;
	const char *pBoneName;
	const char *pParentName;
	int parent;
	uint32 reserved;
};

struct TagChunk
{
	MFMatrix tagMatrix;
	const char *pTagName;

	uint32 reserved[3];
};

struct BinaryFilesystemChunk
{
	// this is a binary archive that can be mounted and accessed with the memory filesystem..
	// it could contain texture data, or other relevant model related data..
};


#endif
