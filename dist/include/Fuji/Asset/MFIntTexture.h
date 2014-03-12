#pragma once
#if !defined(_MFINTTEXTURE_H)
#define _MFINTTEXTURE_H

#include "MFImage.h"

enum MFIntTextureFormat
{
	MFITF_Unknown = -1,

	MFITF_TGA = 0,
	MFITF_BMP,
	MFITF_PNG,
	MFITF_DDS,
	MFITF_JPEG,
	MFITF_WEBP,

	MFITF_Max,
	MFITF_ForceInt = 0x7FFFFFFF
};

enum MFIntTextureFlags
{
	MFITF_PreMultipliedAlpha = 1,	// use pre-multiply alpha
	MFITF_FadeAlpha = 2				// fade the alpha to transparent across mip levels
};

struct MFTextureTemplateData;

// very high precision intermediate image format
struct MFIntTexturePixel
{
	float r,g,b,a;
};

struct MFIntTextureSurface
{
	int width, height;
	MFIntTexturePixel *pData;
};

struct MFIntTexture
{
	MFIntTextureSurface *pSurfaces;
	int numSurfaces;

	bool opaque;
	bool oneBitAlpha;
};

MF_API MFIntTexture *MFIntTexture_CreateFromFile(const char *pFilename);
MF_API MFIntTexture *MFIntTexture_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntTextureFormat format);

MF_API void MFIntTexture_Destroy(MFIntTexture *pTexture);

MF_API void MFIntTexture_CreateRuntimeData(MFIntTexture *pTexture, MFTextureTemplateData **ppTemplateData, size_t *pSize, MFPlatform platform, uint32 flags = 0, MFImageFormat targetFormat = ImgFmt_Unknown);

MF_API void MFIntTexture_WriteToHeaderFile(MFIntTexture *pTexture, const char *pFilename);

MF_API void MFIntTexture_FilterMipMaps(MFIntTexture *pTexture, int numMipLevels, uint32 mipFilterOptions);

MF_API void MFIntTexture_ScanImage(MFIntTexture *pTexture);
MF_API void MFIntTexture_FlipImage(MFIntTexture *pTexture);

#endif
