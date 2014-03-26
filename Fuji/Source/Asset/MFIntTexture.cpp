#include "Fuji_Internal.h"

#define MF_ENABLE_PNG
#define MF_ENABLE_JPEG

#if defined(MF_ENABLE_PNG)
	#if defined(MF_LINUX) || defined(MF_OSX)
		#include <png.h>
	#else
		#include "png.h"
		#include "pngstruct.h"
		#include "pnginfo.h"
	#endif
#endif

#if defined(MF_ENABLE_JPEG)
	#if defined(MF_WINDOWS)
		#include "libjpeg-turbo/jpeglib.h"
		#pragma comment(lib, "jpeg-static")

		#define SUPPORTS_JPEG
	#elif defined(MF_LINUX) || defined(MF_OSX)
//		#include <jpeg.h>

//		#define SUPPORTS_JPEG
	#endif
#endif

#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFString.h"
#include "MFImage.h"
#include "MFTexture_Internal.h"
#include "Util.h"
#include "Asset/MFIntTexture.h"


/**** Structures ****/

#pragma pack(1)
struct TgaHeader
{
	uint8 idLength;
	uint8 colourMapType;
	uint8 imageType;

	uint16 colourMapStart;
	uint16 colourMapLength;
	uint8 colourMapBits;

	uint16 xStart;
	uint16 yStart;
	uint16 width;
	uint16 height;
	uint8 bpp;
	uint8 flags;
};
#pragma pack ()

// BMP support

enum BMPCompressionType
{
	BMCT_RGB = 0,		/* No compression - straight BGR data */
	BMCT_RLE8 = 1,		/* 8-bit run-length compression */
	BMCT_RLE4 = 2,		/* 4-bit run-length compression */
	BMCT_BITFIELDS = 3	/* RGB bitmap with RGB masks */
};

#pragma pack(1)
struct BMPHeader
{
   unsigned short int type;                 /* Magic identifier            */
   unsigned int size;                       /* File size in bytes          */
   unsigned short int reserved1, reserved2;
   unsigned int offset;                     /* Offset to image data, bytes */
};

struct BMPInfoHeader
{
   unsigned int size;               /* Header size in bytes      */
   int width,height;                /* Width and height of image */
   unsigned short int planes;       /* Number of colour planes   */
   unsigned short int bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int importantcolours;   /* Important colours         */
};
#pragma pack ()

struct BMPPaletteEntry /**** Colourmap entry structure ****/
{
	unsigned char  rgbBlue;          /* Blue value */
	unsigned char  rgbGreen;         /* Green value */
	unsigned char  rgbRed;           /* Red value */
	unsigned char  rgbReserved;      /* Reserved */
};

// DDS support

struct DDS_PIXELFORMAT {
  uint32 dwSize;
  uint32 dwFlags;
  uint32 dwFourCC;
  uint32 dwRGBBitCount;
  uint32 dwRBitMask;
  uint32 dwGBitMask;
  uint32 dwBBitMask;
  uint32 dwABitMask;
};

typedef struct {
  uint32          dwSize;
  uint32          dwFlags;
  uint32          dwHeight;
  uint32          dwWidth;
  uint32          dwPitchOrLinearSize;
  uint32          dwDepth;
  uint32          dwMipMapCount;
  uint32          dwReserved1[11];
  DDS_PIXELFORMAT ddspf;
  uint32          dwCaps;
  uint32          dwCaps2;
  uint32          dwCaps3;
  uint32          dwCaps4;
  uint32          dwReserved2;
} DDS_HEADER;

typedef enum DXGI_FORMAT { 
  DXGI_FORMAT_UNKNOWN                     = 0,
  DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
  DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
  DXGI_FORMAT_R32G32B32A32_UINT           = 3,
  DXGI_FORMAT_R32G32B32A32_SINT           = 4,
  DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
  DXGI_FORMAT_R32G32B32_FLOAT             = 6,
  DXGI_FORMAT_R32G32B32_UINT              = 7,
  DXGI_FORMAT_R32G32B32_SINT              = 8,
  DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
  DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
  DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
  DXGI_FORMAT_R16G16B16A16_UINT           = 12,
  DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
  DXGI_FORMAT_R16G16B16A16_SINT           = 14,
  DXGI_FORMAT_R32G32_TYPELESS             = 15,
  DXGI_FORMAT_R32G32_FLOAT                = 16,
  DXGI_FORMAT_R32G32_UINT                 = 17,
  DXGI_FORMAT_R32G32_SINT                 = 18,
  DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
  DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
  DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
  DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
  DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
  DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
  DXGI_FORMAT_R10G10B10A2_UINT            = 25,
  DXGI_FORMAT_R11G11B10_FLOAT             = 26,
  DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
  DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
  DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
  DXGI_FORMAT_R8G8B8A8_UINT               = 30,
  DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
  DXGI_FORMAT_R8G8B8A8_SINT               = 32,
  DXGI_FORMAT_R16G16_TYPELESS             = 33,
  DXGI_FORMAT_R16G16_FLOAT                = 34,
  DXGI_FORMAT_R16G16_UNORM                = 35,
  DXGI_FORMAT_R16G16_UINT                 = 36,
  DXGI_FORMAT_R16G16_SNORM                = 37,
  DXGI_FORMAT_R16G16_SINT                 = 38,
  DXGI_FORMAT_R32_TYPELESS                = 39,
  DXGI_FORMAT_D32_FLOAT                   = 40,
  DXGI_FORMAT_R32_FLOAT                   = 41,
  DXGI_FORMAT_R32_UINT                    = 42,
  DXGI_FORMAT_R32_SINT                    = 43,
  DXGI_FORMAT_R24G8_TYPELESS              = 44,
  DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
  DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
  DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
  DXGI_FORMAT_R8G8_TYPELESS               = 48,
  DXGI_FORMAT_R8G8_UNORM                  = 49,
  DXGI_FORMAT_R8G8_UINT                   = 50,
  DXGI_FORMAT_R8G8_SNORM                  = 51,
  DXGI_FORMAT_R8G8_SINT                   = 52,
  DXGI_FORMAT_R16_TYPELESS                = 53,
  DXGI_FORMAT_R16_FLOAT                   = 54,
  DXGI_FORMAT_D16_UNORM                   = 55,
  DXGI_FORMAT_R16_UNORM                   = 56,
  DXGI_FORMAT_R16_UINT                    = 57,
  DXGI_FORMAT_R16_SNORM                   = 58,
  DXGI_FORMAT_R16_SINT                    = 59,
  DXGI_FORMAT_R8_TYPELESS                 = 60,
  DXGI_FORMAT_R8_UNORM                    = 61,
  DXGI_FORMAT_R8_UINT                     = 62,
  DXGI_FORMAT_R8_SNORM                    = 63,
  DXGI_FORMAT_R8_SINT                     = 64,
  DXGI_FORMAT_A8_UNORM                    = 65,
  DXGI_FORMAT_R1_UNORM                    = 66,
  DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
  DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
  DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
  DXGI_FORMAT_BC1_TYPELESS                = 70,
  DXGI_FORMAT_BC1_UNORM                   = 71,
  DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
  DXGI_FORMAT_BC2_TYPELESS                = 73,
  DXGI_FORMAT_BC2_UNORM                   = 74,
  DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
  DXGI_FORMAT_BC3_TYPELESS                = 76,
  DXGI_FORMAT_BC3_UNORM                   = 77,
  DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
  DXGI_FORMAT_BC4_TYPELESS                = 79,
  DXGI_FORMAT_BC4_UNORM                   = 80,
  DXGI_FORMAT_BC4_SNORM                   = 81,
  DXGI_FORMAT_BC5_TYPELESS                = 82,
  DXGI_FORMAT_BC5_UNORM                   = 83,
  DXGI_FORMAT_BC5_SNORM                   = 84,
  DXGI_FORMAT_B5G6R5_UNORM                = 85,
  DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
  DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
  DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
  DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
  DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
  DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
  DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
  DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
  DXGI_FORMAT_BC6H_TYPELESS               = 94,
  DXGI_FORMAT_BC6H_UF16                   = 95,
  DXGI_FORMAT_BC6H_SF16                   = 96,
  DXGI_FORMAT_BC7_TYPELESS                = 97,
  DXGI_FORMAT_BC7_UNORM                   = 98,
  DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
  DXGI_FORMAT_AYUV                        = 100,
  DXGI_FORMAT_Y410                        = 101,
  DXGI_FORMAT_Y416                        = 102,
  DXGI_FORMAT_NV12                        = 103,
  DXGI_FORMAT_P010                        = 104,
  DXGI_FORMAT_P016                        = 105,
  DXGI_FORMAT_420_OPAQUE                  = 106,
  DXGI_FORMAT_YUY2                        = 107,
  DXGI_FORMAT_Y210                        = 108,
  DXGI_FORMAT_Y216                        = 109,
  DXGI_FORMAT_NV11                        = 110,
  DXGI_FORMAT_AI44                        = 111,
  DXGI_FORMAT_IA44                        = 112,
  DXGI_FORMAT_P8                          = 113,
  DXGI_FORMAT_A8P8                        = 114,
  DXGI_FORMAT_B4G4R4A4_UNORM              = 115,
  DXGI_FORMAT_FORCE_UINT                  = 0xffffffffUL
} DXGI_FORMAT;

typedef enum D3D10_RESOURCE_DIMENSION { 
  D3D10_RESOURCE_DIMENSION_UNKNOWN    = 0,
  D3D10_RESOURCE_DIMENSION_BUFFER     = 1,
  D3D10_RESOURCE_DIMENSION_TEXTURE1D  = 2,
  D3D10_RESOURCE_DIMENSION_TEXTURE2D  = 3,
  D3D10_RESOURCE_DIMENSION_TEXTURE3D  = 4
} D3D10_RESOURCE_DIMENSION;

typedef enum DDSCAPS2 {
  DDSCAPS2_CUBEMAP = 0x200,
  DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,
  DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,
  DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
  DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
  DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
  DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,
  DDSCAPS2_VOLUME = 0x200000
} DDSCAPS2;

typedef enum DDPIXELFORMAT {
  DDPF_ALPHAPIXELS = 0x1,
  DDPF_ALPHA = 0x2,
  DDPF_FOURCC = 0x4,
  DDPF_RGB = 0x40,
  DDPF_YUV = 0x200,
  DDPF_LUMINANCE = 0x20000
} DDPIXELFORMAT;

typedef struct {
  DXGI_FORMAT              dxgiFormat;
  D3D10_RESOURCE_DIMENSION resourceDimension;
  uint32                   miscFlag;
  uint32                   arraySize;
  uint32                   miscFlags2;
} DDS_HEADER_DXT10;

struct DDS_IMAGE
{
	uint32 magic; // == 0x20534444
	DDS_HEADER header;
	union
	{
		char imageData[1];
		struct
		{
			DDS_HEADER_DXT10 dx10Header;
			char imageData10[1];
		};
	};
};

/**** Globals ****/

static const char *gFileExtensions[] =
{
	".tga",
	".bmp",
	".png",
	".dds",
	".jpg",
	".jpeg",
	".webp"
};

static const int gNumFileExtensions = sizeof(gFileExtensions) / sizeof(gFileExtensions[0]);

MFIntTextureFormat gFileTypeMap[gNumFileExtensions] =
{
	MFITF_TGA,
	MFITF_BMP,
	MFITF_PNG,
	MFITF_DDS,
	MFITF_JPEG,
	MFITF_JPEG,
	MFITF_WEBP
};


/**** Functions ****/

#if defined(MF_ENABLE_PNG)
void PNGAPI png_file_read(png_structp png, png_bytep pBuffer, png_size_t bytes)
{
	MFFile_Read((MFFile*)png->io_ptr, pBuffer, (uint32)bytes, false);
}

MFIntTexture* LoadPNG(const void *pMemory, size_t size)
{
	if(png_sig_cmp((uint8*)pMemory, 0, 8))
	{
		MFDebug_Warn(2, "Not a PNG file..");
		return NULL;
	}

	MFFile *pFile = MFFile_CreateMemoryFile(pMemory, size);

	if(!pFile)
		return NULL;

	// skip past the sig
	MFFile_Seek(pFile, 8, MFSeek_Begin);

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	// initialize stuff
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
//	setjmp(png_jmpbuf(png_ptr));

	png_set_read_fn(png_ptr, pFile, png_file_read);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	// read file
//	setjmp(png_jmpbuf(png_ptr));

	row_pointers = (png_bytep*)MFHeap_Alloc((sizeof(png_bytep) + info_ptr->rowbytes)*height);
	for(int y=0; y<height; y++)
		row_pointers[y] = (png_byte*)(row_pointers + height) + info_ptr->rowbytes*y;

	png_read_image(png_ptr, row_pointers);

	MFFile_Close(pFile);

	// allocate internal image structures
	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	pImage->numSurfaces = 1;
	pImage->numMips = 1;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface));

	pImage->pSurfaces[0].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*width*height);
	pImage->pSurfaces[0].width = width;
	pImage->pSurfaces[0].height = height;
	pImage->pSurfaces[0].depth = 1;

	MFIntTexturePixel *pPixel = pImage->pSurfaces[0].pData;

	switch(color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					MFImage_Convert(width, 1, row_pointers[y], ImgFmt_L8, pPixel, ImgFmt_ABGR_F32);
					pPixel += width;
				}
				else if(bit_depth == 16)
				{
					MFImage_Convert(width, 1, row_pointers[y], ImgFmt_L16, pPixel, ImgFmt_ABGR_F32);
					pPixel += width;
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
		case PNG_COLOR_TYPE_PALETTE:
			break;
		case PNG_COLOR_TYPE_RGB:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					MFImage_Convert(width, 1, row_pointers[y], ImgFmt_B8G8R8, pPixel, ImgFmt_ABGR_F32);
					pPixel += width;
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/65535.0f);
						pPixel->g = (float)p[1] * (1.0f/65535.0f);
						pPixel->b = (float)p[2] * (1.0f/65535.0f);
						pPixel->a = 1.0f;

						p += 3;
						++pPixel;
					}
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					MFImage_Convert(width, 1, row_pointers[y], ImgFmt_A8B8G8R8, pPixel, ImgFmt_ABGR_F32);
					pPixel += width;
				}
				else if(bit_depth == 16)
				{
					MFImage_Convert(width, 1, row_pointers[y], ImgFmt_A16B16G16R16, pPixel, ImgFmt_ABGR_F32);
					pPixel += width;
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					MFImage_Convert(width, 1, row_pointers[y], ImgFmt_A8L8, pPixel, ImgFmt_ABGR_F32);
					pPixel += width;
				}
				else if(bit_depth == 16)
				{
					MFImage_Convert(width, 1, row_pointers[y], ImgFmt_A16L16, pPixel, ImgFmt_ABGR_F32);
					pPixel += width;
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
	}

	// free image
	MFHeap_Free(row_pointers);

	return pImage;
}
#endif

MFIntTexture* LoadTGA(const void *pMemory, size_t imageSize)
{
	unsigned char *pTarga = (unsigned char *)pMemory;

	if(imageSize < (sizeof(TgaHeader) + 1))
		return NULL;

	TgaHeader *pHeader = (TgaHeader*)pTarga;
	unsigned char *pImageData = pTarga + sizeof(TgaHeader);

	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	pImage->numSurfaces = 1;
	pImage->numMips = 1;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface));

	pImage->pSurfaces[0].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*pHeader->width*pHeader->height);
	pImage->pSurfaces[0].width = pHeader->width;
	pImage->pSurfaces[0].height = pHeader->height;
	pImage->pSurfaces[0].depth = 1;

	MFIntTexturePixel *pPixel = pImage->pSurfaces[0].pData;

	unsigned char *pPosition;
	bool isSavedFlipped = true;

	pPosition = pTarga;
	pPosition += sizeof(TgaHeader);

	if((pHeader->imageType != 1) && (pHeader->imageType != 2) && (pHeader->imageType != 10))
	{
		MFDebug_Warn(2, MFStr("Failed loading image (Unhandled TGA type (%d))\n", pHeader->imageType));
		return NULL;
	}

	if((pHeader->bpp != 24) && (pHeader->bpp != 32) && (pHeader->bpp != 16))
	{
		MFDebug_Warn(2, MFStr("Failed loading image (Invalid colour depth (%d))", pHeader->bpp));
		return NULL;
	}

	if((pHeader->flags & 0xC0))
	{
		MFDebug_Warn(2, "Failed loading image (Interleaved images not supported)");
		return NULL;
	}

	if((pHeader->flags & 0x20) >> 5)
	{
		isSavedFlipped = false;
	}

	if((pPosition + pHeader->idLength + (pHeader->colourMapLength * pHeader->colourMapBits * pHeader->colourMapType)) >= pTarga + imageSize)
	{
		MFDebug_Warn(2, "Failed loading image (Unexpected end of file)");
		return NULL;
	}

	pPosition += pHeader->idLength;

	int bytesPerPixel = pHeader->bpp/8;

	if(pHeader->imageType == 10) // RLE, ick...
	{
		uint32 pixelsRead = 0;

		while(pixelsRead < (uint32)(pHeader->width * pHeader->height))
		{
			if(pPosition >= pTarga + imageSize)
			{
				MFDebug_Warn(2, "Failed loading image (Unexpected end of file)");
				return NULL;
			}

			if(*pPosition & 0x80) // Run length packet
			{
				uint8 length = ((*pPosition) & 0x7F) + 1;

				pPosition += 1;

				if((pPosition + bytesPerPixel) > pTarga + imageSize)
				{
					MFDebug_Warn(2, "Failed loading image (Unexpected end of file)");
					return NULL;
				}

				if((pixelsRead + length) > (uint32)(pHeader->width * pHeader->height))
				{
					MFDebug_Warn(2, "Failed loading image (Unexpected end of file)");
					return NULL;
				}

				MFIntTexturePixel pixel;

				pixel.r = (float)pPosition[2] * (1.0f/255.0f);
				pixel.g = (float)pPosition[1] * (1.0f/255.0f);
				pixel.b = (float)pPosition[0] * (1.0f/255.0f);
				if(pHeader->bpp == 32)
					pixel.a = (float)pPosition[3] * (1.0f/255.0f);
				else
					pixel.a = 1.0f;

				for(int i = 0; i < length; i++)
				{
					*pPixel = pixel;
					++pPixel;
				}

				pixelsRead += length;
				pPosition += bytesPerPixel;
			}
			else
			{ // Raw packet
				uint8 length = ((*pPosition) & 0x7F) + 1;

				pPosition += 1;

				if((pPosition + (bytesPerPixel * length)) > pTarga + imageSize)
				{
					MFDebug_Warn(2, "Failed loading image (Unexpected end of file)");
					return NULL;
				}

				if((pixelsRead + length) > (uint32)(pHeader->width * pHeader->height))
				{
					MFDebug_Warn(2, "Failed loading image (Unexpected end of file)");
					return NULL;
				}

				for(int i=0; i<length; i++)
				{
					pPixel->r = (float)pPosition[2] * (1.0f/255.0f);
					pPixel->g = (float)pPosition[1] * (1.0f/255.0f);
					pPixel->b = (float)pPosition[0] * (1.0f/255.0f);
					if(pHeader->bpp == 32)
						pPixel->a = (float)pPosition[3] * (1.0f/255.0f);
					else
						pPixel->a = 1.0f;

					++pPixel;

					pPosition += bytesPerPixel;
				}

				pixelsRead += length;
			}
		}
	}
	else if(pHeader->imageType == 2) // raw RGB
	{
		if((pPosition + (bytesPerPixel * (pHeader->width * pHeader->height))) > pTarga + imageSize)
		{
			MFDebug_Warn(2, "Failed loading image (Unexpected end of file)");
			return NULL;
		}

		if(pHeader->bpp == 16)
		{
			// this doesnt actually seem to be valid even tho the spec says so.
//			uint8 hasAlpha = pHeader->flags & 0xF;
			uint8 hasAlpha = 0;

			for(int a=0; a<pHeader->width*pHeader->height; a++)
			{
				uint16 c = *(uint16*)pImageData;
				uint8 r, g, b;

				r = (c >> 7) & 0xF8;
				r |= r >> 5;
				g = (c >> 2) & 0xF8;
				g |= g >> 5;
				b = (c << 3) & 0xF8;
				b |= b >> 5;

				pPixel->r = (float)r * (1.0f/255.0f);
				pPixel->g = (float)g * (1.0f/255.0f);
				pPixel->b = (float)b * (1.0f/255.0f);
				pPixel->a = hasAlpha ? ((c & 0x8000) ? 1.0f : 0.0f) : 1.0f;

				pImageData += bytesPerPixel;
				++pPixel;
			}
		}
		else if(pHeader->bpp == 24)
		{
			MFImage_Convert(pHeader->width, pHeader->height, pImageData, ImgFmt_R8G8B8, pPixel, ImgFmt_ABGR_F32);
		}
		else if(pHeader->bpp == 32)
		{
			MFImage_Convert(pHeader->width, pHeader->height, pImageData, ImgFmt_A8R8G8B8, pPixel, ImgFmt_ABGR_F32);
		}
	}
	else if(pHeader->imageType == 1) // paletted
	{
		MFDebug_Assert(false, "Paletted images not yet supported....");
	}

	if(isSavedFlipped)
	{
		MFIntTexture_FlipImage(pImage);
	}

	return pImage;
}

MFIntTexture* LoadBMP(const void *pMemory, size_t imageSize)
{
	unsigned char *pBMP = (unsigned char *)pMemory;

	if(imageSize < (sizeof(BMPHeader) + 1))
		return NULL;

	if(pBMP[0] != 'B' || pBMP[1] != 'M')
	{
		MFDebug_Warn(2, "Not a bitmap image.");
		return NULL;
	}

	BMPHeader *pHeader = (BMPHeader*)pBMP;
	BMPInfoHeader *pInfoHeader = (BMPInfoHeader*)&pHeader[1];

	unsigned char *pImageData = pBMP + pHeader->offset;

	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	pImage->numSurfaces = 1;
	pImage->numMips = 1;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface));

	pImage->pSurfaces[0].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*pInfoHeader->width*pInfoHeader->height);
	pImage->pSurfaces[0].width = pInfoHeader->width;
	pImage->pSurfaces[0].height = pInfoHeader->height;
	pImage->pSurfaces[0].depth = 1;

	MFIntTexturePixel *pPixel = pImage->pSurfaces[0].pData;

	bool isSavedFlipped = true;

	switch(pInfoHeader->compression)
	{
		case BMCT_RGB:
			if(pInfoHeader->bits == 24)
			{
				MFImage_Convert(pInfoHeader->width, pInfoHeader->height, pImageData, ImgFmt_R8G8B8, pPixel, ImgFmt_ABGR_F32);
			}
			else if(pInfoHeader->bits == 32)
			{
				MFImage_Convert(pInfoHeader->width, pInfoHeader->height, pImageData, ImgFmt_A8R8G8B8, pPixel, ImgFmt_ABGR_F32);
			}
			else if(pInfoHeader->bits == 8)
			{
				struct Pixel32
				{
					unsigned char b, g, r, a;
				};

				Pixel32 *pPalette = (Pixel32*)((char*)pInfoHeader + pInfoHeader->size);
				uint8 *p = (uint8*)pImageData;

				float alpha = 0.f;

				for(int y=0; y<pInfoHeader->height; y++)
				{
					for(int x=0; x<pInfoHeader->width; x++)
					{
						Pixel32 *pColour = pPalette + *p;

						pPixel->r = (float)pColour->r * (1.0f/255.0f);
						pPixel->g = (float)pColour->g * (1.0f/255.0f);
						pPixel->b = (float)pColour->b * (1.0f/255.0f);
						pPixel->a = alpha = (float)pColour->a * (1.0f/255.0f);

						++pPixel;
						++p;
					}
				}

				// if there was no alpha present in the image... set it all to white
				if(alpha == 0.f)
				{
					pPixel = pImage->pSurfaces[0].pData;

					for(int y=0; y<pInfoHeader->height; y++)
					{
						for(int x=0; x<pInfoHeader->width; x++)
						{
							pPixel->a = 1.0f;
							++pPixel;
						}
					}
				}
			}
			else
			{
				MFDebug_Warn(2, "Unsupported colour depth.");
				return NULL;
			}
			break;

		case BMCT_RLE8:
		case BMCT_RLE4:
		case BMCT_BITFIELDS:
		default:
		{
			MFDebug_Warn(2, "Compressed bitmaps not supported.");
			return NULL;
		}
	}

	if(isSavedFlipped)
	{
		MFIntTexture_FlipImage(pImage);
	}

	return pImage;
}

MFIntTexture* LoadDDS(const void *pMemory, size_t imageSize)
{
	if(imageSize < sizeof(DDS_HEADER))
		return NULL;

	DDS_IMAGE *pDDS = (DDS_IMAGE*)pMemory;
	if(pDDS->magic != MFMAKEFOURCC('D', 'D', 'S', ' ') || pDDS->header.dwSize != sizeof(DDS_HEADER))
	{
		MFDebug_Warn(2, "Not a DDS image.");
		return NULL;
	}

	bool bHasDX10Header = pDDS->header.ddspf.dwFourCC == MFMAKEFOURCC('D', 'X', '1', '0');
	char *pData = bHasDX10Header ? (char*)&pDDS->imageData10 : (char*)&pDDS->imageData;

	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	// gather image info
	uint32 width = pDDS->header.dwWidth;
	uint32 height = pDDS->header.dwHeight;

	uint32 numMips = pDDS->header.dwMipMapCount ? pDDS->header.dwMipMapCount : 1;

	uint32 numArrayElements = bHasDX10Header ? pDDS->dx10Header.arraySize : 1;
	MFDebug_Assert(numArrayElements == 1, "Array textures not (yet) supported!");

	uint32 depthLayerCount = pDDS->header.dwDepth;
	MFDebug_Assert(depthLayerCount == 0, "Depth textures not (yet) supported!");

	bool bCubeMap = !!(pDDS->header.dwCaps2 & DDSCAPS2_CUBEMAP);
	MFDebug_Assert(bCubeMap == false, "Cube maps not (yet) supported!");

	uint32 numSurfaces = (bCubeMap ? 6 : 1) * numArrayElements;

	bool bHasAlpha = !!(pDDS->header.ddspf.dwFlags & DDPF_ALPHAPIXELS);

	// DDS can express MANY formats...
	MFImageFormat ddsFormat = ImgFmt_Unknown;
	if(bHasDX10Header)
	{
		// format is explicit:
		switch(pDDS->dx10Header.dxgiFormat)
		{
			case DXGI_FORMAT_R8G8B8A8_UNORM:		ddsFormat = ImgFmt_A8B8G8R8; break;
			case DXGI_FORMAT_B8G8R8A8_UNORM:		ddsFormat = ImgFmt_A8R8G8B8; break;
			case DXGI_FORMAT_B8G8R8X8_UNORM:		ddsFormat = ImgFmt_A8R8G8B8; bHasAlpha = false; break;
			case DXGI_FORMAT_B5G6R5_UNORM:			ddsFormat = ImgFmt_R5G6B5; break;
			case DXGI_FORMAT_B5G5R5A1_UNORM:		ddsFormat = ImgFmt_A1R5G5B5; break;
			case DXGI_FORMAT_B4G4R4A4_UNORM:		ddsFormat = ImgFmt_A4R4G4B4; break;
			case DXGI_FORMAT_R10G10B10A2_UNORM:		ddsFormat = ImgFmt_A2B10G10R10; break;
			case DXGI_FORMAT_R16G16B16A16_UNORM:	ddsFormat = ImgFmt_A16B16G16R16; break;
			case DXGI_FORMAT_R16G16B16A16_FLOAT:	ddsFormat = ImgFmt_ABGR_F16; break;
			case DXGI_FORMAT_R32G32B32A32_FLOAT:	ddsFormat = ImgFmt_ABGR_F32; break;
			case DXGI_FORMAT_R11G11B10_FLOAT:		ddsFormat = ImgFmt_R11G11B10_F; break;
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:	ddsFormat = ImgFmt_R9G9B9_E5; break;
			case DXGI_FORMAT_BC1_UNORM:				ddsFormat = ImgFmt_DXT1; break;
			case DXGI_FORMAT_BC2_UNORM:				ddsFormat = ImgFmt_DXT3; break;
			case DXGI_FORMAT_BC3_UNORM:				ddsFormat = ImgFmt_DXT5; break;
			case DXGI_FORMAT_BC4_UNORM:				ddsFormat = ImgFmt_ATI1; break;
			case DXGI_FORMAT_BC4_SNORM:				ddsFormat = ImgFmt_Signed(ImgFmt_ATI1); break;
			case DXGI_FORMAT_BC5_UNORM:				ddsFormat = ImgFmt_ATI2; break;
			case DXGI_FORMAT_BC5_SNORM:				ddsFormat = ImgFmt_Signed(ImgFmt_ATI2); break;
			case DXGI_FORMAT_BC6H_UF16:				ddsFormat = ImgFmt_BPTC_F; break;
			case DXGI_FORMAT_BC6H_SF16:				ddsFormat = ImgFmt_Signed(ImgFmt_BPTC_F); break;
			case DXGI_FORMAT_BC7_UNORM:				ddsFormat = ImgFmt_BPTC; break;
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:	ddsFormat = ImgFmt_D32FS8X24; break;
			case DXGI_FORMAT_D32_FLOAT:				ddsFormat = ImgFmt_D32F; break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:		ddsFormat = ImgFmt_D24S8; break;
			case DXGI_FORMAT_D16_UNORM:				ddsFormat = ImgFmt_D16; break;
			case DXGI_FORMAT_R16G16B16A16_UINT:		ddsFormat = ImgFmt_Integer(ImgFmt_A16B16G16R16); break;
			case DXGI_FORMAT_R16G16B16A16_SNORM:	ddsFormat = ImgFmt_Signed(ImgFmt_A16B16G16R16); break;
			case DXGI_FORMAT_R16G16B16A16_SINT:		ddsFormat = ImgFmt_SignedInteger(ImgFmt_A16B16G16R16); break;
			case DXGI_FORMAT_R32G32_FLOAT:			ddsFormat = ImgFmt_GR_F32; break;
			case DXGI_FORMAT_R10G10B10A2_UINT:		ddsFormat = ImgFmt_Integer(ImgFmt_A2B10G10R10); break;
			case DXGI_FORMAT_R8G8B8A8_UINT:			ddsFormat = ImgFmt_Integer(ImgFmt_A8B8G8R8); break;
			case DXGI_FORMAT_R8G8B8A8_SNORM:		ddsFormat = ImgFmt_Signed(ImgFmt_A8B8G8R8); break;
			case DXGI_FORMAT_R8G8B8A8_SINT:			ddsFormat = ImgFmt_SignedInteger(ImgFmt_A8B8G8R8); break;
			case DXGI_FORMAT_R16G16_FLOAT:			ddsFormat = ImgFmt_GR_F16; break;
			case DXGI_FORMAT_R16G16_UNORM:			ddsFormat = ImgFmt_G16R16; break;
			case DXGI_FORMAT_R16G16_UINT:			ddsFormat = ImgFmt_Integer(ImgFmt_G16R16); break;
			case DXGI_FORMAT_R16G16_SNORM:			ddsFormat = ImgFmt_Signed(ImgFmt_G16R16); break;
			case DXGI_FORMAT_R16G16_SINT:			ddsFormat = ImgFmt_SignedInteger(ImgFmt_G16R16); break;
			case DXGI_FORMAT_R32_FLOAT:				ddsFormat = ImgFmt_R_F32; break;
			case DXGI_FORMAT_R8G8_UNORM:			ddsFormat = ImgFmt_G8R8; break;
			case DXGI_FORMAT_R8G8_UINT:				ddsFormat = ImgFmt_Integer(ImgFmt_G8R8); break;
			case DXGI_FORMAT_R8G8_SNORM:			ddsFormat = ImgFmt_Signed(ImgFmt_G8R8); break;
			case DXGI_FORMAT_R8G8_SINT:				ddsFormat = ImgFmt_SignedInteger(ImgFmt_G8R8); break;
			case DXGI_FORMAT_R16_FLOAT:				ddsFormat = ImgFmt_R_F16; break;
			case DXGI_FORMAT_R16_UNORM:				ddsFormat = ImgFmt_L16; break;
			case DXGI_FORMAT_R16_UINT:				ddsFormat = ImgFmt_Integer(ImgFmt_L16); break;
			case DXGI_FORMAT_R16_SNORM:				ddsFormat = ImgFmt_Signed(ImgFmt_L16); break;
			case DXGI_FORMAT_R16_SINT:				ddsFormat = ImgFmt_SignedInteger(ImgFmt_L16); break;
			case DXGI_FORMAT_R8_UNORM:				ddsFormat = ImgFmt_L8; break;
			case DXGI_FORMAT_R8_UINT:				ddsFormat = ImgFmt_Integer(ImgFmt_L8); break;
			case DXGI_FORMAT_R8_SNORM:				ddsFormat = ImgFmt_Signed(ImgFmt_L8); break;
			case DXGI_FORMAT_R8_SINT:				ddsFormat = ImgFmt_SignedInteger(ImgFmt_L8); break;
			case DXGI_FORMAT_A8_UNORM:				ddsFormat = ImgFmt_A8; break;

			case DXGI_FORMAT_R32G32B32A32_UINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_SINT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R1_UNORM:

			case DXGI_FORMAT_R8G8_B8G8_UNORM:
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			case DXGI_FORMAT_AYUV:
			case DXGI_FORMAT_Y410:
			case DXGI_FORMAT_Y416:
			case DXGI_FORMAT_NV12:
			case DXGI_FORMAT_P010:
			case DXGI_FORMAT_P016:
			case DXGI_FORMAT_420_OPAQUE:
			case DXGI_FORMAT_YUY2:
			case DXGI_FORMAT_Y210:
			case DXGI_FORMAT_Y216:
			case DXGI_FORMAT_NV11:
			case DXGI_FORMAT_AI44:
			case DXGI_FORMAT_IA44:
			case DXGI_FORMAT_P8:
			case DXGI_FORMAT_A8P8:

			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
			default:
				break;
		}
	}
	else if(pDDS->header.ddspf.dwFlags & DDPF_FOURCC)
	{
		// compressed image
		switch(pDDS->header.ddspf.dwFourCC)
		{
			case MFMAKEFOURCC('D', 'X', 'T', '1'):	ddsFormat = ImgFmt_DXT1; break;
			case MFMAKEFOURCC('D', 'X', 'T', '2'):	ddsFormat = ImgFmt_DXT2; break;
			case MFMAKEFOURCC('D', 'X', 'T', '3'):	ddsFormat = ImgFmt_DXT3; break;
			case MFMAKEFOURCC('D', 'X', 'T', '4'):	ddsFormat = ImgFmt_DXT4; break;
			case MFMAKEFOURCC('D', 'X', 'T', '5'):	ddsFormat = ImgFmt_DXT5; break;
			case MFMAKEFOURCC('A', 'T', 'I', '1'):
			case MFMAKEFOURCC('B', 'C', '4', 'U'):	ddsFormat = ImgFmt_ATI1; break;
			case MFMAKEFOURCC('B', 'C', '4', 'S'):	ddsFormat = ImgFmt_Signed(ImgFmt_ATI1); break;
			case MFMAKEFOURCC('A', 'T', 'I', '2'):
			case MFMAKEFOURCC('B', 'C', '5', 'U'):	ddsFormat = ImgFmt_ATI2; break;
			case MFMAKEFOURCC('B', 'C', '5', 'S'):	ddsFormat = ImgFmt_Signed(ImgFmt_ATI2); break;
			case MFMAKEFOURCC('E', 'T', 'C', '1'):	ddsFormat = ImgFmt_ETC1; break;
			case MFMAKEFOURCC('A', 'T', 'C', '1'):	ddsFormat = ImgFmt_ATCRGB; break;
			case MFMAKEFOURCC('A', 'T', 'C', '3'):	ddsFormat = ImgFmt_ATCRGBA_EXPLICIT; break;
			case MFMAKEFOURCC('A', 'T', 'C', '5'):	ddsFormat = ImgFmt_ATCRGBA; break;
			case 36:								ddsFormat = ImgFmt_A16B16G16R16; break;
			case 110:								ddsFormat = ImgFmt_Signed(ImgFmt_A16B16G16R16); break;
			case 113:								ddsFormat = ImgFmt_ABGR_F16; break;
			case 112:								ddsFormat = ImgFmt_GR_F16; break;
			case 111:								ddsFormat = ImgFmt_R_F16; break;
			case 116:								ddsFormat = ImgFmt_ABGR_F32; break;
			case 115:								ddsFormat = ImgFmt_GR_F32; break;
			case 114:								ddsFormat = ImgFmt_R_F32; break;

			// Unsupported...
			case MFMAKEFOURCC('R', 'G', 'B', 'G'):	// D3DFMT_R8G8_B8G8
			case MFMAKEFOURCC('G', 'R', 'G', 'B'):	// D3DFMT_G8R8_G8B8
			case MFMAKEFOURCC('U', 'Y', 'V', 'Y'):	// D3DFMT_UYVY
			case MFMAKEFOURCC('Y', 'U', 'Y', '2'):	// D3DFMT_YUY2
			case 117:								// D3DFMT_CxV8U8
				break;
		}
	}
	else if((pDDS->header.ddspf.dwFlags & DDPF_ALPHA) || (pDDS->header.ddspf.dwFlags & DDPF_LUMINANCE))
	{
		// 8 bit alpha/luminance data
		if((pDDS->header.ddspf.dwFlags & DDPF_ALPHA) && (pDDS->header.ddspf.dwFlags & DDPF_LUMINANCE))
			ddsFormat = ImgFmt_A8L8;
		else if(pDDS->header.ddspf.dwFlags & DDPF_ALPHA)
			ddsFormat = ImgFmt_A8;
		else
			ddsFormat = ImgFmt_L8;
	}
	else if(pDDS->header.ddspf.dwFlags & DDPF_RGB)
	{
		if(pDDS->header.ddspf.dwRGBBitCount == 32)
		{
			if(pDDS->header.ddspf.dwRBitMask == 0xFF0000 && pDDS->header.ddspf.dwGBitMask == 0xFF00 && pDDS->header.ddspf.dwBBitMask == 0xFF)
				ddsFormat = ImgFmt_A8R8G8B8;
			else if(pDDS->header.ddspf.dwRBitMask == 0xFF && pDDS->header.ddspf.dwGBitMask == 0xFF00 && pDDS->header.ddspf.dwBBitMask == 0xFF0000)
				ddsFormat = ImgFmt_A8B8G8R8;
			else if(pDDS->header.ddspf.dwRBitMask == 0xFF00 && pDDS->header.ddspf.dwGBitMask == 0xFF0000 && pDDS->header.ddspf.dwBBitMask == 0xFF000000)
				ddsFormat = ImgFmt_B8G8R8A8;
			else if(pDDS->header.ddspf.dwRBitMask == 0xFF000000 && pDDS->header.ddspf.dwGBitMask == 0xFF0000 && pDDS->header.ddspf.dwBBitMask == 0xFF00)
				ddsFormat = ImgFmt_R8G8B8A8;
			else if(pDDS->header.ddspf.dwRBitMask == 0x3FF00000 && pDDS->header.ddspf.dwGBitMask == 0xFFC00 && pDDS->header.ddspf.dwBBitMask == 0x3FF)
				ddsFormat = ImgFmt_A2R10G10B10;
			else if(pDDS->header.ddspf.dwRBitMask == 0x3FF && pDDS->header.ddspf.dwGBitMask == 0xFFC00 && pDDS->header.ddspf.dwBBitMask == 0x3FF00000)
				ddsFormat = ImgFmt_A2B10G10R10;
		}
		if(pDDS->header.ddspf.dwRGBBitCount == 24)
		{
			if(pDDS->header.ddspf.dwRBitMask == 0xFF0000 && pDDS->header.ddspf.dwGBitMask == 0xFF00 && pDDS->header.ddspf.dwBBitMask == 0xFF)
				ddsFormat = ImgFmt_R8G8B8;
			else if(pDDS->header.ddspf.dwRBitMask == 0xFF && pDDS->header.ddspf.dwGBitMask == 0xFF00 && pDDS->header.ddspf.dwBBitMask == 0xFF0000)
				ddsFormat = ImgFmt_B8G8R8;
		}
		else if(pDDS->header.ddspf.dwRGBBitCount == 16)
		{
			if(pDDS->header.ddspf.dwRBitMask == 0xf800 && pDDS->header.ddspf.dwGBitMask == 0x7e0 && pDDS->header.ddspf.dwBBitMask == 0x1f)
				ddsFormat = ImgFmt_R5G6B5;
			else if(pDDS->header.ddspf.dwRBitMask == 0xfc00 && pDDS->header.ddspf.dwGBitMask == 0x3e0 && pDDS->header.ddspf.dwBBitMask == 0x1f)
				ddsFormat = ImgFmt_R6G5B5;
			else if(pDDS->header.ddspf.dwRBitMask == 0x1f && pDDS->header.ddspf.dwGBitMask == 0x7e0 && pDDS->header.ddspf.dwBBitMask == 0xf800)
				ddsFormat = ImgFmt_B5G6R5;
			else if(pDDS->header.ddspf.dwRBitMask == 0x7c00 && pDDS->header.ddspf.dwGBitMask == 0x3e0 && pDDS->header.ddspf.dwBBitMask == 0x1f)
				ddsFormat = ImgFmt_A1R5G5B5;
			else if(pDDS->header.ddspf.dwRBitMask == 0xf800 && pDDS->header.ddspf.dwGBitMask == 0x7c0 && pDDS->header.ddspf.dwBBitMask == 0x3e)
				ddsFormat = ImgFmt_R5G5B5A1;
			else if(pDDS->header.ddspf.dwRBitMask == 0x1f && pDDS->header.ddspf.dwGBitMask == 0x3e0 && pDDS->header.ddspf.dwBBitMask == 0x7c00)
				ddsFormat = ImgFmt_A1B5G5R5;
			else if(pDDS->header.ddspf.dwRBitMask == 0xf00 && pDDS->header.ddspf.dwGBitMask == 0xf0 && pDDS->header.ddspf.dwBBitMask == 0xf)
				ddsFormat = ImgFmt_A4R4G4B4;
			else if(pDDS->header.ddspf.dwRBitMask == 0xf && pDDS->header.ddspf.dwGBitMask == 0xf0 && pDDS->header.ddspf.dwBBitMask == 0xf00)
				ddsFormat = ImgFmt_A4B4G4R4;
			else if(pDDS->header.ddspf.dwRBitMask == 0xf000 && pDDS->header.ddspf.dwGBitMask == 0xf00 && pDDS->header.ddspf.dwBBitMask == 0xf0)
				ddsFormat = ImgFmt_R4G4B4A4;
		}
	}

	MFDebug_Assert(ddsFormat != ImgFmt_Unknown, "Couldn't detect image format...");

	// load the image...
	pImage->numSurfaces = (int)numSurfaces;
	pImage->numMips = (int)numMips;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface) * numSurfaces*numMips);

	for(uint32 i=0; i<numSurfaces; ++i)
	{
		for(uint32 mip=0; mip<numMips; ++mip)
		{
			if(mip > 0)
				continue; // HACK: we want to load the mips!

			uint32 mipWidth = width >> mip;
			uint32 mipHeight = height >> mip;
			mipWidth = mipWidth ? mipWidth : 1;
			mipHeight = mipHeight ? mipHeight : 1;

			// the number of depth layers mip too...
			uint32 depthLayers = depthLayerCount >> mip;
			depthLayers = depthLayers ? depthLayers : 1;

			int s = i*numMips+mip;
			pImage->pSurfaces[s].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*mipWidth*mipHeight*depthLayers);
			pImage->pSurfaces[s].width = mipWidth;
			pImage->pSurfaces[s].height = mipHeight;
			pImage->pSurfaces[s].depth = depthLayers;

			MFIntTexturePixel *pPixel = pImage->pSurfaces[s].pData;

			for(uint32 depth=0; depth<depthLayers; ++depth)
			{
				// load image data...
				if(ddsFormat >= ImgFmt_DXT1 && ddsFormat <= ImgFmt_PSP_DXT5)
				{
					// load compressed image
					MFDebug_Assert(false, "Not done...");

					// pitch is the size of the entire compressed image
					pData += pDDS->header.dwPitchOrLinearSize;
				}
				else
				{
					// load raw image by scanline
					for(uint32 y=0; y<height; ++y)
					{
						MFImage_Convert(width, 1, pData, ddsFormat, pPixel, ImgFmt_ABGR_F32);

						if(pDDS->header.ddspf.dwRGBBitCount != 24 && !bHasAlpha)
						{
							// set alpha to 1
							for(uint32 x=0; x<width; ++x)
								pPixel[x].a = 1.f;
						}

						pData += pDDS->header.dwPitchOrLinearSize;
						pPixel += width;
					}
				}
			}
		}
	}

	return pImage;
}

#if defined(SUPPORTS_JPEG)
MFIntTexture* LoadJPEG(const void *pMemory, size_t imageSize)
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	// We set up the normal JPEG error routines, then override error_exit.
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (unsigned char *)pMemory, (unsigned long)imageSize);

	jpeg_read_header(&cinfo, TRUE);

	// Step 4: set parameters for decompression...
	// In this example, we don't need to change any of the defaults set by jpeg_read_header(), so we do nothing here.

	jpeg_start_decompress(&cinfo);

	// allocate internal image structures
	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	pImage->numSurfaces = 1;
	pImage->numMips = 1;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface));

	pImage->pSurfaces[0].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*cinfo.image_width*cinfo.image_height);
	pImage->pSurfaces[0].width = cinfo.image_width;
	pImage->pSurfaces[0].height = cinfo.image_height;
	pImage->pSurfaces[0].depth = 1;

	// do this thing one row at a time; use a lot less memory!
	int row_stride = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	MFIntTexturePixel *pPixel = pImage->pSurfaces[0].pData;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);

		MFImage_Convert(cinfo.image_width, 1, buffer[0], ImgFmt_B8G8R8, pPixel, ImgFmt_ABGR_F32);
		pPixel += cinfo.image_width;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(jerr.num_warnings != 0)
	{
		// warnings occurred?
	}

	return pImage;
}
#endif

//-------------------------------------------------------------------

bool IsPowerOf2(int x)
{
	while(x)
	{
		if(x&1)
		{
			x>>=1;
			if(x)
				return false;
		}
		else
			x>>=1;
	}

	return true;
}

void Swizzle_PSP(char* out, const char* in, uint32 width, uint32 height, MFImageFormat format)
{
	uint32 blockx, blocky;
	uint32 j;

	// calculate width in bytes
	width = (width * MFImage_GetBitsPerPixel(format)) / 8;

	uint32 width_blocks = (width / 16);
	uint32 height_blocks = (height / 8);

	uint32 src_pitch = (width-16)/4;
	uint32 src_row = width * 8;

	const char* ysrc = in;
	uint32* dst = (uint32*)out;

	for(blocky = 0; blocky < height_blocks; ++blocky)
	{
		const char* xsrc = ysrc;
		for(blockx = 0; blockx < width_blocks; ++blockx)
		{
			const uint32* src = (uint32*)xsrc;
			for(j = 0; j < 8; ++j)
			{
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				src += src_pitch;
			}
			xsrc += 16;
		}
		ysrc += src_row;
	}
}

int ConvertSurface(MFIntTextureSurface *pSourceSurface, MFTextureSurfaceLevel *pOutputSurface, MFImageFormat targetFormat, MFPlatform platform)
{
	MFDebug_Assert(!(targetFormat & ImgFmt_Swizzle), "TODO: Fix up swizzled format support!");

	// convert image...
	int width = pSourceSurface->width;
	int height = pSourceSurface->height;
//	int depth = pSourceSurface->depth;

	MFImage_Convert(width, height, pSourceSurface->pData, ImgFmt_ABGR_F32, pOutputSurface->pImageData, targetFormat);

	// test for swizzled format..
	if(targetFormat & ImgFmt_Swizzle)
	{
		uint32 imageBytes = (width * height * MFImage_GetBitsPerPixel(targetFormat)) / 8;

		char *pBuffer = (char*)MFHeap_Alloc(imageBytes);

#if 0
		uint32 bytesperpixel = MFImage_GetBitsPerPixel(targetFormat) / 8;

		if(platform == FP_XBox)
		{
			// swizzle for xbox
			// TODO: Swizzle here.. But we'll swizzle at runtime for the time being....
//			XGSwizzleRect(pOutputSurface->pImageData, 0, NULL, pBuffer, width, height, NULL, bytesperpixel);
			MFCopyMemory(pBuffer, pOutputSurface->pImageData, width*height*bytesperpixel);
		}
		else
#endif
		if(platform == FP_PSP)
		{
			// swizzle for PSP
			Swizzle_PSP(pBuffer, pOutputSurface->pImageData, width, height, targetFormat);
		}

		MFCopyMemory(pOutputSurface->pImageData, pBuffer, imageBytes);
		MFHeap_Free(pBuffer);
	}

	return 0;
}

void PremultiplyAlpha(MFIntTexture *pImage)
{
	MFIntTexturePixel *pPx = pImage->pSurfaces[0].pData;

	for(int s=0; s<pImage->numSurfaces; ++s)
	{
		for(int a=0; a<pImage->pSurfaces[s].width; a++)
		{
			for(int a=0; a<pImage->pSurfaces[s].height; a++)
			{
				pPx->r *= pPx->a;
				pPx->g *= pPx->a;
				pPx->b *= pPx->a;
				++pPx;
			}
		}
	}
}

MF_API MFIntTexture *MFIntTexture_CreateFromFile(const char *pFilename)
{
	// find format
	const char *pExt = MFString_GetFileExtension(pFilename);

	MFIntTextureFormat format = MFITF_Max;
	for(int a=0; a<gNumFileExtensions; ++a)
	{
		if(!MFString_Compare(pExt, gFileExtensions[a]))
		{
			format = gFileTypeMap[a];
			break;
		}
	}
	if(format == MFITF_Max)
		return NULL;

	// load file
	size_t size;
	char *pData = MFFileSystem_Load(pFilename, &size);
	if(!pData)
		return NULL;

	// load the image
	MFIntTexture *pImage = MFIntTexture_CreateFromFileInMemory(pData, size, format);

	// free file
	MFHeap_Free(pData);

	return pImage;
}

MF_API MFIntTexture *MFIntTexture_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntTextureFormat format)
{
	MFIntTexture *pImage = NULL;

	switch(format)
	{
		case MFITF_TGA:
			pImage = LoadTGA(pMemory, size);
			break;
		case MFITF_BMP:
			pImage = LoadBMP(pMemory, size);
			break;
		case MFITF_DDS:
			pImage = LoadDDS(pMemory, size);
			break;
		case MFITF_PNG:
#if defined(MF_ENABLE_PNG)
			pImage = LoadPNG(pMemory, size);
#else
			MFDebug_Assert(false, "PNG support is not enabled in this build.");
#endif
			break;
		case MFITF_JPEG:
#if defined(SUPPORTS_JPEG)
			pImage = LoadJPEG(pMemory, size);
#else
			MFDebug_Assert(false, "JPEG support is not enabled in this build.");
#endif
			break;
		default:
			MFDebug_Assert(false, "Unsupported image format.");
	}

	if(pImage)
	{
		// scan for alpha information
		MFIntTexture_ScanImage(pImage);

		// build the mip chain
		if(pImage->numMips == 1)
			MFIntTexture_FilterMipMaps(pImage, 0, 0);
	}

	return pImage;
}

MF_API void MFIntTexture_Destroy(MFIntTexture *pTexture)
{
	for(int a=0; a<pTexture->numSurfaces; a++)
	{
		if(pTexture->pSurfaces[a].pData)
			MFHeap_Free(pTexture->pSurfaces[a].pData);
	}

	if(pTexture->pSurfaces)
		MFHeap_Free(pTexture->pSurfaces);

	if(pTexture)
		MFHeap_Free(pTexture);
}

MF_API MFImageFormat ChooseBestFormat(MFIntTexture *pTexture, MFPlatform platform)
{
	MFImageFormat targetFormat = ImgFmt_A8B8G8R8;

	// choose target format..
	switch(platform)
	{
		case FP_Windows:
		case FP_Linux:
		case FP_OSX:
/*
			if(pImage->opaque || (pImage->oneBitAlpha && premultipliedAlpha))
				targetFormat = ImgFmt_DXT1;
			else
				targetFormat = ImgFmt_DXT5;
*/
			if(pTexture->opaque)
				targetFormat = ImgFmt_A8R8G8B8; //ImgFmt_R5G6B5;
			else if(pTexture->oneBitAlpha)
				targetFormat = ImgFmt_A8R8G8B8; //ImgFmt_A1R5G5B5;
			else
				targetFormat = ImgFmt_A8R8G8B8;
			break;

		case FP_IPhone:
			if(pTexture->opaque)
				targetFormat = ImgFmt_R5G6B5;
			else if(pTexture->oneBitAlpha)
				targetFormat = ImgFmt_R5G5B5A1;
			else
				targetFormat = ImgFmt_R4G4B4A4;
			break;

		case FP_XBox:
			if(pTexture->opaque)
				targetFormat = ImgFmt_Swizzle(ImgFmt_R5G6B5);
			else if(pTexture->oneBitAlpha)
				targetFormat = ImgFmt_Swizzle(ImgFmt_A1R5G5B5);
			else
				targetFormat = ImgFmt_Swizzle(ImgFmt_A8R8G8B8);
			break;

		case FP_PSP:
			if(pTexture->opaque)
				targetFormat = ImgFmt_Swizzle(ImgFmt_B5G6R5);
			else if(pTexture->oneBitAlpha)
				targetFormat = ImgFmt_Swizzle(ImgFmt_A1B5G5R5);
			else
				targetFormat = ImgFmt_Swizzle(ImgFmt_A4B4G4R4);
			break;

		case FP_PS2:
			if(pTexture->opaque || pTexture->oneBitAlpha)
				targetFormat = ImgFmt_A1B5G5R5;
			else
				targetFormat = ImgFmt_A8B8G8R8;
			break;

		default:
			break;
	}

	return targetFormat;
}

MF_API void MFIntTexture_CreateRuntimeData(MFIntTexture *pTexture, MFTextureTemplateData **ppTemplateData, size_t *pSize, MFPlatform platform, uint32 flags, MFImageFormat targetFormat)
{
	*ppTemplateData = NULL;
	if(pSize)
		*pSize = 0;

	MFDebug_Assert(pTexture->numSurfaces == 1, "Multi-surface textures not (yet) supported!");

	// choose target image format
	if(targetFormat == ImgFmt_Unknown)
		targetFormat = ChooseBestFormat(pTexture, platform);

	// check minimum pitch
	MFDebug_Assert((pTexture->pSurfaces[0].width*MFImage_GetBitsPerPixel(targetFormat)) / 8 >= 16, "Textures should have a minimum pitch of 16 bytes.");

	// check power of 2 dimensions
//	MFDebug_Assert(IsPowerOf2(pTexture->pSurfaces[0].width) && IsPowerOf2(pTexture->pSurfaces[0].height), "Texture dimensions are not a power of 2.");

	// check dimensions are a multiple of 4
//	MFDebug_Assert((pTexture->pSurfaces[0].width & 0x3) == 0 && (pTexture->pSurfaces[0].height & 3) == 0, "Texture dimensions are not multiples of 4.");

	// begin processing...
	if(flags & MFITF_PreMultipliedAlpha)
		PremultiplyAlpha(pTexture);

	// calculate texture data size..
	size_t headerBytes = MFALIGN(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pTexture->numSurfaces*pTexture->numMips, 0x100);
	size_t imageBytes = headerBytes;

	for(int a=0; a<pTexture->numMips; a++)
	{
		MFDebug_Assert(pTexture->pSurfaces[a].depth == 1, "Volume textures not (yet) supported!");

		imageBytes += (pTexture->pSurfaces[a].width * pTexture->pSurfaces[a].height * MFImage_GetBitsPerPixel(targetFormat)) / 8;

		// add palette
		uint32 paletteBytes = 0;

		if(targetFormat == ImgFmt_P8)
			paletteBytes = 4*256;
		if(targetFormat == ImgFmt_P4)
			paletteBytes = 4*16;

		imageBytes += paletteBytes;
	}

	// allocate buffer
//	MFHeap_SetAllocAlignment(4096);
	char *pOutputBuffer = (char*)MFHeap_Alloc(imageBytes);

	MFTextureTemplateData *pTemplate = (MFTextureTemplateData*)pOutputBuffer;
	MFZeroMemory(pTemplate, sizeof(MFTextureTemplateData));

	pTemplate->magicNumber = MFMAKEFOURCC('F','T','E','X');

	pTemplate->imageFormat = targetFormat;

	if(targetFormat & ImgFmt_Swizzle)
		pTemplate->flags |= TEX_Swizzled;

	if(!pTexture->opaque)
	{
		if(pTexture->oneBitAlpha)
			pTemplate->flags |= 3;
		else
			pTemplate->flags |= 1;
	}

	if(flags & MFITF_PreMultipliedAlpha)
		pTemplate->flags |= TEX_PreMultipliedAlpha;

	pTemplate->mipLevels = pTexture->numMips;
	pTemplate->pSurfaces = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	MFTextureSurfaceLevel *pSurfaceLevels = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	char *pDataPointer = pOutputBuffer + headerBytes;

	for(int a=0; a<pTexture->numMips; a++)
	{
		MFZeroMemory(&pSurfaceLevels[a], sizeof(MFTextureSurfaceLevel));

		pSurfaceLevels[a].width = pTexture->pSurfaces[a].width;
		pSurfaceLevels[a].height = pTexture->pSurfaces[a].height;
		pSurfaceLevels[a].bitsPerPixel = MFImage_GetBitsPerPixel(targetFormat);

		pSurfaceLevels[a].xBlocks = -1;
		pSurfaceLevels[a].yBlocks = -1;
		pSurfaceLevels[a].bitsPerBlock = -1;

		pSurfaceLevels[a].pImageData = pDataPointer;
		pSurfaceLevels[a].bufferLength = (pTexture->pSurfaces[a].width*pTexture->pSurfaces[a].height * MFImage_GetBitsPerPixel(targetFormat)) / 8;
		pDataPointer += pSurfaceLevels[a].bufferLength;

		uint32 paletteBytes = 0;

		if(targetFormat == ImgFmt_P8)
			paletteBytes = 4*256;
		if(targetFormat == ImgFmt_P4)
			paletteBytes = 4*16;

		if(paletteBytes)
		{
			pSurfaceLevels[a].pImageData = pDataPointer;
			pSurfaceLevels[a].paletteBufferLength = paletteBytes;
			pDataPointer += paletteBytes;
		}

		// convert surface
		ConvertSurface(&pTexture->pSurfaces[a], &pSurfaceLevels[a], targetFormat, platform);
	}

	// fix up pointers
	for(int a=0; a<pTemplate->mipLevels; a++)
	{
		MFFixUp(pTemplate->pSurfaces[a].pImageData, pOutputBuffer, 0);
		MFFixUp(pTemplate->pSurfaces[a].pPaletteEntries, pOutputBuffer, 0);
	}
	MFFixUp(pTemplate->pSurfaces, pOutputBuffer, 0);

	*ppTemplateData = (MFTextureTemplateData*)pOutputBuffer;
	if(pSize)
		*pSize = imageBytes;
}

MF_API void MFIntTexture_WriteToHeaderFile(MFIntTexture *pTexture, const char *pFilename)
{

}

MF_API void MFIntTexture_FilterMipMaps(MFIntTexture *pTexture, int numMipLevels, uint32 mipFilterOptions)
{
	struct MipLevels
	{
		int width, height, depth;
	} levels[128];
	int numLevels = 1;

	levels[0].width = pTexture->pSurfaces[0].width;
	levels[0].height = pTexture->pSurfaces[0].height;
	levels[0].depth = pTexture->pSurfaces[0].depth;

	while(levels[numLevels-1].width > 1 || levels[numLevels-1].height > 1 || levels[numLevels-1].depth > 1)
	{
		levels[numLevels].width = levels[numLevels-1].width;
		levels[numLevels].height = levels[numLevels-1].height;
		levels[numLevels].depth = levels[numLevels-1].depth;
		if(levels[numLevels].width > 1)
			levels[numLevels].width >>= 1;
		if(levels[numLevels].height > 1)
			levels[numLevels].height >>= 1;
		if(levels[numLevels].depth > 1)
			levels[numLevels].depth >>= 1;
		++numLevels;
	}

	if(numMipLevels > 0)
		numLevels = MFMin(numLevels, numMipLevels);

	if(numLevels == 1)
		return;

	pTexture->numMips = numLevels;
	pTexture->pSurfaces = (MFIntTextureSurface*)MFHeap_Realloc(pTexture->pSurfaces, sizeof(MFIntTextureSurface)*pTexture->numSurfaces*numLevels);

	for(int s = pTexture->numSurfaces-1; s >= 0; --s)
	{
		pTexture->pSurfaces[s*numLevels] = pTexture->pSurfaces[s];

		for(int l = 1; l < numLevels; ++l)
		{
			int m = s*numLevels+l;

			pTexture->pSurfaces[m].width = levels[l].width;
			pTexture->pSurfaces[m].height = levels[l].height;
			pTexture->pSurfaces[m].depth = levels[l].depth;
			pTexture->pSurfaces[m].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*levels[l].width*levels[l].height*levels[l].depth);

			MFDebug_Assert(pTexture->pSurfaces[l-1].depth == 1, "Can't generate volume texture mips (yet)!");

			MFScaleImage scaleData;
			scaleData.algorithm = SA_Box;
			scaleData.format = ImgFmt_ABGR_F32;
			scaleData.pSourceImage = pTexture->pSurfaces[l-1].pData;
			scaleData.sourceWidth = pTexture->pSurfaces[l-1].width;
			scaleData.sourceHeight = pTexture->pSurfaces[l-1].height;
			scaleData.sourceStride = pTexture->pSurfaces[l-1].width;
			scaleData.pTargetBuffer = pTexture->pSurfaces[l].pData;
			scaleData.targetWidth = pTexture->pSurfaces[l].width;
			scaleData.targetHeight = pTexture->pSurfaces[l].height;
			scaleData.targetStride = pTexture->pSurfaces[l].width;

			MFImage_Scale(&scaleData);
		}
	}
}

MF_API void MFIntTexture_FilterMipMap(MFIntTexturePixel *pSource, MFIntTexturePixel *pDest, int destWidth, int destHeight, uint32 mipFilterOptions)
{
	// TODO: this naive filter doesn't work properly with non-power-of-2 textures!
	// the right and lower edges of the image will be trimmed

	int sourceWidth = destWidth*2;
	for(int y = 0, sy = 0; y < destHeight; ++y, sy += 2)
	{
		for(int x = 0, sx = 0; x < destWidth; ++x, sx += 2)
		{
			int sourceOffset = sx + sy*sourceWidth;
			MFIntTexturePixel &tl = pSource[sourceOffset];
			MFIntTexturePixel &tr = pSource[sourceOffset + 1];
			MFIntTexturePixel &bl = pSource[sourceOffset + sourceWidth];
			MFIntTexturePixel &br = pSource[sourceOffset + 1 + sourceWidth];
			MFIntTexturePixel &result = pDest[x + y*destWidth];

			result.r = (tl.r + tr.r + bl.r + br.r) * 0.25f;
			result.g = (tl.g + tr.g + bl.g + br.g) * 0.25f;
			result.b = (tl.b + tr.b + bl.b + br.b) * 0.25f;
			result.a = (tl.a + tr.a + bl.a + br.a) * 0.25f;
		}
	}
}

MF_API void MFIntTexture_ScanImage(MFIntTexture *pTexture)
{
	pTexture->opaque = true;
	pTexture->oneBitAlpha = true;

	for(int a=0; a<pTexture->numSurfaces; a++)
	{
		for(int b=0; b<pTexture->pSurfaces[a].width; b++)
		{
			for(int c=0; c<pTexture->pSurfaces[a].height; c++)
			{
				float alpha = pTexture->pSurfaces[a].pData[c*pTexture->pSurfaces[a].width + b].a;

				if(alpha != 1.0f)
				{
					pTexture->opaque = false;

					if(alpha != 0.0f)
						pTexture->oneBitAlpha = false;
				}
			}
		}
	}
}

MF_API void MFIntTexture_FlipImage(MFIntTexture *pTexture)
{
	for(int a=0; a<pTexture->numSurfaces; a++)
	{
		int halfHeight = pTexture->pSurfaces[a].height / 2;
		int stride = pTexture->pSurfaces[a].width * sizeof(MFIntTexturePixel);
		int height = pTexture->pSurfaces[a].height;

		char *pBuffer = (char*)MFHeap_Alloc(stride);
		char *pData = (char*)pTexture->pSurfaces[a].pData;

		for(int b=0; b<halfHeight; b++)
		{
			// swap lines
			MFCopyMemory(pBuffer, &pData[b*stride], stride);
			MFCopyMemory(&pData[b*stride], &pData[(height-b-1)*stride], stride);
			MFCopyMemory(&pData[(height-b-1)*stride], pBuffer, stride);
		}

		MFHeap_Free(pBuffer);
	}
}
