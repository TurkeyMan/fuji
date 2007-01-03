/**** Defines ****/

/**** Includes ****/

#include "Fuji.h"
#include "Display_Internal.h"
#include "MFTexture_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#include <string.h>

/**** Globals ****/
#include <stdio.h>

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	// no processing required on PS2..
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
