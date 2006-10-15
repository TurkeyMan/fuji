#include "Fuji.h"

#include "ConvertTex.h"
#include "IntImage.h"
#include "LoadBMP.h"

SourceImage* LoadBMP(const char *pFilename)
{
	FILE *pFile = fopen(pFilename, "rb");

	if(!pFile)
		return NULL;

	fseek(pFile, 0, SEEK_END);
	int imageSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	unsigned char *pBMP = (unsigned char*)malloc(imageSize);

	if(pBMP == NULL || imageSize < (sizeof(BMPHeader) + 1))
		return NULL;

	fread(pBMP, 1, imageSize, pFile);
	fclose(pFile);

	if(pBMP[0] != 'B' || pBMP[1] != 'M')
	{
		MFDebug_Assert(false, "Not a bitmap image.");
		return NULL;
	}

	BMPHeader *pHeader = (BMPHeader*)pBMP;
	BMPInfoHeader *pInfoHeader = (BMPInfoHeader*)&pHeader[1];

	unsigned char *pImageData = pBMP + pHeader->offset;

	SourceImage *pImage = (SourceImage*)malloc(sizeof(SourceImage));

	pImage->mipLevels = 1;
	pImage->pLevels = (SourceImageLevel*)malloc(sizeof(SourceImageLevel));

	pImage->pLevels[0].pData = (Pixel*)malloc(sizeof(Pixel)*pInfoHeader->width*pInfoHeader->height);
	pImage->pLevels[0].width = pInfoHeader->width;
	pImage->pLevels[0].height = pInfoHeader->height;

	Pixel *pPixel = pImage->pLevels[0].pData;

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
				MFDebug_Assert(false, "Unsupported colour depth.");
				return NULL;
			}
			break;

		case RLE8:
		case RLE4:
		case BITFIELDS:
		default:
		{
			MFDebug_Assert(false, "Compressed bitmaps not supported.");
			return NULL;
		}
	}

	free(pBMP);

	if(isSavedFlipped)
	{
		FlipImage(pImage);
	}

	// scan for alpha information
	ScanImage(pImage);

	return pImage;
}
