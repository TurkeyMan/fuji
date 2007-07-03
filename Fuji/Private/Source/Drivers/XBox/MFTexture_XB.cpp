#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_XBOX

/**** Defines ****/

/**** Includes ****/

#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#include <xgraphics.h>

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

extern IDirect3DDevice8 *pd3dDevice;

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	// create texture
#if defined(XB_XGTEXTURES)
	pTexture->pTexture = &pTexture->texture;
	XGSetTextureHeader(pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, 1, 0, (D3DFORMAT)pTemplate->platformFormat, 0, pTexture->pTexture, 0, 0);
	pTexture->pTexture->Register(pTemplate->pSurfaces[0].pImageData);

	if(pTemplate->imageFormat >= TexFmt_XB_A8R8G8B8s && pTemplate->imageFormat <= TexFmt_XB_R4G4B4A4s)
	{
		XGSwizzleRect(pTemplate->pSurfaces[0].pImageData, 0, NULL, pTemplate->pSurfaces[0].pImageData, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, NULL, pTemplate->pSurfaces[0].bitsPerPixel/8);
	}
#else
	HRESULT hr;
	hr = D3DXCreateTexture(pd3dDevice, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, (D3DFORMAT)pTemplate->platformFormat, 0, &pTexture->pTexture);

	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	MFDebug_Assert(hr != D3DXERR_INVALIDDATA, MFStr("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	MFDebug_Assert(hr == D3D_OK, MFStr("Failed to create texture '%s'.", pTexture->name));

	// copy image data
	D3DLOCKED_RECT rect;
	pTexture->pTexture->LockRect(0, &rect, NULL, 0);

	if(pTemplate->imageFormat >= TexFmt_XB_A8R8G8B8s && pTemplate->imageFormat <= TexFmt_XB_R4G4B4A4s)
	{
		XGSwizzleRect(pTemplate->pSurfaces[0].pImageData, 0, NULL, rect.pBits, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, NULL, pTemplate->pSurfaces[0].bitsPerPixel/8);
	}
	else
	{
		MFCopyMemory(rect.pBits, pTemplate->pSurfaces[0].pImageData, pTemplate->pSurfaces[0].bufferLength);
	}

	pTexture->pTexture->UnlockRect(0);

	// filter mip levels
	if(generateMipChain)
		D3DXFilterTexture(pTexture->pTexture, NULL, 0, D3DX_DEFAULT);
#endif
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	MFDebug_Assert(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		pTexture->pTexture->Release();
		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}

#endif
