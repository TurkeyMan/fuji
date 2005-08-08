/**** Defines ****/

/**** Includes ****/

#include "Common.h"
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
Texture* Texture_Create(const char *pName, bool generateMipChain)
{
	Texture *pTexture = Texture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;
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
		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}
