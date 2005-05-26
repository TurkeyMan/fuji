#include <stdio.h>
#include <d3d9.h>

#include "Common.h"
#include "Array.h"
#include "F3D.h"
#include "Heap.h"
#include "MFStringCache.h"

#include "MFModel_Internal.h"
#include "Material_Internal.h"

int F3DFile::ReadFromDisk(char *pFilename)
{
	FILE *infile;
	char *file = NULL;
	uint32 filesize = 0;

	infile = fopen(pFilename, "r");

	if(!infile)
	{
		printf("Failed to open F3D file %s", pFilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	file = (char*)malloc(filesize);
	fread(file, 1, filesize, infile);

	fclose(infile);

	ReadFromMemory(file);

	free(file);

	return 0;
}

void F3DFile::ImportMesh(F3DMesh *pMesh, char *pBase)
{
	F3DSubObject &sub = meshChunk.subObjects.push();

	memcpy(sub.name, pMesh->name, 64);
	sub.materialIndex = pMesh->materialIndex;

	F3DMatSub *pMatSubs = (F3DMatSub*)(pBase + pMesh->pMatSubOffset);

	sub.matSubobjects.resize(pMesh->matSubCount);
	for(uint32 a=0; a<pMesh->matSubCount; a++)
	{
		sub.matSubobjects[a].triangles.resize(pMatSubs[a].triangleCount);
		memcpy(sub.matSubobjects[a].triangles.pData, pBase + pMatSubs[a].pTriangleOffset, sizeof(sub.matSubobjects[a].triangles[0])*pMatSubs[a].triangleCount);

		sub.matSubobjects[a].vertices.resize(pMatSubs[a].vertexCount);
		memcpy(sub.matSubobjects[a].vertices.pData, pBase + pMatSubs[a].pVertexOffset, sizeof(sub.matSubobjects[a].vertices[0])*pMatSubs[a].vertexCount);
	}

	sub.positions.resize(pMesh->positionCount);
	memcpy(sub.positions.pData, pBase + pMesh->pPositionOffset, sizeof(sub.positions[0])*pMesh->positionCount);

	sub.uvs.resize(pMesh->texCount);
	memcpy(sub.uvs.pData, pBase + pMesh->pTexOffset, sizeof(sub.uvs[0])*pMesh->texCount);

	sub.normals.resize(pMesh->normalCount);
	memcpy(sub.normals.pData, pBase + pMesh->pNormalOffset, sizeof(sub.normals[0])*pMesh->normalCount);

	sub.colours.resize(pMesh->colourCount);
	memcpy(sub.colours.pData, pBase + pMesh->pColourOffset, sizeof(sub.colours[0])*pMesh->colourCount);

	sub.illumination.resize(pMesh->illumCount);
	memcpy(sub.illumination.pData, pBase + pMesh->pIllumOffset, sizeof(sub.illumination[0])*pMesh->illumCount);
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
		pMesh[a].materialIndex = meshChunk.subObjects[a].materialIndex;

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
			memcpy(pData, meshChunk.subObjects[a].matSubobjects[b].triangles.pData, size);
			pMatSubs[b].pTriangleOffset = (uint32)(pData - pBase);
			pData += size;

			size = pMatSubs[b].vertexCount * sizeof(meshChunk.subObjects[a].matSubobjects[b].vertices[0]);
			memcpy(pData, meshChunk.subObjects[a].matSubobjects[b].vertices.pData, size);
			pMatSubs[b].pVertexOffset = (uint32)(pData - pBase);
			pData += size;
		}

		size = pMesh[a].positionCount * sizeof(meshChunk.subObjects[a].positions[0]);
		memcpy(pData, meshChunk.subObjects[a].positions.pData, size);
		pMesh[a].pPositionOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].texCount * sizeof(meshChunk.subObjects[a].uvs[0]);
		memcpy(pData, meshChunk.subObjects[a].uvs.pData, size);
		pMesh[a].pTexOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].normalCount * sizeof(meshChunk.subObjects[a].normals[0]);
		memcpy(pData, meshChunk.subObjects[a].normals.pData, size);
		pMesh[a].pNormalOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].colourCount * sizeof(meshChunk.subObjects[a].colours[0]);
		memcpy(pData, meshChunk.subObjects[a].colours.pData, size);
		pMesh[a].pColourOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].illumCount * sizeof(meshChunk.subObjects[a].illumination[0]);
		memcpy(pData, meshChunk.subObjects[a].illumination.pData, size);
		pMesh[a].pIllumOffset = (uint32)(pData - pBase);
		pData += size;
	}
}

void F3DFile::ExportSkeleton(char* &pData, char *pBase)
{
	int size = sizeof(F3DBone)*skeletonChunk.bones.size();
	memcpy(pData, skeletonChunk.bones.pData, size);
	pData += size;
}

void F3DFile::ExportMaterial(char* &pData, char *pBase)
{
	int size = sizeof(F3DMaterial)*materialChunk.materials.size();
	memcpy(pData, materialChunk.materials.pData, size);
	pData += size;
}

void F3DFile::WriteToDisk(char *pFilename)
{
	char *pFile;
	char *pOffset;
	F3DHeader *pHeader;
	F3DChunkDesc *pChunks;

	FILE *file = fopen(pFilename, "wb");

	if(!file)
	{
		printf("Count open \"%f\" for writing.", pFilename);
		return;
	}

	pFile = (char*)malloc(1024*1024*10);
	pHeader = (F3DHeader*)pFile;

	pHeader->ID = MAKEFOURCC('M','F','3','D');
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

	free(pFile);
}

void F3DFile::ReadFromMemory(char *pMemory)
{
	F3DHeader *pHeader = (F3DHeader*)pMemory;
	F3DChunkDesc *pChunks;

	if(pHeader->ID != MAKEFOURCC('M','F','3','D'))
	{
		printf("Not an F3D file.");
		return;
	}

	printf("Reading F3D file version %d.%d", pHeader->major, pHeader->minor);

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

void F3DFile::WriteMDL(char *pFilename, int system)
{
	int a, b, c;

	MFModelTemplate *pModelData;

	FILE *file = fopen(pFilename, "wb");

	if(!file)
	{
		printf("Count open \"%f\" for writing.", pFilename);
		return;
	}

	MFStringCache *pStringCache;
	pStringCache = MFStringCache::Create(1024*1024);

	char *pFile;
	char *pOffset;

	const int maxFileSize = 1024*1024*4;

	pFile = (char*)malloc_aligned(maxFileSize); // allocating 10mb ... yeah this is REALLY weak! ;)
	memset(pFile, 0, maxFileSize);
	pModelData = (MFModelTemplate*)pFile;

	DataChunk *pDataHeaders = (DataChunk*)(pFile+ALIGN16(sizeof(MFModelTemplate)));

	pModelData->IDtag = MAKEFOURCC('M','D','L','2');
	pModelData->pName = pStringCache->Add(name);

	int numChunks = 0;
	int meshChunkIndex = -1;
	int skeletonChunkIndex = -1;
	int tagChunkIndex = -1;
	int dataChunkIndex = -1;

	// figure out number of chunks somehow.....
	if(GetMeshChunk()->subObjects.size())
	{
		meshChunkIndex = numChunks++;
		pDataHeaders[meshChunkIndex].chunkType = CT_SubObjects;
	}

	if(GetSkeletonChunk()->bones.size())
	{
		skeletonChunkIndex = numChunks++;
		pDataHeaders[skeletonChunkIndex].chunkType = CT_Bones;
	}

	if(GetRefPointChunk()->refPoints.size())
	{
		tagChunkIndex = numChunks++;
		pDataHeaders[tagChunkIndex].chunkType = CT_Tags;
	}

	// then do something with them....
	pModelData->numDataChunks = numChunks;
	pModelData->pDataChunks = pDataHeaders;

	pOffset = (char*)pDataHeaders + ALIGN16(sizeof(DataChunk)*numChunks);

	// write out mesh data
	if(meshChunkIndex > -1)
	{
		SubObjectChunk *pSubobjectChunk = (SubObjectChunk*)pOffset;

		pDataHeaders[meshChunkIndex].pData = pSubobjectChunk;
		pDataHeaders[meshChunkIndex].count = GetMeshChunk()->subObjects.size();

		pOffset += ALIGN16(sizeof(SubObjectChunk)*pDataHeaders[meshChunkIndex].count);

		for(a=0; a<pDataHeaders[meshChunkIndex].count; a++)
		{
			const F3DSubObject &sub = GetMeshChunk()->subObjects[a];
			MFMeshChunk *pMeshChunks = (MFMeshChunk*)pOffset;

			pSubobjectChunk[a].pSubObjectName = pStringCache->Add(sub.name);
			pSubobjectChunk[a].pMaterial = (Material*)pStringCache->Add(GetMaterialChunk()->materials.pData[sub.materialIndex].name);
			pSubobjectChunk[a].numMeshChunks = sub.matSubobjects.size();
			pSubobjectChunk[a].pMeshChunks = pMeshChunks;

			pOffset += ALIGN16(sizeof(MFMeshChunk)*pSubobjectChunk[a].numMeshChunks);

			// fill out msh chunk, and build mesh...
			for(b=0; b<pSubobjectChunk[a].numMeshChunks; b++)
			{
				struct Vert
				{
					float pos[3];
					float normal[3];
					uint32 colour;
					float uv[2];
				};

				int numVertices = sub.matSubobjects.pData[b].vertices.size();
				int numIndices = sub.matSubobjects.pData[b].triangles.size()*3;

				pSubobjectChunk[a].pMeshChunks[b].numVertices = numVertices;
				pSubobjectChunk[a].pMeshChunks[b].vertexStride = sizeof(Vert);
				pSubobjectChunk[a].pMeshChunks[b].vertexDataSize = numVertices * pSubobjectChunk[a].pMeshChunks[b].vertexStride;
				pSubobjectChunk[a].pMeshChunks[b].indexDataSize = numIndices*sizeof(uint16);

				// write declaration
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements = (D3DVERTEXELEMENT9*)pOffset;

				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[0].Stream = 0;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[0].Offset = 0;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[0].Type = D3DDECLTYPE_FLOAT3;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[0].Method = D3DDECLMETHOD_DEFAULT;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[0].Usage = D3DDECLUSAGE_POSITION;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[0].UsageIndex = 0;

				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[1].Stream = 0;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[1].Offset = 12;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[1].Type = D3DDECLTYPE_FLOAT3;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[1].Method = D3DDECLMETHOD_DEFAULT;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[1].Usage = D3DDECLUSAGE_NORMAL;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[1].UsageIndex = 0;

				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[2].Stream = 0;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[2].Offset = 24;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[2].Type = D3DDECLTYPE_D3DCOLOR;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[2].Method = D3DDECLMETHOD_DEFAULT;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[2].Usage = D3DDECLUSAGE_COLOR;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[2].UsageIndex = 0;

				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[3].Stream = 0;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[3].Offset = 28;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[3].Type = D3DDECLTYPE_FLOAT2;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[3].Method = D3DDECLMETHOD_DEFAULT;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[3].Usage = D3DDECLUSAGE_TEXCOORD;
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[3].UsageIndex = 0;

				D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
				pSubobjectChunk[a].pMeshChunks[b].pVertexElements[4] = endMacro;

				pOffset += ALIGN16(sizeof(D3DVERTEXELEMENT9)*5);

				// write vertices
				pSubobjectChunk[a].pMeshChunks[b].pVertexData = pOffset;
				Vert *pVert = (Vert*)pOffset;

				for(c=0; c<numVertices; c++)
				{
					int posIndex = sub.matSubobjects.pData[b].vertices.pData[c].position;
					int normalIndex = sub.matSubobjects.pData[b].vertices.pData[c].normal;
					int uvIndex = sub.matSubobjects.pData[b].vertices.pData[c].uv1;
					int colourIndex = sub.matSubobjects.pData[b].vertices.pData[c].colour;

					const Vector3 &pos = posIndex > -1 ? sub.positions.pData[posIndex] : Vector(0,0,0);
					const Vector3 &normal = normalIndex > -1 ? sub.normals.pData[normalIndex] : Vector(0,1,0);
					const Vector3 &uv = uvIndex > -1 ? sub.uvs.pData[uvIndex] : Vector(0,0,0);
					uint32 colour = colourIndex > -1 ? sub.colours.pData[colourIndex].ToPackedColour() : 0xFFFFFFFF;

					pVert[c].pos[0] = pos.x;
					pVert[c].pos[1] = pos.y;
					pVert[c].pos[2] = pos.z;
					pVert[c].normal[0] = normal.x;
					pVert[c].normal[1] = normal.y;
					pVert[c].normal[2] = normal.z;
					pVert[c].colour = colour;
					pVert[c].uv[0] = uv.x;
					pVert[c].uv[1] = uv.y;
				}

				pOffset += ALIGN16(sizeof(Vert)*numVertices);

				// write indices
				pSubobjectChunk[a].pMeshChunks[b].pIndexData = pOffset;
				uint16 *pIndices = (uint16*)pOffset;

				int triCount = numIndices/3;

				for(c=0; c<triCount; c++)
				{
					pIndices[0] = sub.matSubobjects.pData[b].triangles.pData[c].v[0];
					pIndices[1] = sub.matSubobjects.pData[b].triangles.pData[c].v[1];
					pIndices[2] = sub.matSubobjects.pData[b].triangles.pData[c].v[2];

					pIndices += 3;
				}

				pOffset += ALIGN16(sizeof(uint16)*numIndices);
			}
		}
	}

	// write out skeleton data
	if(skeletonChunkIndex > -1)
	{
		BoneChunk *pBoneChunk = (BoneChunk*)pOffset;

		pDataHeaders[skeletonChunkIndex].pData = pBoneChunk;
		pDataHeaders[skeletonChunkIndex].count = GetSkeletonChunk()->bones.size();

		pOffset += ALIGN16(sizeof(BoneChunk)*pDataHeaders[skeletonChunkIndex].count);

		for(a=0; a<pDataHeaders[skeletonChunkIndex].count; a++)
		{
			pBoneChunk[a].pBoneName = pStringCache->Add(GetSkeletonChunk()->bones[a].name);
			pBoneChunk[a].pParentName = pStringCache->Add(GetSkeletonChunk()->bones[a].parentName);
			pBoneChunk[a].boneOrigin = GetSkeletonChunk()->bones[a].worldMatrix.GetTrans3();
		}
	}

	// wite strings to end of file
	memcpy(pOffset, pStringCache->GetCache(), pStringCache->GetSize());

	uint32 stringBase = (uint32)pStringCache->GetCache() - ((uint32)pOffset - (uint32)pFile);
	pOffset += pStringCache->GetSize(); // pOffset now equals the file size..

	// un-fix-up all the pointers...
	uint32 base = (uint32)pModelData;

	pModelData->pName -= stringBase;

	for(a=0; a<pModelData->numDataChunks; a++)
	{
		switch(pModelData->pDataChunks[a].chunkType)
		{
			case CT_SubObjects:
			{
				SubObjectChunk *pSubobjectChunk = (SubObjectChunk*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					pSubobjectChunk[b].pSubObjectName -= stringBase;
					pSubobjectChunk[b].pMaterial = (Material*)((char*)pSubobjectChunk[b].pMaterial - stringBase);

					for(c=0; c<pSubobjectChunk[b].numMeshChunks; c++)
					{
						pSubobjectChunk[b].pMeshChunks[c].pVertexData -= base;
						pSubobjectChunk[b].pMeshChunks[c].pIndexData -= base;
						(char*&)pSubobjectChunk[b].pMeshChunks[c].pVertexElements -= base;
					}

					(char*&)pSubobjectChunk[b].pMeshChunks -= base;
				}
				break;
			}

			case CT_Bones:
			{
				BoneChunk *pBoneChunk = (BoneChunk*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					pBoneChunk[b].pBoneName -= stringBase;
					pBoneChunk[b].pParentName -= stringBase;
				}
				break;
			}

			case CT_Tags:
			{
				break;
			}
		}

		(char*&)pModelData->pDataChunks[a].pData -= base;
	}
	(char*&)pModelData->pDataChunks -= base;

	// write to disk..
	uint32 fileSize = (uint32)pOffset - base;
	fwrite(pFile, fileSize, 1, file);
	fclose(file);

	// we're done!!!! clean up..
	free_aligned(pFile);
}

void F3DFile::Optimise()
{
	// optimise mesh and stuff

	// minimise normals

	// minimise verts


}

void F3DFile::ProcessSkeletonData()
{
	// generate localMatrix and stuff..
}

F3DSubObject::F3DSubObject()
{
	memset(name, 0, 64);
	materialIndex = 0;
}

F3DBone::F3DBone()
{
	memset(name, 0, 64);
	memset(parentName, 0, 64);
	memset(options, 0, 1024);
}

F3DVertex::F3DVertex()
{
	position = -1;
	normal = -1;
	uv1 = uv2 = uv3 = uv4 = uv5 = uv6 = uv7 = uv8 = -1;
	colour = -1;
	illum = -1;
	bone[0] = bone[1] = bone[2] = bone[3] = -1;
	weight[0] = weight[1] = weight[2] = weight[3] = 0.0f;
}

