#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PSP

/**** Defines ****/

/**** Includes ****/

#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#include <pspdisplay.h>
#include <pspgu.h>

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	// no processing required on PSP..
}

MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	MFCALLSTACK;

	MFDebug_Assert(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	MFCALLSTACK;

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

#endif
