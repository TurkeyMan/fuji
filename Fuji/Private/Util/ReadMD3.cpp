#include "Fuji.h"
#include "Util/F3D.h"

#include "zlib.h"
#include "zlib.h"
#include "zlib/minizip/zip.h"
#include "zlib/minizip/unzip.h"

static F3DFile *pModel;

typedef float MD3Vector[3];

typedef struct
{
	char        tagName[64];
	MD3Vector	origin;
	float       rotationMatrix[3][3];
} MD3Tag;

typedef struct
{
	float u, v;
} MD3TexCoord;

typedef struct
{
	signed short x;
	signed short y;
	signed short z;
	unsigned short encodedNormal;
} MD3Vertex;


typedef struct
{
	int indices[3];
} MD3Triangle;

typedef struct
{
	char shaderName[64];
	int  shaderIndex;
} MD3Shader;

typedef struct
{
	int startFrame;
	int endFrame;
	float duration;
} MD3Action;

typedef struct
{
	char    identity[4];
	char    surfaceName[64];
	int     flags;
	int     numFrames;
	int     numSkins;
	int     numVertices;
	int     numTriangles;
	MD3Triangle *pTriangles;
	int     headerSize;
	MD3TexCoord *pTextureCoords;
	MD3Vertex *pVertices;
	int     surfaceSize;
} MD3Surface;

typedef struct
{
	char identity[4];
	int  version;
	char fileName[64];
	int  flags;
	int  numFrames;
	int  numTags;
	int  numSurfaces;
	int  numSkins;
	int  headerSize;
	MD3Tag *pTags;
	MD3Surface *pSurfaces;
	uint32 offsetEnd;
} MD3Header;

MFVector MD3DecodeNormal(unsigned short code)
{
	float latitude = ((float)(code & 0xFF)) * (2 * MFPI) / ((float)255);
	float longtitude = ((float)((code >> 8) & 0xFF)) * (2 * MFPI) / ((float)255);

	return MakeVector(-(float)(MFCos(latitude) * MFSin(longtitude)), -(float)(MFSin(latitude) * MFSin(longtitude)), -(float)(MFCos(longtitude)));
}

const char* GetMaterialName(const char *pSkin, const char *pSubobjectName)
{
	if(pSkin)
	{
		char *pTok = MFString_Chr(pSkin, ',');

		while(pTok)
		{
			char *pT = pTok-1;

			for(; pT > pSkin && pT[-1] != '\n'; --pT) { }

			// get subobject name
			char *pTokTemp = pTok - (uint32&)pT;
			const char *pSubName = MFStrN(pT, (int&)pTokTemp);

			++pTok;

			if(!MFString_CaseCmp(pSubName, pSubobjectName))
			{
				for(pT = pTok; *pT != 0 && *pT != '\r' && *pT != '\n'; ++pT) { }

				// get texture name
				pTokTemp = pT - (uint32&)pTok;
				char *pMaterialName = (char*)MFStrN(pTok, (int&)pTokTemp);

				for(pT = pMaterialName+MFString_Length(pMaterialName); pT > pMaterialName && pT[-1] != '/' && pT[-1] != '\\' && pT[-1] != '\n' && pT[-1] != '\r'; --pT) { }
				pT[MFString_Length(pT) - 4] = 0;

				return pT;
			}

			pTok = MFString_Chr(pTok, ',');
		}
	}

	return NULL;
}

void ParseMD3File(char *pBuffer, uint32 bufferSize, const char *pFilename, const char *pSkin)
{
	int a,b;

	F3DMeshChunk *pMC = pModel->GetMeshChunk();
	F3DSkeletonChunk *pSC = pModel->GetSkeletonChunk();

	MD3Header *pHeader = (MD3Header*)pBuffer;

//	DBGASSERT(pHeader->ident == (('3'<<24) | ('P'<<16) | ('D'<<8) | 'I'), "Invalid MD3 header.");
//	DBGASSERT(pHeader->version == 15, "Invalid MD3 version.");
//	DBGASSERT(pHeader->offsetEnd == bufferSize, "Incorrect MD3 Size.");

	(char*&)pHeader->pTags += (uint32&)pBuffer;
	(char*&)pHeader->pSurfaces += (uint32&)pBuffer;

	// read materials
	if(pSkin)
	{
		char *pT, *pTok = MFString_Chr(pSkin, ',');

		while(pTok)
		{
			++pTok;
			for(pT = pTok; *pT != 0 && *pT != '\r' && *pT != '\n'; ++pT) { }

			// get texture name
			char *pTT = pT - (uint32&)pTok;
			char *pMaterialName = (char*)MFStrN(pTok, (int&)pTT);

			for(pT = pMaterialName+MFString_Length(pMaterialName); pT > pMaterialName && pT[-1] != '/' && pT[-1] != '\\' && pT[-1] != '\n' && pT[-1] != '\r'; --pT) { }
			pT[MFString_Length(pT) - 4] = 0;

			if(*pT && pModel->GetMaterialChunk()->GetMaterialIndexByName(pT) == -1)
			{
				F3DMaterial &mat = pModel->GetMaterialChunk()->materials.push();

				MFString_Copy(mat.name, pT);
				MFString_Copy(mat.maps[0], pT);
			}

			pTok = MFString_Chr(pTok, ',');
		}
	}

	// process bones
	for(a=0; a<pHeader->numTags; a++)
	{
		F3DBone &bone = pSC->bones.push();

		MFString_Copy(bone.name, pHeader->pTags[a].tagName);

		bone.worldMatrix.SetXAxis3(MakeVector(pHeader->pTags[a].rotationMatrix[0][0], pHeader->pTags[a].rotationMatrix[0][1], pHeader->pTags[a].rotationMatrix[0][2]));
		bone.worldMatrix.SetYAxis3(MakeVector(pHeader->pTags[a].rotationMatrix[1][0], pHeader->pTags[a].rotationMatrix[1][1], pHeader->pTags[a].rotationMatrix[1][2]));
		bone.worldMatrix.SetZAxis3(MakeVector(pHeader->pTags[a].rotationMatrix[2][0], pHeader->pTags[a].rotationMatrix[2][1], pHeader->pTags[a].rotationMatrix[2][2]));
		bone.worldMatrix.SetTrans3(MakeVector(pHeader->pTags[a].origin[0], pHeader->pTags[a].origin[1], pHeader->pTags[a].origin[2]));
	}

	// process mesh
	for(a=0; a<pHeader->numSurfaces; a++)
	{
//		DBGASSERT(pHeader->pSurfaces->identity == (('3'<<24) | ('P'<<16) | ('D'<<8) | 'I'), "Invalid MD3 surface header.");

		(char*&)pHeader->pSurfaces->pTextureCoords += (uint32&)pHeader->pSurfaces;
		(char*&)pHeader->pSurfaces->pTriangles += (uint32&)pHeader->pSurfaces;
		(char*&)pHeader->pSurfaces->pVertices += (uint32&)pHeader->pSurfaces;

		F3DSubObject &sub = pMC->subObjects.push();
		F3DMaterialSubobject &matSub = sub.matSubobjects[0];

		// read subobject name
		MFString_Copy(sub.name, pHeader->pSurfaces->surfaceName);

		// find material info
		matSub.materialIndex = pModel->GetMaterialChunk()->GetMaterialIndexByName(GetMaterialName(pSkin, sub.name));

		// create one colour (white)
		sub.colours.resize(1);
		sub.colours[0] = MFVector::one;

		// read vertex data
		sub.uvs.resize(pHeader->pSurfaces->numVertices);
		sub.positions.resize(pHeader->pSurfaces->numVertices);
		sub.normals.resize(pHeader->pSurfaces->numVertices);

		for(b=0; b<pHeader->pSurfaces->numVertices; b++)
		{
			sub.uvs[b] = MakeVector(pHeader->pSurfaces->pTextureCoords[b].u, pHeader->pSurfaces->pTextureCoords[b].v);
			sub.normals[b] = MD3DecodeNormal(pHeader->pSurfaces->pVertices[b].encodedNormal);
			sub.positions[b] = MakeVector(pHeader->pSurfaces->pVertices[b].x, pHeader->pSurfaces->pVertices[b].y, pHeader->pSurfaces->pVertices[b].z);

			// scale vertex appropriately..
//			sub.positions[b].Mul3(sub.positions[b], 1.0f/6400.0f);
			sub.positions[b].Mul3(sub.positions[b], 1.0f/64.0f);

			matSub.vertices[b].colour = 0;
			matSub.vertices[b].normal = b;
			matSub.vertices[b].uv1 = b;
			matSub.vertices[b].position = b;
		}

		// read triangles
		matSub.triangles.resize(pHeader->pSurfaces->numTriangles);

		for(b=0; b<pHeader->pSurfaces->numTriangles; b++)
		{
			matSub.triangles[b].v[0] = pHeader->pSurfaces->pTriangles[b].indices[0];
			matSub.triangles[b].v[1] = pHeader->pSurfaces->pTriangles[b].indices[2];
			matSub.triangles[b].v[2] = pHeader->pSurfaces->pTriangles[b].indices[1];
		}

		// skip to next surface
		(char*&)pHeader->pSurfaces += (uint32&)pHeader->pSurfaces->surfaceSize;
	}
}

int F3DFile::ReadMD3(const char *pFilename)
{
	pModel = this;

	char *pBuffer = NULL;

	// open .pk3 file
	unzFile zipFile = unzOpen(pFilename);

	// iterate files in zip
	int zipFileIndex = unzGoToFirstFile(zipFile);

	while(zipFileIndex != UNZ_END_OF_LIST_OF_FILE)
	{
		MFDebug_Assert(zipFileIndex == UNZ_OK, "Error in .zip file.");

		char fileName[256];

		unz_file_info fileInfo;
		unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, 256, NULL, 0, NULL, 0);

		int filenameLen = MFString_Length(fileName);

		if(!MFString_CaseCmp(".md3", &fileName[MFMax(filenameLen - 4, 0)]))
		{
			// read fle from zip
			pBuffer = (char*)malloc(fileInfo.uncompressed_size);

			unzOpenCurrentFile(zipFile);
			uint32 bytesRead = unzReadCurrentFile(zipFile, pBuffer, fileInfo.uncompressed_size);
			unzCloseCurrentFile(zipFile);

			MFDebug_Assert(bytesRead == fileInfo.uncompressed_size, "Incorrect number of bytes read..");

			// get subobject and model name..
			int a, b;
			for(a = filenameLen - 4; a >= 0; a--)
			{
				if(fileName[a] == '/' || fileName[a] == '\\')
					break;
			}

			char *pSubobjectName = &fileName[a+1];

			for(b = a-1; b >= 0; b--)
			{
				if(fileName[b] == '/' || fileName[b] == '\\')
					break;
			}

			MFString_Copy(pModel->name, &fileName[b+1]);
			pModel->name[a-b-1] = 0;

			// search for skin file
			char skinFilename[256];
			MFString_Copy(skinFilename, fileName);
			skinFilename[MFString_Length(skinFilename) - 4] = 0;
			MFString_Cat(skinFilename, "_");
			MFString_Cat(skinFilename, pModel->name);
			MFString_Cat(skinFilename, ".skin");

			// attempt to read skin..
			char *pSkinFile = NULL;

			zipFileIndex = unzLocateFile(zipFile, skinFilename, 0);

			if(zipFileIndex != UNZ_END_OF_LIST_OF_FILE)
			{
				// read skin file from zip
				unz_file_info skinInfo;
				char skinName[256];

				unzGetCurrentFileInfo(zipFile, &skinInfo, skinName, 256, NULL, 0, NULL, 0);

				pSkinFile = (char*)malloc(skinInfo.uncompressed_size + 1);
				pSkinFile[skinInfo.uncompressed_size] = 0;

				unzOpenCurrentFile(zipFile);
				uint32 skinBytesRead = unzReadCurrentFile(zipFile, pSkinFile, skinInfo.uncompressed_size);
				unzCloseCurrentFile(zipFile);

				MFDebug_Assert(skinBytesRead == skinInfo.uncompressed_size, "Incorrect number of bytes read..");
			}

			zipFileIndex = unzLocateFile(zipFile, fileName, 0);

			// parse MD3
			ParseMD3File(pBuffer, fileInfo.uncompressed_size, pSubobjectName, pSkinFile);

			// free file
			free(pBuffer);
			pBuffer = NULL;
		}
/*
		else if(!MFString_CaseCmp(".skin", &fileName[Max(filenameLen - 5, 0)]))
		{
			int a, b;
			char skinName[256];

			// get subobject and model name
			for(a = filenameLen - 5; a >= 0; a--)
			{
				if(fileName[a] == '/' || fileName[a] == '\\')
					break;
			}

			char *pSkinName = &fileName[a+1];

			for(b = a-1; b >= 0; b--)
			{
				if(fileName[b] == '/' || fileName[b] == '\\')
					break;
			}

			MFString_Copy(skinName, &fileName[b+1]);
			skinName[a-b-1] = 0;

			pSkinName = strchr(pSkinName, '_');
			DBGASSERT(pSkinName, "Incorrectly named .skin file in .pk3");
			++pSkinName;

			// check that this is the default skin for this model
			if(!MFString_CaseCmpN(pSkinName, skinName, MFString_Length(skinName)))
			{
				// read material file from zip
				pBuffer = (char*)malloc(fileInfo.uncompressed_size);

				unzOpenCurrentFile(zipFile);
				uint32 bytesRead = unzReadCurrentFile(zipFile, pBuffer, fileInfo.uncompressed_size);
				unzCloseCurrentFile(zipFile);

				// parse .skin file

				free(pBuffer);
				pBuffer = NULL;
			}
			else
			{
				// we have an alternate skin.. do nothing..
			}
		}
*/

		zipFileIndex = unzGoToNextFile(zipFile);
	}

	// close .pk3 file
	unzClose(zipFile);

	return 0;
}
