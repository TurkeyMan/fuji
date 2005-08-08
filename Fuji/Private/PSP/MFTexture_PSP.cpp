/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "PtrList.h"
#include "FileSystem/MFFileSystemNative.h"
#include "Image.h"

#include <pspdisplay.h>
#include <pspgu.h>

/**** Globals ****/

extern PtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

/**** Functions ****/

// interface functions
MFTexture* MFTexture_Create(const char *pName, bool generateMipChain)
{
	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		const char *pFileName = STR("%s.tex", pName);

		pTexture->pTemplateData = (MFTextureTemplateData*)MFFileSystem_Load(pFileName);

		if(!pTexture->pTemplateData)
		{
			const char *pFileName = STR("%s.tga", pName);

			FujiImage *pImage = LoadTGA(pFileName, false);
			pImage->Convert(FUJI_RGBA);

			uint32 numPixels = pImage->width * pImage->height;

			char *pImageData = (char*)Heap_Alloc(numPixels * 2);

			char *pSrcData = (char*)pImage->pixels;
			uint16 *pData = (uint16*)pImageData;

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

			// create template data
			char *pTemplate = (char*)Heap_Alloc(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));

			pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
			pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

			pTexture->pTemplateData->imageFormat = TexFmt_A4B4G4R4;
			pTexture->pTemplateData->platformFormat = GU_PSM_4444;

			pTexture->pTemplateData->mipLevels = 1;

			pTexture->pTemplateData->pSurfaces[0].width = pImage->width;
			pTexture->pTemplateData->pSurfaces[0].height = pImage->height;
			pTexture->pTemplateData->pSurfaces[0].pImageData = pImageData;

			delete pImage;
		}
	}

	pTexture->refCount++;

	return pTexture;
}

MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormats format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		int numPixels = width * height;

		char *pImageData= (char*)Heap_Alloc(numPixels * 2);

		uint32 *pSrcData = (uint32*)pData;
		uint16 *pData = (uint16*)pImageData;

		switch(format)
		{
			case TexFmt_A8R8G8B8:
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

			default:
				DBGASSERT(false, "Source format not supported...");
				break;
		}

		strcpy(pTexture->name, pName);

		// create template data
		char *pTemplate = (char*)Heap_Alloc(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));

		pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

		pTexture->pTemplateData->imageFormat = TexFmt_A4B4G4R4;
		pTexture->pTemplateData->platformFormat = GU_PSM_4444;

		pTexture->pTemplateData->mipLevels = 1;

		pTexture->pTemplateData->pSurfaces[0].width = width;
		pTexture->pTemplateData->pSurfaces[0].height = height;
		pTexture->pTemplateData->pSurfaces[0].pImageData = pImageData;
	}

	pTexture->refCount++;

	return pTexture;
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	DBGASSERT(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		Heap_Free(pTexture->pTemplateData);
		Heap_Free(pTexture->pTemplateData->pSurfaces[0].pImageData);

		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}
