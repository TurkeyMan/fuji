#if !defined(_INTIMAGE_H)
#define _INTIMAGE_H

#define BIT(x) (1<<(x))

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

typedef unsigned __int64	uint64;
typedef __int64				int64;
typedef unsigned int		uint32;
typedef int					int32;
typedef unsigned short		uint16;
typedef short				int16;
typedef unsigned char		uint8;
typedef char				int8;

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
