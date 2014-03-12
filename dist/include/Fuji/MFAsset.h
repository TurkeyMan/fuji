#pragma once
#if !defined(_MFASSET_H)
#define _MFASSET_H

#include "MFShader.h"
#include "MFImage.h"

MF_API const char ** MFAsset_GetImageFileTypes();
MF_API const char ** MFAsset_GetGeometryFileTypes();
MF_API const char ** MFAsset_GetSoundFileTypes();
MF_API const char ** MFAsset_GetShaderFileTypes();

MF_API bool MFAsset_IsImageFile(const char *pExt);
MF_API bool MFAsset_IsGeometryFile(const char *pExt);
MF_API bool MFAsset_IsSoundFile(const char *pExt);
MF_API bool MFAsset_IsShaderFile(const char *pExt);

MF_API bool MFAsset_ConvertAssetFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes = 0);
MF_API bool MFAsset_ConvertModelAndAnimationFromFile(const char *pFilename, void **ppMesh, size_t *pMeshSize, void **ppAnimation, size_t *pAnimationSize, MFPlatform platform, size_t extraBytes = 0);
MF_API bool MFAsset_ConvertTextureFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, uint32 flags = 0, MFImageFormat targetFormat = ImgFmt_Unknown, size_t extraBytes = 0);
MF_API bool MFAsset_ConvertShaderFromFile(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, MFShaderType shaderType, MFShaderMacro *pMacros, MFRendererDrivers renderDriver, MFShaderLanguage language = MFSL_Unknown);

#endif
