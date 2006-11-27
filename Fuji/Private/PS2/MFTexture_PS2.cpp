/**** Defines ****/

/**** Includes ****/

#include "Fuji.h"
#include "Display_Internal.h"
#include "MFTexture_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	// no processing required on PS2..
}

MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormat format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		/* Stuff ??  */
		
		MFString_Copy(pTexture->name, pName);

		int internalFormat = gMFTexturePlatformFormat[FP_PS2][format];

		// create template data
		char *pTemplate = (char*)MFHeap_Alloc(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));

		pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

		pTexture->pTemplateData->imageFormat = format;
		pTexture->pTemplateData->platformFormat = internalFormat;

		pTexture->pTemplateData->mipLevels = 1;

		pTexture->pTemplateData->pSurfaces->width = width;
		pTexture->pTemplateData->pSurfaces->height = height;
		pTexture->pTemplateData->pSurfaces->pImageData = (char*)pData;
	}

	pTexture->refCount++;

	return pTexture;
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	MFCALLSTACK;

	MFDebug_Assert(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		MFHeap_Free(pTexture->pTemplateData);
		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}
