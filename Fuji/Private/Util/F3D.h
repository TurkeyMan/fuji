#if !defined(_F3D_H)
#define _F3D_H

#include "MFArray.h"
#include "MFVector.h"
#include "MFMatrix.h"

enum F3DChunkType
{
	CT_Material,
	CT_Mesh,
	CT_Skeleton,
	CT_ReferencePoint,
	CT_Collision,
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

	bool operator==(const F3DVertex &v)
	{
		return position == v.position &&
				uv1 == v.uv1 &&
				normal == v.normal &&
				colour == v.colour;
	}

	int position;
	int normal;
	int biNormal;
	int tangent;
	int uv1, uv2, uv3, uv4, uv5, uv6, uv7, uv8;
	int colour;
	int illum;
	int bone[4];
	float weight[4];
};

class F3DTriangle
{
public:
	int v[3];
	int reserved;
	MFVector normal;
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

//	uint32 materialIndex;
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
	F3DMaterialSubobject();

	int materialIndex;

	MFArray<F3DTriangle> triangles;
	MFArray<F3DVertex> vertices;
};

class F3DSubObject
{
public:
	F3DSubObject();

	char name[64];

//	int materialIndex;

	MFArray<F3DMaterialSubobject> matSubobjects;

	MFArray<MFVector> positions;
	MFArray<MFVector> normals;
	MFArray<MFVector> biNormals;
	MFArray<MFVector> tangents;
	MFArray<MFVector> uvs;
	MFArray<MFVector> colours;
	MFArray<MFVector> illumination;

	bool dontExportThisSubobject;
};

class F3DMeshChunk
{
public:
	MFArray<F3DSubObject> subObjects;
};

class F3DBone
{
public:
	F3DBone();

	MFMatrix boneMatrix;
	MFMatrix worldMatrix;
	char name[64];
	char parentName[64];

	char options[1024];
};

class F3DSkeletonChunk
{
public:
	MFArray<F3DBone> bones;
};

class F3DAnimationChunk
{
public:
	MFArray<MFArray<MFMatrix> > keyframes;
};


class F3DCollisionObject
{
public:
	MFVector boundMin;
	MFVector boundMax;
	MFVector boundSphere;

	uint32 objectType;

	char name[64];
};

class F3DCollisionSphere : public F3DCollisionObject
{
public:
	MFVector sphere;
};

struct F3DCollisionTri
{
	MFVector plane;
	MFVector boundPlanes[3];
	MFVector point[3];

	int adjacent[3];
	uint32 flags;
};

class F3DCollisionMesh : public F3DCollisionObject
{
public:
	MFArray<F3DCollisionTri> tris;
};

class F3DCollisionChunk
{
public:
	~F3DCollisionChunk();

	MFArray<F3DCollisionObject*> collisionObjects;
};

class F3DMaterial
{
public:
	F3DMaterial();

	MFVector diffuse;
	MFVector ambient;
	MFVector emissive;
	MFVector specular;
	float specularLevel;
	float glossiness;

	char name[64];
	char maps[8][64];
};

class F3DMaterialChunk
{
public:
	int GetMaterialIndexByName(const char *pName);

	MFArray<F3DMaterial> materials;
};

class F3DRefPoint
{
public:
	F3DRefPoint();

	MFMatrix worldMatrix;
	MFMatrix localMatrix;
	uint16 bone[4];
	float weight[4];
	char name[64];
	char options[1024];
};

class F3DRefPointChunk
{
public:
	MFArray<F3DRefPoint> refPoints;
};


class F3DOptions
{
public:
	F3DOptions();

	bool noCollision;
	bool noAnimation;
	bool dontDeleteCollisionSubobjects;
};

class F3DFile
{
public:
	int ReadF3D(char *pFilename);
	int ReadF3DFromMemory(char *pMemory);
	int ReadASE(char *pFilename);
	int ReadOBJ(const char *pFilename);
	int ReadDAE(char *pFilename);
	int ReadMD2(char *pFilename);
	int ReadMD3(char *pFilename);
	int ReadMEMD2(char *pFilename);

	void WriteF3D(char *pFilename);
	void WriteMDL(char *pFilename, MFPlatform platform);

	void ProcessSkeletonData();
	void ProcessCollisionData();
	void Optimise();
	void StripModel();

	F3DMeshChunk *GetMeshChunk() { return &meshChunk; }
	F3DSkeletonChunk *GetSkeletonChunk() { return &skeletonChunk; }
	F3DMaterialChunk *GetMaterialChunk() { return &materialChunk; }
	F3DRefPointChunk *GetRefPointChunk() { return &refPointChunk; }
	F3DAnimationChunk *GetAnimationChunk() { return &animationChunk; }
	F3DCollisionChunk *GetCollisionChunk() { return &collisionChunk; }

	char name[256];
	char author[256];
	char authoringTool[256];
	char copyrightString[256];

	F3DOptions options;

	F3DMeshChunk meshChunk;
	F3DSkeletonChunk skeletonChunk;
	F3DMaterialChunk materialChunk;
	F3DRefPointChunk refPointChunk;
	F3DAnimationChunk animationChunk;
	F3DCollisionChunk collisionChunk;

protected:
	void ImportMesh(F3DMesh *pMsh, char *pBase);

	void ExportMesh(char* &pBuffer, char *pBase);
	void ExportSkeleton(char* &pData, char *pBase);
	void ExportMaterial(char* &pData, char *pBase);
};

#endif
