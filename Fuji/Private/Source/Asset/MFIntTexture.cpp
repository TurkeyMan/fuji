#include "Fuji.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFString.h"
#include "Asset/MFIntTexture.h"

#define MF_ENABLE_PNG
#if defined(MF_WINDOWS)
//	#define MF_ENABLE_ATI_COMPRESSOR
#endif
#if defined(MF_WINDOWS) || defined(MF_XBOX)
	#define MF_ENABLE_MS_COMPRESSOR
#endif

#if defined(MF_ENABLE_ATI_COMPRESSOR)
	#include <windows.h>
	#include "ATI_Compress/ATI_Compress.h"
	#pragma comment(lib, "ATI_Compress/ATI_Compress.lib")
#endif

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

enum BMPCompressionType
{
	RGB = 0,			/* No compression - straight BGR data */
	RLE8 = 1,		/* 8-bit run-length compression */
	RLE4 = 2,		/* 4-bit run-length compression */
	BITFIELDS = 3	/* RGB bitmap with RGB masks */
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

struct BMPPaletteEntry /**** Colormap entry structure ****/
{
	unsigned char  rgbBlue;          /* Blue value */
	unsigned char  rgbGreen;         /* Green value */
	unsigned char  rgbRed;           /* Red value */
	unsigned char  rgbReserved;      /* Reserved */
};


/**** Globals ****/

static char *gFileExtensions[] =
{
	".tga",
	".bmp",
	".png"
};


/**** Functions ****/

#if defined(MF_ENABLE_PNG)
#include "png.h"

void PNGAPI png_file_read(png_structp png, png_bytep pBuffer, png_size_t bytes)
{
	MFFile_Read((MFFile*)png->io_ptr, pBuffer, bytes, false);
}

MFIntTexture* LoadPNG(const void *pMemory, uint32 size)
{
	MFFile *pFile = MFFile_CreateMemoryFile(pMemory, size);

	if(!pFile)
		return NULL;

	if(png_sig_cmp((uint8*)pMemory, 0, 8))
	{
		printf("Not a PNG file..");
		return NULL;
	}

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
	setjmp(png_jmpbuf(png_ptr));

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
	setjmp(png_jmpbuf(png_ptr));

	row_pointers = (png_bytep*)MFHeap_Alloc(sizeof(png_bytep) * height);
	for(int y=0; y<height; y++)
		row_pointers[y] = (png_byte*)MFHeap_Alloc(info_ptr->rowbytes);

	png_read_image(png_ptr, row_pointers);

	MFFile_Close(pFile);

	// allocate internal image structures
	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	pImage->mipLevels = 1;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface));

	pImage->pSurfaces[0].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*width*height);
	pImage->pSurfaces[0].width = width;
	pImage->pSurfaces[0].height = height;

	MFIntTexturePixel *pPixel = pImage->pSurfaces[0].pData;

	switch(color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)*p * (1.0f/255.0f);
						pPixel->g = (float)*p * (1.0f/255.0f);
						pPixel->b = (float)*p * (1.0f/255.0f);
						pPixel->a = 1.0f;

						++p;
						++pPixel;
					}
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)*p * (1.0f/65535.0f);
						pPixel->g = (float)*p * (1.0f/65535.0f);
						pPixel->b = (float)*p * (1.0f/65535.0f);
						pPixel->a = 1.0f;

						++p;
						++pPixel;
					}
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
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/255.0f);
						pPixel->g = (float)p[1] * (1.0f/255.0f);
						pPixel->b = (float)p[2] * (1.0f/255.0f);
						pPixel->a = 1.0f;

						p += 3;
						++pPixel;
					}
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
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/255.0f);
						pPixel->g = (float)p[1] * (1.0f/255.0f);
						pPixel->b = (float)p[2] * (1.0f/255.0f);
						pPixel->a = (float)p[3] * (1.0f/255.0f);

						p += 4;
						++pPixel;
					}
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/65535.0f);
						pPixel->g = (float)p[1] * (1.0f/65535.0f);
						pPixel->b = (float)p[2] * (1.0f/65535.0f);
						pPixel->a = (float)p[3] * (1.0f/65535.0f);

						p += 4;
						++pPixel;
					}
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
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/255.0f);
						pPixel->g = (float)p[0] * (1.0f/255.0f);
						pPixel->b = (float)p[0] * (1.0f/255.0f);
						pPixel->a = (float)p[1] * (1.0f/255.0f);

						p += 2;
						++pPixel;
					}
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/65535.0f);
						pPixel->g = (float)p[0] * (1.0f/65535.0f);
						pPixel->b = (float)p[0] * (1.0f/65535.0f);
						pPixel->a = (float)p[1] * (1.0f/65535.0f);

						p += 2;
						++pPixel;
					}
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
	}

	// free image
	for(int y=0; y<height; y++)
		MFHeap_Free(row_pointers[y]);
	MFHeap_Free(row_pointers);

	return pImage;
}
#endif

MFIntTexture* LoadTGA(const void *pMemory, uint32 imageSize)
{
	unsigned char *pTarga = (unsigned char *)pMemory;

	if(imageSize < (sizeof(TgaHeader) + 1))
		return NULL;

	TgaHeader *pHeader = (TgaHeader*)pTarga;
	unsigned char *pImageData = pTarga + sizeof(TgaHeader);

	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	pImage->mipLevels = 1;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface));

	pImage->pSurfaces[0].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*pHeader->width*pHeader->height);
	pImage->pSurfaces[0].width = pHeader->width;
	pImage->pSurfaces[0].height = pHeader->height;

	MFIntTexturePixel *pPixel = pImage->pSurfaces[0].pData;

	unsigned char *pPosition;
	bool isSavedFlipped = true;

	pPosition = pTarga;
	pPosition += sizeof(TgaHeader);

	if((pHeader->imageType != 1) && (pHeader->imageType != 2) && (pHeader->imageType != 10))
	{
		MFDebug_Message(MFStr("Failed loading image (Unhandled TGA type (%d))\n", pHeader->imageType));
		MFHeap_Free(pTarga);
		return NULL;
	}

	if((pHeader->bpp != 24) && (pHeader->bpp != 32) && (pHeader->bpp != 16))
	{
		MFDebug_Message(MFStr("Failed loading image (Invalid colour depth (%d))", pHeader->bpp));
		MFHeap_Free(pTarga);
		return NULL;
	}

	if((pHeader->flags & 0xC0))
	{
		MFDebug_Message("Failed loading image (Interleaved images not supported)");
		MFHeap_Free(pTarga);
		return NULL;
	}

	if((pHeader->flags & 0x20) >> 5)
	{
		isSavedFlipped = false;
	}

	if((pPosition + pHeader->idLength + (pHeader->colourMapLength * pHeader->colourMapBits * pHeader->colourMapType)) >= pTarga + imageSize)
	{
		MFDebug_Message("Failed loading image (Unexpected end of file)");
		MFHeap_Free(pTarga);
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
				MFDebug_Message("Failed loading image (Unexpected end of file)");
				MFHeap_Free(pTarga);
				return NULL;
			}

			if(*pPosition & 0x80) // Run length packet
			{
				uint8 length = ((*pPosition) & 0x7F) + 1;

				pPosition += 1;

				if((pPosition + bytesPerPixel) > pTarga + imageSize)
				{
					MFDebug_Message("Failed loading image (Unexpected end of file)");
					MFHeap_Free(pTarga);
					return NULL;
				}

				if((pixelsRead + length) > (uint32)(pHeader->width * pHeader->height))
				{
					MFDebug_Message("Failed loading image (Unexpected end of file)");
					MFHeap_Free(pTarga);
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
					MFDebug_Message("Failed loading image (Unexpected end of file)");
					MFHeap_Free(pTarga);
					return NULL;
				}

				if((pixelsRead + length) > (uint32)(pHeader->width * pHeader->height))
				{
					MFDebug_Message("Failed loading image (Unexpected end of file)");
					MFHeap_Free(pTarga);
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
			MFDebug_Message("Failed loading image (Unexpected end of file)");
			MFHeap_Free(pTarga);
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
			for(int a=0; a<pHeader->width*pHeader->height; a++)
			{
				pPixel->r = (float)pImageData[2] * (1.0f/255.0f);
				pPixel->g = (float)pImageData[1] * (1.0f/255.0f);
				pPixel->b = (float)pImageData[0] * (1.0f/255.0f);
				pPixel->a = 1.0f;
				pImageData += bytesPerPixel;
				++pPixel;
			}
		}
		else if(pHeader->bpp == 32)
		{
			for(int a=0; a<pHeader->width*pHeader->height; a++)
			{
				pPixel->r = (float)pImageData[2] * (1.0f/255.0f);
				pPixel->g = (float)pImageData[1] * (1.0f/255.0f);
				pPixel->b = (float)pImageData[0] * (1.0f/255.0f);
				pPixel->a = (float)pImageData[3] * (1.0f/255.0f);
				pImageData += bytesPerPixel;
				++pPixel;
			}
		}
	}
	else if(pHeader->imageType == 1) // paletted
	{
		MFDebug_Assert(false, "Paletted images not yet supported....");
	}

	MFHeap_Free(pTarga);

	if(isSavedFlipped)
	{
		MFIntTexture_FlipImage(pImage);
	}

	return pImage;
}

MFIntTexture* LoadBMP(const void *pMemory, uint32 imageSize)
{
	unsigned char *pBMP = (unsigned char *)pMemory;

	if(imageSize < (sizeof(BMPHeader) + 1))
		return NULL;

	if(pBMP[0] != 'B' || pBMP[1] != 'M')
	{
		MFDebug_Message("Not a bitmap image.");
		MFHeap_Free(pBMP);
		return NULL;
	}

	BMPHeader *pHeader = (BMPHeader*)pBMP;
	BMPInfoHeader *pInfoHeader = (BMPInfoHeader*)&pHeader[1];

	unsigned char *pImageData = pBMP + pHeader->offset;

	MFIntTexture *pImage = (MFIntTexture*)MFHeap_Alloc(sizeof(MFIntTexture));

	pImage->mipLevels = 1;
	pImage->pSurfaces = (MFIntTextureSurface*)MFHeap_Alloc(sizeof(MFIntTextureSurface));

	pImage->pSurfaces[0].pData = (MFIntTexturePixel*)MFHeap_Alloc(sizeof(MFIntTexturePixel)*pInfoHeader->width*pInfoHeader->height);
	pImage->pSurfaces[0].width = pInfoHeader->width;
	pImage->pSurfaces[0].height = pInfoHeader->height;

	MFIntTexturePixel *pPixel = pImage->pSurfaces[0].pData;

	bool isSavedFlipped = true;

	switch(pInfoHeader->compression)
	{
		case RGB:
			if(pInfoHeader->bits == 24)
			{
				struct Pixel24
				{
					unsigned char b, g, r;
				};

				Pixel24 *p = (Pixel24*)pImageData;

				for(int y=0; y<pInfoHeader->height; y++)
				{
					for(int x=0; x<pInfoHeader->width; x++)
					{
						pPixel->r = (float)p->r * (1.0f/255.0f);
						pPixel->g = (float)p->g * (1.0f/255.0f);
						pPixel->b = (float)p->b * (1.0f/255.0f);
						pPixel->a = 1.0f;

						++pPixel;
						++p;
					}
				}
			}
			else if(pInfoHeader->bits == 32)
			{
				struct Pixel32
				{
					unsigned char b, g, r, a;
				};

				Pixel32 *p = (Pixel32*)pImageData;

				for(int y=0; y<pInfoHeader->height; y++)
				{
					for(int x=0; x<pInfoHeader->width; x++)
					{
						pPixel->r = (float)p->r * (1.0f/255.0f);
						pPixel->g = (float)p->g * (1.0f/255.0f);
						pPixel->b = (float)p->b * (1.0f/255.0f);
						pPixel->a = (float)p->a * (1.0f/255.0f);

						++pPixel;
						++p;
					}
				}
			}
			else
			{
				MFDebug_Message("Unsupported colour depth.");
				MFHeap_Free(pBMP);
				return NULL;
			}
			break;

		case RLE8:
		case RLE4:
		case BITFIELDS:
		default:
		{
			MFDebug_Message("Compressed bitmaps not supported.");
			MFHeap_Free(pBMP);
			return NULL;
		}
	}

	MFHeap_Free(pBMP);

	if(isSavedFlipped)
	{
		MFIntTexture_FlipImage(pImage);
	}

	return pImage;
}

MFIntTexture *MFIntTexture_CreateFromFile(const char *pFilename)
{
	// find format
	const char *pExt = MFString_GetFileExtension(pFilename);

	int format;
	for(format=0; format<MFIMF_Max; ++format)
	{
		if(!MFString_Compare(pExt, gFileExtensions[format]))
			break;
	}
	if(format == MFIMF_Max)
		return NULL;

	// load file
	uint32 size;
	char *pData = MFFileSystem_Load(pFilename, &size);
	if(!pData)
		return NULL;

	// load the image
	MFIntTexture *pImage = MFIntTexture_CreateFromFileInMemory(pData, size, (MFIntTextureFormat)format);

	// free file
	MFHeap_Free(pData);

	return pImage;
}

MFIntTexture *MFIntTexture_CreateFromFileInMemory(const void *pMemory, uint32 size, MFIntTextureFormat format)
{
	MFIntTexture *pImage = NULL;

	switch(format)
	{
		case MFIMF_TGA:
			pImage = LoadTGA(pMemory, size);
			break;
		case MFIMF_BMP:
			pImage = LoadBMP(pMemory, size);
			break;
		case MFIMF_PNG:
#if defined(MF_ENABLE_PNG)
			pImage = LoadPNG(pMemory, size);
#else
			MFDebug_Assert(false, "PNG support is not enabled in this build.");
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
		MFIntTexture_FilterMipMaps(pImage, 0, 0);
	}

	return pImage;
}

void MFIntTexture_Destroy(MFIntTexture *pTexture)
{
	for(int a=0; a<pTexture->mipLevels; a++)
	{
		if(pTexture->pSurfaces[a].pData)
			MFHeap_Free(pTexture->pSurfaces[a].pData);
	}

	if(pTexture->pSurfaces)
		MFHeap_Free(pTexture->pSurfaces);

	if(pTexture)
		MFHeap_Free(pTexture);
}

void MFIntTexture_CreateRuntimeData(MFIntTexture *pTexture, void **ppOutput, uint32 *pSize, MFPlatform platform)
{

}

void MFIntTexture_WriteToHeaderFile(MFIntTexture *pTexture, const char *pFilename)
{

}

void MFIntTexture_FilterMipMaps(MFIntTexture *pTexture, int numMipLevels, uint32 mipFilterOptions)
{

}

void MFIntTexture_ScanImage(MFIntTexture *pTexture)
{
	pTexture->opaque = true;
	pTexture->oneBitAlpha = true;

	for(int a=0; a<pTexture->mipLevels; a++)
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

void MFIntTexture_FlipImage(MFIntTexture *pTexture)
{
	for(int a=0; a<pTexture->mipLevels; a++)
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
