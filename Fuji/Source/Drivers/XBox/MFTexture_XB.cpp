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

uint8 gXBoxFormat[ImgFmt_Max] =
{
	0x12,	// ImgFmt_A8R8G8B8
	0x3F,	// ImgFmt_A8B8G8R8
	0x40,	// ImgFmt_B8G8R8A8
	0x41,	// ImgFmt_R8G8B8A8

	0,		// ImgFmt_R8G8B8
	0,		// ImgFmt_B8G8R8

	0,		// ImgFmt_A2R10G10B10
	0,		// ImgFmt_A2B10G10R10

	0,		// ImgFmt_A16B16G16R16

	0x11,	// ImgFmt_R5G6B5
	0x37,	// ImgFmt_R6G5B5
	0,		// ImgFmt_B5G6R5

	0x10,	// ImgFmt_A1R5G5B5
	0x3D,	// ImgFmt_R5G5B5A1
	0,		// ImgFmt_A1B5G5R5

	0x1D,	// ImgFmt_A4R4G4B4
	0,		// ImgFmt_A4B4G4R4
	0x3E,	// ImgFmt_R4G4B4A4

	0,		// ImgFmt_ABGR_F16
	0,		// ImgFmt_ABGR_F32

	0,		// ImgFmt_R11G11B10_F
	0,		// ImgFmt_R9G9B9_E5

	0xB,	// ImgFmt_I8
	0,		// ImgFmt_I4

	0,		// ImgFmt_D16
	0,		// ImgFmt_D24X8
	0,		// ImgFmt_D32
	0,		// ImgFmt_D32F

	0,		// ImgFmt_D15S1
	0,		// ImgFmt_D24S8
	0,		// ImgFmt_D24FS8
	0,		// ImgFmt_D32FS8X24

	0xC,	// ImgFmt_DXT1
	0xE,	// ImgFmt_DXT2
	0xE,	// ImgFmt_DXT3
	0xF,	// ImgFmt_DXT4
	0xF,	// ImgFmt_DXT5

	0,		// ImgFmt_PSP_DXT1
	0,		// ImgFmt_PSP_DXT3
	0,		// ImgFmt_PSP_DXT5

	0x6,	// ImgFmt_XB_A8R8G8B8
	0x3A,	// ImgFmt_XB_A8B8G8R8
	0x3B,	// ImgFmt_XB_B8G8R8A8
	0x3C,	// ImgFmt_XB_R8G8B8A8

	0x5,	// ImgFmt_XB_R5G6B5
	0x27,	// ImgFmt_XB_R6G5B5

	0x2,	// ImgFmt_XB_A1R5G5B5
	0x38,	// ImgFmt_XB_R5G5B5A1

	0x4,	// ImgFmt_XB_A4R4G4B4
	0x39,	// ImgFmt_XB_R4G4B4A4

	0,		// ImgFmt_PSP_A8B8G8R8s
	0,		// ImgFmt_PSP_B5G6R5s
	0,		// ImgFmt_PSP_A1B5G5R5s
	0,		// ImgFmt_PSP_A4B4G4R4s

	0,		// ImgFmt_PSP_I8s
	0,		// ImgFmt_PSP_I4s

	0,		// ImgFmt_PSP_DXT1s
	0,		// ImgFmt_PSP_DXT3s
	0,		// ImgFmt_PSP_DXT5s
};

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	// create texture
	D3DFORMAT platformFormat = (D3DFORMAT)MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFDD_XBOX);

#if defined(XB_XGTEXTURES)
	pTexture->pTexture = &pTexture->texture;
	XGSetTextureHeader(pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, 1, 0, platformFormat, 0, pTexture->pTexture, 0, 0);
	pTexture->pTexture->Register(pTemplate->pSurfaces[0].pImageData);

	if(pTemplate->imageFormat >= ImgFmt_XB_A8R8G8B8s && pTemplate->imageFormat <= ImgFmt_XB_R4G4B4A4s)
	{
		XGSwizzleRect(pTemplate->pSurfaces[0].pImageData, 0, NULL, pTemplate->pSurfaces[0].pImageData, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, NULL, pTemplate->pSurfaces[0].bitsPerPixel/8);
	}
#else

	HRESULT hr;
	hr = D3DXCreateTexture(pd3dDevice, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, platformFormat, 0, &pTexture->pTexture);

	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	MFDebug_Assert(hr != D3DXERR_INVALIDDATA, MFStr("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	MFDebug_Assert(hr == D3D_OK, MFStr("Failed to create texture '%s'.", pTexture->name));

	// copy image data
	D3DLOCKED_RECT rect;
	pTexture->pTexture->LockRect(0, &rect, NULL, 0);

	if(pTemplate->imageFormat >= ImgFmt_XB_A8R8G8B8s && pTemplate->imageFormat <= ImgFmt_XB_R4G4B4A4s)
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

MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
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
