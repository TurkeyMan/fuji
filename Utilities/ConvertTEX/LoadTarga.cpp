#include "Common.h"

#include "ConvertTex.h"
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

	if(pTarga == NULL || imageSize < (sizeof(TgaHeader) + 1))
	{
		return NULL;
	}

	TgaHeader *pHeader = (TgaHeader*)pTarga;
	unsigned char *pImageData = pTarga + sizeof(TgaHeader);

	SourceImage *pImage = (SourceImage*)malloc(sizeof(SourceImage));

	pImage->mipLevels = 1;
	pImage->pLevels = (SourceImageLevel*)malloc(sizeof(SourceImageLevel));

	pImage->pLevels[0].pData = (Pixel*)malloc(sizeof(Pixel)*pHeader->width*pHeader->height);
	pImage->pLevels[0].width = pHeader->width;
	pImage->pLevels[0].height = pHeader->height;

	Pixel *pPixel = pImage->pLevels[0].pData;

	unsigned char *pPosition;
	bool isSavedFlipped = true;

	pPosition = pTarga;
	pPosition += sizeof(TgaHeader);

	if((pHeader->imageType != 2) && (pHeader->imageType != 10))
	{
		LOGERROR("Failed loading image: %s (Unhandled TGA type (%d))\n", pFilename, pHeader->imageType);
		free(pTarga);
		return NULL;
	}

	if((pHeader->bpp != 24) && (pHeader->bpp != 32))
	{
		LOGERROR("Failed loading image: %s (Invalid colour depth (%d))", pFilename, pHeader->bpp);
		free(pTarga);
		return NULL;
	}

	if((pHeader->flags & 0xC0))
	{
		LOGERROR("Failed loading image: %s (Interleaved images not supported)", pFilename);
		free(pTarga);
		return NULL;
	}

	if((pHeader->flags & 0x20) >> 5)
	{
		isSavedFlipped = false;
	}

	if((pPosition + pHeader->idLength + (pHeader->colourMapLength * pHeader->colourMapBits * pHeader->colourMapType)) >= pTarga + imageSize)
	{
		LOGD(STR("Failed loading image: %s (Unexpected end of file)", pFilename));
		free(pTarga);
		return NULL;
	}

	pPosition += pHeader->idLength;

	int bytesPerPixel = pHeader->bpp/8;

	if(pHeader->imageType == 10) // The hard way
	{
		uint32 pixelsRead = 0;

		while(pixelsRead < (uint32)(pHeader->width * pHeader->height))
		{
			if(pPosition >= pTarga + imageSize)
			{
				LOGERROR("Failed loading image: %s (Unexpected end of file)", pFilename);
				free(pTarga);
				return NULL;
			}

			if(*pPosition & 0x80) // Run length packet
			{
				uint8 length = ((*pPosition) & 0x7F) + 1;

				pPosition += 1;

				if((pPosition + bytesPerPixel) > pTarga + imageSize)
				{
					LOGERROR("Failed loading image: %s (Unexpected end of file)", pFilename);
					free(pTarga);
					return NULL;
				}

				if((pixelsRead + length) > (uint32)(pHeader->width * pHeader->height))
				{
					LOGERROR("Failed loading image: %s (Unexpected end of file)", pFilename);
					free(pTarga);
					return NULL;
				}

				Pixel pixel;

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
					LOGERROR("Failed loading image: %s (Unexpected end of file)", pFilename);
					free(pTarga);
					return NULL;
				}

				if((pixelsRead + length) > (uint32)(pHeader->width * pHeader->height))
				{
					LOGERROR("Failed loading image: %s (Unexpected end of file)", pFilename);
					free(pTarga);
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
	else // 2 - The easy way
	{
		if((pPosition + (bytesPerPixel * (pHeader->width * pHeader->height))) > pTarga + imageSize)
		{
			LOGERROR("Failed loading image: %s (Unexpected end of file)", pFilename);
			free(pTarga);
			return NULL;
		}

		if(pHeader->bpp == 24)
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

	free(pTarga);

	if(isSavedFlipped)
	{
		FlipImage(pImage);
	}

	// scan for alpha information
	ScanImage(pImage);

	return pImage;
}
