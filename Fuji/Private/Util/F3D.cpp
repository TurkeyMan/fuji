#include <stdio.h>
#include <d3d9.h>

#include "Common.h"
#include "Array.h"
#include "F3D.h"

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
	char *pFile;
	char *pOffset;
	ModelData *pModelData;

	int a, index;
	uint32 b;
/*
	FILE *file = fopen(pFilename, "wb");

	if(!file)
	{
		printf("Count open \"%f\" for writing.", pFilename);
		return;
	}

	pFile = (char*)malloc(1024*1024*10);
	pModelData = (ModelData*)pFile;

	pModelData->IDtag = MAKEFOURCC('M','D','L','1');

	pModelData->meshChunkCount = meshChunk.subObjects.size();
	pModelData->materialCount = materialChunk.materials.size();
	pModelData->boneCount = skeletonChunk.bones.size();
	pModelData->customDataCount = 0;
	pModelData->flags = 0;

	// calculate custom data
#if defined(_WINDOWS) || defined(_XBOX)
	// add one for vertex buffers
//	pModelData->customDataCount++;
#endif

	// write file
	pOffset = pFile + ALIGN16(sizeof(ModelData));

	ModelData::MaterialData *pMaterials = (ModelData::MaterialData*)pOffset;
	pModelData->pMaterials = (ModelData::MaterialData*)(pOffset - pFile);

	pOffset += ALIGN16(sizeof(ModelData::MaterialData) * pModelData->materialCount);

	ModelData::Subobject *pSubObjects = (ModelData::Subobject*)pOffset;
	pModelData->pSubobjects = (ModelData::Subobject*)(pOffset - pFile);

	pOffset += ALIGN16(sizeof(ModelData::Subobject) * pModelData->subobjectCount);

	ModelData::CustomData *pCustomData = (ModelData::CustomData*)pOffset;
	pModelData->pCustomData = (ModelData::CustomData*)(pOffset - pFile);

	pOffset += ALIGN16(sizeof(ModelData::CustomData) * pModelData->customDataCount);

	uint32 vertOffset = 0;
	uint32 indexOffset = 0;
	pModelData->pVertexData = (char*)(pOffset - pFile);
	pModelData->vertexCount = 0;
	pModelData->indexCount = 0;

	for(a=0; a<pModelData->subobjectCount; a++)
	{
		pSubObjects[a].vertexFormat = VF_Position|VF_Normal|VF_Colour|VF_Tex0;
		pSubObjects[a].vertexSize = sizeof(FileVertex);
		pSubObjects[a].vertexCount = meshChunk.subObjects[a].vertices.size();
		pSubObjects[a].vertexOffset = vertOffset;
		vertOffset += pSubObjects[a].vertexCount;
		pModelData->vertexCount += pSubObjects[a].vertexCount;

		pSubObjects[a].indexCount = meshChunk.subObjects[a].triangles.size()*3;
		pSubObjects[a].indexOffset = indexOffset;
		indexOffset += pSubObjects[a].indexCount;
		pModelData->indexCount += pSubObjects[a].indexCount;

		pSubObjects[a].materialIndex = meshChunk.subObjects[a].materialIndex;

		pSubObjects[a].reserved[0] = pSubObjects[a].reserved[0] = 0;

		for(b=0; b<pSubObjects[a].vertexCount; b++)
		{
			index = meshChunk.subObjects[a].vertices[b].position;
			((FileVertex*)pOffset)[b].pos.x = meshChunk.subObjects[a].positions[index].x;
			((FileVertex*)pOffset)[b].pos.y = meshChunk.subObjects[a].positions[index].y;
			((FileVertex*)pOffset)[b].pos.z = meshChunk.subObjects[a].positions[index].z;

			index = meshChunk.subObjects[a].vertices[b].normal;
			if(index != -1)
			{
				((FileVertex*)pOffset)[b].normal.x = meshChunk.subObjects[a].normals[index].x;
				((FileVertex*)pOffset)[b].normal.y = meshChunk.subObjects[a].normals[index].y;
				((FileVertex*)pOffset)[b].normal.z = meshChunk.subObjects[a].normals[index].z;
			}
			else
			{
				((FileVertex*)pOffset)[b].normal.x = 0.0f;
				((FileVertex*)pOffset)[b].normal.y = 1.0f;
				((FileVertex*)pOffset)[b].normal.z = 0.0f;
			}

			index = meshChunk.subObjects[a].vertices[b].uv1;
			if(index != -1)
			{
				((FileVertex*)pOffset)[b].u = meshChunk.subObjects[a].uvs[index].x;
				((FileVertex*)pOffset)[b].v = meshChunk.subObjects[a].uvs[index].y;
			}
			else
			{
				((FileVertex*)pOffset)[b].u = 0.0f;
				((FileVertex*)pOffset)[b].v = 0.0f;
			}

			index = meshChunk.subObjects[a].vertices[b].colour;
			if(index != -1)
			{
				((FileVertex*)pOffset)[b].colour = meshChunk.subObjects[a].colours[index].ToColour();
			}
			else
			{
				((FileVertex*)pOffset)[b].colour = 0xFFFFFFFF;
			}
		}

		pOffset += sizeof(FileVertex) * pSubObjects[a].vertexCount;
	}

//	pOffset = ALIGN16(pOffset);

	pModelData->pIndexData = (char*)(pOffset - pFile);

	for(a=0; a<pModelData->subobjectCount; a++)
	{
		for(b=0; b<pSubObjects[a].indexCount/3; b++)
		{
			((int*)pOffset)[0] = meshChunk.subObjects[a].triangles[b].v[0];
			((int*)pOffset)[1] = meshChunk.subObjects[a].triangles[b].v[1];
			((int*)pOffset)[2] = meshChunk.subObjects[a].triangles[b].v[2];
			pOffset += sizeof(int)*3;
		}
	}

//	pOffset = ALIGN16(pOfset);

	for(a=0; a<pModelData->customDataCount; a++)
	{
		pCustomData[a].customDataType;
		pCustomData[a].ustomDataCount;
		pCustomData[a].pData;
		pCustomData[a].res;
		pCustomData[a].res2;
	}

	strcpy(pOffset, name);
	pModelData->pName = (char*)(pOffset - pFile);
	pOffset += strlen(name)+1;

	for(a=0; a<pModelData->materialCount; a++)
	{
		strcpy(pOffset, materialChunk.materials[a].name);
		pMaterials[a].pName = (char*)(pOffset - pFile);
		pOffset += strlen(pOffset)+1;

		*pOffset = NULL;
		pMaterials[a].pMaterialDescription = (char*)(pOffset - pFile);
		pOffset++;

		pMaterials[a].pMaterial = NULL;
		pMaterials[a].reserved = 0;
	}

	fwrite(pFile, 1, pOffset - pFile, file);

	fclose(file);

	free(pFile);
*/
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

