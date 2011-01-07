#include "Fuji.h"
#include "Util/F3D.h"

#include "MFFileSystem.h"

static F3DFile *pModel;

static bool vertsInGroup = false;
static bool vertsInMatSub = false;
static int subObject = 0;
static int matSub = 0;

static int minVertIndex = -1;
static int minUVIndex = -1;
static int minNormIndex = -1;
static int maxVertIndex = -1;
static int maxUVIndex = -1;
static int maxNormIndex = -1;

static MFArray<MFVector> verts;
static MFArray<MFVector> uvs;
static MFArray<MFVector> normals;

const char *GetNextToken(const char *&pString)
{
	while(MFIsWhite(*pString) || MFIsNewline(*pString))
		++pString;

	const char *pEnd = pString;

	while(*pEnd && !MFIsWhite(*pEnd) && !MFIsNewline(*pEnd))
		++pEnd;

	const char *pToken = MFStrN(pString, (int)((uintp)pEnd-(uintp)pString));
	pString = pEnd;

	return pToken;
}

const char *GetNextIndex(const char *&pString)
{
	if(*pString == '/')
		++pString;

	const char *pEnd = pString;

	while(*pEnd && *pEnd != '/')
		++pEnd;

	const char *pToken = MFStrN(pString, (int)((uintp)pEnd-(uintp)pString));
	pString = pEnd;

	return pToken;
}

const char *GetRestOfLine(const char *&pString)
{
	while(MFIsWhite(*pString))
		++pString;

	const char *pEnd = pString;
	while(*pEnd && !MFIsNewline(*pEnd))
		++pEnd;

	const char *pRestOfLine = MFStrN(pString, (int)((uintp)pEnd - (uintp)pString));
	pString = pEnd;

	return pRestOfLine;
}

int AddToMaterials(const char *pMatName)
{
	F3DMaterialChunk *pChunk = pModel->GetMaterialChunk();

	for(int a=0; a<pChunk->materials.size(); a++)
	{
		if(!MFString_CaseCmp(pChunk->materials[a].name, pMatName))
			return a;
	}

	int i = pChunk->materials.size();

	F3DMaterial &mat = pChunk->materials.push();
	MFString_Copy(mat.name, pMatName);

	return i;
}

int GetMaterialID(const char *pMatName)
{
	return pModel->GetMaterialChunk()->GetMaterialIndexByName(pMatName);
}

void CopyDataIntoSubobject(int subobject)
{
	F3DSubObject &sub = pModel->GetMeshChunk()->subObjects[subobject];

	int a;

	for(a=minVertIndex; a<=maxVertIndex; a++)
		sub.positions.push(verts[a]);

	if(minUVIndex != -1)
	{
		for(a=minUVIndex; a<=maxUVIndex; a++)
			sub.uvs.push(uvs[a]);
	}

	if(minNormIndex != -1)
	{
		for(a=minNormIndex; a<=maxNormIndex; a++)
			sub.normals.push(normals[a]);
	}

	for(a=0; a<sub.matSubobjects.size(); a++)
	{
		for(int b=0; b<sub.matSubobjects[a].vertices.size(); b++)
		{
			F3DVertex &vert = sub.matSubobjects[a].vertices[b];

			vert.position -= minVertIndex;
			if(minUVIndex > 0)
				vert.uv1 -= minUVIndex;
			if(minNormIndex > 0)
				vert.normal -= minNormIndex;
		}
	}
}

void ParseOBJFile(const char *pFilePtr)
{
	const char *pToken = GetNextToken(pFilePtr);

	while(*pToken != 0)
	{
		if(!MFString_CaseCmp(pToken, "o"))
		{
			const char *pName = GetRestOfLine(pFilePtr);

			MFString_Copy(pModel->name, pName);
		}
		else if(!MFString_CaseCmp(pToken, "g"))
		{
			const char *pName = GetRestOfLine(pFilePtr);

			if(!vertsInGroup)
			{
				// we'll just rename the current subobject, since theres nothing in it..
				F3DSubObject &sub = pModel->GetMeshChunk()->subObjects[subObject];
				MFString_Copy(sub.name, pName);
			}
			else
			{
				// probably wanna copy vertex data in at this point..
				// and subtract the min from each of the components indices..
				CopyDataIntoSubobject(subObject);

				++subObject;

				matSub = 0;

				minVertIndex = -1;
				minUVIndex = -1;
				minNormIndex = -1;
				maxVertIndex = -1;
				maxUVIndex = -1;
				maxNormIndex = -1;

				vertsInGroup = false;
				vertsInMatSub = false;

				F3DSubObject &sub = pModel->GetMeshChunk()->subObjects[subObject];
				MFString_Copy(sub.name, pName);
			}
		}
		else if(!MFString_CaseCmp(pToken, "v"))
		{
			const char *pX = GetNextToken(pFilePtr);
			const char *pY = GetNextToken(pFilePtr);
			const char *pZ = GetNextToken(pFilePtr);
			pFilePtr = MFSeekNewline(pFilePtr);

			MFVector v;
			v.x = (float)atof(pX);
			v.y = (float)atof(pY);
			v.z = (float)atof(pZ);
			v.w = 1.0f;

			verts.push(v);
		}
		else if(!MFString_CaseCmp(pToken, "vt"))
		{
			const char *pU = GetNextToken(pFilePtr);
			const char *pV = GetNextToken(pFilePtr);
			pFilePtr = MFSeekNewline(pFilePtr);

			MFVector v;
			v.x = (float)atof(pU);
			v.y = (float)atof(pV);
			v.z = 0.0f;
			v.w = 1.0f;

			uvs.push(v);
		}
		else if(!MFString_CaseCmp(pToken, "vn"))
		{
			const char *pX = GetNextToken(pFilePtr);
			const char *pY = GetNextToken(pFilePtr);
			const char *pZ = GetNextToken(pFilePtr);
			pFilePtr = MFSeekNewline(pFilePtr);

			MFVector v;
			v.x = (float)atof(pX);
			v.y = (float)atof(pY);
			v.z = (float)atof(pZ);
			v.w = 1.0f;

			normals.push(v);
		}
		else if(!MFString_CaseCmp(pToken, "f"))
		{
			vertsInGroup = true;
			vertsInMatSub = true;

			F3DSubObject &sub = pModel->GetMeshChunk()->subObjects[subObject];

			const char *pRestOfLine = GetRestOfLine(pFilePtr);

			int firstVert = sub.matSubobjects[matSub].vertices.size();

			pToken = GetNextToken(pRestOfLine);

			while(*pToken)
			{
				const char *pPos = GetNextIndex(pToken);
				const char *pUV = GetNextIndex(pToken);
				const char *pNorm = GetNextIndex(pToken);

				int posid = atoi(pPos);
				int texid = atoi(pUV);
				int normid = atoi(pNorm);

				if(posid < 0)
					posid = verts.size() - posid;
				else
					posid = posid - 1;

				if(texid < 0)
					texid = uvs.size() - texid;
				else
					texid = texid - 1;

				if(normid < 0)
					normid = normals.size() - normid;
				else
					normid = normid - 1;

				minVertIndex = minVertIndex == -1 ? posid : MFMin(minVertIndex, posid);
				minUVIndex = minUVIndex == -1 ? texid : MFMin(minUVIndex, texid);
				minNormIndex = minNormIndex == -1 ? normid : MFMin(minNormIndex, normid);
				maxVertIndex = minVertIndex == -1 ? posid : MFMax(maxVertIndex, posid);
				maxUVIndex = maxUVIndex == -1 ? texid : MFMax(maxUVIndex, texid);
				maxNormIndex = maxNormIndex == -1 ? normid : MFMax(maxNormIndex, normid);

				int vi = sub.matSubobjects[matSub].vertices.size();
				int f = vi - firstVert;

				F3DVertex &vert = sub.matSubobjects[matSub].vertices[firstVert + f];
				vert.position = posid;
				vert.uv1 = texid;
				vert.normal = normid;

				// add a triangle if we are up to the third vert or beyond
				if(f >= 2)
				{
					int curVert = sub.matSubobjects[matSub].vertices.size() - 1;

					F3DTriangle &tri = sub.matSubobjects[matSub].triangles.push();

					tri.v[0] = firstVert;
					tri.v[1] = vi-1;
					tri.v[2] = vi;
				}

				pToken = GetNextToken(pRestOfLine);
			}
		}
		else if(!MFString_CaseCmp(pToken, "usemtl"))
		{
			F3DSubObject &sub = pModel->GetMeshChunk()->subObjects[subObject];

			if(vertsInGroup && vertsInMatSub)
			{
				++matSub;
				vertsInMatSub = false;
			}

			const char *pName = GetRestOfLine(pFilePtr);

			sub.matSubobjects[matSub].materialIndex = GetMaterialID(pName);
		}
		else if(!MFString_CaseCmp(pToken, "mtllib"))
		{

		}
		else if(pToken[0] == '#')
		{
			pFilePtr = MFSeekNewline(pFilePtr);
		}
		else
		{
			MFDebug_Warn(2, MFStr("Unknown token encountered in obj file '%s'!", pToken));
			pFilePtr = MFSeekNewline(pFilePtr);
		}

		pToken = GetNextToken(pFilePtr);
	}

	// want to copy vertex data into the last subobject at this point...
	if(vertsInGroup)
	{
		CopyDataIntoSubobject(subObject);
	}
}

int F3DFile::ReadOBJ(const char *pFilename)
{
	pModel = this;

	MFFile *pFile = MFFileSystem_Open(pFilename, MFOF_Read);
	if(!pFile)
	{
		MFDebug_Warn(2, MFStr("Failed to open OBJ file %s", pFilename));
		return 1;
	}

	int size = MFFile_Seek(pFile, 0, MFSeek_End);
	MFFile_Seek(pFile, 0, MFSeek_Begin);

	char *pMem = (char*)MFHeap_Alloc(size+1);
	MFFile_Read(pFile, pMem, size);
	pMem[size] = 0;

	MFFile_Close(pFile);

	ParseOBJFile(pMem);
	MFHeap_Free(pMem);

	return 0;
}
