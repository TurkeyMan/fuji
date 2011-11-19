#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFTexture_InitModulePlatformSpecific MFTexture_InitModulePlatformSpecific_D3D9
	#define MFTexture_DeinitModulePlatformSpecific MFTexture_DeinitModulePlatformSpecific_D3D9
	#define MFTexture_Release MFTexture_Release_D3D9
	#define MFTexture_Recreate MFTexture_Recreate_D3D9
	#define MFTexture_CreatePlatformSpecific MFTexture_CreatePlatformSpecific_D3D9
	#define MFTexture_CreateRenderTarget MFTexture_CreateRenderTarget_D3D9
	#define MFTexture_DestroyPlatformSpecific MFTexture_DestroyPlatformSpecific_D3D9
#endif

/**** Defines ****/

/**** Includes ****/

#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFFileSystem_Internal.h"

#include <d3dx9.h>

/**** Globals ****/

extern MFTexture *pNoneTexture;

extern IDirect3DDevice9 *pd3dDevice;

/**** Functions ****/

void MFTexture_InitModulePlatformSpecific()
{
}

void MFTexture_DeinitModulePlatformSpecific()
{
}

void MFTexture_Release()
{
	MFTexturePool::Iterator pTex = gTextureBank.First();

	while(pTex)
	{
		MFTextureTemplateData *pTemplate = pTex->pTemplateData;

		if(pTemplate->flags & TEX_RenderTarget)
		{
			IDirect3DTexture9 *pD3DTex = (IDirect3DTexture9*)pTex->pInternalData;

			// release the old render target
			pD3DTex->Release();

			pTex->pInternalData = NULL;
		}

		pTex = gTextureBank.Next(pTex);
	}
}

void MFTexture_Recreate()
{
	MFTexturePool::Iterator pTex = gTextureBank.First();

	while(pTex)
	{
		MFTextureTemplateData *pTemplate = pTex->pTemplateData;

		if(pTemplate->flags & TEX_RenderTarget)
		{
			// recreate a new one
			D3DFORMAT platformFormat = (D3DFORMAT)MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFDD_D3D9);
			pd3dDevice->CreateTexture(pTemplate->pSurfaces->width, pTemplate->pSurfaces->height, 1, D3DUSAGE_RENDERTARGET, platformFormat, D3DPOOL_DEFAULT, (IDirect3DTexture9**)&pTex->pInternalData, NULL);
		}

		pTex = gTextureBank.Next(pTex);
	}
}

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFCALLSTACK;

	HRESULT hr;
	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	// create texture
	D3DFORMAT platformFormat = (D3DFORMAT)MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFDD_D3D9);
	hr = D3DXCreateTexture(pd3dDevice, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, platformFormat, D3DPOOL_MANAGED, (IDirect3DTexture9**)&pTexture->pInternalData);

	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	MFDebug_Assert(hr != D3DXERR_INVALIDDATA, MFStr("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	MFDebug_Assert(hr == D3D_OK, MFStr("Failed to create texture '%s'.", pTexture->name));

	IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;

	// copy image data
	D3DLOCKED_RECT rect;
	pTex->LockRect(0, &rect, NULL, 0);
	MFCopyMemory(rect.pBits, pTemplate->pSurfaces[0].pImageData, pTemplate->pSurfaces[0].bufferLength);
	pTex->UnlockRect(0);

	// filter mip levels
	if(generateMipChain)
		D3DXFilterTexture(pTex, NULL, 0, D3DX_DEFAULT);
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFTextureFormat targetFormat)
{
	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = &gTextureBank.Create(pName);

		if(targetFormat & TexFmt_SelectNicest)
		{
			targetFormat = TexFmt_A8R8G8B8;
		}

		pTexture->pTemplateData = (MFTextureTemplateData*)MFHeap_AllocAndZero(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)&pTexture->pTemplateData[1];
		pTexture->pTemplateData->magicNumber = MFMAKEFOURCC('F','T','E','X');
		pTexture->pTemplateData->imageFormat = targetFormat;
		pTexture->pTemplateData->mipLevels = 1;
		pTexture->pTemplateData->flags = TEX_RenderTarget;

		pTexture->pTemplateData->pSurfaces->width = width;
		pTexture->pTemplateData->pSurfaces->height = height;
		pTexture->pTemplateData->pSurfaces->bitsPerPixel = MFTexture_GetBitsPerPixel(pTexture->pTemplateData->imageFormat);
		pTexture->pTemplateData->pSurfaces->xBlocks = width;
		pTexture->pTemplateData->pSurfaces->yBlocks = height;
		pTexture->pTemplateData->pSurfaces->bitsPerBlock = pTexture->pTemplateData->pSurfaces->bitsPerPixel;
		pTexture->pTemplateData->pSurfaces->pImageData = NULL;
		pTexture->pTemplateData->pSurfaces->bufferLength = 0;
		pTexture->pTemplateData->pSurfaces->pPaletteEntries = NULL;
		pTexture->pTemplateData->pSurfaces->paletteBufferLength = 0;

		pTexture->refCount = 1;
		MFString_CopyN(pTexture->name, pName, sizeof(pTexture->name) - 1);
		pTexture->name[sizeof(pTexture->name) - 1] = 0;

		D3DFORMAT platformFormat = (D3DFORMAT)MFTexture_GetPlatformFormatID(targetFormat, MFDD_D3D9);
		HRESULT hr = pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, platformFormat, D3DPOOL_DEFAULT, (IDirect3DTexture9**)&pTexture->pInternalData, NULL);

		if(hr != D3D_OK)
		{
			MFHeap_Free(pTexture->pTemplateData);
			gTextureBank.DestroyItem(*pTexture);
			pTexture = NULL;
		}
	}
	else
	{
		pTexture->refCount++;
	}

	return pTexture;
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;
	pTex->Release();
}

#endif // MF_RENDERER
