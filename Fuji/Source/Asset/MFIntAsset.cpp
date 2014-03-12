#include "Fuji.h"
#include "Asset/MFIntEffect.h"
#include "Asset/MFIntShader.h"
#include "Asset/MFIntTexture.h"
#include "Asset/MFIntModel.h"
#include "Asset/MFIntSound.h"
#include "Asset/MFIntFont.h"
#include "MFFileSystem.h"
#include "MFAsset.h"


/*
MFAsset API:
  Hot load FujiAsset.dll?
  If unavailable, request from service automatically?
  Receive MFFile instead of filename maybe?
*/


MF_API bool MFIntAsset_ConvertAssetFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes)
{
	// get asset type from filename and convert it...
	const char *pExt = MFString_GetFileExtension(pFilename);
	if(!MFString_CaseCmp(pExt, ".mfx"))
	{
		MFIntEffect *pIntEffect = MFIntEffect_CreateFromSourceData(pFilename);
		if(!pIntEffect)
			return false;
		MFIntEffect_CreateRuntimeData(pIntEffect, (MFEffect**)ppOutput, pSize, platform, extraBytes);
		MFIntEffect_Destroy(pIntEffect);
		return true;
	}
	else if(MFAsset_IsShaderFile(pExt))
	{
		MFDebug_Assert(false, "MFIntAsset_ConvertAssetFromFile() can't build shaders; not enough information available...");
//		MFIntShader_CreateFromFile(, pFilename, NULL, ppOutput, pSize, platform, , );
	}
	else if(MFAsset_IsImageFile(pExt))
	{
		MFIntTexture *pTex = MFIntTexture_CreateFromFile(pFilename);
		if(!pTex)
			return false;
		MFIntTexture_CreateRuntimeData(pTex, (MFTextureTemplateData**)ppOutput, pSize, platform);
		MFIntTexture_Destroy(pTex);
		return true;
	}
	else if(MFAsset_IsGeometryFile(pExt))
	{
		MFIntModel *pModel = MFIntModel_CreateFromFile(pFilename);
		if(!pModel)
			return false;
		MFIntModel_Optimise(pModel);
		MFIntModel_CreateRuntimeData(pModel, ppOutput, pSize, platform, extraBytes);
		MFIntModel_Destroy(pModel);
		return true;
	}
	else if(MFAsset_IsSoundFile(pExt))
	{
		MFIntSound *pSound = MFIntSound_CreateFromFile(pFilename);
		if(!pSound)
			return false;
		MFIntSound_CreateRuntimeData(pSound, ppOutput, pSize, platform);
		MFIntSound_Destroy(pSound);
		return true;
	}
	else if(!MFString_CaseCmp(pExt, ".fnt"))
	{
		MFIntFont_CreateFromSourceData(pFilename, ppOutput, pSize, platform);
		return true;
	}

	MFDebug_Assert(false, MFStr("Unknown asset type: %s", pFilename));
	return false;
}

MF_API bool MFIntAsset_ConvertModelAndAnimationFromFile(const char *pFilename, void **ppMesh, size_t *pMeshSize, void **ppAnimation, size_t *pAnimationSize, MFPlatform platform, size_t extraBytes = 0)
{
	MFIntModel *pModel = MFIntModel_CreateFromFile(pFilename);
	if(!pModel)
		return false;

	MFIntModel_Optimise(pModel);
	MFIntModel_CreateRuntimeData(pModel, ppMesh, pMeshSize, platform, extraBytes);
	if(ppAnimation)
		MFIntModel_CreateAnimationData(pModel, ppAnimation, pAnimationSize, platform, extraBytes);
	MFIntModel_Destroy(pModel);
	return true;
}

MF_API bool MFIntAsset_ConvertTextureFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, uint32 flags = 0, MFImageFormat targetFormat = ImgFmt_Unknown, size_t extraBytes = 0)
{
	MFIntTexture *pTex = MFIntTexture_CreateFromFile(pFilename);
	if(!pTex)
		return false;

	MFIntTexture_CreateRuntimeData(pTex, (MFTextureTemplateData**)ppOutput, pSize, platform, flags, targetFormat);
	MFIntTexture_Destroy(pTex);
	return true;
}

MF_API bool MFIntAsset_ConvertShaderFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, MFShaderType shaderType, MFShaderMacro *pMacros, MFRendererDrivers renderDriver, MFShaderLanguage language)
{
	return MFIntShader_CreateFromFile(shaderType, pFilename, pMacros, ppOutput, pSize, platform, renderDriver, language);
}
