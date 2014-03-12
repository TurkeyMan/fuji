#include "Fuji_Internal.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFString.h"
#include "MFModel_Internal.h"
#include "Util.h"
#include "Asset/MFIntModel.h"
#include "Asset/Util/F3D.h"


/**** Structures ****/


/**** Globals ****/

static const char *gFileExtensions[] =
{
	".f3d",
	".dae",
	".x",
	".ase",
	".obj",
	".md2",
	".md3",
	".memd2",

	// assimp formats
	".fbx",
	".blend",
	".3ds",
	".dxf",
	".lwo",
	".lws",
	".ms3d",
	".mdl",
	".pk3",
	".mdc",
	".md5",
	".smd",
	".vta",
	".m3",
	".3d"
};


/**** Functions ****/

MF_API MFIntModel *MFIntModel_CreateFromFile(const char *pFilename)
{
	// find format
	const char *pExt = MFString_GetFileExtension(pFilename);

	int format;
	for(format=0; format<MFIMF_Max; ++format)
	{
		if(!MFString_Compare(pExt, gFileExtensions[format]))
			break;
	}
	if(format == MFIMF_Max)
		return NULL; // unsupported model format

	// some text formats need a null character appended for processing
	bool bAppendNull = false;
	if(format == MFIMF_X || format == MFIMF_DAE || format == MFIMF_ASE || format == MFIMF_OBJ)
		bAppendNull = true;

	// load file
	size_t size;
	char *pData = MFFileSystem_Load(pFilename, &size, bAppendNull ? 1 : 0);
	if(!pData)
		return NULL;

	// load the image
	MFString name = pFilename;
	name.TruncateExtension();
	MFIntModel *pModel = MFIntModel_CreateFromFileInMemory(pData, size, (MFIntModelFormat)format, name.CStr());

	// free file
	MFHeap_Free(pData);

	return pModel;
}

void ParseAssimpMesh(char *pFile, size_t size, const char *pExt, F3DFile *_pModel);

void ParseDAEFileFromMemory(char *pFile, size_t size, F3DFile *_pModel);
void ParseXFileFromMemory(char *pFile, F3DFile *_pModel);
void ParseASEFile(char *, F3DFile *);
void ParseOBJFileFromMemory(char *pFile, F3DFile *_pModel);
void ParseMD2FileFromMemory(char *pFile, size_t size, F3DFile *_pModel);

MF_API MFIntModel *MFIntModel_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntModelFormat format, const char *pName)
{
	F3DFile *pF3D = new F3DFile;
	pF3D->name = pName;

	MFDebug_Log(0, MFStr("%s: %s", pName, gFileExtensions[format]));


#if defined(USE_ASSIMP)
	ParseAssimpMesh((char*)pMemory, size, gFileExtensions[format], pF3D);
#else
	switch(format)
	{
		case MFIMF_DAE:
			ParseDAEFileFromMemory((char*)pMemory, size, pF3D);
			break;
		case MFIMF_X:
			ParseXFileFromMemory((char*)pMemory, pF3D);
			break;
		case MFIMF_ASE:
			ParseASEFile((char*)pMemory, pF3D);
			break;
		case MFIMF_OBJ:
			ParseOBJFileFromMemory((char*)pMemory, pF3D);
			break;
		case MFIMF_MD2:
			ParseMD2FileFromMemory((char*)pMemory, size, pF3D);
			break;
		default:
			MFDebug_Assert(false, "Unsupported model format.");
	}
#endif

	return (MFIntModel*)pF3D;
}

MF_API void MFIntModel_Optimise(MFIntModel *pModel)
{
	F3DFile *pF3D = (F3DFile*)pModel;

	pF3D->Optimise();
}

MF_API void MFIntModel_CreateRuntimeData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes)
{
	F3DFile *pF3D = (F3DFile*)pModel;

	*ppOutput = NULL;
	if(pSize)
		*pSize = 0;

	pF3D->ProcessSkeletonData();
	pF3D->ProcessCollisionData();
	pF3D->BuildBatches(platform);
	pF3D->StripModel();

	if(pF3D->GetMeshChunk()->subObjects.size() ||
		pF3D->GetCollisionChunk()->collisionObjects.size() ||
		pF3D->GetRefPointChunk()->refPoints.size())
	{
		*ppOutput = pF3D->CreateMDL(pSize, platform, extraBytes);
	}
}

MF_API void MFIntModel_CreateAnimationData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes)
{
	F3DFile *pF3D = (F3DFile*)pModel;

	*ppOutput = NULL;
	*pSize = 0;

	// we'll assume the user wrote out the model first (so it does the processing and stripping/etc)

	if(pF3D->GetAnimationChunk()->anims.size())
	{
		*ppOutput = pF3D->CreateANM(pSize, platform, extraBytes);
	}
}

MF_API void MFIntModel_Destroy(MFIntModel *pModel)
{
	F3DFile *pF3D = (F3DFile*)pModel;
	delete pF3D;
}
