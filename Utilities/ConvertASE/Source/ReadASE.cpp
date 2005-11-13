#include "Fuji.h"
#include "F3D.h"

char *ProcessBlock(char *pFilePtr, char *pBlockName, char* (*BlockFunc)(char*, char*));

extern F3DFile *pModel;
F3DMaterial *pMaterial = NULL;

F3DSubObject *pSub = NULL;
F3DBone *pBone = NULL;
F3DRefPoint *pRefPoint = NULL;
int nodeType = -1;

char* GetInt(char *pFilePtr, int *pInt)
{
	char *pEnd, *pToken;
	bool negative = false;

	pFilePtr = MFSkipWhite(pFilePtr);

	if(*pFilePtr == '-')
	{
		negative = true;
		pFilePtr++;
	}

	pEnd = pFilePtr;
	while(MFIsNumeric(*pEnd)) pEnd++;

	if(!MFIsWhite(*pEnd) && !MFIsNewline(*pEnd) && *pEnd != NULL)
	{
		printf("Error: GetInt() found non numeric character.\n");
		*pInt = 0;
		return pFilePtr;
	}

	pToken = MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
	pFilePtr = pEnd;

	*pInt = atoi(pToken);
	if(negative) *pInt = -*pInt;

	return pFilePtr;
}

char* GetFloat(char *pFilePtr, float *pFloat)
{
	char *pEnd, *pToken;
	bool negative = false;
	int dotFound = 1;

	pFilePtr = MFSkipWhite(pFilePtr);

	if(*pFilePtr == '-')
	{
		negative = true;
		pFilePtr++;
	}

	pEnd = pFilePtr;
	while(MFIsNumeric(*pEnd) || (*pEnd == '.' && dotFound--)) pEnd++;

	pToken = MFStrN(pFilePtr, (int)(pEnd-pFilePtr));
	if(*pEnd == 'f') pEnd++;

	if(!MFIsWhite(*pEnd) && !MFIsNewline(*pEnd) && *pEnd != NULL)
	{
		printf("Error: GetFloat() found non numeric character.\n");
		*pFloat = 0.0f;
		return pFilePtr;
	}

	pFilePtr = pEnd;

	*pFloat = (float)atof(pToken);
	if(negative) *pFloat = -*pFloat;

	return pFilePtr;
}

char* GetString(char *pFilePtr, char **ppString)
{
	char *pEnd;

	pFilePtr = MFSkipWhite(pFilePtr);

	if(*pFilePtr != '\"')
	{
		printf("Error: GetString() expected a string.\n");
		*ppString = "";
		return pFilePtr;
	}

	pFilePtr++;

	pEnd = pFilePtr;
	while(*pEnd != '\"' && *pEnd != NULL && !MFIsNewline(*pEnd)) pEnd++;

	if(*pEnd != '\"')
	{
		printf("Error: GetString() encountered an unterminated String.\n");
		*ppString = "";
		return pFilePtr;
	}

	*ppString = MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
	pFilePtr = pEnd + 1;

	return pFilePtr;
}

char* GetLabel(char *pFilePtr, char **ppString)
{
	char *pEnd;

	pFilePtr = MFSkipWhite(pFilePtr);

	pEnd = pFilePtr;
	while(*pEnd != ':' && *pEnd != NULL && !MFIsNewline(*pEnd)) pEnd++;

	if(*pEnd != ':')
	{
		printf("Error: GetLabel() encountered an unterminated label.\n");
		*ppString = "";
		return pFilePtr;
	}

	*ppString = MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
	pFilePtr = pEnd + 1;

	return pFilePtr;
}

char* ReadSceneChunk(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*SCENE_FILENAME"))
	{
		char *pName;

		pFilePtr = GetString(pFilePtr, &pName);

		if(strlen(pName) > 255)
		{
			printf("Error: More than 256 characters in nodel name, \"%s\"", pName);
			return pFilePtr;
		}

		strcpy(pModel->name, pName);

		printf("Model: %s\n", pName);
	}
	else if(!stricmp(pToken, "*SCENE_FIRSTFRAME"))
	{

	}
	else if(!stricmp(pToken, "*SCENE_LASTFRAME"))
	{

	}
	else if(!stricmp(pToken, "*SCENE_FRAMESPEED"))
	{

	}
	else if(!stricmp(pToken, "*SCENE_TICKSPERFRAME"))
	{

	}
	else if(!stricmp(pToken, "*SCENE_BACKGROUND_STATIC"))
	{

	}
	else if(!stricmp(pToken, "*SCENE_AMBIENT_STATIC"))
	{

	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadMaterial(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*MATERIAL_NAME"))
	{
		char *pName;

		pFilePtr = GetString(pFilePtr, &pName);

		if(strlen(pName) > 63)
		{
			printf("Error: More than 64 characters in material name, \"%s\"", pName);
			return pFilePtr;
		}

		strcpy(pMaterial->name, pName);

		printf("Found material: \"%s\"\n", pName);
	}
	else if(!stricmp(pToken, "*MATERIAL_CLASS"))
	{

	}
	else if(!stricmp(pToken, "*MATERIAL_AMBIENT"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->ambient.x);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->ambient.y);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->ambient.z);
		pMaterial->ambient.w = 1.0f;
	}
	else if(!stricmp(pToken, "*MATERIAL_DIFFUSE"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.x);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.y);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.z);
		pMaterial->diffuse.w = 1.0f;
	}
	else if(!stricmp(pToken, "*MATERIAL_SPECULAR"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specular.x);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specular.y);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specular.z);
		pMaterial->specular.w = 1.0f;
	}
	else if(!stricmp(pToken, "*MATERIAL_SHINE"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->glossiness);
	}
	else if(!stricmp(pToken, "*MATERIAL_SHINESTRENGTH"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specularLevel);
	}
	else if(!stricmp(pToken, "*MATERIAL_TRANSPARENCY"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.w);
		pMaterial->diffuse.w = 1.0f - pMaterial->diffuse.w;
	}
	else if(!stricmp(pToken, "*MATERIAL_WIRESIZE"))
	{

	}
	else if(!stricmp(pToken, "*MATERIAL_SHADING"))
	{

	}
	else if(!stricmp(pToken, "*MATERIAL_XP_FALLOFF"))
	{

	}
	else if(!stricmp(pToken, "*MATERIAL_SELFILLUM"))
	{
		float selfIllum;
		pFilePtr = GetFloat(pFilePtr, &selfIllum);

		pMaterial->emissive = pMaterial->diffuse * selfIllum;
		pMaterial->emissive.w = 1.0f;
	}
	else if(!stricmp(pToken, "*MATERIAL_FALLOFF"))
	{

	}
	else if(!stricmp(pToken, "*MATERIAL_XP_TYPE"))
	{

	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadMaterialChunk(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*MATERIAL_COUNT"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pModel->GetMaterialChunk()->materials.resize(count);

		printf("Found %d materials.\n", count);
	}
	else if(!stricmp(pToken, "*MATERIAL"))
	{
		int matID;

		pFilePtr = GetInt(pFilePtr, &matID);

		pMaterial = &pModel->GetMaterialChunk()->materials[matID];

		pFilePtr = ProcessBlock(pFilePtr, "*MATERIAL", ReadMaterial);
	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadBone(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*NODE_NAME"))
	{
		char *pName;

		pFilePtr = GetString(pFilePtr, &pName);

		if(strlen(pName) > 63)
		{
			printf("Error: More than 64 characters in bone name, \"%s\"", pName);
			return pFilePtr;
		}

		strcpy(pBone->name, pName);
	}
	else if(!stricmp(pToken, "*INHERIT_POS"))
	{

	}
	else if(!stricmp(pToken, "*INHERIT_ROT"))
	{

	}
	else if(!stricmp(pToken, "*INHERIT_SCL"))
	{

	}
	else if(!stricmp(pToken, "*TM_ROW0"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 0.0f;

		pBone->worldMatrix.SetXAxis4(v);
	}
	else if(!stricmp(pToken, "*TM_ROW1"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 0.0f;

		pBone->worldMatrix.SetYAxis4(v);
	}
	else if(!stricmp(pToken, "*TM_ROW2"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 0.0f;

		pBone->worldMatrix.SetZAxis4(v);
	}
	else if(!stricmp(pToken, "*TM_ROW3"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 1.0f;

		pBone->worldMatrix.SetTrans4(v);
	}
	else if(!stricmp(pToken, "*TM_POS"))
	{

	}
	else if(!stricmp(pToken, "*TM_ROTAXIS"))
	{

	}
	else if(!stricmp(pToken, "*TM_ROTANGLE"))
	{

	}
	else if(!stricmp(pToken, "*TM_SCALE"))
	{

	}
	else if(!stricmp(pToken, "*TM_SCALEAXIS"))
	{

	}
	else if(!stricmp(pToken, "*TM_SCALEAXISANG"))
	{

	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadVertList(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*MESH_VERTEX"))
	{
		int index;

		pFilePtr = GetInt(pFilePtr, &index);

		pFilePtr = GetFloat(pFilePtr, &pSub->positions[index].x);
		pFilePtr = GetFloat(pFilePtr, &pSub->positions[index].z);
		pFilePtr = GetFloat(pFilePtr, &pSub->positions[index].y);
	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadFaceList(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*MESH_FACE"))
	{
		char *pLabel;
		int index;

		pFilePtr = GetLabel(pFilePtr, &pLabel);

		index = atoi(pLabel);

		pFilePtr = GetLabel(pFilePtr, &pLabel);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 0].position);
		pSub->matSubobjects[0].triangles[index].v[0] = index*3 + 0;

		pFilePtr = GetLabel(pFilePtr, &pLabel);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 1].position);
		pSub->matSubobjects[0].triangles[index].v[1] = index*3 + 1;

		pFilePtr = GetLabel(pFilePtr, &pLabel);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 2].position);
		pSub->matSubobjects[0].triangles[index].v[2] = index*3 + 2;

		pSub->matSubobjects[0].triangles[index].reserved = 0;
	}
	if(!stricmp(pToken, "*MESH_SMOOTHING"))
	{

	}
	if(!stricmp(pToken, "*MESH_MTLID"))
	{

	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadTVertList(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*MESH_TVERT"))
	{
		int index;

		pFilePtr = GetInt(pFilePtr, &index);

		pFilePtr = GetFloat(pFilePtr, &pSub->uvs[index].x);
		pFilePtr = GetFloat(pFilePtr, &pSub->uvs[index].y);
		pFilePtr = GetFloat(pFilePtr, &pSub->uvs[index].z);

		pSub->uvs[index].y = -pSub->uvs[index].y + 1.0f;
	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadTFaceList(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*MESH_TFACE"))
	{
		int index;

		pFilePtr = GetInt(pFilePtr, &index);

		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 0].uv1);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 1].uv1);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 2].uv1);
	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadNormals(char *pFilePtr, char *pToken)
{
	static int curFace = 0;
	static int curNormal = 0;

	if(!stricmp(pToken, "*MESH_FACENORMAL"))
	{
		pFilePtr = GetInt(pFilePtr, &curFace);

		pFilePtr = GetFloat(pFilePtr, &pSub->matSubobjects[0].triangles[curFace].normal.x);
		pFilePtr = GetFloat(pFilePtr, &pSub->matSubobjects[0].triangles[curFace].normal.z);
		pFilePtr = GetFloat(pFilePtr, &pSub->matSubobjects[0].triangles[curFace].normal.y);

		curNormal = curFace*3;
	}
	if(!stricmp(pToken, "*MESH_VERTEXNORMAL"))
	{
		int index;

		pFilePtr = GetInt(pFilePtr, &index);

		pFilePtr = GetFloat(pFilePtr, &pSub->normals[curNormal].x);
		pFilePtr = GetFloat(pFilePtr, &pSub->normals[curNormal].z);
		pFilePtr = GetFloat(pFilePtr, &pSub->normals[curNormal].y);

		pSub->matSubobjects[0].vertices[curNormal].normal = curNormal;

		curNormal++;
	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadMesh(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*TIMEVALUE"))
	{

	}
	else if(!stricmp(pToken, "*MESH_NUMVERTEX"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->positions.resize(count);
	}
	else if(!stricmp(pToken, "*MESH_NUMFACES"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->matSubobjects[0].triangles.resize(count);
		pSub->matSubobjects[0].vertices.resize(count*3);
		pSub->normals.resize(count*3);
	}
	else if(!stricmp(pToken, "*MESH_VERTEX_LIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_VERTEX_LIST", ReadVertList);
	}
	else if(!stricmp(pToken, "*MESH_FACE_LIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_FACE_LIST", ReadFaceList);
	}
	else if(!stricmp(pToken, "*MESH_NUMTVERTEX"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->uvs.resize(count);
	}
	else if(!stricmp(pToken, "*MESH_TVERTLIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_TVERTLIST", ReadTVertList);
	}
	else if(!stricmp(pToken, "*MESH_NUMTVFACES"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		if(count != pSub->matSubobjects[0].triangles.size())
		{
			printf("Error: Number of faces does not match number of texture faces.");
		}
	}
	else if(!stricmp(pToken, "*MESH_TFACELIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_TFACELIST", ReadTFaceList);
	}
	else if(!stricmp(pToken, "*MESH_NUMCVERTEX"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->colours.resize(count);
	}
	else if(!stricmp(pToken, "*MESH_NORMALS"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_NORMALS", ReadNormals);
	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char* ReadGeomChunk(char *pFilePtr, char *pToken)
{
	if(!stricmp(pToken, "*NODE_NAME"))
	{
		char *pNodeName;

		pFilePtr = GetString(pFilePtr, &pNodeName);

		if(!strnicmp(pNodeName, "z_", 2))
		{
			// node is bone
			nodeType = 2;
		}
		else if(!strnicmp(pNodeName, "r_", 2))
		{
			// node is refPoint
			nodeType = 4;
		}
		else
		{
			// node is mesh
			nodeType = 1;
		}

		// if exporting geometry, add a subobject
		if(nodeType & 1)
		{
			pSub = &pModel->GetMeshChunk()->subObjects.push();

			if(strlen(pNodeName) > 63)
			{
				printf("Error: More than 64 characters in mesh name, \"%s\"", pNodeName);
			}
			else
				strcpy(pSub->name, pNodeName);
		}

		// if exporting a bone, add a bone
		if(nodeType & 2)
		{
			pBone = &pModel->GetSkeletonChunk()->bones.push();
		}

		// if exporting a refPoint, add a refPoint
		if(nodeType & 4)
		{
			pRefPoint = &pModel->GetRefPointChunk()->refPoints.push();

			if(strlen(pNodeName) > 63)
			{
				printf("Error: More than 64 characters in refPoint name, \"%s\"", pNodeName);
			}
			else
                strcpy(pRefPoint->name, pNodeName);
		}
	}
	else if(!stricmp(pToken, "*NODE_PARENT"))
	{
		if(nodeType & 2)
		{
			char *pParentName;

			pFilePtr = GetString(pFilePtr, &pParentName);

			if(strlen(pParentName) > 63)
			{
				printf("Error: More than 64 characters in bone parent name, \"%s\"", pParentName);
				return pFilePtr;
			}

			strcpy(pBone->parentName, pParentName);
		}
	}
	else if(!stricmp(pToken, "*NODE_TM"))
	{
		if(nodeType & 2)
		{
			pFilePtr = ProcessBlock(pFilePtr, "*NODE_TM", ReadBone);
		}
	}
	else if(!stricmp(pToken, "*MESH"))
	{
		if(nodeType & 1)
		{
			pFilePtr = ProcessBlock(pFilePtr, "*MESH", ReadMesh);
		}
	}
	else if(!stricmp(pToken, "*MATERIAL_REF"))
	{
		if(nodeType & 1)
		{
			pFilePtr = GetInt(pFilePtr, &pSub->matSubobjects[0].materialIndex);
		}
	}
	else
	{
		printf("Unknown token: %s\n", pToken);
	}

	return pFilePtr;
}

char *ProcessBlock(char *pFilePtr, char *pBlockName, char* (*BlockFunc)(char*, char*))
{
	char *pEnd;
	char *pToken;

	int braceCount = 0;
	bool inQuote = false;

	pFilePtr = MFSkipWhite(pFilePtr);

	if(*pFilePtr != '{')
	{
		printf("Error: Expected %s Block.\n", pBlockName);
		return pFilePtr;
	}

	pFilePtr++;

	while(*pFilePtr != NULL)
	{
		while(!(*pFilePtr == '*' && !braceCount && !inQuote) && *pFilePtr != NULL)
		{
			if(*pFilePtr == '\"') inQuote = !inQuote;
			if(!inQuote)
			{
				if(*pFilePtr == '{') braceCount++;
				if(*pFilePtr == '}') braceCount--;
			}
			pFilePtr++;

			if(braceCount < 0) return pFilePtr;
		}

		pEnd = pFilePtr;

		while(!MFIsWhite(*pEnd) && *pEnd != NULL) pEnd++;

		pToken = MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
		pFilePtr = pEnd;

		pFilePtr = BlockFunc(pFilePtr, pToken);
	}

	return pFilePtr;
}

void ParseASEFile(char *pFilePtr)
{
	char *pEnd;
	char *pToken;

	int braceCount = 0;
	bool inQuote = false;

	while(*pFilePtr != NULL)
	{
		while(!(*pFilePtr == '*' && !braceCount && !inQuote) && *pFilePtr != NULL)
		{
			if(*pFilePtr == '\"') inQuote = !inQuote;
			if(!inQuote)
			{
				if(*pFilePtr == '{') braceCount++;
				if(*pFilePtr == '}') braceCount--;
			}
			pFilePtr++;
		}

		pEnd = pFilePtr;

		while(!MFIsWhite(*pEnd) && *pEnd != NULL) pEnd++;

		pToken = MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
		pFilePtr = pEnd;

		if(!stricmp(pToken, "*3DSMAX_ASCIIEXPORT"))
		{
			int version = 0;

			pFilePtr = GetInt(pFilePtr, &version);

			printf("Recognised .ASE file version: %d\n", version);
		}
		else if(!stricmp(pToken, "*COMMENT"))
		{
			char *pComment;

			pFilePtr = GetString(pFilePtr, &pComment);

			printf("Comment: %s\n", pComment);
		}
		else if(!stricmp(pToken, "*SCENE"))
		{
			pFilePtr = ProcessBlock(pFilePtr, "*SCENE", ReadSceneChunk);
		}
		else if(!stricmp(pToken, "*MATERIAL_LIST"))
		{
			pFilePtr = ProcessBlock(pFilePtr, "*MATERIAL_LIST", ReadMaterialChunk);
		}
		else if(!stricmp(pToken, "*GEOMOBJECT"))
		{
			pFilePtr = ProcessBlock(pFilePtr, "*GEOMOBJECT", ReadGeomChunk);
		}
		else
		{
			printf("Unknown token: %s\n", pToken);
		}
	}
}

int F3DFile::ReadASE(char *pFilename)
{
	FILE *infile;
	char *file = NULL;
	uint32 filesize = 0;

	infile = fopen(pFilename, "r");

	if(!infile)
	{
		printf("Failed to open ASE file %s", pFilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	file = (char*)malloc(filesize+1);
	fread(file, 1, filesize, infile);
	file[filesize] = NULL;

	fclose(infile);

	ParseASEFile(file);

	free(file);

	return 0;
}
