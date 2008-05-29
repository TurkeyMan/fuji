#if !defined(_MFINTTEXTURE_H)
#define _MFINTTEXTURE_H

enum MFIntTextureMipFilterOptions
{
	MFITO_FadeAlpha = 1 // fade the alpha to transparent across mip levels
};

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
	int mipLevels;

	bool opaque;
	bool oneBitAlpha;
};

enum MFIntTextureFormat
{
	MFIMF_Unknown = -1,

	MFIMF_TGA = 0,
	MFIMF_BMP,
	MFIMF_PNG,

	MFIMF_Max,
	MFIMF_ForceInt = 0x7FFFFFFF
};

MFIntTexture *MFIntTexture_CreateFromFile(const char *pFilename);
MFIntTexture *MFIntTexture_CreateFromFileInMemory(const void *pMemory, uint32 size, MFIntTextureFormat format);

void MFIntTexture_Destroy(MFIntTexture *pTexture);

void MFIntTexture_CreateRuntimeData(MFIntTexture *pTexture, void **ppOutput, uint32 *pSize, MFPlatform platform);

void MFIntTexture_FilterMipMaps(MFIntTexture *pTexture, int numMipLevels, uint32 mipFilterOptions);
void MFIntTexture_ScanImage(MFIntTexture *pTexture);
void MFIntTexture_FlipImage(MFIntTexture *pTexture);

#endif
