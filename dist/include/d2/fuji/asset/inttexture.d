module fuji.asset.inttexture;

import fuji.fuji;
import fuji.texture;

enum MFIntTextureFormat
{
	Unknown = -1,

	TGA = 0,
	BMP,
	PNG
}

enum MFIntTextureFlags : uint
{
	PreMultipliedAlpha = 1,	// use pre-multiply alpha
	FadeAlpha = 2			// fade the alpha to transparent across mip levels
}

struct MFTextureTemplateData;

// very high precision intermediate image format
struct MFIntTexturePixel
{
	float r, g, b, a;
}

struct MFIntTextureSurface
{
	int width, height;
	MFIntTexturePixel* pData;
}

struct MFIntTexture
{
	MFIntTextureSurface* pSurfaces;
	int numSurfaces;

	bool opaque;
	bool oneBitAlpha;
}

extern (C) MFIntTexture* MFIntTexture_CreateFromFile(const(char*) pFilename);
extern (C) MFIntTexture* MFIntTexture_CreateFromFileInMemory(const(void*) pMemory, size_t size, MFIntTextureFormat format);

extern (C) void MFIntTexture_Destroy(MFIntTexture* pTexture);

extern (C) void MFIntTexture_CreateRuntimeData(MFIntTexture* pTexture, MFTextureTemplateData** ppTemplateData, size_t* pSize, MFPlatform platform, uint flags = 0, MFTextureFormat targetFormat = MFTextureFormat.Unknown);

extern (C) void MFIntTexture_WriteToHeaderFile(MFIntTexture* pTexture, const(char*) pFilename);

extern (C) void MFIntTexture_FilterMipMaps(MFIntTexture* pTexture, int numMipLevels, uint mipFilterOptions);
extern (C) void MFIntTexture_ScanImage(MFIntTexture* pTexture);
extern (C) void MFIntTexture_FlipImage(MFIntTexture* pTexture);

