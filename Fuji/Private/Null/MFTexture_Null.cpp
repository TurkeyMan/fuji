/**** Defines ****/

/**** Includes ****/

#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
}

MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormat format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		strcpy(pTexture->name, pName);

		// create template data
		char *pTemplate = (char*)MFHeap_Alloc(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));

		pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

		pTexture->pTemplateData->imageFormat = format;
		pTexture->pTemplateData->platformFormat = 0;

		pTexture->pTemplateData->mipLevels = 1;

		pTexture->pTemplateData->pSurfaces->width = width;
		pTexture->pTemplateData->pSurfaces->height = height;
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
		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}
