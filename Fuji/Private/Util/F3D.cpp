#include "Fuji.h"
#include "MFArray.h"
#include "F3D.h"
#include "MFHeap.h"
#include "MFStringCache.h"
#include "MFString.h"
#include "MFCollision_Internal.h"

#include "MFModel_Internal.h"
#include "MFMaterial_Internal.h"

#if !defined(_LINUX) && !defined(_OSX)
#include <d3d9.h>
#endif

void AdjustBoundingSphere(const MFVector &point, MFVector *pSphere)
{
	// if point is outside bounding sphere
	MFVector diff = point - *pSphere;
	float mag = diff.Magnitude3();

	if(mag > pSphere->w)
	{
		// fit sphere to include point
		mag -= pSphere->w;
		mag *= 0.5f;
		diff.Normalise3();
		pSphere->Mad3(diff, mag, *pSphere);
		pSphere->w += mag;
	}
}

int F3DFile::ReadF3D(char *pFilename)
{
	FILE *infile;
	char *file = NULL;
	uint32 filesize = 0;

	infile = fopen(pFilename, "r");

	if(!infile)
	{
		printf("Failed to open F3D file '%s'\n", pFilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	file = (char*)MFHeap_Alloc(filesize);
	fread(file, 1, filesize, infile);

	fclose(infile);

	ReadF3DFromMemory(file);

	MFHeap_Free(file);

	return 0;
}

void F3DFile::ImportMesh(F3DMesh *pMesh, char *pBase)
{
	F3DSubObject &sub = meshChunk.subObjects.push();

	memcpy(sub.name, pMesh->name, 64);
//	sub.materialIndex = pMesh->materialIndex;

	F3DMatSub *pMatSubs = (F3DMatSub*)(pBase + pMesh->pMatSubOffset);

	sub.matSubobjects.resize(pMesh->matSubCount);
	for(uint32 a=0; a<pMesh->matSubCount; a++)
	{
		sub.matSubobjects[a].triangles.resize(pMatSubs[a].triangleCount);
		memcpy(sub.matSubobjects[a].triangles.getpointer(), pBase + pMatSubs[a].pTriangleOffset, sizeof(sub.matSubobjects[a].triangles[0])*pMatSubs[a].triangleCount);

		sub.matSubobjects[a].vertices.resize(pMatSubs[a].vertexCount);
		memcpy(sub.matSubobjects[a].vertices.getpointer(), pBase + pMatSubs[a].pVertexOffset, sizeof(sub.matSubobjects[a].vertices[0])*pMatSubs[a].vertexCount);
	}

	sub.positions.resize(pMesh->positionCount);
	memcpy(sub.positions.getpointer(), pBase + pMesh->pPositionOffset, sizeof(sub.positions[0])*pMesh->positionCount);

	sub.uvs.resize(pMesh->texCount);
	memcpy(sub.uvs.getpointer(), pBase + pMesh->pTexOffset, sizeof(sub.uvs[0])*pMesh->texCount);

	sub.normals.resize(pMesh->normalCount);
	memcpy(sub.normals.getpointer(), pBase + pMesh->pNormalOffset, sizeof(sub.normals[0])*pMesh->normalCount);

	sub.colours.resize(pMesh->colourCount);
	memcpy(sub.colours.getpointer(), pBase + pMesh->pColourOffset, sizeof(sub.colours[0])*pMesh->colourCount);

	sub.illumination.resize(pMesh->illumCount);
	memcpy(sub.illumination.getpointer(), pBase + pMesh->pIllumOffset, sizeof(sub.illumination[0])*pMesh->illumCount);
}

void F3DFile::ExportMesh(char* &pData, char *pBase)
{
	F3DMesh *pMesh = (F3DMesh*)pData;
	int size;

	for(int a=0; a<meshChunk.subObjects.size(); a++)
	{
		pMesh[a].size = sizeof(F3DMesh);
		pData += pMesh[a].size;
		memcpy(pMesh[a].name, meshChunk.subObjects[a].name, 64);
//		pMesh[a].materialIndex = meshChunk.subObjects[a].materialIndex;

		pMesh[a].matSubCount = meshChunk.subObjects[a].matSubobjects.size();
//		pMesh[a].triangleCount = meshChunk.subObjects[a].triangles.size();
//		pMesh[a].vertexCount = meshChunk.subObjects[a].vertices.size();
		pMesh[a].positionCount = meshChunk.subObjects[a].positions.size();
		pMesh[a].texCount = meshChunk.subObjects[a].uvs.size();
		pMesh[a].normalCount = meshChunk.subObjects[a].normals.size();
		pMesh[a].colourCount = meshChunk.subObjects[a].colours.size();
		pMesh[a].illumCount = meshChunk.subObjects[a].illumination.size();
	}

	for(int a=0; a<meshChunk.subObjects.size(); a++)
	{
		size = pMesh[a].matSubCount * sizeof(F3DMatSub);
		F3DMatSub *pMatSubs = (F3DMatSub*)pData;
		pData += size;

		for(uint32 b=0; b<pMesh[a].matSubCount; b++)
		{
			pMatSubs[b].materialIndex = meshChunk.subObjects[a].matSubobjects[b].materialIndex;

			pMatSubs[b].triangleCount = meshChunk.subObjects[a].matSubobjects[b].triangles.size();
			pMatSubs[b].vertexCount = meshChunk.subObjects[a].matSubobjects[b].vertices.size();

			size = pMatSubs[b].triangleCount * sizeof(meshChunk.subObjects[a].matSubobjects[b].triangles[0]);
			memcpy(pData, meshChunk.subObjects[a].matSubobjects[b].triangles.getpointer(), size);
			pMatSubs[b].pTriangleOffset = (uint32)(pData - pBase);
			pData += size;

			size = pMatSubs[b].vertexCount * sizeof(meshChunk.subObjects[a].matSubobjects[b].vertices[0]);
			memcpy(pData, meshChunk.subObjects[a].matSubobjects[b].vertices.getpointer(), size);
			pMatSubs[b].pVertexOffset = (uint32)(pData - pBase);
			pData += size;
		}

		size = pMesh[a].positionCount * sizeof(meshChunk.subObjects[a].positions[0]);
		memcpy(pData, meshChunk.subObjects[a].positions.getpointer(), size);
		pMesh[a].pPositionOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].texCount * sizeof(meshChunk.subObjects[a].uvs[0]);
		memcpy(pData, meshChunk.subObjects[a].uvs.getpointer(), size);
		pMesh[a].pTexOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].normalCount * sizeof(meshChunk.subObjects[a].normals[0]);
		memcpy(pData, meshChunk.subObjects[a].normals.getpointer(), size);
		pMesh[a].pNormalOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].colourCount * sizeof(meshChunk.subObjects[a].colours[0]);
		memcpy(pData, meshChunk.subObjects[a].colours.getpointer(), size);
		pMesh[a].pColourOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].illumCount * sizeof(meshChunk.subObjects[a].illumination[0]);
		memcpy(pData, meshChunk.subObjects[a].illumination.getpointer(), size);
		pMesh[a].pIllumOffset = (uint32)(pData - pBase);
		pData += size;
	}
}

void F3DFile::ExportSkeleton(char* &pData, char *pBase)
{
	int size = sizeof(F3DBone)*skeletonChunk.bones.size();
	memcpy(pData, skeletonChunk.bones.getpointer(), size);
	pData += size;
}

void F3DFile::ExportMaterial(char* &pData, char *pBase)
{
	int size = sizeof(F3DMaterial)*materialChunk.materials.size();
	memcpy(pData, materialChunk.materials.getpointer(), size);
	pData += size;
}

void F3DFile::WriteF3D(char *pFilename)
{
	char *pFile;
	char *pOffset;
	F3DHeader *pHeader;
	F3DChunkDesc *pChunks;

	FILE *file = fopen(pFilename, "wb");

	if(!file)
	{
		printf("Count open '%s' for writing.\n", pFilename);
		return;
	}

	pFile = (char*)MFHeap_Alloc(1024*1024*10);
	pHeader = (F3DHeader*)pFile;

	pHeader->ID = MFMAKEFOURCC('M','F','3','D');
	pHeader->major = 1;
	pHeader->minor = 0;
	pHeader->chunkCount = 0;
	pHeader->res[0] = 0;
	pHeader->res[1] = 0;

	pChunks = (F3DChunkDesc*)&pHeader[1];

	if(meshChunk.subObjects.size())
	{
		pChunks[pHeader->chunkCount].chunkType = CT_Mesh;
		pChunks[pHeader->chunkCount].elementCount = 0;
		pChunks[pHeader->chunkCount].elementSize = 0;
		pChunks[pHeader->chunkCount].res1 = 0;
		pChunks[pHeader->chunkCount].pOffset = 0;
		pChunks[pHeader->chunkCount].res2 = 0;

		pHeader->chunkCount++;
	}

	if(materialChunk.materials.size())
	{
		pChunks[pHeader->chunkCount].chunkType = CT_Material;
		pChunks[pHeader->chunkCount].elementCount = 0;
		pChunks[pHeader->chunkCount].elementSize = 0;
		pChunks[pHeader->chunkCount].res1 = 0;
		pChunks[pHeader->chunkCount].pOffset = 0;
		pChunks[pHeader->chunkCount].res2 = 0;

		pHeader->chunkCount++;
	}

	if(skeletonChunk.bones.size())
	{
		pChunks[pHeader->chunkCount].chunkType = CT_Skeleton;
		pChunks[pHeader->chunkCount].elementCount = 0;
		pChunks[pHeader->chunkCount].elementSize = 0;
		pChunks[pHeader->chunkCount].res1 = 0;
		pChunks[pHeader->chunkCount].pOffset = 0;
		pChunks[pHeader->chunkCount].res2 = 0;

		pHeader->chunkCount++;
	}

	if(refPointChunk.refPoints.size())
	{
		pChunks[pHeader->chunkCount].chunkType = CT_ReferencePoint;
		pChunks[pHeader->chunkCount].elementCount = 0;
		pChunks[pHeader->chunkCount].elementSize = 0;
		pChunks[pHeader->chunkCount].res1 = 0;
		pChunks[pHeader->chunkCount].pOffset = 0;
		pChunks[pHeader->chunkCount].res2 = 0;

		pHeader->chunkCount++;
	}

	if(animationChunk.keyframes.size())
	{
		pChunks[pHeader->chunkCount].chunkType = CT_Animation;
		pChunks[pHeader->chunkCount].elementCount = 0;
		pChunks[pHeader->chunkCount].elementSize = 0;
		pChunks[pHeader->chunkCount].res1 = 0;
		pChunks[pHeader->chunkCount].pOffset = 0;
		pChunks[pHeader->chunkCount].res2 = 0;

		pHeader->chunkCount++;
	}

	pOffset = (char*)&pChunks[pHeader->chunkCount];

	for(int a=0; a<pHeader->chunkCount; a++)
	{
		switch(pChunks[a].chunkType)
		{
			case CT_Mesh:
			{
				pChunks[a].pOffset = (uint32)(pOffset - pFile);
				pChunks[a].elementCount = (uint32)meshChunk.subObjects.size();
				pChunks[a].elementSize = sizeof(F3DMesh);
				ExportMesh(pOffset, pFile);
			}
			break;

			case CT_Skeleton:
			{
				pChunks[a].pOffset = (uint32)(pOffset - pFile);
				pChunks[a].elementCount = (uint32)skeletonChunk.bones.size();
				pChunks[a].elementSize = sizeof(F3DBone);
				ExportSkeleton(pOffset, pFile);
			}
			break;

			case CT_Material:
			{
				pChunks[a].pOffset = (uint32)(pOffset - pFile);
				pChunks[a].elementCount = (uint32)materialChunk.materials.size();
				pChunks[a].elementSize = sizeof(F3DMaterial);
				ExportMaterial(pOffset, pFile);
			}
			break;
		}
	}

	fwrite(pFile, 1, pOffset - pFile, file);

	fclose(file);

	MFHeap_Free(pFile);
}

int F3DFile::ReadF3DFromMemory(char *pMemory)
{
	F3DHeader *pHeader = (F3DHeader*)pMemory;
	F3DChunkDesc *pChunks;

	if(pHeader->ID != MFMAKEFOURCC('M','F','3','D'))
	{
		printf("Not an F3D file.\n");
		return 1;
	}

	printf("Reading F3D file version %d.%d\n", pHeader->major, pHeader->minor);

	pChunks = (F3DChunkDesc*)&pHeader[1];

	for(int a=0; a<pHeader->chunkCount; a++)
	{
		switch(pChunks[a].chunkType)
		{
			case CT_Mesh:
			{
				for(int b=0; b<pChunks[a].elementCount; b++)
				{
					F3DMesh *pMesh = (F3DMesh*)(pMemory + pChunks[a].pOffset + b*pChunks[a].elementSize);
					ImportMesh(pMesh, pMemory);
				}
			}
			break;

			case CT_Skeleton:
			{
				skeletonChunk.bones.resize(pChunks[a].elementCount);
				for(int b=0; b<pChunks[a].elementCount; b++)
				{
					memcpy(&skeletonChunk.bones[b], pMemory + pChunks[a].pOffset + b*pChunks[a].elementSize, pChunks[a].elementSize);
				}
			}
			break;

			case CT_Material:
			{
				materialChunk.materials.resize(pChunks[a].elementCount);
				for(int b=0; b<pChunks[a].elementCount; b++)
				{
					memcpy(&materialChunk.materials[b], pMemory + pChunks[a].pOffset + b*pChunks[a].elementSize, pChunks[a].elementSize);
				}
			}
			break;
		}
	}

	return 0;
}

struct Vec3
{
	float x,y,z;
};

struct FileVertex
{
	Vec3 pos;
	Vec3 normal;
	unsigned int colour;
	float u,v;
};

void WriteMeshChunk_PC(F3DFile *pModel, MFMeshChunk *pMeshChunks, const F3DSubObject &sub, char *&pOffset, MFStringCache *pStringCache, MFBoundingVolume *pVolume)
{
#if !defined(_LINUX)
	int numMeshChunks = sub.matSubobjects.size();
	int a, b;

	// increment size of MeshChunk_PC structure
	pOffset += MFALIGN16(sizeof(MFMeshChunk_PC)*numMeshChunks);

	MFMeshChunk_PC *pMeshChunk = (MFMeshChunk_PC*)pMeshChunks;

	// fill out mesh chunk, and build mesh...
	for(a=0; a<numMeshChunks; a++)
	{
		struct Vert
		{
			float pos[3];
			float normal[3];
			uint32 colour;
			float uv[2];
		};

		int numVertices = sub.matSubobjects[a].vertices.size();
		int numTriangles = sub.matSubobjects[a].triangles.size();
		int numIndices = numTriangles*3;

		pMeshChunk[a].numVertices = numVertices;
		pMeshChunk[a].numIndices = numIndices;
		pMeshChunk[a].vertexStride = sizeof(Vert);
		pMeshChunk[a].vertexDataSize = numVertices * pMeshChunk->vertexStride;
		pMeshChunk[a].indexDataSize = numIndices*sizeof(uint16);

		pMeshChunk[a].pMaterial = (MFMaterial*)MFStringCache_Add(pStringCache, pModel->GetMaterialChunk()->materials[sub.matSubobjects[a].materialIndex].name);

		// setup pointers
		pMeshChunk[a].pVertexElements = (D3DVERTEXELEMENT9*)pOffset;
		pOffset += MFALIGN16(sizeof(D3DVERTEXELEMENT9)*5);
		pMeshChunk[a].pVertexData = pOffset;
		pOffset += MFALIGN16(sizeof(Vert)*numVertices);
		pMeshChunk[a].pIndexData = pOffset;
		pOffset += MFALIGN16(sizeof(uint16)*numIndices);

		// write declaration
		pMeshChunk[a].pVertexElements[0].Stream = 0;
		pMeshChunk[a].pVertexElements[0].Offset = 0;
		pMeshChunk[a].pVertexElements[0].Type = D3DDECLTYPE_FLOAT3;
		pMeshChunk[a].pVertexElements[0].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[0].Usage = D3DDECLUSAGE_POSITION;
		pMeshChunk[a].pVertexElements[0].UsageIndex = 0;

		pMeshChunk[a].pVertexElements[1].Stream = 0;
		pMeshChunk[a].pVertexElements[1].Offset = 12;
		pMeshChunk[a].pVertexElements[1].Type = D3DDECLTYPE_FLOAT3;
		pMeshChunk[a].pVertexElements[1].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[1].Usage = D3DDECLUSAGE_NORMAL;
		pMeshChunk[a].pVertexElements[1].UsageIndex = 0;

		pMeshChunk[a].pVertexElements[2].Stream = 0;
		pMeshChunk[a].pVertexElements[2].Offset = 24;
		pMeshChunk[a].pVertexElements[2].Type = D3DDECLTYPE_D3DCOLOR;
		pMeshChunk[a].pVertexElements[2].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[2].Usage = D3DDECLUSAGE_COLOR;
		pMeshChunk[a].pVertexElements[2].UsageIndex = 0;

		pMeshChunk[a].pVertexElements[3].Stream = 0;
		pMeshChunk[a].pVertexElements[3].Offset = 28;
		pMeshChunk[a].pVertexElements[3].Type = D3DDECLTYPE_FLOAT2;
		pMeshChunk[a].pVertexElements[3].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[3].Usage = D3DDECLUSAGE_TEXCOORD;
		pMeshChunk[a].pVertexElements[3].UsageIndex = 0;

		D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
		pMeshChunk[a].pVertexElements[4] = endMacro;

		// write vertices
		Vert *pVert = (Vert*)pMeshChunk[a].pVertexData;

		for(b=0; b<numVertices; b++)
		{
			int posIndex = sub.matSubobjects[a].vertices[b].position;
			int normalIndex = sub.matSubobjects[a].vertices[b].normal;
			int uvIndex = sub.matSubobjects[a].vertices[b].uv1;
			int colourIndex = sub.matSubobjects[a].vertices[b].colour;

			const MFVector &pos = posIndex > -1 ? sub.positions[posIndex] : MFVector::zero;
			const MFVector &normal = normalIndex > -1 ? sub.normals[normalIndex] : MFVector::up;
			const MFVector &uv = uvIndex > -1 ? sub.uvs[uvIndex] : MFVector::zero;
			uint32 colour = colourIndex > -1 ? sub.colours[colourIndex].ToPackedColour() : 0xFFFFFFFF;

			pVert[b].pos[0] = pos.x;
			pVert[b].pos[1] = pos.y;
			pVert[b].pos[2] = pos.z;
			pVert[b].normal[0] = normal.x;
			pVert[b].normal[1] = normal.y;
			pVert[b].normal[2] = normal.z;
			pVert[b].colour = colour;
			pVert[b].uv[0] = uv.x;
			pVert[b].uv[1] = uv.y;

			pVolume->min = MFMin(pVolume->min, pos);
			pVolume->max = MFMax(pVolume->max, pos);
			AdjustBoundingSphere(pos, &pVolume->boundingSphere);
		}

		// write indices
		uint16 *pIndices = (uint16*)pMeshChunk[a].pIndexData;

		for(b=0; b<numTriangles; b++)
		{
			pIndices[0] = sub.matSubobjects[a].triangles[b].v[0];
			pIndices[1] = sub.matSubobjects[a].triangles[b].v[1];
			pIndices[2] = sub.matSubobjects[a].triangles[b].v[2];

			pIndices += 3;
		}
	}
#endif
}

void FixUpMeshChunk_PC(MFMeshChunk *pMeshChunks, int count, uint32 base, uint32 stringBase)
{
#if !defined(_LINUX)
	MFMeshChunk_PC *pMC = (MFMeshChunk_PC*)pMeshChunks;

	for(int a=0; a<count; a++)
	{
		pMC[a].pMaterial = (MFMaterial*)((char*)pMC[a].pMaterial - stringBase);

		pMC[a].pVertexData -= base;
		pMC[a].pIndexData -= base;
		(char*&)pMC[a].pVertexElements -= base;
	}
#endif
}

void WriteMeshChunk_XB(F3DFile *pModel, MFMeshChunk *pMeshChunks, const F3DSubObject &sub, char *&pOffset, MFStringCache *pStringCache, MFBoundingVolume *pVolume)
{
#if 0//!defined(_LINUX)
	int numMeshChunks = sub.matSubobjects.size();
	int a, b;

	// increment size of MeshChunk_PC structure
	pOffset += MFALIGN16(sizeof(MFMeshChunk_PC)*numMeshChunks);

	MFMeshChunk_XB *pMeshChunk = (MFMeshChunk_XB*)pMeshChunks;

	// fill out mesh chunk, and build mesh...
	for(a=0; a<numMeshChunks; a++)
	{
		struct Vert
		{
			float pos[3];
			float normal[3];
			uint32 colour;
			float uv[2];
		};

		int numVertices = sub.matSubobjects[a].vertices.size();
		int numTriangles = sub.matSubobjects[a].triangles.size();
		int numIndices = numTriangles*3;

		pMeshChunk[a].numVertices = numVertices;
		pMeshChunk[a].numIndices = numIndices;
		pMeshChunk[a].vertexStride = sizeof(Vert);
		pMeshChunk[a].vertexDataSize = numVertices * pMeshChunk->vertexStride;
		pMeshChunk[a].indexDataSize = numIndices*sizeof(uint16);

		pMeshChunk[a].pMaterial = (MFMaterial*)MFStringCache_Add(pStringCache, pModel->GetMaterialChunk()->materials[sub.matSubobjects[a].materialIndex].name);

		// setup pointers
		pMeshChunk[a].pVertexElements = (D3DVERTEXELEMENT9*)pOffset;
		pOffset += MFALIGN16(sizeof(D3DVERTEXELEMENT9)*5);
		pMeshChunk[a].pVertexData = pOffset;
		pOffset += MFALIGN16(sizeof(Vert)*numVertices);
		pMeshChunk[a].pIndexData = pOffset;
		pOffset += MFALIGN16(sizeof(uint16)*numIndices);

		// write declaration
		pMeshChunk[a].pVertexElements[0].Stream = 0;
		pMeshChunk[a].pVertexElements[0].Offset = 0;
		pMeshChunk[a].pVertexElements[0].Type = D3DDECLTYPE_FLOAT3;
		pMeshChunk[a].pVertexElements[0].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[0].Usage = D3DDECLUSAGE_POSITION;
		pMeshChunk[a].pVertexElements[0].UsageIndex = 0;

		pMeshChunk[a].pVertexElements[1].Stream = 0;
		pMeshChunk[a].pVertexElements[1].Offset = 12;
		pMeshChunk[a].pVertexElements[1].Type = D3DDECLTYPE_FLOAT3;
		pMeshChunk[a].pVertexElements[1].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[1].Usage = D3DDECLUSAGE_NORMAL;
		pMeshChunk[a].pVertexElements[1].UsageIndex = 0;

		pMeshChunk[a].pVertexElements[2].Stream = 0;
		pMeshChunk[a].pVertexElements[2].Offset = 24;
		pMeshChunk[a].pVertexElements[2].Type = D3DDECLTYPE_D3DCOLOR;
		pMeshChunk[a].pVertexElements[2].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[2].Usage = D3DDECLUSAGE_COLOR;
		pMeshChunk[a].pVertexElements[2].UsageIndex = 0;

		pMeshChunk[a].pVertexElements[3].Stream = 0;
		pMeshChunk[a].pVertexElements[3].Offset = 28;
		pMeshChunk[a].pVertexElements[3].Type = D3DDECLTYPE_FLOAT2;
		pMeshChunk[a].pVertexElements[3].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[a].pVertexElements[3].Usage = D3DDECLUSAGE_TEXCOORD;
		pMeshChunk[a].pVertexElements[3].UsageIndex = 0;

		D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
		pMeshChunk[a].pVertexElements[4] = endMacro;

		// write vertices
		Vert *pVert = (Vert*)pMeshChunk[a].pVertexData;

		for(b=0; b<numVertices; b++)
		{
			int posIndex = sub.matSubobjects[a].vertices[b].position;
			int normalIndex = sub.matSubobjects[a].vertices[b].normal;
			int uvIndex = sub.matSubobjects[a].vertices[b].uv1;
			int colourIndex = sub.matSubobjects[a].vertices[b].colour;

			const MFVector &pos = posIndex > -1 ? sub.positions[posIndex] : MFVector::zero;
			const MFVector &normal = normalIndex > -1 ? sub.normals[normalIndex] : MFVector::up;
			const MFVector &uv = uvIndex > -1 ? sub.uvs[uvIndex] : MFVector::zero;
			uint32 colour = colourIndex > -1 ? sub.colours[colourIndex].ToPackedColour() : 0xFFFFFFFF;

			pVert[b].pos[0] = pos.x;
			pVert[b].pos[1] = pos.y;
			pVert[b].pos[2] = pos.z;
			pVert[b].normal[0] = normal.x;
			pVert[b].normal[1] = normal.y;
			pVert[b].normal[2] = normal.z;
			pVert[b].colour = colour;
			pVert[b].uv[0] = uv.x;
			pVert[b].uv[1] = uv.y;

			pVolume->min = MFMin(pVolume->min, pos);
			pVolume->max = MFMax(pVolume->max, pos);
			AdjustBoundingSphere(pos, &pVolume->boundingSphere);
		}

		// write indices
		uint16 *pIndices = (uint16*)pMeshChunk[a].pIndexData;

		for(b=0; b<numTriangles; b++)
		{
			pIndices[0] = sub.matSubobjects[a].triangles[b].v[0];
			pIndices[1] = sub.matSubobjects[a].triangles[b].v[1];
			pIndices[2] = sub.matSubobjects[a].triangles[b].v[2];

			pIndices += 3;
		}
	}
#endif
}

void FixUpMeshChunk_XB(MFMeshChunk *pMeshChunks, int count, uint32 base, uint32 stringBase)
{
#if 0//!defined(_LINUX)
	MFMeshChunk_XB *pMC = (MFMeshChunk_XB*)pMeshChunks;

	for(int a=0; a<count; a++)
	{
		pMC[a].pMaterial = (MFMaterial*)((char*)pMC[a].pMaterial - stringBase);

		pMC[a].pVertexData -= base;
		pMC[a].pIndexData -= base;
		(char*&)pMC[a].pVertexElements -= base;
	}
#endif
}

void WriteMeshChunk_PSP(F3DFile *pModel, MFMeshChunk *pMeshChunks, const F3DSubObject &sub, char *&pOffset, MFStringCache *pStringCache, MFBoundingVolume *pVolume)
{
	int numMeshChunks = sub.matSubobjects.size();
	int a, b, c;

	// increment size of MeshChunk_PSP structure
	pOffset += MFALIGN16(sizeof(MFMeshChunk_PSP)*numMeshChunks);

	MFMeshChunk_PSP *pMeshChunk = (MFMeshChunk_PSP*)pMeshChunks;

	struct Vert
	{
		float uv[2];
		uint32 colour;
		float normal[3];
		float pos[3];
	};

	// fill out mesh chunk, and build mesh...
	for(a=0; a<numMeshChunks; a++)
	{
		const F3DMaterialSubobject &matsub = sub.matSubobjects[a];

		int numTriangles = matsub.triangles.size();

		pMeshChunk[a].numVertices = numTriangles * 3;
		pMeshChunk[a].vertexStride = sizeof(Vert);
		pMeshChunk[a].vertexDataSize = pMeshChunk[a].numVertices * pMeshChunk[a].vertexStride;
		pMeshChunk[a].vertexFormat = (0x03 << 0) | (0x07 << 2) | (0x03 << 5) | (0x03 << 7);

		pMeshChunk[a].pMaterial = (MFMaterial*)MFStringCache_Add(pStringCache, pModel->GetMaterialChunk()->materials[sub.matSubobjects[a].materialIndex].name);

		pMeshChunk[a].pVertexData = pOffset;
		pOffset += MFALIGN16(pMeshChunk[a].vertexDataSize);

		// write triangles
		Vert *pVert = (Vert*)pMeshChunk[a].pVertexData;

		for(b=0; b<numTriangles; b++)
		{
			for(c=0; c<3; c++)
			{
				const F3DVertex &vert = matsub.vertices[matsub.triangles[b].v[c]];

				const MFVector &pos = sub.positions[vert.position];
				const MFVector &uv = sub.uvs[vert.uv1];
				const MFVector &norm = sub.normals[vert.normal];
				const MFVector &col = sub.colours[vert.colour];

				pVert->uv[0] = uv.x;
				pVert->uv[1] = uv.y;
				pVert->colour = (uint32)(col.x * 255.0f) | ((uint32)(col.y * 255.0f) << 8) | ((uint32)(col.z * 255.0f) << 16) | ((uint32)(col.w * 255.0f) << 24);
				pVert->normal[0] = norm.x;
				pVert->normal[1] = norm.y;
				pVert->normal[2] = norm.z;
				pVert->pos[0] = pos.x;
				pVert->pos[1] = pos.y;
				pVert->pos[2] = pos.z;

				pVolume->min = MFMin(pVolume->min, pos);
				pVolume->max = MFMax(pVolume->max, pos);
				AdjustBoundingSphere(pos, &pVolume->boundingSphere);

				++pVert;
			}
		}
	}
}

void FixUpMeshChunk_PSP(MFMeshChunk *pMeshChunks, int count, uint32 base, uint32 stringBase)
{
	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunks;

	for(int a=0; a<count; a++)
	{
		pMC[a].pMaterial = (MFMaterial*)((char*)pMC[a].pMaterial - stringBase);
		pMC[a].pVertexData -= base;
	}
}

void WriteMeshChunk_Linux(F3DFile *pModel, MFMeshChunk *pMeshChunks, const F3DSubObject &sub, char *&pOffset, MFStringCache *pStringCache, MFBoundingVolume *pVolume)
{
	int numMeshChunks = sub.matSubobjects.size();
	int a, b, c;

	// increment size of MeshChunk_PSP structure
	pOffset += MFALIGN16(sizeof(MFMeshChunk_PSP)*numMeshChunks);

	MFMeshChunk_Linux *pMeshChunk = (MFMeshChunk_Linux*)pMeshChunks;

	// fill out mesh chunk, and build mesh...
	for(a=0; a<numMeshChunks; a++)
	{
		const F3DMaterialSubobject &matsub = sub.matSubobjects[a];

		int numTriangles = matsub.triangles.size();

		pMeshChunk[a].numVertices = matsub.vertices.size();
		pMeshChunk[a].numIndices = numTriangles * 3;
		pMeshChunk[a].indexDataSize = pMeshChunk[a].numIndices * sizeof(uint16);

		pMeshChunk[a].pMaterial = (MFMaterial*)MFStringCache_Add(pStringCache, pModel->GetMaterialChunk()->materials[sub.matSubobjects[a].materialIndex].name);

		// write verts
		struct V3 { float x, y, z; };
		struct V2 { float u, v; };

		pMeshChunk[a].pVertexData = pOffset;
		pMeshChunk[a].vertexDataSize = pMeshChunk[a].numVertices * sizeof(V3);
		pOffset += MFALIGN16(pMeshChunk[a].vertexDataSize);
		pMeshChunk[a].pNormalData = pOffset;
		pMeshChunk[a].normalDataSize = pMeshChunk[a].numVertices * sizeof(V3);
		pOffset += MFALIGN16(pMeshChunk[a].normalDataSize);
		pMeshChunk[a].pColourData = pOffset;
		pMeshChunk[a].colourDataSize = pMeshChunk[a].numVertices * sizeof(uint32);
		pOffset += MFALIGN16(pMeshChunk[a].colourDataSize);
		pMeshChunk[a].pUVData = pOffset;
		pMeshChunk[a].uvDataSize = pMeshChunk[a].numVertices * sizeof(V2);
		pOffset += MFALIGN16(pMeshChunk[a].uvDataSize);

		V3 *pVert = (V3*)pMeshChunk[a].pVertexData;
		V3 *pNorm = (V3*)pMeshChunk[a].pNormalData;
		uint32 *pCol = (uint32*)pMeshChunk[a].pColourData;
		V2 *pUV = (V2*)pMeshChunk[a].pUVData;

		for(b=0; b<(int)pMeshChunk[a].numVertices; b++)
		{
			const F3DVertex &vert = matsub.vertices[b];

			const MFVector &pos = sub.positions[vert.position];
			const MFVector &uv = sub.uvs[vert.uv1];
			const MFVector &norm = sub.normals[vert.normal];
			const MFVector &col = sub.colours[vert.colour];

			pVert->x = pos.x;
			pVert->y = pos.y;
			pVert->z = pos.z;
			pNorm->x = norm.x;
			pNorm->y = norm.y;
			pNorm->z = norm.z;
			*pCol = (uint32)(col.x * 255.0f) | ((uint32)(col.y * 255.0f) << 8) | ((uint32)(col.z * 255.0f) << 16) | ((uint32)(col.w * 255.0f) << 24);
			pUV->u = uv.x;
			pUV->v = uv.y;

			pVolume->min = MFMin(pVolume->min, pos);
			pVolume->max = MFMax(pVolume->max, pos);
			AdjustBoundingSphere(pos, &pVolume->boundingSphere);

			++pVert;
			++pNorm;
			++pCol;
			++pUV;
		}

		// write indices
		pMeshChunk[a].pIndexData = pOffset;
		pOffset += MFALIGN16(pMeshChunk[a].indexDataSize);

		uint16 *pIndex = (uint16*)pMeshChunk[a].pIndexData;

		for(b=0; b<numTriangles; b++)
		{
			for(c=0; c<3; c++)
			{
				*pIndex = (uint16)matsub.triangles[b].v[c];
				++pIndex;
			}
		}
	}
}

void FixUpMeshChunk_Linux(MFMeshChunk *pMeshChunks, int count, uint32 base, uint32 stringBase)
{
	MFMeshChunk_Linux *pMC = (MFMeshChunk_Linux*)pMeshChunks;

	for(int a=0; a<count; a++)
	{
		pMC[a].pMaterial = (MFMaterial*)((char*)pMC[a].pMaterial - stringBase);
		pMC[a].pVertexData -= base;
		pMC[a].pNormalData -= base;
		pMC[a].pColourData -= base;
		pMC[a].pUVData -= base;
		pMC[a].pIndexData -= base;
	}
}

void F3DFile::WriteMDL(char *pFilename, MFPlatform platform)
{
	int a, b;

	MFModelTemplate *pModelData;

	FILE *file = fopen(pFilename, "wb");

	if(!file)
	{
		printf("Count open '%s' for writing.\n", pFilename);
		return;
	}

	MFStringCache *pStringCache;
	pStringCache = MFStringCache_Create(1024*1024);

	char *pFile;
	char *pOffset;

	const int maxFileSize = 1024*1024*4;

	pFile = (char*)MFHeap_Alloc(maxFileSize); // allocating 10mb ... yeah this is REALLY weak! ;)
	memset(pFile, 0, maxFileSize);
	pModelData = (MFModelTemplate*)pFile;

	MFModelDataChunk *pDataHeaders = (MFModelDataChunk*)(pFile+MFALIGN16(sizeof(MFModelTemplate)));

	pModelData->IDtag = MFMAKEFOURCC('M','D','L','2');
	pModelData->pName = MFStringCache_Add(pStringCache, name);

	int numChunks = 0;
	int meshChunkIndex = -1;
	int skeletonChunkIndex = -1;
	int collisionChunkIndex = -1;
	int tagChunkIndex = -1;
	int dataChunkIndex = -1;

	int numOutputMeshChunks = 0;

	// calculate how many subobjects we're actually going to write...
	for(a=0; a<GetMeshChunk()->subObjects.size(); a++)
	{
		if(!GetMeshChunk()->subObjects[a].dontExportThisSubobject)
			++numOutputMeshChunks;
	}

	// figure out number of chunks somehow.....
	if(numOutputMeshChunks)
	{
		meshChunkIndex = numChunks++;
		pDataHeaders[meshChunkIndex].chunkType = MFChunkType_SubObjects;
	}

	if(GetSkeletonChunk()->bones.size())
	{
		skeletonChunkIndex = numChunks++;
		pDataHeaders[skeletonChunkIndex].chunkType = MFChunkType_Bones;
	}

	if(GetRefPointChunk()->refPoints.size())
	{
		tagChunkIndex = numChunks++;
		pDataHeaders[tagChunkIndex].chunkType = MFChunkType_Tags;
	}

	if(GetCollisionChunk()->collisionObjects.size())
	{
		collisionChunkIndex = numChunks++;
		pDataHeaders[collisionChunkIndex].chunkType = MFChunkType_Collision;
	}

	// then do something with them....
	pModelData->numDataChunks = numChunks;
	pModelData->pDataChunks = pDataHeaders;

	pOffset = (char*)pDataHeaders + MFALIGN16(sizeof(MFModelDataChunk)*numChunks);

	// write out mesh data
	if(meshChunkIndex > -1)
	{
		SubObjectChunk *pSubobjectChunk = (SubObjectChunk*)pOffset;

		pDataHeaders[meshChunkIndex].pData = pSubobjectChunk;
		pDataHeaders[meshChunkIndex].count = numOutputMeshChunks;

		// prime bounding volume
		const F3DSubObject &sub0 = GetMeshChunk()->subObjects[0];
		pModelData->boundingVolume.boundingSphere = MakeVector(sub0.positions[sub0.matSubobjects[0].vertices[sub0.matSubobjects[0].triangles[0].v[0]].position], 0.0f);
		pModelData->boundingVolume.min = pModelData->boundingVolume.boundingSphere;
		pModelData->boundingVolume.max = pModelData->boundingVolume.boundingSphere;

		pOffset += MFALIGN16(sizeof(SubObjectChunk)*pDataHeaders[meshChunkIndex].count);

		for(a=0, b=0; a<GetMeshChunk()->subObjects.size(); a++)
		{
			const F3DSubObject &sub = GetMeshChunk()->subObjects[a];

			if(sub.dontExportThisSubobject)
				continue;

			pSubobjectChunk[b].pSubObjectName = MFStringCache_Add(pStringCache, sub.name);
//			pSubobjectChunk[b].pMaterial = (MFMaterial*)pStringCache->Add(GetMaterialChunk()->materials[sub.materialIndex].name);
			pSubobjectChunk[b].numMeshChunks = sub.matSubobjects.size();

			MFMeshChunk *pMeshChunks = (MFMeshChunk*)pOffset;
			pSubobjectChunk[b].pMeshChunks = pMeshChunks;

			// build platform specific mesh chunk
			switch(platform)
			{
				case FP_PC:
					WriteMeshChunk_PC(this, pMeshChunks, sub, pOffset, pStringCache, &pModelData->boundingVolume);
					break;
				case FP_XBox:
					WriteMeshChunk_XB(this, pMeshChunks, sub, pOffset, pStringCache, &pModelData->boundingVolume);
					break;
				case FP_Linux:
				case FP_OSX:
					WriteMeshChunk_Linux(this, pMeshChunks, sub, pOffset, pStringCache, &pModelData->boundingVolume);
					break;
				case FP_PSP:
					WriteMeshChunk_PSP(this, pMeshChunks, sub, pOffset, pStringCache, &pModelData->boundingVolume);
					break;
				case FP_PS2:
				case FP_DC:
				case FP_GC:
				default:
					printf("Invalid platform...\n");
			}

			++b;
		}
	}

	// write out skeleton data
	if(skeletonChunkIndex > -1)
	{
		BoneChunk *pBoneChunk = (BoneChunk*)pOffset;

		pDataHeaders[skeletonChunkIndex].pData = pBoneChunk;
		pDataHeaders[skeletonChunkIndex].count = GetSkeletonChunk()->bones.size();

		pOffset += MFALIGN16(sizeof(BoneChunk)*pDataHeaders[skeletonChunkIndex].count);

		for(a=0; a<pDataHeaders[skeletonChunkIndex].count; a++)
		{
			pBoneChunk[a].pBoneName = MFStringCache_Add(pStringCache, GetSkeletonChunk()->bones[a].name);
			pBoneChunk[a].pParentName = MFStringCache_Add(pStringCache, GetSkeletonChunk()->bones[a].parentName);
			pBoneChunk[a].boneMatrix = GetSkeletonChunk()->bones[a].boneMatrix;
			pBoneChunk[a].worldMatrix = GetSkeletonChunk()->bones[a].worldMatrix;
		}
	}

	// write out collision data
	if(collisionChunkIndex > -1)
	{
		MFCollisionTemplate *pCollisionChunk = (MFCollisionTemplate*)pOffset;

		pDataHeaders[collisionChunkIndex].pData = pCollisionChunk;
		pDataHeaders[collisionChunkIndex].count = GetCollisionChunk()->collisionObjects.size();

		pOffset += MFALIGN16(sizeof(MFCollisionTemplate)*pDataHeaders[collisionChunkIndex].count);

		for(a=0; a<pDataHeaders[collisionChunkIndex].count; a++)
		{
			F3DCollisionObject *pColObj = GetCollisionChunk()->collisionObjects[a];
			
			pCollisionChunk[a].boundingVolume.min = pColObj->boundMin;
			pCollisionChunk[a].boundingVolume.max = pColObj->boundMax;
			pCollisionChunk[a].boundingVolume.boundingSphere = pColObj->boundSphere;
			pCollisionChunk[a].type = pColObj->objectType;
			pCollisionChunk[a].pName = MFStringCache_Add(pStringCache, pColObj->name);

			pCollisionChunk[a].pCollisionTemplateData = pOffset;

			switch(pCollisionChunk[a].type)
			{
				case MFCT_Mesh:
				{
					F3DCollisionMesh *pColMesh = (F3DCollisionMesh*)pColObj;
					MFCollisionMesh *pMesh = (MFCollisionMesh*)pOffset;

					pOffset += MFALIGN16(sizeof(MFCollisionMesh));

					pMesh->numTris = pColMesh->tris.size();
					pMesh->pTriangles = (MFCollisionTriangle*)pOffset;

					uint32 triBlockSize = sizeof(MFCollisionTriangle) * pMesh->numTris;
					memcpy(pMesh->pTriangles, pColMesh->tris.getpointer(), triBlockSize);

					pOffset += MFALIGN16(triBlockSize);
					break;
				}

				default:
					MFDebug_Assert(false, "Unsupported collision object type.");
			}
		}
	}

	// write out collision data
	if(tagChunkIndex > -1)
	{
		TagChunk *pTags = (TagChunk*)pOffset;

		pDataHeaders[tagChunkIndex].pData = pTags;
		pDataHeaders[tagChunkIndex].count = GetRefPointChunk()->refPoints.size();

		for(int a=0; a<pDataHeaders[tagChunkIndex].count; a++)
		{
			pTags[a].pTagName = MFStringCache_Add(pStringCache, GetRefPointChunk()->refPoints[a].name);
			pTags[a].tagMatrix = GetRefPointChunk()->refPoints[a].worldMatrix;
		}

		pOffset += MFALIGN16(sizeof(TagChunk)*pDataHeaders[tagChunkIndex].count);
	}

	// write strings to end of file
	memcpy(pOffset, MFStringCache_GetCache(pStringCache), MFStringCache_GetSize(pStringCache));

	char *pCache = MFStringCache_GetCache(pStringCache);
	uint32 stringBase = (uint32&)pCache - ((uint32&)pOffset - (uint32&)pFile);
	pOffset += MFStringCache_GetSize(pStringCache); // pOffset now equals the file size..

	// un-fix-up all the pointers...
	uint32 base = (uint32&)pModelData;

	pModelData->pName -= stringBase;

	for(a=0; a<pModelData->numDataChunks; a++)
	{
		switch(pModelData->pDataChunks[a].chunkType)
		{
			case MFChunkType_SubObjects:
			{
				SubObjectChunk *pSubobjectChunk = (SubObjectChunk*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					pSubobjectChunk[b].pSubObjectName -= stringBase;
//					pSubobjectChunk[b].pMaterial = (MFMaterial*)((char*)pSubobjectChunk[b].pMaterial - stringBase);

					switch(platform)
					{
						case FP_PC:
							FixUpMeshChunk_PC(pSubobjectChunk[b].pMeshChunks, pSubobjectChunk[b].numMeshChunks, base, stringBase);
							break;
						case FP_XBox:
							FixUpMeshChunk_XB(pSubobjectChunk[b].pMeshChunks, pSubobjectChunk[b].numMeshChunks, base, stringBase);
							break;
						case FP_Linux:
						case FP_OSX:
							FixUpMeshChunk_Linux(pSubobjectChunk[b].pMeshChunks, pSubobjectChunk[b].numMeshChunks, base, stringBase);
							break;
						case FP_PSP:
							FixUpMeshChunk_PSP(pSubobjectChunk[b].pMeshChunks, pSubobjectChunk[b].numMeshChunks, base, stringBase);
							break;
						case FP_PS2:
						case FP_DC:
						case FP_GC:
						default:
							printf("Invalid platform...\n");
					}

					(char*&)pSubobjectChunk[b].pMeshChunks -= base;
				}
				break;
			}

			case MFChunkType_Bones:
			{
				BoneChunk *pBoneChunk = (BoneChunk*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					pBoneChunk[b].pBoneName -= stringBase;
					pBoneChunk[b].pParentName -= stringBase;
				}
				break;
			}

			case MFChunkType_Collision:
			{
				MFCollisionTemplate *pCollisionChunk = (MFCollisionTemplate*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					pCollisionChunk[b].pName -= stringBase;

					if(pCollisionChunk[b].type == MFCT_Mesh)
					{
						MFCollisionMesh *pColMesh = (MFCollisionMesh*)pCollisionChunk[b].pCollisionTemplateData;
						(char*&)pColMesh->pTriangles -= base;
					}

					(char*&)pCollisionChunk[b].pCollisionTemplateData -= base;
				}
				break;
			}

			case MFChunkType_Tags:
			{
				TagChunk *pTags = (TagChunk*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					pTags[b].pTagName -= stringBase;
				}
				break;
			}

			default:
				printf("Shouldnt be here?...\n");
				break;
		}

		(char*&)pModelData->pDataChunks[a].pData -= base;
	}
	(char*&)pModelData->pDataChunks -= base;

	// write to disk..
	uint32 fileSize = (uint32&)pOffset - base;
	fwrite(pFile, fileSize, 1, file);
	fclose(file);

	// we're done!!!! clean up..
	MFHeap_Free(pFile);
}

void F3DFile::Optimise()
{
	MFArray<int> vertexUsage;
	MFArray<int> vertexMapping;
	MFArray<int> posUsage;
	MFArray<int> posMapping;
	MFArray<int> uvUsage;
	MFArray<int> uvMapping;
	MFArray<int> normUsage;
	MFArray<int> normMapping;
	MFArray<int> colUsage;
	MFArray<int> colMapping;
	int a, b, c, d, x;

	// remove all verts not referenced by triangles
	for(a=0; a<GetMeshChunk()->subObjects.size(); a++)
	{
		F3DSubObject &sub = GetMeshChunk()->subObjects[a];

		for(b=0; b<sub.matSubobjects.size(); b++)
		{
			F3DMaterialSubobject &matsub = sub.matSubobjects[b];

			vertexUsage.resize(matsub.vertices.size());
			vertexMapping.resize(matsub.vertices.size());

			for(c=0; c<matsub.triangles.size(); c++)
			{
				F3DTriangle &tri = matsub.triangles[c];
				++vertexUsage[tri.v[0]];
				++vertexUsage[tri.v[1]];
				++vertexUsage[tri.v[2]];
			}

			x=0;

			for(c=0; c<matsub.vertices.size(); c++)
			{
				if(vertexUsage[c])
				{
					if(x != c)
						matsub.vertices[x] = matsub.vertices[c];

					vertexMapping[c] = x;
					++x;
				}
			}

			matsub.vertices.resize(x);

			for(c=0; c<matsub.triangles.size(); c++)
			{
				F3DTriangle &tri = matsub.triangles[c];
				tri.v[0] = vertexMapping[tri.v[0]];
				tri.v[1] = vertexMapping[tri.v[1]];
				tri.v[2] = vertexMapping[tri.v[2]];
			}
		}

		int posSize = sub.positions.size();
		int uvSize = sub.uvs.size();
		int normSize = sub.normals.size();
		int colSize = sub.colours.size();

		posUsage.resize(posSize);
		posMapping.resize(posSize);
		uvUsage.resize(uvSize);
		uvMapping.resize(uvSize);
		normUsage.resize(normSize);
		normMapping.resize(normSize);
		colUsage.resize(colSize);
		colMapping.resize(colSize);

		// remove vertex data not referenced by verts
		for(b=0; b<sub.matSubobjects.size(); b++)
		{
			F3DMaterialSubobject &matsub = sub.matSubobjects[b];

			for(c=0; c<matsub.vertices.size(); c++)
			{
				F3DVertex &vert = matsub.vertices[c];

				if(vert.position != -1)
					++posUsage[vert.position];
				if(vert.uv1 != -1)
					++uvUsage[vert.uv1];
				if(vert.normal != -1)
					++normUsage[vert.normal];
				if(vert.colour != -1)
					++colUsage[vert.colour];
			}
		}

		x = 0;
		for(b=0; b<posSize; b++)
		{
			if(posUsage[b])
			{
				if(x != b)
					sub.positions[x] = sub.positions[b];

				posMapping[b] = x;

				// remove duplicate vertex data
				for(c=b+1; c<posSize; c++)
				{
					if(sub.positions[x] == sub.positions[c])
					{
						posMapping[c] = x;
						posUsage[c] = 0;
					}
				}

				++x;
			}
		}
		sub.positions.resize(x);

		x = 0;
		for(b=0; b<uvSize; b++)
		{
			if(uvUsage[b])
			{
				if(x != b)
					sub.uvs[x] = sub.uvs[b];

				uvMapping[b] = x;

				// remove duplicate vertex data
				for(c=b+1; c<uvSize; c++)
				{
					if(sub.uvs[x] == sub.uvs[c])
					{
						uvMapping[c] = x;
						uvUsage[c] = 0;
					}
				}

				++x;
			}
		}
		sub.uvs.resize(x);

		x = 0;
		for(b=0; b<normSize; b++)
		{
			if(normUsage[b])
			{
				if(x != b)
					sub.normals[x] = sub.normals[b];

				normMapping[b] = x;

				// remove duplicate vertex data
				for(c=b+1; c<normSize; c++)
				{
					if(sub.normals[x] == sub.normals[c])
					{
						normMapping[c] = x;
						normUsage[c] = 0;
					}
				}

				++x;
			}
		}
		sub.normals.resize(x);

		x = 0;
		for(b=0; b<colSize; b++)
		{
			if(colUsage[b])
			{
				if(x != b)
					sub.colours[x] = sub.colours[b];

				colMapping[b] = x;

				// remove duplicate vertex data
				for(c=b+1; c<colSize; c++)
				{
					if(sub.colours[x] == sub.colours[c])
					{
						colMapping[c] = x;
						colUsage[c] = 0;
					}
				}

				++x;
			}
		}
		sub.colours.resize(x);

		for(b=0; b<sub.matSubobjects.size(); b++)
		{
			F3DMaterialSubobject &matsub = sub.matSubobjects[b];

			for(c=0; c<matsub.vertices.size(); c++)
			{
				F3DVertex &vert = matsub.vertices[c];

				if(vert.position != -1)
					vert.position = posMapping[vert.position];
				if(vert.uv1 != -1)
					vert.uv1 = uvMapping[vert.uv1];
				if(vert.normal != -1)
					vert.normal = normMapping[vert.normal];
				if(vert.colour != -1)
					vert.colour = colMapping[vert.colour];
			}
		}

		// remove duplicate verts
		for(b=0; b<sub.matSubobjects.size(); b++)
		{
			F3DMaterialSubobject &matsub = sub.matSubobjects[b];

			vertexMapping.resize(matsub.vertices.size());

			int vertSize = matsub.vertices.size();

			for(c=0; c<vertSize; c++)
				vertexUsage[c] = 1;

			x=0;
			for(c=0; c<vertSize; c++)
			{
				if(vertexUsage[c])
				{
					if(x != c)
						matsub.vertices[x] = matsub.vertices[c];

					vertexMapping[c] = x;

					// remove duplicate vertex data
					for(d=c+1; d<vertSize; d++)
					{
						if(matsub.vertices[x] == matsub.vertices[d])
						{
							vertexMapping[d] = x;
							vertexUsage[d] = 0;
						}
					}

					++x;
				}
			}
			matsub.vertices.resize(x);

			for(c=0; c<matsub.triangles.size(); c++)
			{
				F3DTriangle &tri = matsub.triangles[c];
				tri.v[0] = vertexMapping[tri.v[0]];
				tri.v[1] = vertexMapping[tri.v[1]];
				tri.v[2] = vertexMapping[tri.v[2]];
			}
		}
	}
}

void F3DFile::StripModel()
{
	// run stripper on model
}

void F3DFile::ProcessSkeletonData()
{
	// generate localMatrix and stuff..
}

void F3DFile::ProcessCollisionData()
{
	// convert c_ subobjects into collision data

	if(options.noCollision)
		return;

	for(int i=0; i<GetMeshChunk()->subObjects.size(); i++)
	{
		F3DSubObject &sub = GetMeshChunk()->subObjects[i];

		if(!MFString_CaseCmpN(sub.name, "c_", 2))
		{
			int triCount = 0;
			int a, b, t = 0;

			// count num triangles
			for(a=0; a<sub.matSubobjects.size(); a++)
			{
				triCount += sub.matSubobjects[a].triangles.size();
			}

			if(triCount == 0)
				continue;

			F3DCollisionMesh *pMesh = new F3DCollisionMesh;

			MFString_Copy(pMesh->name, sub.name);
			pMesh->objectType = MFCT_Mesh;

			pMesh->boundSphere = MakeVector(sub.positions[sub.matSubobjects[0].vertices[sub.matSubobjects[0].triangles[0].v[0]].position], 0.0f);
			pMesh->boundMin = pMesh->boundSphere;
			pMesh->boundMax = pMesh->boundSphere;

			for(a=0; a<sub.matSubobjects.size(); a++)
			{
				F3DMaterialSubobject &matSub = sub.matSubobjects[a];

				int numTris = matSub.triangles.size();

				for(b=0; b<numTris; b++)
				{
					F3DCollisionTri &tri = pMesh->tris[t];

					for(int c=0; c<3; c++)
					{
						tri.point[c] = sub.positions[matSub.vertices[matSub.triangles[b].v[c]].position];

						pMesh->boundMin = MFMin(pMesh->boundMin, tri.point[c]);
						pMesh->boundMax = MFMax(pMesh->boundMax, tri.point[c]);
						AdjustBoundingSphere(tri.point[c], &pMesh->boundSphere);
					}

					tri.plane = MFCollision_MakePlaneFromPoints(tri.point[0], tri.point[1], tri.point[2]);

					tri.boundPlanes[0] = MFCollision_MakePlaneFromPointAndNormal(tri.point[0], (tri.point[0] - tri.point[1]).Cross3(tri.plane));
					tri.boundPlanes[1] = MFCollision_MakePlaneFromPointAndNormal(tri.point[1], (tri.point[1] - tri.point[2]).Cross3(tri.plane));
					tri.boundPlanes[2] = MFCollision_MakePlaneFromPointAndNormal(tri.point[2], (tri.point[2] - tri.point[0]).Cross3(tri.plane));

					tri.adjacent[0] = -1;
					tri.adjacent[1] = -1;
					tri.adjacent[2] = -1;

					tri.flags = 0;

					++t;
				}
			}

			// fill in all the adjacencies
			for(a=0; a<triCount; a++)
			{
				for(b=0; b<3; b++)
				{
					// find its neighbour
					for(int c=0; c<triCount; c++)
					{
						if(a == c)
							continue;

						bool foundOne = false;

						int d;

						for(d=0; d<3; d++)
						{
							if(pMesh->tris[a].point[b] == pMesh->tris[c].point[d])
							{
								foundOne = true;
								break;
							}
						}

						if(foundOne)
						{
							int i = (b+1)%3;

							for(d=0; d<3; d++)
							{
								if(pMesh->tris[a].point[i] == pMesh->tris[c].point[d])
								{
									pMesh->tris[a].adjacent[b] = c;
									goto cont;
								}
							}
						}
					}
		cont:
					continue;
				}
			}

			GetCollisionChunk()->collisionObjects.push() = pMesh;

			if(!options.dontDeleteCollisionSubobjects)
				sub.dontExportThisSubobject = true;
		}
	}
}

int F3DMaterialChunk::GetMaterialIndexByName(const char *pName)
{
	for(int a=0; a<materials.size(); a++)
	{
		if(!MFString_CaseCmp(pName, materials[a].name))
			return a;
	}

	return -1;
}

F3DMaterialSubobject::F3DMaterialSubobject()
{
	materialIndex = 0;
}

F3DSubObject::F3DSubObject()
{
	memset(name, 0, 64);
//	materialIndex = 0;

	dontExportThisSubobject = false;
}

F3DBone::F3DBone()
{
	memset(name, 0, 64);
	memset(parentName, 0, 64);
	memset(options, 0, 1024);

	boneMatrix = MFMatrix::identity;
	worldMatrix = MFMatrix::identity;
}

F3DVertex::F3DVertex()
{
	position = -1;
	normal = -1;
	uv1 = uv2 = uv3 = uv4 = uv5 = uv6 = uv7 = uv8 = -1;
	colour = -1;
	illum = -1;
	biNormal = -1;
	tangent = -1;
	bone[0] = bone[1] = bone[2] = bone[3] = -1;
	weight[0] = weight[1] = weight[2] = weight[3] = 0.0f;
}

F3DMaterial::F3DMaterial()
{
	diffuse = MFVector::one;
	ambient = MFVector::one;
	emissive = MFVector::zero;
	specular = MFVector::zero;
	specularLevel = 0.0f;
	glossiness = 0.0f;

	strcpy(name, "");
	for(int a=0; a<8; a++)
	{
		strcpy(maps[a], "");
	}
}

F3DRefPoint::F3DRefPoint()
{
	worldMatrix = MFMatrix::identity;
	localMatrix = MFMatrix::identity;
	bone[0] = -1; bone[1] = -1; bone[2] = -1; bone[3] = -1;
	weight[0] = 0.0f; weight[1] = 0.0f; weight[2] = 0.0f; weight[3] = 0.0f;
	strcpy(name, "");
	strcpy(options, "");
}

F3DCollisionChunk::~F3DCollisionChunk()
{
	for(int a=0; a<collisionObjects.size(); a++)
	{
		delete collisionObjects[a];
	}
}

F3DOptions::F3DOptions()
{
	noAnimation = false;
	noCollision = false;
	dontDeleteCollisionSubobjects = false;
}
