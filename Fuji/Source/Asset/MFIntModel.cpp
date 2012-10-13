#include "Fuji.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFString.h"
#include "MFModel_Internal.h"
#include "Asset/MFIntModel.h"
#include "Util/F3D.h"

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
	".memd2"
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
	char *pData = MFFileSystem_Load(pFilename, &size, bAppendNull);
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

void ParseDAEFileFromMemory(char *pFile, size_t size, F3DFile *_pModel);
void ParseXFileFromMemory(char *pFile, F3DFile *_pModel);
void ParseASEFile(char *, F3DFile *);
void ParseOBJFileFromMemory(char *pFile, F3DFile *_pModel);
void ParseMD2FileFromMemory(char *pFile, size_t size, F3DFile *_pModel);

MF_API MFIntModel *MFIntModel_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntModelFormat format, const char *pName)
{
	F3DFile *pF3D = new F3DFile;
	MFString_Copy(pF3D->name, pName);

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

	return (MFIntModel*)pF3D;
}

MF_API void MFIntModel_Optimise(MFIntModel *pModel)
{
	F3DFile *pF3D = (F3DFile*)pModel;

	pF3D->Optimise();
}

MF_API void MFIntModel_CreateRuntimeData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform)
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
		*ppOutput = pF3D->CreateMDL(pSize, platform);
	}
}

MF_API void MFIntModel_CreateAnimationData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform)
{
	F3DFile *pF3D = (F3DFile*)pModel;

	*ppOutput = NULL;
	*pSize = 0;

	// we'll assume the user wrote out the model first (so it does the processing and stripping/etc)

	if(pF3D->GetAnimationChunk()->anims.size())
	{
		*ppOutput = pF3D->CreateANM(pSize, platform);
	}
}

MF_API void MFIntModel_Destroy(MFIntModel *pModel)
{
	F3DFile *pF3D = (F3DFile*)pModel;
	delete pF3D;
}
