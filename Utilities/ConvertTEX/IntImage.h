#if !defined(_INTIMAGE_H)
#define _INTIMAGE_H

#define BIT(x) (1<<(x))

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

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
