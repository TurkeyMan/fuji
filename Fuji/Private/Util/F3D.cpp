#include "Fuji.h"
#include "MFArray.h"
#include "Util/F3D.h"
#include "MFHeap.h"
#include "MFStringCache.h"
#include "MFString.h"
#include "MFCollision_Internal.h"

#include "MFMesh_Internal.h"
#include "MFModel_Internal.h"
#include "MFAnimation_Internal.h"
#include "MFMaterial_Internal.h"

#include "MFFileSystem.h"

#include <stdlib.h>
#include <string.h>

// This forces ConvertASE and the shaders to use vertex data formats supported by D3D8
#define SUPPORT_D3D8

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

int F3DFile::ReadF3D(const char *pFilename)
{
	char *pFile = MFFileSystem_Load(pFilename);
	if(!pFile)
	{
		MFDebug_Warn(2, MFStr("Failed to open F3D file %s", pFilename));
		return 1;
	}

	ReadF3DFromMemory(pFile);
	MFHeap_Free(pFile);

	return 0;
}

void F3DFile::ImportMesh(F3DMesh *pMesh, char *pBase)
{
	F3DSubObject &sub = meshChunk.subObjects.push();

	MFCopyMemory(sub.name, pMesh->name, 64);
//	sub.materialIndex = pMesh->materialIndex;

	F3DMatSub *pMatSubs = (F3DMatSub*)(pBase + pMesh->pMatSubOffset);

	sub.matSubobjects.resize(pMesh->matSubCount);
	for(uint32 a=0; a<pMesh->matSubCount; a++)
	{
		sub.matSubobjects[a].triangles.resize(pMatSubs[a].triangleCount);
		MFCopyMemory(sub.matSubobjects[a].triangles.getpointer(), pBase + pMatSubs[a].pTriangleOffset, sizeof(sub.matSubobjects[a].triangles[0])*pMatSubs[a].triangleCount);

		sub.matSubobjects[a].vertices.resize(pMatSubs[a].vertexCount);
		MFCopyMemory(sub.matSubobjects[a].vertices.getpointer(), pBase + pMatSubs[a].pVertexOffset, sizeof(sub.matSubobjects[a].vertices[0])*pMatSubs[a].vertexCount);
	}

	sub.positions.resize(pMesh->positionCount);
	MFCopyMemory(sub.positions.getpointer(), pBase + pMesh->pPositionOffset, sizeof(sub.positions[0])*pMesh->positionCount);

	sub.uvs.resize(pMesh->texCount);
	MFCopyMemory(sub.uvs.getpointer(), pBase + pMesh->pTexOffset, sizeof(sub.uvs[0])*pMesh->texCount);

	sub.normals.resize(pMesh->normalCount);
	MFCopyMemory(sub.normals.getpointer(), pBase + pMesh->pNormalOffset, sizeof(sub.normals[0])*pMesh->normalCount);

	sub.colours.resize(pMesh->colourCount);
	MFCopyMemory(sub.colours.getpointer(), pBase + pMesh->pColourOffset, sizeof(sub.colours[0])*pMesh->colourCount);

	sub.illumination.resize(pMesh->illumCount);
	MFCopyMemory(sub.illumination.getpointer(), pBase + pMesh->pIllumOffset, sizeof(sub.illumination[0])*pMesh->illumCount);
}

void F3DFile::ExportMesh(char* &pData, char *pBase)
{
	F3DMesh *pMesh = (F3DMesh*)pData;
	int size;

	for(int a=0; a<meshChunk.subObjects.size(); a++)
	{
		pMesh[a].size = sizeof(F3DMesh);
		pData += pMesh[a].size;
		MFCopyMemory(pMesh[a].name, meshChunk.subObjects[a].name, 64);
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
			MFCopyMemory(pData, meshChunk.subObjects[a].matSubobjects[b].triangles.getpointer(), size);
			pMatSubs[b].pTriangleOffset = (uint32)(pData - pBase);
			pData += size;

			size = pMatSubs[b].vertexCount * sizeof(meshChunk.subObjects[a].matSubobjects[b].vertices[0]);
			MFCopyMemory(pData, meshChunk.subObjects[a].matSubobjects[b].vertices.getpointer(), size);
			pMatSubs[b].pVertexOffset = (uint32)(pData - pBase);
			pData += size;
		}

		size = pMesh[a].positionCount * sizeof(meshChunk.subObjects[a].positions[0]);
		MFCopyMemory(pData, meshChunk.subObjects[a].positions.getpointer(), size);
		pMesh[a].pPositionOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].texCount * sizeof(meshChunk.subObjects[a].uvs[0]);
		MFCopyMemory(pData, meshChunk.subObjects[a].uvs.getpointer(), size);
		pMesh[a].pTexOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].normalCount * sizeof(meshChunk.subObjects[a].normals[0]);
		MFCopyMemory(pData, meshChunk.subObjects[a].normals.getpointer(), size);
		pMesh[a].pNormalOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].colourCount * sizeof(meshChunk.subObjects[a].colours[0]);
		MFCopyMemory(pData, meshChunk.subObjects[a].colours.getpointer(), size);
		pMesh[a].pColourOffset = (uint32)(pData - pBase);
		pData += size;

		size = pMesh[a].illumCount * sizeof(meshChunk.subObjects[a].illumination[0]);
		MFCopyMemory(pData, meshChunk.subObjects[a].illumination.getpointer(), size);
		pMesh[a].pIllumOffset = (uint32)(pData - pBase);
		pData += size;
	}
}

void F3DFile::ExportSkeleton(char* &pData, char *pBase)
{
	int size = sizeof(F3DBone)*skeletonChunk.bones.size();
	MFCopyMemory(pData, skeletonChunk.bones.getpointer(), size);
	pData += size;
}

void F3DFile::ExportMaterial(char* &pData, char *pBase)
{
	int size = sizeof(F3DMaterial)*materialChunk.materials.size();
	MFCopyMemory(pData, materialChunk.materials.getpointer(), size);
	pData += size;
}

void F3DFile::WriteF3D(const char *pFilename)
{
	char *pFile;
	char *pOffset;
	F3DHeader *pHeader;
	F3DChunkDesc *pChunks;

	MFFile *hFile = MFFileSystem_Open(pFilename, MFOF_Write);

	if(!hFile)
	{
		MFDebug_Warn(1, MFStr("Count open '%s' for writing.\n", pFilename));
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

	if(animationChunk.anims.size())
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

	MFFile_Write(hFile, pFile, (uint32)(pOffset - pFile));

	MFFile_Close(hFile);

	MFHeap_Free(pFile);
}

int F3DFile::ReadF3DFromMemory(const char *pMemory)
{
	F3DHeader *pHeader = (F3DHeader*)pMemory;
	F3DChunkDesc *pChunks;

	if(pHeader->ID != MFMAKEFOURCC('M','F','3','D'))
	{
		MFDebug_Warn(1, "Not an F3D file.\n");
		return 1;
	}

	MFDebug_Log(4, MFStr("Reading F3D file version %d.%d\n", pHeader->major, pHeader->minor));

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
					ImportMesh(pMesh, (char*)pMemory);
				}
			}
			break;

			case CT_Skeleton:
			{
				skeletonChunk.bones.resize(pChunks[a].elementCount);
				for(int b=0; b<pChunks[a].elementCount; b++)
				{
					MFCopyMemory(&skeletonChunk.bones[b], pMemory + pChunks[a].pOffset + b*pChunks[a].elementSize, pChunks[a].elementSize);
				}
			}
			break;

			case CT_Material:
			{
				materialChunk.materials.resize(pChunks[a].elementCount);
				for(int b=0; b<pChunks[a].elementCount; b++)
				{
					MFCopyMemory(&materialChunk.materials[b], pMemory + pChunks[a].pOffset + b*pChunks[a].elementSize, pChunks[a].elementSize);
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

void WriteMeshChunk_Generic(F3DFile *pModel, MFMeshChunk *pMeshChunks, const F3DSubObject &sub, char *&pOffset, MFStringCache *pStringCache, MFBoundingVolume *pVolume)
{
	int numMeshChunks = 0;
	int a, b, c, mc = 0;

	// count valid mesh chunks
	for(a=0; a<sub.matSubobjects.size(); a++)
	{
		if(sub.matSubobjects[a].triangles.size() != 0)
			numMeshChunks += sub.matSubobjects[a].triangleBatches.size();
	}

	// increment size of MFMeshChunk_Generic structure
	pOffset += MFALIGN16(sizeof(MFMeshChunk_Generic)*numMeshChunks);

	MFMeshChunk_Generic *pMeshChunk = (MFMeshChunk_Generic*)pMeshChunks;
	MFZeroMemory(pMeshChunk, sizeof(MFMeshChunk_Generic)*numMeshChunks);

	bool subobjectAnimation = (sub.IsSubobjectAnimation() != -1);

	// fill out mesh chunk, and build mesh...
	for(a=0; a<sub.matSubobjects.size(); a++)
	{
		const F3DMaterialSubobject &matsub = sub.matSubobjects[a];

		struct Vert
		{
			float pos[3];
			float normal[3];
			uint32 colour;
			float uv[2];
		};

		struct W4
		{
#if defined(SUPPORT_D3D8)
			uint8 w3;
			uint8 w2;
			uint8 w1;
			uint8 w4;
#else
			uint8 w1;
			uint8 w2;
			uint8 w3;
			uint8 w4;
#endif
		};

		struct AnimVert
		{
			W4 i;
			W4 w;
		};

		if(matsub.triangles.size() == 0)
			continue;

		bool bAnimating = (matsub.numBones && !subobjectAnimation);

		for(b=0; b<matsub.triangleBatches.size(); b++)
		{
			const F3DBatch &batch = matsub.triangleBatches[b];

			int numTriangles = batch.tris.size();
			int numIndices = numTriangles*3;
			int numVertices = batch.vertices.size();

			MFMeshChunk_Generic &chunk = pMeshChunk[mc];

			chunk.type = MFMCT_Generic;
			chunk.pMaterial = (MFMaterial*)MFStringCache_Add(pStringCache, pModel->GetMaterialChunk()->materials[matsub.materialIndex].name);

			chunk.numVertices = numVertices;
			chunk.numIndices = numIndices;
			chunk.maxBlendWeights = bAnimating ? matsub.maxWeights : 0;
			chunk.matrixBatchSize = batch.bones.size();

			// setup vertex format structure
			chunk.pVertexFormat = (MFMeshVertexFormat*)pOffset;
			pOffset += MFALIGN16(sizeof(MFMeshVertexFormat));
			int numVertexStreams = bAnimating ? 2 : 1;
			chunk.pVertexFormat->numVertexStreams = numVertexStreams;
			chunk.pVertexFormat->pStreams = (MFMeshVertexStream*)pOffset;
			pOffset += MFALIGN16(sizeof(MFMeshVertexStream)*numVertexStreams);
			chunk.pVertexFormat->pStreams[0].pElements = (MFMeshVertexElement*)pOffset;
			pOffset += MFALIGN16(sizeof(MFMeshVertexElement)*4);
			if(numVertexStreams > 1)
			{
				chunk.pVertexFormat->pStreams[1].pElements = (MFMeshVertexElement*)pOffset;
				pOffset += MFALIGN16(sizeof(MFMeshVertexElement)*2);
			}

			// write declaration
			chunk.pVertexFormat->pStreams[0].pStreamName = NULL;
			chunk.pVertexFormat->pStreams[0].numVertexElements = 4;
			chunk.pVertexFormat->pStreams[0].streamStride = sizeof(Vert);
			chunk.pVertexFormat->pStreams[0].pElements[0].offset = 0;
			chunk.pVertexFormat->pStreams[0].pElements[0].type = MFMVDT_Float3;
			chunk.pVertexFormat->pStreams[0].pElements[0].usage = MFVET_Position;
			chunk.pVertexFormat->pStreams[0].pElements[0].usageIndex = 0;
			chunk.pVertexFormat->pStreams[0].pElements[1].offset = 12;
			chunk.pVertexFormat->pStreams[0].pElements[1].type = MFMVDT_Float3;
			chunk.pVertexFormat->pStreams[0].pElements[1].usage = MFVET_Normal;
			chunk.pVertexFormat->pStreams[0].pElements[1].usageIndex = 0;
			chunk.pVertexFormat->pStreams[0].pElements[2].offset = 24;
			chunk.pVertexFormat->pStreams[0].pElements[2].type = MFMVDT_ColourBGRA;
			chunk.pVertexFormat->pStreams[0].pElements[2].usage = MFVET_Colour;
			chunk.pVertexFormat->pStreams[0].pElements[2].usageIndex = 0;
			chunk.pVertexFormat->pStreams[0].pElements[3].offset = 28;
			chunk.pVertexFormat->pStreams[0].pElements[3].type = MFMVDT_Float2;
			chunk.pVertexFormat->pStreams[0].pElements[3].usage = MFVET_TexCoord;
			chunk.pVertexFormat->pStreams[0].pElements[3].usageIndex = 0;

			if(bAnimating)
			{
				chunk.pVertexFormat->pStreams[1].pStreamName = NULL;
				chunk.pVertexFormat->pStreams[1].numVertexElements = 2;
				chunk.pVertexFormat->pStreams[1].streamStride = sizeof(AnimVert);

#if defined(SUPPORT_D3D8)
				chunk.pVertexFormat->pStreams[1].pElements[0].type = MFMVDT_ColourBGRA;
				chunk.pVertexFormat->pStreams[1].pElements[1].type = MFMVDT_ColourBGRA;
#else
				chunk.pVertexFormat->pStreams[1].pElements[0].type = MFMVDT_UByte4;
				chunk.pVertexFormat->pStreams[1].pElements[1].type = MFMVDT_UByte4N;
#endif
				chunk.pVertexFormat->pStreams[1].pElements[0].offset = 0;
				chunk.pVertexFormat->pStreams[1].pElements[0].usage = MFVET_Indices;
				chunk.pVertexFormat->pStreams[1].pElements[0].usageIndex = 0;
				chunk.pVertexFormat->pStreams[1].pElements[1].offset = 4;
				chunk.pVertexFormat->pStreams[1].pElements[1].usage = MFVET_Weights;
				chunk.pVertexFormat->pStreams[1].pElements[1].usageIndex = 0;
			}

			// setup the rest of the pointers
			chunk.ppVertexStreams = (void**)pOffset;
			pOffset += MFALIGN16(sizeof(void**)*numVertexStreams);
			chunk.ppVertexStreams[0] = pOffset;
			pOffset += MFALIGN16(chunk.pVertexFormat->pStreams[0].streamStride*numVertices);
			if(numVertexStreams > 1)
			{
				chunk.ppVertexStreams[1] = pOffset;
				pOffset += MFALIGN16(chunk.pVertexFormat->pStreams[1].streamStride*numVertices);
			}

			chunk.pIndexData = (uint16*)pOffset;
			pOffset += MFALIGN16(sizeof(uint16)*numIndices);
			chunk.pBatchIndices = bAnimating ? (uint16*)pOffset : 0;
			pOffset += bAnimating ? MFALIGN16(sizeof(uint16)*chunk.matrixBatchSize) : 0;

			// write vertices
			Vert *pVert = (Vert*)chunk.ppVertexStreams[0];
			AnimVert *pAnimVert = (AnimVert*)chunk.ppVertexStreams[1];

			for(c=0; c<numVertices; c++)
			{
				int p = batch.vertices[c];

				const F3DVertex &vert = matsub.vertices[p];

				int posIndex = vert.position;
				int normalIndex = vert.normal;
				int uvIndex = vert.uv1;
				int colourIndex = vert.colour;

				const MFVector &pos = posIndex > -1 ? sub.positions[posIndex] : MFVector::zero;
				const MFVector &normal = normalIndex > -1 ? sub.normals[normalIndex] : MFVector::up;
				const MFVector &uv = uvIndex > -1 ? sub.uvs[uvIndex] : MFVector::zero;
				uint32 colour = colourIndex > -1 ? sub.colours[colourIndex].ToPackedColour() : 0xFFFFFFFF;

				pVert[c].pos[0] = pos.x;
				pVert[c].pos[1] = pos.y;
				pVert[c].pos[2] = pos.z;
				pVert[c].normal[0] = normal.x;
				pVert[c].normal[1] = normal.y;
				pVert[c].normal[2] = normal.z;
				pVert[c].colour = colour;
				pVert[c].uv[0] = uv.x;
				pVert[c].uv[1] = uv.y;

				pVolume->min = MFMin(pVolume->min, pos);
				pVolume->max = MFMax(pVolume->max, pos);
				AdjustBoundingSphere(pos, &pVolume->boundingSphere);

				if(bAnimating)
				{
					pAnimVert[c].i.w1 = vert.bone[0] != -1 ? batch.boneMapping[vert.bone[0]] * 3 : 0;
					pAnimVert[c].i.w2 = vert.bone[1] != -1 ? batch.boneMapping[vert.bone[1]] * 3 : 0;
					pAnimVert[c].i.w3 = vert.bone[2] != -1 ? batch.boneMapping[vert.bone[2]] * 3 : 0;
					pAnimVert[c].i.w4 = vert.bone[3] != -1 ? batch.boneMapping[vert.bone[3]] * 3 : 0;
					pAnimVert[c].w.w1 = (uint8)(vert.weight[0] * 255.0f);
					pAnimVert[c].w.w2 = (uint8)(vert.weight[1] * 255.0f);
					pAnimVert[c].w.w3 = (uint8)(vert.weight[2] * 255.0f);
					pAnimVert[c].w.w4 = (uint8)(vert.weight[3] * 255.0f);

					// we need to make sure the weights sum to 255
					int leftOver = 255 - pAnimVert[c].w.w1 - pAnimVert[c].w.w2 - pAnimVert[c].w.w3 - pAnimVert[c].w.w4;

					uint8 *pW = (uint8*)&pAnimVert[c].w;
					int d, biggest = 0, weight = pW[0];
					for(d=1; d<4; d++)
					{
						biggest = pW[d] > weight ? d : biggest;
						weight = MFMax(weight, (int)pW[d]);
					}

					pW[biggest] += leftOver;
				}
			}

			// write indices
			uint16 *pIndices = (uint16*)chunk.pIndexData;

			for(c=0; c<numTriangles; c++)
			{
				int t = batch.tris[c];

				pIndices[0] = batch.vertexMapping[matsub.triangles[t].v[0]];
				pIndices[1] = batch.vertexMapping[matsub.triangles[t].v[1]];
				pIndices[2] = batch.vertexMapping[matsub.triangles[t].v[2]];

				pIndices += 3;
			}

			// write out batch
			if(bAnimating)
			{
				for(int c=0; c<chunk.matrixBatchSize; c++)
					chunk.pBatchIndices[c] = (uint16)batch.bones[c];
			}

			++mc;
		}
	}
}

void FixUpMeshChunk_Generic(MFMeshChunk *pMeshChunks, int count, void *pBase, void *pStringBase)
{
	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunks;

	for(int a=0; a<count; a++)
	{
		MFFixUp(pMC[a].pMaterial, pStringBase, 0);
		for(int b=0; b<pMC[a].pVertexFormat->numVertexStreams; ++b)
		{
			if(pMC[a].pVertexFormat->pStreams[b].pStreamName)
				MFFixUp(pMC[a].pVertexFormat->pStreams[b].pStreamName, pStringBase, 0);
			MFFixUp(pMC[a].pVertexFormat->pStreams[b].pElements, pBase, 0);
			MFFixUp(pMC[a].ppVertexStreams[b], pBase, 0);
		}
		MFFixUp(pMC[a].pVertexFormat->pStreams, pBase, 0);
		MFFixUp(pMC[a].pVertexFormat, pBase, 0);
		MFFixUp(pMC[a].ppVertexStreams, pBase, 0);
		MFFixUp(pMC[a].pIndexData, pBase, 0);
		MFFixUp(pMC[a].pBatchIndices, pBase, 0);
	}
}

void WriteMeshChunk_XB(F3DFile *pModel, MFMeshChunk *pMeshChunks, const F3DSubObject &sub, char *&pOffset, MFStringCache *pStringCache, MFBoundingVolume *pVolume)
{
#if 0//!defined(_LINUX)
	int numMeshChunks = 0;
	int a, b, mc = 0;

	// count valid mesh chunks
	for(a=0; a<sub.matSubobjects.size(); a++)
	{
		if(sub.matSubobjects[a].triangles.size() != 0)
			++numMeshChunks;
	}

	// increment size of MeshChunk_PC structure
	pOffset += MFALIGN16(sizeof(MFMeshChunk_D3D9)*numMeshChunks);

	MFMeshChunk_XB *pMeshChunk = (MFMeshChunk_XB*)pMeshChunks;

	// fill out mesh chunk, and build mesh...
	for(a=0; a<sub.matSubobjects.size(); a++)
	{
		struct Vert
		{
			float pos[3];
			float normal[3];
			uint32 colour;
			float uv[2];
		};

		if(sub.matSubobjects[a].triangles.size() == 0)
			continue;

		int numVertices = sub.matSubobjects[a].vertices.size();
		int numTriangles = sub.matSubobjects[a].triangles.size();
		int numIndices = numTriangles*3;

		pMeshChunk[mc].numVertices = numVertices;
		pMeshChunk[mc].numIndices = numIndices;
		pMeshChunk[mc].vertexStride = sizeof(Vert);
		pMeshChunk[mc].vertexDataSize = numVertices * pMeshChunk->vertexStride;
		pMeshChunk[mc].indexDataSize = numIndices*sizeof(uint16);

		pMeshChunk[mc].type = MFMCT_XB;
		pMeshChunk[mc].pMaterial = (MFMaterial*)MFStringCache_Add(pStringCache, pModel->GetMaterialChunk()->materials[sub.matSubobjects[a].materialIndex].name);

		// setup pointers
		pMeshChunk[mc].pVertexElements = (D3DVERTEXELEMENT9*)pOffset;
		pOffset += MFALIGN16(sizeof(D3DVERTEXELEMENT9)*5);
		pMeshChunk[mc].pVertexData = pOffset;
		pOffset += MFALIGN16(sizeof(Vert)*numVertices);
		pMeshChunk[mc].pIndexData = pOffset;
		pOffset += MFALIGN16(sizeof(uint16)*numIndices);

		// write declaration
		pMeshChunk[mc].pVertexElements[0].Stream = 0;
		pMeshChunk[mc].pVertexElements[0].Offset = 0;
		pMeshChunk[mc].pVertexElements[0].Type = D3DDECLTYPE_FLOAT3;
		pMeshChunk[mc].pVertexElements[0].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[mc].pVertexElements[0].Usage = D3DDECLUSAGE_POSITION;
		pMeshChunk[mc].pVertexElements[0].UsageIndex = 0;

		pMeshChunk[mc].pVertexElements[1].Stream = 0;
		pMeshChunk[mc].pVertexElements[1].Offset = 12;
		pMeshChunk[mc].pVertexElements[1].Type = D3DDECLTYPE_FLOAT3;
		pMeshChunk[mc].pVertexElements[1].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[mc].pVertexElements[1].Usage = D3DDECLUSAGE_NORMAL;
		pMeshChunk[mc].pVertexElements[1].UsageIndex = 0;

		pMeshChunk[mc].pVertexElements[2].Stream = 0;
		pMeshChunk[mc].pVertexElements[2].Offset = 24;
		pMeshChunk[mc].pVertexElements[2].Type = D3DDECLTYPE_D3DCOLOR;
		pMeshChunk[mc].pVertexElements[2].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[mc].pVertexElements[2].Usage = D3DDECLUSAGE_COLOR;
		pMeshChunk[mc].pVertexElements[2].UsageIndex = 0;

		pMeshChunk[mc].pVertexElements[3].Stream = 0;
		pMeshChunk[mc].pVertexElements[3].Offset = 28;
		pMeshChunk[mc].pVertexElements[3].Type = D3DDECLTYPE_FLOAT2;
		pMeshChunk[mc].pVertexElements[3].Method = D3DDECLMETHOD_DEFAULT;
		pMeshChunk[mc].pVertexElements[3].Usage = D3DDECLUSAGE_TEXCOORD;
		pMeshChunk[mc].pVertexElements[3].UsageIndex = 0;

		D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
		pMeshChunk[mc].pVertexElements[4] = endMacro;

		// write vertices
		Vert *pVert = (Vert*)pMeshChunk[mc].pVertexData;

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
		uint16 *pIndices = (uint16*)pMeshChunk[mc].pIndexData;

		for(b=0; b<numTriangles; b++)
		{
			pIndices[0] = sub.matSubobjects[a].triangles[b].v[0];
			pIndices[1] = sub.matSubobjects[a].triangles[b].v[1];
			pIndices[2] = sub.matSubobjects[a].triangles[b].v[2];

			pIndices += 3;
		}

		++mc;
	}
#endif
}

void FixUpMeshChunk_XB(MFMeshChunk *pMeshChunks, int count, void *pBase, void *pStringBase)
{
#if 0//!defined(_LINUX)
	MFMeshChunk_XB *pMC = (MFMeshChunk_XB*)pMeshChunks;

	for(int a=0; a<count; a++)
	{
		MFFixUp(pMC[a].pMaterial, pStringBase, 0);
		MFFixUp(pMC[a].pVertexData, pBase, 0);
		MFFixUp(pMC[a].pIndexData, pBase, 0);
		MFFixUp(pMC[a].pVertexElements, pBase, 0);
	}
#endif
}

void WriteMeshChunk_PSP(F3DFile *pModel, MFMeshChunk *pMeshChunks, const F3DSubObject &sub, char *&pOffset, MFStringCache *pStringCache, MFBoundingVolume *pVolume)
{
	int numMeshChunks = 0;
	int a, b, c, mc = 0;

	// count valid mesh chunks
	for(a=0; a<sub.matSubobjects.size(); a++)
	{
		if(sub.matSubobjects[a].triangles.size() != 0)
			++numMeshChunks;
	}

	// increment size of MeshChunk_PSP structure
	pOffset += MFALIGN16(sizeof(MFMeshChunk_PSP)*numMeshChunks);

	MFMeshChunk_PSP *pMeshChunk = (MFMeshChunk_PSP*)pMeshChunks;

	bool subobjectAnimation = (sub.IsSubobjectAnimation() != -1);

	struct Vert
	{
		float uv[2];
		uint32 colour;
		float normal[3];
		float pos[3];
	};

	struct AnimVert
	{
		uint8 weights[8];
		float uv[2];
		uint32 colour;
		float normal[3];
		float pos[3];
	};

	// fill out mesh chunk, and build mesh...
	for(a=0; a<sub.matSubobjects.size(); a++)
	{
		const F3DMaterialSubobject &matsub = sub.matSubobjects[a];

		if(matsub.triangles.size() == 0)
			continue;

		bool bAnimating = (matsub.numBones && !subobjectAnimation);

		for(b=0; b<matsub.triangleBatches.size(); b++)
		{
			const F3DBatch &batch = matsub.triangleBatches[b];

			int numTriangles = batch.tris.size();
			int numVertices = numTriangles * 3;

			pMeshChunk[mc].numVertices = numVertices;
			pMeshChunk[mc].vertexStride = bAnimating ? sizeof(AnimVert) : sizeof(Vert);
			pMeshChunk[mc].vertexDataSize = pMeshChunk[mc].numVertices * pMeshChunk[mc].vertexStride;
			pMeshChunk[mc].vertexFormat = (0x03 << 0) | (0x07 << 2) | (0x03 << 5) | (0x03 << 7) | (bAnimating ? ((0x01 << 9) | (0x07 << 14)) : 0);
			pMeshChunk[mc].maxWeights = bAnimating ? matsub.maxWeights : 0;
			pMeshChunk[mc].matrixBatchSize = batch.bones.size();

			pMeshChunk[mc].type = MFMCT_PSP;
			pMeshChunk[mc].pMaterial = (MFMaterial*)MFStringCache_Add(pStringCache, pModel->GetMaterialChunk()->materials[matsub.materialIndex].name);

			pMeshChunk[mc].pVertexData = pOffset;
			pOffset += MFALIGN16(pMeshChunk[mc].vertexDataSize);
			pMeshChunk[mc].pBatchIndices = bAnimating ? (uint16*)pOffset : 0;
			pOffset += bAnimating ? MFALIGN16(sizeof(uint16)*pMeshChunk[mc].matrixBatchSize) : 0;

			if(!bAnimating)
			{
				// write triangles
				Vert *pVert = (Vert*)pMeshChunk[mc].pVertexData;

				for(b=0; b<numTriangles; b++)
				{
					for(c=0; c<3; c++)
					{
						int t = batch.tris[b];
						int v = matsub.triangles[t].v[c];

						const F3DVertex &vert = matsub.vertices[v];

						int posIndex = vert.position;
						int normalIndex = vert.normal;
						int uvIndex = vert.uv1;
						int colourIndex = vert.colour;

						const MFVector &pos = posIndex > -1 ? sub.positions[posIndex] : MFVector::zero;
						const MFVector &uv = uvIndex > -1 ? sub.uvs[uvIndex] : MFVector::zero;
						const MFVector &norm = normalIndex > -1 ? sub.normals[normalIndex] : MFVector::up;
						const MFVector &col = colourIndex > -1 ? sub.colours[colourIndex] : MFVector::one;

						pVert->uv[0] = uv.x;
						pVert->uv[1] = uv.y;
						pVert->colour = ((uint32)(col.x * 255.0f) << 0) | ((uint32)(col.y * 255.0f) << 8) | ((uint32)(col.z * 255.0f) << 16) | ((uint32)(col.w * 255.0f) << 24);
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
			else
			{
				// write animated triangles
				AnimVert *pVert = (AnimVert*)pMeshChunk[mc].pVertexData;

				for(b=0; b<numTriangles; b++)
				{
					for(c=0; c<3; c++)
					{
						int t = batch.tris[b];
						int v = matsub.triangles[t].v[c];

						const F3DVertex &vert = matsub.vertices[v];

						int posIndex = vert.position;
						int normalIndex = vert.normal;
						int uvIndex = vert.uv1;
						int colourIndex = vert.colour;

						const MFVector &pos = posIndex > -1 ? sub.positions[posIndex] : MFVector::zero;
						const MFVector &uv = uvIndex > -1 ? sub.uvs[uvIndex] : MFVector::zero;
						const MFVector &norm = normalIndex > -1 ? sub.normals[normalIndex] : MFVector::up;
						const MFVector &col = colourIndex > -1 ? sub.colours[colourIndex] : MFVector::one;

						pVert->uv[0] = uv.x;
						pVert->uv[1] = uv.y;
						pVert->colour = ((uint32)(col.x * 255.0f) << 0) | ((uint32)(col.y * 255.0f) << 8) | ((uint32)(col.z * 255.0f) << 16) | ((uint32)(col.w * 255.0f) << 24);
						pVert->normal[0] = norm.x;
						pVert->normal[1] = norm.y;
						pVert->normal[2] = norm.z;
						pVert->pos[0] = pos.x;
						pVert->pos[1] = pos.y;
						pVert->pos[2] = pos.z;

						for(int a=0; a<8; ++a)
						{
							int bid = vert.bone[a] != -1 ? batch.boneMapping[vert.bone[a]] : -1;
							if(bid > -1)
								pVert->weights[bid] = (uint8)(vert.weight[a] * 255.0f);
						}

						// we need to make sure the weights sum to 255
						int leftOver = 255 - pVert->weights[0] - pVert->weights[1] - pVert->weights[2] - pVert->weights[3] - pVert->weights[4] - pVert->weights[5] - pVert->weights[6] - pVert->weights[7];

						int d, biggest = 0, weight = pVert->weights[0];
						for(d=1; d<8; d++)
						{
							biggest = pVert->weights[d] > weight ? d : biggest;
							weight = MFMax(weight, (int)pVert->weights[d]);
						}

						pVert->weights[biggest] += leftOver;

						pVolume->min = MFMin(pVolume->min, pos);
						pVolume->max = MFMax(pVolume->max, pos);
						AdjustBoundingSphere(pos, &pVolume->boundingSphere);

						++pVert;
					}
				}

				// write out batch
				for(int c=0; c<pMeshChunk[mc].matrixBatchSize; c++)
					pMeshChunk[mc].pBatchIndices[c] = (uint16)batch.bones[c];
			}

			++mc;
		}
	}
}

void FixUpMeshChunk_PSP(MFMeshChunk *pMeshChunks, int count, void *pBase, void *pStringBase)
{
	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunks;

	for(int a=0; a<count; a++)
	{
		MFFixUp(pMC[a].pMaterial, pStringBase, 0);
		MFFixUp(pMC[a].pVertexData, pBase, 0);
	}
}

void *F3DFile::CreateMDL(uint32 *pSize, MFPlatform platform)
{
	int a, b = 0;

	MFModelTemplate *pModelData;

	MFStringCache *pStringCache;
	pStringCache = MFStringCache_Create(1024*1024);

	char *pFile;
	char *pOffset;

	const int maxFileSize = 1024*1024*4;

	pFile = (char*)MFHeap_Alloc(maxFileSize); // allocating 10mb ... yeah this is REALLY weak! ;)
	MFZeroMemory(pFile, maxFileSize);
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
		MFModelSubObject *pSubobject = (MFModelSubObject*)pOffset;

		pDataHeaders[meshChunkIndex].pData = pSubobject;
		pDataHeaders[meshChunkIndex].count = numOutputMeshChunks;

		F3DMeshChunk *pMC = GetMeshChunk();

		// HACKY: gotta find the first matsub with mesh..
		for(a=0; a<pMC->subObjects.size(); a++)
			for(b=0; b<pMC->subObjects[a].matSubobjects.size(); b++)
				if(pMC->subObjects[a].matSubobjects[b].triangles.size())
					goto found;
found:
		MFDebug_Assert(a < pMC->subObjects.size(), "No mesh in model.");

		// prime bounding volume
		const F3DSubObject &sub0 = pMC->subObjects[a];
		pModelData->boundingVolume.boundingSphere = MakeVector(sub0.positions[sub0.matSubobjects[b].vertices[sub0.matSubobjects[b].triangles[0].v[0]].position], 0.0f);
		pModelData->boundingVolume.min = pModelData->boundingVolume.boundingSphere;
		pModelData->boundingVolume.max = pModelData->boundingVolume.boundingSphere;

		pOffset += MFALIGN16(sizeof(MFModelSubObject)*pDataHeaders[meshChunkIndex].count);

		for(a=0, b=0; a<pMC->subObjects.size(); a++)
		{
			const F3DSubObject &sub = pMC->subObjects[a];

			if(sub.dontExportThisSubobject)
				continue;

			pSubobject[b].pSubObjectName = MFStringCache_Add(pStringCache, sub.name);
//			pSubobject[b].pMaterial = (MFMaterial*)pStringCache->Add(materialChunk.materials[sub.materialIndex].name);
			pSubobject[b].numMeshChunks = 0;
			pSubobject[b].subobjectAnimMatrix = sub.IsSubobjectAnimation();

			// count valid mesh chunks
			for(int c=0; c<sub.matSubobjects.size(); c++)
			{
				if(sub.matSubobjects[c].triangles.size() != 0)
					++pSubobject[b].numMeshChunks;
			}

			MFMeshChunk *pMeshChunks = (MFMeshChunk*)pOffset;
			pSubobject[b].pMeshChunks = pMeshChunks;

			// build platform specific mesh chunk
			switch(platform)
			{
				case FP_PC:
				case FP_Linux:
				case FP_OSX:
					WriteMeshChunk_Generic(this, pMeshChunks, sub, pOffset, pStringCache, &pModelData->boundingVolume);
					break;
				case FP_XBox:
					WriteMeshChunk_XB(this, pMeshChunks, sub, pOffset, pStringCache, &pModelData->boundingVolume);
					break;
				case FP_PSP:
					WriteMeshChunk_PSP(this, pMeshChunks, sub, pOffset, pStringCache, &pModelData->boundingVolume);
					break;
				case FP_PS2:
				case FP_DC:
				case FP_GC:
				default:
					MFDebug_Warn(1, "Invalid platform...");
			}

			++b;
		}
	}

	// write out skeleton data
	if(skeletonChunkIndex > -1)
	{
		MFModelBone *pBoneChunk = (MFModelBone*)pOffset;

		int numBones = GetSkeletonChunk()->bones.size();

		pDataHeaders[skeletonChunkIndex].pData = pBoneChunk;
		pDataHeaders[skeletonChunkIndex].count = GetSkeletonChunk()->GetNumReferencedBones();

		pOffset += sizeof(MFModelBone)*pDataHeaders[skeletonChunkIndex].count;

		int *pBoneRemappingTable = (int*)MFHeap_Alloc(sizeof(int) * (numBones + 1));
		*pBoneRemappingTable = -1;
		++pBoneRemappingTable;

		int bc = 0;
		for(a=0; a<numBones; a++)
		{
			F3DBone &bone = GetSkeletonChunk()->bones[a];

			if(bone.bIsReferenced)
			{
				pBoneRemappingTable[a] = bc;

				pBoneChunk[bc].pBoneName = MFStringCache_Add(pStringCache, bone.name);
				pBoneChunk[bc].pParentName = MFStringCache_Add(pStringCache, bone.parentName);
				pBoneChunk[bc].boneMatrix = bone.boneMatrix;
				pBoneChunk[bc].worldMatrix = bone.worldMatrix;
				pBoneChunk[bc].invWorldMatrix.Inverse(bone.worldMatrix);
				pBoneChunk[bc].parent = (int16)pBoneRemappingTable[bone.parent];

				pBoneChunk[bc].numChildren = (int16)bone.children.size();
				pBoneChunk[bc].pChildren = (int16*)pOffset;

				for(int b=0; b<pBoneChunk[bc].numChildren; b++)
					pBoneChunk[bc].pChildren[b] = bone.children[b];

				pOffset += sizeof(int16)*pBoneChunk[bc].numChildren;

				++bc;
			}
			else
				pBoneRemappingTable[a] = -1;
		}

		pOffset = (char*)MFALIGN16(pOffset);

		--pBoneRemappingTable;
		MFHeap_Free(pBoneRemappingTable);
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
					MFCopyMemory(pMesh->pTriangles, pColMesh->tris.getpointer(), triBlockSize);

					pOffset += MFALIGN16(triBlockSize);
					break;
				}

				default:
					MFDebug_Assert(false, "Unsupported collision object type.");
			}
		}
	}

	// write out tag data
	if(tagChunkIndex > -1)
	{
		MFModelTag *pTags = (MFModelTag*)pOffset;

		pDataHeaders[tagChunkIndex].pData = pTags;
		pDataHeaders[tagChunkIndex].count = GetRefPointChunk()->refPoints.size();

		for(int a=0; a<pDataHeaders[tagChunkIndex].count; a++)
		{
			pTags[a].pTagName = MFStringCache_Add(pStringCache, GetRefPointChunk()->refPoints[a].name);
			pTags[a].tagMatrix = GetRefPointChunk()->refPoints[a].worldMatrix;
		}

		pOffset += MFALIGN16(sizeof(MFModelTag)*pDataHeaders[tagChunkIndex].count);
	}

	// write strings to end of file
	char *pCache = MFStringCache_GetCache(pStringCache);
	MFCopyMemory(pOffset, pCache, MFStringCache_GetSize(pStringCache));

	char *pStringBase = pCache - (pOffset-pFile);
	pOffset += MFStringCache_GetSize(pStringCache); // pOffset now equals the file size..

	// un-fix-up all the pointers...
	MFFixUp(pModelData->pName, pStringBase, 0);

	for(a=0; a<pModelData->numDataChunks; a++)
	{
		switch(pModelData->pDataChunks[a].chunkType)
		{
			case MFChunkType_SubObjects:
			{
				MFModelSubObject *pSubobjectChunk = (MFModelSubObject*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					MFFixUp(pSubobjectChunk[b].pSubObjectName, pStringBase, 0);
//					MFFixUp(pSubobjectChunk[b].pMaterial, pStringBase, 0);

					switch(platform)
					{
						case FP_PC:
						case FP_Linux:
						case FP_OSX:
							FixUpMeshChunk_Generic(pSubobjectChunk[b].pMeshChunks, pSubobjectChunk[b].numMeshChunks, pModelData, pStringBase);
							break;
						case FP_XBox:
							FixUpMeshChunk_XB(pSubobjectChunk[b].pMeshChunks, pSubobjectChunk[b].numMeshChunks, pModelData, pStringBase);
							break;
						case FP_PSP:
							FixUpMeshChunk_PSP(pSubobjectChunk[b].pMeshChunks, pSubobjectChunk[b].numMeshChunks, pModelData, pStringBase);
							break;
						case FP_PS2:
						case FP_DC:
						case FP_GC:
						default:
							MFDebug_Warn(1, "Invalid platform...");
					}

					MFFixUp(pSubobjectChunk[b].pMeshChunks, pModelData, 0);
				}
				break;
			}

			case MFChunkType_Bones:
			{
				MFModelBone *pBoneChunk = (MFModelBone*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					MFFixUp(pBoneChunk[b].pBoneName, pStringBase, 0);
					MFFixUp(pBoneChunk[b].pParentName, pStringBase, 0);
					MFFixUp(pBoneChunk[b].pChildren, pModelData, 0);
				}
				break;
			}

			case MFChunkType_Collision:
			{
				MFCollisionTemplate *pCollisionChunk = (MFCollisionTemplate*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					MFFixUp(pCollisionChunk[b].pName, pStringBase, 0);

					if(pCollisionChunk[b].type == MFCT_Mesh)
					{
						MFCollisionMesh *pColMesh = (MFCollisionMesh*)pCollisionChunk[b].pCollisionTemplateData;
						MFFixUp(pColMesh->pTriangles, pModelData, 0);
					}

					MFFixUp(pCollisionChunk[b].pCollisionTemplateData, pModelData, 0);
				}
				break;
			}

			case MFChunkType_Tags:
			{
				MFModelTag *pTags = (MFModelTag*)pModelData->pDataChunks[a].pData;

				for(b=0; b<pModelData->pDataChunks[a].count; b++)
				{
					MFFixUp(pTags[b].pTagName, pStringBase, 0);
				}
				break;
			}

			default:
				MFDebug_Assert(false, "Shouldnt be here?...");
				break;
		}

		MFFixUp(pModelData->pDataChunks[a].pData, pModelData, 0);
	}
	MFFixUp(pModelData->pDataChunks, pModelData, 0);

	size_t fileSize = pOffset - pFile;
	void *pMDL = MFHeap_Alloc(fileSize);
	MFCopyMemory(pMDL, pFile, fileSize);

	// we're done!!!! clean up..
	MFHeap_Free(pFile);

	if(pSize)
		*pSize = (uint32)fileSize;
	return pMDL;
}

void *F3DFile::CreateANM(uint32 *pSize, MFPlatform platform)
{
	if(!animationChunk.anims.size())
	{
		// no animation
		return NULL;
	}

	MFAnimationTemplate *pAnimData;
	int a, b;

	MFStringCache *pStringCache;
	pStringCache = MFStringCache_Create(1024*1024);

	char *pFile;
	char *pOffset;

	// this is a fuck load of animation!
	const int maxFileSize = 1024*1024*10;

	pFile = (char*)MFHeap_Alloc(maxFileSize); // allocating 10mb ... yeah this is REALLY weak! ;)
	MFZeroMemory(pFile, maxFileSize);
	pAnimData = (MFAnimationTemplate*)pFile;

	pAnimData->IDtag = MFMAKEFOURCC('A','N','M','2');
	pAnimData->pName = MFStringCache_Add(pStringCache, name);

	pOffset = (char*)pAnimData + MFALIGN16(sizeof(MFAnimationTemplate));

	// write data....
	float minTime, maxTime;
	minTime = animationChunk.anims[0].minTime;
	maxTime = animationChunk.anims[0].maxTime;

	pAnimData->numBones = animationChunk.anims.size();

	pAnimData->pBones = (MFAnimationBone*)pOffset;
	pOffset += MFALIGN16(sizeof(MFAnimationBone)*pAnimData->numBones);

	for(a=0; a<pAnimData->numBones; a++)
	{
		MFAnimationBone &bone = pAnimData->pBones[a];

		bone.pBoneName = MFStringCache_Add(pStringCache, skeletonChunk.bones[animationChunk.anims[a].boneID].name);
		bone.numFrames = animationChunk.anims[a].keyframes.size();

		bone.pTime = (float*)pOffset;
		pOffset += MFALIGN16(sizeof(MFAnimationFrame)*bone.numFrames);

		bone.pFrames = (MFAnimationFrame*)pOffset;
		pOffset += MFALIGN16(sizeof(MFAnimationFrame)*bone.numFrames);

		minTime = MFMin(minTime, animationChunk.anims[a].minTime);
		maxTime = MFMax(maxTime, animationChunk.anims[a].maxTime);

		// fill kay array
		for(b=0; b<pAnimData->pBones[a].numFrames; b++)
		{
			F3DKeyFrame &key = animationChunk.anims[a].keyframes[b];

			bone.pTime[b] = key.time;
			bone.pFrames[b].key = key.key;
		}
	}

	pAnimData->startTime = minTime;
	pAnimData->endTime = maxTime;

	// write strings to end of file
	char *pCache = MFStringCache_GetCache(pStringCache);
	MFCopyMemory(pOffset, pCache, MFStringCache_GetSize(pStringCache));

	uintp stringBase = (uintp)pCache - ((uintp)pOffset - (uintp)pFile);
	pOffset += MFStringCache_GetSize(pStringCache); // pOffset now equals the file size..

	// un-fix-up all the pointers...
	for(int a=0; a<pAnimData->numBones; a++)
	{
		MFFixUp(pAnimData->pBones[a].pBoneName, (void*)stringBase, 0);
		MFFixUp(pAnimData->pBones[a].pTime, pAnimData, 0);
		MFFixUp(pAnimData->pBones[a].pFrames, pAnimData, 0);
	}

	MFFixUp(pAnimData->pName, (void*)stringBase, 0);
	MFFixUp(pAnimData->pBones, pAnimData, 0);

	uintp fileSize = (uintp)pOffset - (uintp)pAnimData;
	void *pANM = MFHeap_Alloc(fileSize);
	MFCopyMemory(pANM, pFile, fileSize);

	// we're done!!!! clean up..
	MFHeap_Free(pFile);

	if(pSize)
		*pSize = (uint32)fileSize;
	return pANM;
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

bool IsBoneInBatch(const MFArray<int> &batch, int bone)
{
	if(bone == -1) return true;

	for(int a=0; a<batch.size(); a++)
	{
		if(batch[a] == bone)
			return true;
	}

	return false;
}

int GetNumBonesNotInBatch(const F3DBatch &batch, const F3DMaterialSubobject &matSub, const F3DTriangle &tri, int numBones, int *pAdded)
{
	int numNotInBatch = 0;

	MFZeroMemory(pAdded, sizeof(int)*numBones);

	for(int a=0; a<matSub.maxWeights; a++)
	{
		int b0 = matSub.vertices[tri.v[0]].bone[a];
		int b1 = matSub.vertices[tri.v[1]].bone[a];
		int b2 = matSub.vertices[tri.v[2]].bone[a];

		if(b0 > -1 && batch.boneMapping[b0] == -1 && !pAdded[b0])
		{
			pAdded[b0] = 1;
			++numNotInBatch;
		}
		if(b1 > -1 && batch.boneMapping[b1] == -1 && !pAdded[b1])
		{
			pAdded[b1] = 1;
			++numNotInBatch;
		}
		if(b2 > -1 && batch.boneMapping[b2] == -1 && !pAdded[b2])
		{
			pAdded[b2] = 1;
			++numNotInBatch;
		}
	}

	return numNotInBatch;
}

void AddVertsToBatch(F3DBatch &batch, const F3DMaterialSubobject &matSub, const F3DTriangle &tri)
{
	int v0 = tri.v[0];
	int v1 = tri.v[1];
	int v2 = tri.v[2];

	if(batch.vertexMapping[v0] == -1)
	{
		batch.vertexMapping[v0] = batch.vertices.size();
		batch.vertices.push(v0);
	}
	if(batch.vertexMapping[v1] == -1)
	{
		batch.vertexMapping[v1] = batch.vertices.size();
		batch.vertices.push(v1);
	}
	if(batch.vertexMapping[v2] == -1)
	{
		batch.vertexMapping[v2] = batch.vertices.size();
		batch.vertices.push(v2);
	}
}

void AddTriToBatch(F3DBatch &batch, const F3DMaterialSubobject &matSub, const F3DTriangle &tri)
{
	int v0 = tri.v[0];
	int v1 = tri.v[1];
	int v2 = tri.v[2];

	for(int a=0; a<matSub.maxWeights; a++)
	{
		int b0 = matSub.vertices[v0].bone[a];
		int b1 = matSub.vertices[v1].bone[a];
		int b2 = matSub.vertices[v2].bone[a];

		if(batch.boneMapping[b0] == -1)
		{
			batch.boneMapping[b0] = batch.bones.size();
			batch.bones.push(b0);
		}
		if(batch.boneMapping[b1] == -1)
		{
			batch.boneMapping[b1] = batch.bones.size();
			batch.bones.push(b1);
		}
		if(batch.boneMapping[b2] == -1)
		{
			batch.boneMapping[b2] = batch.bones.size();
			batch.bones.push(b2);
		}
	}
}

void F3DFile::BuildBatches(MFPlatform platform)
{
	// build bone batches
	int maxBones = 0;

	switch(platform)
	{
		case FP_PC:		maxBones = 50;	break;
		case FP_XBox:	maxBones = 20;	break;
		case FP_PSP:	maxBones = 8;	break;
		case FP_Linux:	maxBones = 50;	break;
		case FP_OSX:	maxBones = 50;	break;
		default:		maxBones = 256;	break;
	}

	MFArray<int> trisAdded;
	int numAdded;

	for(int a=0; a<meshChunk.subObjects.size(); a++)
	{
		for(int b=0; b<meshChunk.subObjects[a].matSubobjects.size(); b++)
		{
			F3DMaterialSubobject &matSub = meshChunk.subObjects[a].matSubobjects[b];
/*
			if(matSub.numBones < maxBones)
			{
				// do this the fast way :P
				F3DBatch &batch = matSub.triangleBatches.push();

				// ..... harder than i thought ....
			}
			else
*/
			{
				int numTris = matSub.triangles.size();

				trisAdded.resize(numTris);
				MFZeroMemory(trisAdded.getpointer(), sizeof(int)*numTris);
				numAdded = 0;

				while(numAdded < numTris)
				{
					F3DBatch &batch = matSub.triangleBatches.push();

					int numVerts = matSub.vertices.size();
					int numBones = skeletonChunk.bones.size();
					batch.vertexMapping.resize(numVerts);
					batch.boneMapping.resize(numBones);
					MFMemSet(batch.vertexMapping.getpointer(), -1, sizeof(int)*numVerts);
					MFMemSet(batch.boneMapping.getpointer(), -1, sizeof(int)*numBones);

					MFArray<int> gAdded(numBones);
					int *pAdded = gAdded.getpointer();

					for(int c=0; c<numTris; c++)
					{
						if(trisAdded[c])
							continue;

						F3DTriangle &tri = matSub.triangles[c];
						int numNotInBatch = GetNumBonesNotInBatch(batch, matSub, tri, numBones, pAdded);
						if(numNotInBatch + batch.bones.size() > maxBones)
							continue;

						AddVertsToBatch(batch, matSub, tri);
						if(numNotInBatch)
							AddTriToBatch(batch, matSub, tri);
						batch.tris.push(c);
						trisAdded[c] = 1;
						++numAdded;
					}
				}
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
	// build hierarchy and stuff
	skeletonChunk.BuildHierarchy();
	skeletonChunk.FlagReferenced(true);
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

	// the material doesn't seem to exist, we'll add one
	int matId = materials.size();
	F3DMaterial &mat = materials.push();
	MFString_Copy(mat.name, pName);

	return matId;
}

F3DMaterialSubobject::F3DMaterialSubobject()
{
	materialIndex = 0;
	numBones = 0;
	maxWeights = 0;
}

F3DSubObject::F3DSubObject()
{
	MFZeroMemory(name, 64);
//	materialIndex = 0;

	dontExportThisSubobject = false;
}

int F3DSubObject::IsSubobjectAnimation() const
{
	int subobject = -1;

	for(int a=0; a<matSubobjects.size(); a++)
	{
		if(matSubobjects[a].numBones != 1)
			return -1;

		MFDebug_Assert(matSubobjects[a].triangleBatches.size() == 1 && matSubobjects[a].triangleBatches[0].bones.size() == 1, "Internal error!!");

		if(subobject != -1)
		{
			if(subobject != matSubobjects[a].triangleBatches[0].bones[0])
				return -1;
		}
		else
		{
			subobject = matSubobjects[a].triangleBatches[0].bones[0];
		}
	}

	return subobject;
}

int F3DSkeletonChunk::FindBone(const char *pName) const
{
	if(!pName)
		return 0;

	for(int a=0; a<bones.size(); a++)
	{
		if(!MFString_Compare(pName, bones[a].name))
			return a;
	}

	return -1;
}

void F3DSkeletonChunk::BuildHierarchy()
{
	for(int a=0; a<bones.size(); a++)
	{
		bones[a].parent = FindBone(bones[a].parentName);

		if(bones[a].parent > -1)
		{
			bones[bones[a].parent].children.push(a);
		}
	}
}

void F3DSkeletonChunk::FlagReferenced(bool bAll)
{
	for(int a=0; a<bones.size(); a++)
	{
		if((bones[a].bIsSkinned || bAll) && !bones[a].bIsReferenced)
		{
			F3DBone *pBone = &bones[a];
			pBone->bIsReferenced = true;

			while(pBone->parent != -1)
			{
				pBone = &bones[pBone->parent];
				pBone->bIsReferenced = true;
			}
		}
	}
}

int F3DSkeletonChunk::GetNumReferencedBones() const
{
	int numBones = 0;

	for(int a=0; a<bones.size(); a++)
	{
		if(bones[a].bIsReferenced)
			++numBones;
	}

	return numBones;
}

F3DBone::F3DBone()
{
	MFZeroMemory(name, 64);
	MFZeroMemory(parentName, 64);
	MFZeroMemory(options, 1024);

	boneMatrix = MFMatrix::identity;
	worldMatrix = MFMatrix::identity;

	parent = -1;
	bIsSkinned = false;
	bIsReferenced = false;
}

F3DAnimation::F3DAnimation()
{
	boneID = -1;
	minTime = 0.0f;
	maxTime = 0.0f;
}

void F3DAnimation::Optimise(float tolerance)
{

}

F3DKeyFrame::F3DKeyFrame()
{
	time = 0.0f;
	key = MFMatrix::identity;
	rotation = MFQuaternion::identity;
	scale = MFVector::one;
	translation = MFVector::identity;
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
	bone[0] = bone[1] = bone[2] = bone[3] = bone[4] = bone[5] = bone[6] = bone[7] = -1;
	weight[0] = weight[1] = weight[2] = weight[3] = weight[4] = weight[5] = weight[6] = weight[7] = 0.0f;
}

bool F3DVertex::operator==(const F3DVertex &v) const
{
	return !memcmp(this, &v,sizeof(*this));
}

F3DMaterial::F3DMaterial()
{
	diffuse = MFVector::one;
	ambient = MFVector::one;
	emissive = MFVector::zero;
	specular = MFVector::zero;
	specularLevel = 0.0f;
	glossiness = 0.0f;

	MFString_Copy(name, "");
	for(int a=0; a<8; a++)
	{
		MFString_Copy(maps[a], "");
	}
}

F3DRefPoint::F3DRefPoint()
{
	worldMatrix = MFMatrix::identity;
	localMatrix = MFMatrix::identity;
	bone[0] = -1; bone[1] = -1; bone[2] = -1; bone[3] = -1;
	weight[0] = 0.0f; weight[1] = 0.0f; weight[2] = 0.0f; weight[3] = 0.0f;
	MFString_Copy(name, "");
	MFString_Copy(options, "");
}

F3DRefMesh::F3DRefMesh()
{
	worldMatrix = MFMatrix::identity;
	localMatrix = MFMatrix::identity;
	name[0] = 0;
	target[0] = 0;
	options[0] = 0;
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
