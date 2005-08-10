#if !defined(_INTIMAGE_H)
#define _INTIMAGE_H

enum MipFilterOptions
{
	MFO_FadeAlpha = 1 // fade the alpha to transparent across mip levels
};

// very high precision intermediate image format

struct Pixel
{
	float r,g,b,a;
};

struct SourceImageLevel
{
	int width, height;
	Pixel *pData;
};

struct SourceImage
{
	int mipLevels;

	SourceImageLevel *pLevels;
};

void DestroyImage(SourceImage *pImage);
void FilterMipMaps(int numMipLevels, uint32 mipFilterOptions);

#endif
