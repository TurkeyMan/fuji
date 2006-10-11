#include "Fuji.h"
#include "F3D.h"

extern F3DFile *pModel;

char gTokenBuffer[2048];

const char *GetNextToken(const char *pText, const char **ppTokenEnd = NULL, char *pBuffer = gTokenBuffer)
{
	char *pT = pBuffer;

	while(MFIsWhite(*pText) || MFIsNewline(*pText))
		++pText;

	if(!*pText)
		return NULL;

	// skip comments
	while(*pText == '/' && pText[1] == '/')
	{
		while(*pText && !MFIsNewline(*pText))
			++pText;

		while(MFIsWhite(*pText) || MFIsNewline(*pText))
			++pText;

		if(!*pText)
			return NULL;
	}

	while(*pText && !MFIsWhite(*pText) && !MFIsNewline(*pText) && *pText != ',' && *pText != ';' && *pText != '{' && *pText != '}')
	{
		*pT++ = *pText++;
	}

	if(pT == pBuffer)
		*pT++ = *pText++;

	*pT = 0;

	if(ppTokenEnd)
		*ppTokenEnd = pText;

	return pBuffer;
}

void SkipToken(const char *&pText, const char *pToken)
{
	const char *pTok = GetNextToken(pText, &pText);
	MFDebug_Assert(!MFString_Compare(pTok, pToken), MFStr("Expected '%s'\n", pToken));
}

void SkipSection(const char *&pText)
{
	while(MFString_Compare(GetNextToken(pText, &pText), "{")) { }

	int depth = 1;

	while(depth)
	{
		const char *pTok = GetNextToken(pText, &pText);

		if(!MFString_Compare(pTok, "{"))
			++depth;
		if(!MFString_Compare(pTok, "}"))
			--depth;
	}
}

const char *FindSectionInScope(const char *pText, const char *pSection)
{
	const char *pT;
	int depth = 1;

	while(depth)
	{
		const char *pTok = GetNextToken(pText, &pT);

		if(depth == 1)
			if(!MFString_Compare(pTok, pSection))
				return pText;
		pText = pT;

		if(!MFString_Compare(pTok, "{"))
			++depth;
		if(!MFString_Compare(pTok, "}"))
			--depth;
	}

	return NULL;
}

const char *GetValue(const char *pText, const char **ppTokenEnd = NULL)
{
	char temp[1024];

	const char *pVal = GetNextToken(pText, &pText);
	const char *pSemi = GetNextToken(pText, ppTokenEnd, temp);

	MFDebug_Assert(!MFString_Compare(pSemi, ";"), "Value is not terminated with a semicolon.\n");

	return pVal;
}

float GetFloat(const char *pText, const char **ppTokenEnd = NULL)
{
	return (float)atof(GetValue(pText, ppTokenEnd));
}

int GetInt(const char *pText, const char **ppTokenEnd = NULL)
{
	return atoi(GetValue(pText, ppTokenEnd));
}

const char *GetString(const char *pText, const char **ppTokenEnd = NULL)
{
	char gStringBuffer[2048];

	char *pString = (char*)GetNextToken(pText, &pText, gStringBuffer);

	MFDebug_Assert(*pString == '"', "Not a string.");

	int len = MFString_Length(pString);

	if(len > 1 && pString[len - 1] == '"')
	{
		pString[len - 1] = 0;
	}
	else
	{
		while(*pText && *pText != '"')
		{
			MFDebug_Assert(!MFIsNewline(*pText), "Unterminated string found...");

			pString[len] = *pText;
			++len;
			++pText;
		}

		MFDebug_Assert(*pText, "Unterminated string found...");

		pString[len] = 0;
		++pText;
	}

	SkipToken(pText, ";");

	if(ppTokenEnd)
		*ppTokenEnd = pText;

	return &pString[1];
}

void GetIntArray(const char *pText, int *pOutput, int arrayLength, const char **ppTokenEnd = NULL)
{
	char temp[1024];

	for(int a=0; a<arrayLength; a++)
	{
		const char *pVal = GetNextToken(pText, &pText);

		if(a < arrayLength-1)
		{
			const char *pComa = GetNextToken(pText, &pText, temp);
			MFDebug_Assert(!MFString_Compare(pComa, ","), "Array values are separated with a coma.\n");
		}

		pOutput[a] = atoi(pVal);
	}

	const char *pSemi = GetNextToken(pText, ppTokenEnd);

	MFDebug_Assert(!MFString_Compare(pSemi, ";"), "Value is not terminated with a semicolon.\n");
}

void GetFloatArray(const char *pText, float *pOutput, int arrayLength, const char **ppTokenEnd = NULL)
{
	char temp[1024];

	for(int a=0; a<arrayLength; a++)
	{
		const char *pVal = GetNextToken(pText, &pText);

		if(a < arrayLength-1)
		{
			const char *pComa = GetNextToken(pText, &pText, temp);
			MFDebug_Assert(!MFString_Compare(pComa, ","), "Array values are separated with a coma.\n");
		}

		pOutput[a] = (float)atof(pVal);
	}

	const char *pSemi = GetNextToken(pText, ppTokenEnd);

	MFDebug_Assert(!MFString_Compare(pSemi, ";"), "Value is not terminated with a semicolon.\n");
}

const char *ParseMesh(const char *pText, const MFMatrix &mat)
{
	F3DMeshChunk *pMesh = pModel->GetMeshChunk();
	F3DSubObject &sub = pMesh->subObjects.push();

	// read name
	GetNextToken(pText, &pText, sub.name);

	SkipToken(pText, "{");

	// get num positions
	int numPositions = GetInt(pText, &pText);

	sub.positions.resize(numPositions);

	// read positions
	for(int a=0; a<numPositions; a++)
	{
		sub.positions[a].x = GetFloat(pText, &pText);
		sub.positions[a].y = GetFloat(pText, &pText);
		sub.positions[a].z = GetFloat(pText, &pText);

		sub.positions[a] = ApplyMatrixH(sub.positions[a], mat);

		if(a < numPositions-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");

	// get num faces
	int numFaces = GetInt(pText, &pText);

	// see if we have a material face mapping
	const char *pMatList = FindSectionInScope(pText, "MeshMaterialList");
	int *pMatFaces = NULL;

	if(pMatList)
	{
		SkipToken(pMatList, "MeshMaterialList");
		SkipToken(pMatList, "{");

		int numMats = GetInt(pMatList, &pMatList);
		MFDebug_Assert(numMats < 16, "Exceeded maximum 16 materials per subobject...");
		int numMatFaces = GetInt(pMatList, &pMatList);
		MFDebug_Assert(numMatFaces == numFaces, "Number of material faces does not match number of mesh faces");

		pMatFaces = (int*)malloc(sizeof(int)*numMatFaces);
		GetIntArray(pMatList, pMatFaces, numMatFaces, &pMatList);

		// process materials...
		const char *pToken = GetNextToken(pMatList, &pMatList);

		while(MFString_Compare(pToken, "}"))
		{
			if(!MFString_Compare(pToken, "Material"))
			{
				const char *pMatName = GetNextToken(pMatList, &pToken);

				if(MFString_Compare(pMatName, "{"))
					pMatList = pToken;
				else
					pMatName = "Unknown Material";

				int matID = pModel->GetMaterialChunk()->GetMaterialIndexByName(pMatName);

				F3DMaterialSubobject &matSub = sub.matSubobjects.push();

				if(matID != -1)
				{
					matSub.materialIndex = matID;
					SkipSection(pMatList);
				}
				else
				{
					matSub.materialIndex = pModel->GetMaterialChunk()->materials.size();
					F3DMaterial &mat = pModel->GetMaterialChunk()->materials.push();
					MFString_CopyN(mat.name, pMatName, 64);

					SkipToken(pMatList, "{");

					mat.diffuse.x = GetFloat(pMatList, &pMatList);
					mat.diffuse.y = GetFloat(pMatList, &pMatList);
					mat.diffuse.z = GetFloat(pMatList, &pMatList);
					mat.diffuse.w = GetFloat(pMatList, &pMatList);
					SkipToken(pMatList, ";");

					mat.specularLevel = GetFloat(pMatList, &pMatList);

					mat.specular.x = GetFloat(pMatList, &pMatList);
					mat.specular.y = GetFloat(pMatList, &pMatList);
					mat.specular.z = GetFloat(pMatList, &pMatList);
					mat.diffuse.w = 0.0f;
					SkipToken(pMatList, ";");

					mat.emissive.x = GetFloat(pMatList, &pMatList);
					mat.emissive.y = GetFloat(pMatList, &pMatList);
					mat.emissive.z = GetFloat(pMatList, &pMatList);
					mat.emissive.w = 0.0f;
					SkipToken(pMatList, ";");

					// any textures...
					pToken = GetNextToken(pMatList, &pMatList);

					while(MFString_Compare(pToken, "}"))
					{
						if(!MFString_Compare(pToken, "TextureFilename"))
						{
							SkipToken(pMatList, "{");
							MFString_Copy(mat.maps[0], GetString(pMatList, &pMatList));
							SkipToken(pMatList, "}");
						}
						else
						{
							printf("Unknown token '%s'\n", pToken);
						}

						pToken = GetNextToken(pMatList, &pMatList);
					}
				}
			}
			else
			{
				printf("Unknown token '%s'\n", pToken);
			}

			pToken = GetNextToken(pMatList, &pMatList);
		}
	}

	// read faces
	int face[16], numVerts[16], numTris[16];
	MFZeroMemory(numVerts, sizeof(numVerts));
	MFZeroMemory(numTris, sizeof(numTris));
	for(int a=0; a<numFaces; a++)
	{
		int matSub = pMatFaces ? pMatFaces[a] : 0;

		int numPoints = GetInt(pText, &pText);
		MFDebug_Assert(numPoints < 16, "Exceeded maximum 16 points per face...");
		GetIntArray(pText, face, numPoints, &pText);

		for(int b=0; b<numPoints; b++)
			sub.matSubobjects[matSub].vertices[numVerts[matSub]+b].position = face[b];

		for(int b=0; b<numPoints-2; b++)
		{
			sub.matSubobjects[matSub].triangles[numTris[matSub]+b].v[0] = numVerts[matSub]+0;
			sub.matSubobjects[matSub].triangles[numTris[matSub]+b].v[1] = numVerts[matSub]+b+1;
			sub.matSubobjects[matSub].triangles[numTris[matSub]+b].v[2] = numVerts[matSub]+b+2;
		}

		numVerts[matSub] += numPoints;
		numTris[matSub] += numPoints-2;

		if(a < numFaces-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");

	const char *pTok = GetNextToken(pText, &pText);

	while(MFString_Compare(pTok, "}"))
	{
		if(!MFString_Compare(pTok, "MeshMaterialList"))
		{
			SkipSection(pText);
		}
		else if(!MFString_Compare(pTok, "MeshNormals"))
		{
			SkipToken(pText, "{");

			// get num positions
			int numNormals = GetInt(pText, &pText);

			sub.normals.resize(numNormals);

			// read positions
			for(int a=0; a<numNormals; a++)
			{
				sub.normals[a].x = GetFloat(pText, &pText);
				sub.normals[a].y = GetFloat(pText, &pText);
				sub.normals[a].z = GetFloat(pText, &pText);

				sub.normals[a] = ApplyMatrix3(sub.normals[a], mat);
				sub.normals[a].Normalise3();

				if(a < numNormals-1)
					SkipToken(pText, ",");
			}
			SkipToken(pText, ";");

			// get num faces
			int numNormalFaces = GetInt(pText, &pText);
			MFDebug_Assert(numNormalFaces == numFaces, "Number of normal faces does not match the number of faces in the mesh.");

			// read faces
			MFZeroMemory(numVerts, sizeof(numVerts));
			for(int a=0; a<numNormalFaces; a++)
			{
				int matSub = pMatFaces ? pMatFaces[a] : 0;

				int numPoints = GetInt(pText, &pText);
				GetIntArray(pText, face, numPoints, &pText);

				for(int b=0; b<numPoints; b++)
					sub.matSubobjects[matSub].vertices[numVerts[matSub]+b].normal = face[b];

				numVerts[matSub] += numPoints;

				if(a < numNormalFaces-1)
					SkipToken(pText, ",");
			}
			SkipToken(pText, ";");
			SkipToken(pText, "}");
		}
		else if(!MFString_Compare(pTok, "MeshTextureCoords"))
		{
			SkipToken(pText, "{");

			// get num positions
			int numUV = GetInt(pText, &pText);
			MFDebug_Assert(numUV == numPositions, "Number of UV's does not match the number of verts in the mesh.");

			sub.uvs.resize(numUV);

			// read positions
			for(int a=0; a<numUV; a++)
			{
				sub.uvs[a].x = GetFloat(pText, &pText);
				sub.uvs[a].y = GetFloat(pText, &pText);

				if(a < numUV-1)
					SkipToken(pText, ",");
			}
			SkipToken(pText, ";");

			// read faces
			for(int m=0; m<sub.matSubobjects.size(); m++)
			{
				int totalVerts = sub.matSubobjects[m].vertices.size();
				for(int a=0; a<totalVerts; a++)
				{
					sub.matSubobjects[m].vertices[a].uv1 = sub.matSubobjects[m].vertices[a].position;
				}
			}

			SkipToken(pText, "}");
		}
		else
		{
			printf("Unknown token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}

	return pText;
}

const char *ParseFrame(const char *pText, const MFMatrix &mat)
{
	MFMatrix transform = mat;

	char frameName[256];
	GetNextToken(pText, &pText, frameName);

	SkipToken(pText, "{");

	const char *pTok = GetNextToken(pText, &pText);

	while(MFString_Compare(pTok, "}"))
	{
		if(!MFString_Compare(pTok, "Frame"))
		{
			pText = ParseFrame(pText, transform);
		}
		else if(!MFString_Compare(pTok, "FrameTransformMatrix"))
		{
			SkipToken(pText, "{");

			MFMatrix tMat;
			GetFloatArray(pText, (float*)&tMat, 16, &pText);

			transform.Multiply(tMat);

			SkipToken(pText, ";");
			SkipToken(pText, "}");
		}
		else if(!MFString_Compare(pTok, "Mesh"))
		{
			pText = ParseMesh(pText, transform);
		}
		else
		{
			printf("Unexpected token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}

	return pText;
}

void LoadTextXFile(const char *pText)
{
	const char *pTok = GetNextToken(pText, &pText);

	while(pTok)
	{
		if(!MFString_Compare(pTok, "Header"))
		{
			SkipToken(pText, "{");

			printf("XFile V%d.%d, 0x%X\n", GetInt(pText, &pText), GetInt(pText, &pText), GetInt(pText, &pText));

			pTok = GetNextToken(pText, &pText);
			while(MFString_Compare(pTok, "}"))
			{
				pTok = GetNextToken(pText, &pText);
			}
		}
		else if(!MFString_Compare(pTok, "Frame"))
		{
			pText = ParseFrame(pText, MFMatrix::identity);
		}
		else if(!MFString_Compare(pTok, "Mesh"))
		{
			pText = ParseMesh(pText, MFMatrix::identity);
		}
		else if(!MFString_Compare(pTok, "template"))
		{
//			const char *pName = GetNextToken(pText, &pText);
			SkipSection(pText);
		}
		else
		{
			printf("Unknown token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}
}

void ParseXFile(char *pFilePtr)
{
	if(MFString_CaseCmpN(pFilePtr, "xof ", 4))
	{
		printf("File is not an .x file\n");
		return;
	}

	if(!MFString_CaseCmpN(&pFilePtr[8], "txt ", 4))
	{
		pFilePtr += 16;

		LoadTextXFile(pFilePtr);
	}
	else if(!MFString_CaseCmpN(&pFilePtr[8], "bin ", 4))
	{
		printf("Binary .x files not yet supported...\n");
	}
	else
	{
		printf("Not a valid .x file...\n");
	}
}

int F3DFile::ReadX(char *pFilename)
{
	FILE *infile;
	char *file = NULL;
	uint32 filesize = 0;

	infile = fopen(pFilename, "rb");

	if(!infile)
	{
		printf("Failed to open X file %s\n", pFilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	file = (char*)malloc(filesize+1);
	fread(file, 1, filesize, infile);
	file[filesize] = 0;

	fclose(infile);

	ParseXFile(file);

	free(file);

	return 0;
}
