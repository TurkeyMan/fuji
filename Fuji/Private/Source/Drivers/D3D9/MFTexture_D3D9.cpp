#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9

/**** Defines ****/

/**** Includes ****/

#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#include <d3dx9.h>

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

extern IDirect3DDevice9 *pd3dDevice;

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFCALLSTACK;

	HRESULT hr;
	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	// create texture
	D3DFORMAT platformFormat = (D3DFORMAT)MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFDD_D3D9);
	hr = D3DXCreateTexture(pd3dDevice, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, platformFormat, D3DPOOL_MANAGED, &pTexture->pTexture);

	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	MFDebug_Assert(hr != D3DXERR_INVALIDDATA, MFStr("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	MFDebug_Assert(hr == D3D_OK, MFStr("Failed to create texture '%s'.", pTexture->name));

	// copy image data
	D3DLOCKED_RECT rect;
	pTexture->pTexture->LockRect(0, &rect, NULL, 0);
	MFCopyMemory(rect.pBits, pTemplate->pSurfaces[0].pImageData, pTemplate->pSurfaces[0].bufferLength);
	pTexture->pTexture->UnlockRect(0);

	// filter mip levels
	if(generateMipChain)
		D3DXFilterTexture(pTexture->pTexture, NULL, 0, D3DX_DEFAULT);
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	MFCALLSTACK;

	MFDebug_Assert(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	MFCALLSTACK;

	--pTexture->refCount;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		MFHeap_Free(pTexture->pTemplateData);
		pTexture->pTexture->Release();

		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}

#endif // MF_RENDERER
