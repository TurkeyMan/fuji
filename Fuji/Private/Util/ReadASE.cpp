#include "Fuji.h"
#include "Util/F3D.h"

#include "MFFileSystem.h"

char *ProcessBlock(char *pFilePtr, char *pBlockName, char* (*BlockFunc)(char*, char*));

static F3DFile *pModel;
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
		MFDebug_Warn(3, "Error: GetInt() found non numeric character.");
		*pInt = 0;
		return pFilePtr;
	}

	pToken = (char*)MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
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

	pToken = (char*)MFStrN(pFilePtr, (int)(pEnd-pFilePtr));
	if(*pEnd == 'f') pEnd++;

	if(!MFIsWhite(*pEnd) && !MFIsNewline(*pEnd) && *pEnd != NULL)
	{
		MFDebug_Warn(3, "Error: GetFloat() found non numeric character.");
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
		MFDebug_Warn(3, "Error: GetString() expected a string.");
		*ppString = "";
		return pFilePtr;
	}

	pFilePtr++;

	pEnd = pFilePtr;
	while(*pEnd != '\"' && *pEnd != NULL && !MFIsNewline(*pEnd)) pEnd++;

	if(*pEnd != '\"')
	{
		MFDebug_Warn(3, "Error: GetString() encountered an unterminated String.");
		*ppString = "";
		return pFilePtr;
	}

	*ppString = (char*)MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
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
		MFDebug_Warn(3, "Error: GetLabel() encountered an unterminated label.");
		*ppString = "";
		return pFilePtr;
	}

	*ppString = (char*)MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
	pFilePtr = pEnd + 1;

	return pFilePtr;
}

char* ReadSceneChunk(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*SCENE_FILENAME"))
	{
		char *pName;

		pFilePtr = GetString(pFilePtr, &pName);

		if(MFString_Length(pName) > 255)
		{
			MFDebug_Warn(3, MFStr("Error: More than 256 characters in nodel name, \"%s\"", pName));
			return pFilePtr;
		}

		MFString_Copy(pModel->name, pName);

		MFDebug_Log(4, MFStr("Model: %s", pName));
	}
	else if(!MFString_CaseCmp(pToken, "*SCENE_FIRSTFRAME"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*SCENE_LASTFRAME"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*SCENE_FRAMESPEED"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*SCENE_TICKSPERFRAME"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*SCENE_BACKGROUND_STATIC"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*SCENE_AMBIENT_STATIC"))
	{

	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadMaterial(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*MATERIAL_NAME"))
	{
		char *pName;

		pFilePtr = GetString(pFilePtr, &pName);

		if(MFString_Length(pName) > 63)
		{
			MFDebug_Warn(3, MFStr("Error: More than 64 characters in material name, \"%s\"", pName));
			return pFilePtr;
		}

		MFString_Copy(pMaterial->name, pName);

		MFDebug_Log(4, MFStr("Found material: \"%s\"", pName));
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_CLASS"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_AMBIENT"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->ambient.x);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->ambient.y);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->ambient.z);
		pMaterial->ambient.w = 1.0f;
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_DIFFUSE"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.x);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.y);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.z);
		pMaterial->diffuse.w = 1.0f;
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_SPECULAR"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specular.x);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specular.y);
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specular.z);
		pMaterial->specular.w = 1.0f;
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_SHINE"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->glossiness);
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_SHINESTRENGTH"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->specularLevel);
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_TRANSPARENCY"))
	{
		pFilePtr = GetFloat(pFilePtr, &pMaterial->diffuse.w);
		pMaterial->diffuse.w = 1.0f - pMaterial->diffuse.w;
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_WIRESIZE"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_SHADING"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_XP_FALLOFF"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_SELFILLUM"))
	{
		float selfIllum;
		pFilePtr = GetFloat(pFilePtr, &selfIllum);

		pMaterial->emissive = pMaterial->diffuse * selfIllum;
		pMaterial->emissive.w = 1.0f;
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_FALLOFF"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_XP_TYPE"))
	{

	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadMaterialChunk(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*MATERIAL_COUNT"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pModel->GetMaterialChunk()->materials.resize(count);

		MFDebug_Log(4, MFStr("Found %d materials.", count));
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL"))
	{
		int matID;

		pFilePtr = GetInt(pFilePtr, &matID);

		pMaterial = &pModel->GetMaterialChunk()->materials[matID];

		pFilePtr = ProcessBlock(pFilePtr, "*MATERIAL", ReadMaterial);
	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadBone(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*NODE_NAME"))
	{
		char *pName;

		pFilePtr = GetString(pFilePtr, &pName);

		if(MFString_Length(pName) > 63)
		{
			MFDebug_Warn(3, MFStr("Error: More than 64 characters in bone name, \"%s\"", pName));
			return pFilePtr;
		}

		MFString_Copy(pBone->name, pName);
	}
	else if(!MFString_CaseCmp(pToken, "*INHERIT_POS"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*INHERIT_ROT"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*INHERIT_SCL"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*TM_ROW0"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 0.0f;

		pBone->worldMatrix.SetXAxis4(v);
	}
	else if(!MFString_CaseCmp(pToken, "*TM_ROW1"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 0.0f;

		pBone->worldMatrix.SetYAxis4(v);
	}
	else if(!MFString_CaseCmp(pToken, "*TM_ROW2"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 0.0f;

		pBone->worldMatrix.SetZAxis4(v);
	}
	else if(!MFString_CaseCmp(pToken, "*TM_ROW3"))
	{
		MFVector v;

		pFilePtr = GetFloat(pFilePtr, &v.x);
		pFilePtr = GetFloat(pFilePtr, &v.y);
		pFilePtr = GetFloat(pFilePtr, &v.z);
		v.w = 1.0f;

		pBone->worldMatrix.SetTrans4(v);
	}
	else if(!MFString_CaseCmp(pToken, "*TM_POS"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*TM_ROTAXIS"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*TM_ROTANGLE"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*TM_SCALE"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*TM_SCALEAXIS"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*TM_SCALEAXISANG"))
	{

	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadVertList(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*MESH_VERTEX"))
	{
		int index;

		pFilePtr = GetInt(pFilePtr, &index);

		pFilePtr = GetFloat(pFilePtr, &pSub->positions[index].x);
		pFilePtr = GetFloat(pFilePtr, &pSub->positions[index].z);
		pFilePtr = GetFloat(pFilePtr, &pSub->positions[index].y);
	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadFaceList(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*MESH_FACE"))
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
		pSub->matSubobjects[0].triangles[index].v[2] = index*3 + 1;

		pFilePtr = GetLabel(pFilePtr, &pLabel);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 2].position);
		pSub->matSubobjects[0].triangles[index].v[1] = index*3 + 2;

		pSub->matSubobjects[0].triangles[index].reserved = 0;
	}
	if(!MFString_CaseCmp(pToken, "*MESH_SMOOTHING"))
	{

	}
	if(!MFString_CaseCmp(pToken, "*MESH_MTLID"))
	{

	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadTVertList(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*MESH_TVERT"))
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
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadTFaceList(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*MESH_TFACE"))
	{
		int index;

		pFilePtr = GetInt(pFilePtr, &index);

		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 0].uv1);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 1].uv1);
		pFilePtr = GetInt(pFilePtr, (int*)&pSub->matSubobjects[0].vertices[index*3 + 2].uv1);
	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadNormals(char *pFilePtr, char *pToken)
{
	static int curFace = 0;
	static int curNormal = 0;

	if(!MFString_CaseCmp(pToken, "*MESH_FACENORMAL"))
	{
		pFilePtr = GetInt(pFilePtr, &curFace);

		pFilePtr = GetFloat(pFilePtr, &pSub->matSubobjects[0].triangles[curFace].normal.x);
		pFilePtr = GetFloat(pFilePtr, &pSub->matSubobjects[0].triangles[curFace].normal.z);
		pFilePtr = GetFloat(pFilePtr, &pSub->matSubobjects[0].triangles[curFace].normal.y);

		curNormal = curFace*3;
	}
	if(!MFString_CaseCmp(pToken, "*MESH_VERTEXNORMAL"))
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
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadMesh(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*TIMEVALUE"))
	{

	}
	else if(!MFString_CaseCmp(pToken, "*MESH_NUMVERTEX"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->positions.resize(count);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_NUMFACES"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->matSubobjects[0].triangles.resize(count);
		pSub->matSubobjects[0].vertices.resize(count*3);
		pSub->normals.resize(count*3);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_VERTEX_LIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_VERTEX_LIST", ReadVertList);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_FACE_LIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_FACE_LIST", ReadFaceList);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_NUMTVERTEX"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->uvs.resize(count);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_TVERTLIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_TVERTLIST", ReadTVertList);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_NUMTVFACES"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		if(count != pSub->matSubobjects[0].triangles.size())
		{
			MFDebug_Warn(3, "Error: Number of faces does not match number of texture faces.");
		}
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_TFACELIST"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_TFACELIST", ReadTFaceList);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_NUMCVERTEX"))
	{
		int count;

		pFilePtr = GetInt(pFilePtr, &count);

		pSub->colours.resize(count);
	}
	else if(!MFString_CaseCmp(pToken, "*MESH_NORMALS"))
	{
		pFilePtr = ProcessBlock(pFilePtr, "*MESH_NORMALS", ReadNormals);
	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
	}

	return pFilePtr;
}

char* ReadGeomChunk(char *pFilePtr, char *pToken)
{
	if(!MFString_CaseCmp(pToken, "*NODE_NAME"))
	{
		char *pNodeName;

		pFilePtr = GetString(pFilePtr, &pNodeName);

		if(!MFString_CaseCmpN(pNodeName, "z_", 2))
		{
			// node is bone
			nodeType = 2;
		}
		else if(!MFString_CaseCmpN(pNodeName, "r_", 2))
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

			if(MFString_Length(pNodeName) > 63)
			{
				MFDebug_Warn(3, MFStr("Error: More than 64 characters in mesh name, \"%s\"", pNodeName));
			}
			else
				MFString_Copy(pSub->name, pNodeName);
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

			if(MFString_Length(pNodeName) > 63)
			{
				MFDebug_Warn(3, MFStr("Error: More than 64 characters in refPoint name, \"%s\"", pNodeName));
			}
			else
                MFString_Copy(pRefPoint->name, pNodeName);
		}
	}
	else if(!MFString_CaseCmp(pToken, "*NODE_PARENT"))
	{
		if(nodeType & 2)
		{
			char *pParentName;

			pFilePtr = GetString(pFilePtr, &pParentName);

			if(MFString_Length(pParentName) > 63)
			{
				MFDebug_Warn(3, MFStr("Error: More than 64 characters in bone parent name, \"%s\"", pParentName));
				return pFilePtr;
			}

			MFString_Copy(pBone->parentName, pParentName);
		}
	}
	else if(!MFString_CaseCmp(pToken, "*NODE_TM"))
	{
		if(nodeType & 2)
		{
			pFilePtr = ProcessBlock(pFilePtr, "*NODE_TM", ReadBone);
		}
	}
	else if(!MFString_CaseCmp(pToken, "*MESH"))
	{
		if(nodeType & 1)
		{
			pFilePtr = ProcessBlock(pFilePtr, "*MESH", ReadMesh);
		}
	}
	else if(!MFString_CaseCmp(pToken, "*MATERIAL_REF"))
	{
		if(nodeType & 1)
		{
			pFilePtr = GetInt(pFilePtr, &pSub->matSubobjects[0].materialIndex);
		}
	}
	else
	{
		MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
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
		MFDebug_Warn(3, MFStr("Error: Expected %s Block.", pBlockName));
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

		pToken = (char*)MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
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

	while(*pFilePtr != 0)
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

		pToken = (char*)MFStrN(pFilePtr, (int)(pEnd - pFilePtr));
		pFilePtr = pEnd;

		if(!MFString_CaseCmp(pToken, "*3DSMAX_ASCIIEXPORT"))
		{
			int version = 0;

			pFilePtr = GetInt(pFilePtr, &version);

			MFDebug_Log(4, MFStr("Recognised .ASE file version: %d\n", version));
		}
		else if(!MFString_CaseCmp(pToken, "*COMMENT"))
		{
			char *pComment;

			pFilePtr = GetString(pFilePtr, &pComment);

			MFDebug_Log(4, MFStr("Comment: %s", pComment));
		}
		else if(!MFString_CaseCmp(pToken, "*SCENE"))
		{
			pFilePtr = ProcessBlock(pFilePtr, "*SCENE", ReadSceneChunk);
		}
		else if(!MFString_CaseCmp(pToken, "*MATERIAL_LIST"))
		{
			pFilePtr = ProcessBlock(pFilePtr, "*MATERIAL_LIST", ReadMaterialChunk);
		}
		else if(!MFString_CaseCmp(pToken, "*GEOMOBJECT"))
		{
			pFilePtr = ProcessBlock(pFilePtr, "*GEOMOBJECT", ReadGeomChunk);
		}
		else
		{
			MFDebug_Warn(3, MFStr("Unknown token: %s", pToken));
		}
	}
}

int F3DFile::ReadASE(const char *pFilename)
{
	pModel = this;

	MFFile *pFile = MFFileSystem_Open(pFilename, MFOF_Read);
	if(!pFile)
	{
		MFDebug_Warn(2, MFStr("Failed to open ASE file %s", pFilename));
		return 1;
	}

	int size = MFFile_Seek(pFile, 0, MFSeek_End);
	MFFile_Seek(pFile, 0, MFSeek_Begin);

	char *pMem = (char*)MFHeap_Alloc(size+1);
	MFFile_Read(pFile, pMem, size);
	pMem[size] = 0;

	MFFile_Close(pFile);

	ParseASEFile(pMem);
	MFHeap_Free(pMem);

	return 0;
}
