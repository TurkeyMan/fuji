#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_DC

#include "Texture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "PtrList.h"
#include "FileSystem/MFFileSystemNative.h"

/**** Globals ****/

extern PtrListDL<Texture> gTextureBank;
extern Texture *pNoneTexture;

/**** Functions ****/

// interface functions
Texture* MFTexture_Create(const char *pName, bool generateMipChain)
{
	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;
	}

	pTexture->refCount++;

	return pTexture;
}

Texture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	MFDebug_Assert(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(Texture *pTexture)
{
	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}

#endif
