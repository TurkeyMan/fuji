#if !defined(_F3D_H)
#define _F3D_H

#include "Array.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

enum F3DChunkType
{
	CT_Material,
	CT_Skeleton,
	CT_Mesh,
	CT_ReferencePoint,
	CT_Animation,
	CT_Image,
	CT_Cloth
};

struct F3DHeader
{
	uint32	ID;
	uint8	major;
	uint8	minor;

	uint16	chunkCount;

	uint32	res[2];
};

struct F3DChunkDesc
{
	uint16 chunkType;
	uint16 elementCount;
	uint16 elementSize;
	uint16 res1;
	uint32 pOffset;
	uint32 res2;
};

class F3DVertex
{
public:
	F3DVertex();

	uint32 position;
	uint32 normal;
	uint32 biNormal;
	uint32 tangent;
	uint32 uv1, uv2, uv3, uv4, uv5, uv6, uv7, uv8;
	uint32 colour;
	uint32 illum;
	uint32 bone[4];
	float weight[4];
};

class F3DTriangle
{
public:
	int v[3];
	int reserved;
	Vector3 normal;
};

struct F3DMatSub
{
	int materialIndex;

	uint32 vertexCount;
	uint32 pVertexOffset;
	uint32 triangleCount;
	uint32 pTriangleOffset;
};

struct F3DMesh
{
	char name[64];

	uint32 materialIndex;
	uint32 size;

	uint32 matSubCount;
	uint32 pMatSubOffset;

	uint32 positionCount;
	uint32 pPositionOffset;
	uint32 texCount;
	uint32 pTexOffset;
	uint32 colourCount;
	uint32 pColourOffset;
	uint32 normalCount;
	uint32 pNormalOffset;
	uint32 biNormalCount;
	uint32 pBinormalOffset;
	uint32 tangentCount;
	uint32 pTangentOffset;
	uint32 illumCount;
	uint32 pIllumOffset;
};

class F3DMaterialSubobject
{
public:
	int materialIndex;

	Array<F3DTriangle> triangles;
	Array<F3DVertex> vertices;
};

class F3DSubObject
{
public:
	F3DSubObject();

	char name[64];

	int materialIndex;

	Array<F3DMaterialSubobject> matSubobjects;

	Array<Vector3> positions;
	Array<Vector3> normals;
	Array<Vector3> biNormals;
	Array<Vector3> tangents;
	Array<Vector3> uvs;
	Array<Vector4> colours;
	Array<Vector3> illumination;
};

class F3DMeshChunk
{
public:
	Array<F3DSubObject> subObjects;
};

class F3DBone
{
public:
	F3DBone();

	Matrix worldMatrix;
	Matrix localMatrix;
	char name[64];
	char parentName[64];

	char options[1024];
};

class F3DSkeletonChunk
{
public:
	Array<F3DBone> bones;
};

class F3DAnimationChunk
{
public:
	Array<Array<Matrix> > keyframes;
};

class F3DMaterial
{
public:
	Vector4 diffuse;
	Vector4 ambient;
	Vector4 emissive;
	Vector4 specular;
	float specularLevel;
	float glossiness;

	char name[64];
	char maps[8][64];
};

class F3DMaterialChunk
{
public:
	Array<F3DMaterial> materials;
};

class F3DRefPoint
{
public:
	Matrix worldMatrix;
	Matrix localMatrix;
	uint16 bone[4];
	float weight[4];
	char name[64];
	char options[1024];
};

class F3DRefPointChunk
{
public:
	Array<F3DRefPoint> refPoints;
};

class F3DFile
{
public:
	int ReadFromDisk(char *pFilename);
	void WriteToDisk(char *pFilename);
	void ReadFromMemory(char *pMemory);

	int ReadASE(char *pFilename);
	void WriteMDL(char *pFilename, int system);

	void Optimise();
	void ProcessSkeletonData();

	F3DMeshChunk *GetMeshChunk() { return &meshChunk; }
	F3DSkeletonChunk *GetSkeletonChunk() { return &skeletonChunk; }
	F3DMaterialChunk *GetMaterialChunk() { return &materialChunk; }
	F3DRefPointChunk *GetRefPointChunk() { return &refPointChunk; }
	F3DAnimationChunk *GetAnimationChunk() { return &animationChunk; }

	char name[256];

	F3DMeshChunk meshChunk;
	F3DSkeletonChunk skeletonChunk;
	F3DMaterialChunk materialChunk;
	F3DRefPointChunk refPointChunk;
	F3DAnimationChunk animationChunk;

protected:
	void ImportMesh(F3DMesh *pMsh, char *pBase);

	void ExportMesh(char* &pBuffer, char *pBase);
	void ExportSkeleton(char* &pData, char *pBase);
	void ExportMaterial(char* &pData, char *pBase);
};

#endif