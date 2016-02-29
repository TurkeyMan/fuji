#include "Fuji.h"
#include "MFAsset.h"
#include "MFFileSystem.h"

// HACK:
MF_API bool MFIntAsset_ConvertAssetFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes = 0);
MF_API bool MFIntAsset_ConvertModelAndAnimationFromFile(const char *pFilename, void **ppMesh, size_t *pMeshSize, void **ppAnimation, size_t *pAnimationSize, MFPlatform platform, size_t extraBytes = 0);
MF_API bool MFIntAsset_ConvertTextureFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, uint32 flags = 0, MFImageFormat targetFormat = ImgFmt_Unknown, size_t extraBytes = 0);
MF_API bool MFIntAsset_ConvertShaderFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, MFShaderType shaderType, MFShaderMacro *pMacros, MFRendererDrivers renderDriver, MFShaderLanguage language);


static const char *gImageFileExtensions[] =
{
	".tga",
	".bmp",
	".png",
	".dds",
	".jpg",
	".jpeg",
	".webp",
	NULL
};

static const char *gMeshFileExtensions[] =
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
	".3d",
	NULL
};

static const char *gSoundFileExtensions[] =
{
	".wav",
	".mp3",
	".ogg",
	".flac",
	".xm",
	NULL
};

static const char *gShaderFileExtensions[] =
{
	".hlsl",
	".glsl",
	".cg",
	".vsh",
	".psh",
	NULL
};

MF_API const char ** MFAsset_GetImageFileTypes()
{
	return gImageFileExtensions;
}

MF_API const char ** MFAsset_GetGeometryFileTypes()
{
	return gMeshFileExtensions;
}

MF_API const char ** MFAsset_GetSoundFileTypes()
{
	return gSoundFileExtensions;
}

MF_API const char ** MFAsset_GetShaderFileTypes()
{
	return gShaderFileExtensions;
}

MF_API bool MFAsset_IsImageFile(const char *pExt)
{
	return MFString_Enumerate(pExt, gImageFileExtensions) != -1;
}

MF_API bool MFAsset_IsGeometryFile(const char *pExt)
{
	return MFString_Enumerate(pExt, gMeshFileExtensions) != -1;
}

MF_API bool MFAsset_IsSoundFile(const char *pExt)
{
	return MFString_Enumerate(pExt, gSoundFileExtensions) != -1;
}

MF_API bool MFAsset_IsShaderFile(const char *pExt)
{
	return MFString_Enumerate(pExt, gShaderFileExtensions) != -1;
}

MF_API bool MFAsset_ConvertAssetFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes)
{
	// should this request be forewarded to the file server, or should we do it locally?
	//... todo

	size_t size;
	if(!MFIntAsset_ConvertAssetFromFile(pFilename, ppOutput, &size, platform, extraBytes) || !*ppOutput)
		return false;

	if(pSize)
		*pSize = size;

	const char *pExt = MFString_GetFileExtension(pFilename);
	const char *pCacheExt = NULL;
	if(!MFString_CaseCmp(pExt, ".mfx"))
		pCacheExt = ".bfx";
	else if(!MFString_CaseCmp(pExt, ".fnt"))
		pCacheExt = ".fft";
	else if(!MFAsset_IsSoundFile(pExt))
		pCacheExt = ".snd";
	else if(!MFAsset_IsImageFile(pExt))
		pCacheExt = ".tex";
	else if(!MFAsset_IsGeometryFile(pExt))
		pCacheExt = ".mdl";
	else if(!MFAsset_IsShaderFile(pExt))
		pCacheExt = ".fsh";

	MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s%s", pFilename, pCacheExt), MFOF_Write | MFOF_Binary);
	if(pFile)
	{
		MFFile_Write(pFile, *ppOutput, size, false);
		MFFile_Close(pFile);
	}
	return true;
}

MF_API bool MFAsset_ConvertModelAndAnimationFromFile(const char *pFilename, void **ppMesh, size_t *pMeshSize, void **ppAnimation, size_t *pAnimationSize, MFPlatform platform, size_t extraBytes)
{
	// should this request be forewarded to the file server, or should we do it locally?
	//... todo

	if(!MFIntAsset_ConvertModelAndAnimationFromFile(pFilename, ppMesh, pMeshSize, ppAnimation, pAnimationSize, platform, extraBytes))
		return false;

	if(ppMesh && *ppMesh)
	{
		MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.mdl", pFilename), MFOF_Write | MFOF_Binary);
		if(pFile)
		{
			MFFile_Write(pFile, *ppMesh, *pMeshSize, false);
			MFFile_Close(pFile);
		}
	}
	if(ppAnimation && *ppAnimation)
	{
		MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.anm", pFilename), MFOF_Write | MFOF_Binary);
		if(pFile)
		{
			MFFile_Write(pFile, *ppAnimation, *pAnimationSize, false);
			MFFile_Close(pFile);
		}
	}
	return true;
}

MF_API bool MFAsset_ConvertTextureFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, uint32 flags, MFImageFormat targetFormat, size_t extraBytes)
{
	// should this request be forewarded to the file server, or should we do it locally?
	//... todo

	if(!MFIntAsset_ConvertTextureFromFile(pFilename, ppOutput, pSize, platform, flags, targetFormat, extraBytes))
		return false;

	MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.tex", pFilename), MFOF_Write | MFOF_Binary);
	if(pFile)
	{
		MFFile_Write(pFile, *ppOutput, *pSize, false);
		MFFile_Close(pFile);
	}
	return true;
}

MF_API bool MFAsset_ConvertShaderFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, MFShaderType shaderType, MFShaderMacro *pMacros, MFRendererDrivers renderDriver, MFShaderLanguage language)
{
	// should this request be forewarded to the file server, or should we do it locally?
	//... todo

	if(!MFIntAsset_ConvertShaderFromFile(pFilename, ppOutput, pSize, platform, shaderType, pMacros, renderDriver, language))
		return false;

	MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.fsh", pFilename), MFOF_Write | MFOF_Binary);
	if(pFile)
	{
		MFFile_Write(pFile, *ppOutput, *pSize, false);
		MFFile_Close(pFile);
	}
	return true;
}
