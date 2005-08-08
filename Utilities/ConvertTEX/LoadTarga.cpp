#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "IntImage.h"
#include "LoadTarga.h"


SourceImage* LoadTarga(const char *pFilename)
{
	FILE *pFile = fopen(pFilename, "rb");

	if(!pFile)
		return NULL;

	fseek(pFile, 0, SEEK_END);
	int imageSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	unsigned char *pTarga = (unsigned char*)malloc(imageSize);

	fread(pTarga, 1, imageSize, pFile);
	fclose(pFile);

	TgaHeader *pHeader = (TgaHeader*)pTarga;
	unsigned char *pImageData = pTarga + sizeof(TgaHeader);

	SourceImage *pImage = (SourceImage*)malloc(sizeof(SourceImage));

	pImage->mipLevels = 1;
	pImage->pLevels = (SourceImageLevel*)malloc(sizeof(SourceImageLevel));

	pImage->pLevels[0].pData = (Pixel*)malloc(sizeof(Pixel)*pHeader->width*pHeader->height);
	pImage->pLevels[0].width = pHeader->width;
	pImage->pLevels[0].height = pHeader->height;

	Pixel *pPixel = pImage->pLevels[0].pData;

	// process targa...
	if(pHeader->bpp == 24)
	{
		for(int a=0; a<pHeader->width*pHeader->height; a++)
		{
			pPixel->r = (float)pImageData[2] * (1.0f/255.0f);
			pPixel->g = (float)pImageData[1] * (1.0f/255.0f);
			pPixel->b = (float)pImageData[0] * (1.0f/255.0f);
			pPixel->a = 1.0f;
			pImageData += 3;
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
			pImageData += 4;
			++pPixel;
		}
	}
	else
	{
		printf("targa format not supported at this time...");

		DestroyImage(pImage);
		pImage = NULL;
	}

	free(pTarga);

	return pImage;
}
