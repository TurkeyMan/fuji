/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Texture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "PtrList.h"
#include "FileSystem/MFFileSystemNative.h"
#include "Image.h"

#include <pspdisplay.h>
#include <pspgu.h>

/**** Globals ****/

extern PtrListDL<Texture> gTextureBank;
extern Texture *pNoneTexture;

/**** Functions ****/

// interface functions
Texture* Texture_Create(const char *pName, bool generateMipChain)
{
	Texture *pTexture = Texture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		const char *pFileName = STR("%s.tga", pName);

		FujiImage *pImage = LoadTGA(pFileName, false);
		pImage->Convert(FUJI_RGBA);

		uint32 numPixels = pImage->width * pImage->height;

		pTexture->pImageData = (char*)Heap_Alloc(numPixels * 2);

		char *pSrcData = (char*)pImage->pixels;
		uint16 *pData = (uint16*)pTexture->pImageData;

		for(uint32 a=0; a<numPixels; a++)
		{
			if(pImage->bitsPerPixel == 32)
			{
				*pData = ((pSrcData[0] & 0xF0) >> 4) |
						  (pSrcData[1] & 0xF0) |
						 ((pSrcData[2] & 0xF0) << 4) |
						 ((pSrcData[3] & 0xF0) << 8);
			}
			else if(pImage->bitsPerPixel == 24)
			{
				*pData = ((pSrcData[0] & 0xF0) >> 4) |
						  (pSrcData[1] & 0xF0) |
						 ((pSrcData[2] & 0xF0) << 4) |
						  0xF000;
			}

			pSrcData += pImage->bytesPerPixel;
			++pData;
		}

		strcpy(pTexture->name, pName);

		pTexture->width = pImage->width;
		pTexture->height = pImage->height;
		pTexture->format = GU_PSM_4444;

		delete pImage;
	}

	pTexture->refCount++;

	return pTexture;
}

Texture* Texture_CreateFromRawData(const char *pName, void *pData, int width, int height, int format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	Texture *pTexture = Texture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		int numPixels = width * height;

		pTexture->pImageData = (char*)Heap_Alloc(numPixels * 2);

		uint32 *pSrcData = (uint32*)pData;
		uint16 *pData = (uint16*)pTexture->pImageData;

		switch(format)
		{
			case TEXF_A8R8G8B8:
			{
				if(flags & TEX_VerticalMirror)
				{
					pSrcData += numPixels;

					for(int a=0; a<(int)height; a++)
					{
						pSrcData -= width;

						for(int b=0; b<(int)width; b++)
						{
							*pData = ((pSrcData[b] & 0xF0) << 4) |
									((pSrcData[b] & 0xF000) >> 8) |
									((pSrcData[b] & 0xF00000) >> 20) |
									((pSrcData[b] & 0xF0000000) >> 16);

							++pData;
						}
					}
				}
				else
				{
					for(int a=0; a<numPixels; a++)
					{
						*pData = ((*pSrcData & 0xF0) << 4) |
								((*pSrcData & 0xF000) >> 8) |
								((*pSrcData & 0xF00000) >> 20) |
								((*pSrcData & 0xF0000000) >> 16);

						++pSrcData;
						++pData;
					}
				}
				break;
			}
		}

		strcpy(pTexture->name, pName);

		pTexture->width = width;
		pTexture->height = height;
		pTexture->format = GU_PSM_4444;
	}

	pTexture->refCount++;

	return pTexture;
}

Texture* Texture_CreateRenderTarget(const char *pName, int width, int height)
{
	DBGASSERT(false, "Not Written...");

	return NULL;
}

int Texture_Destroy(Texture *pTexture)
{
	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		Heap_Free(pTexture->pImageData);

		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}
