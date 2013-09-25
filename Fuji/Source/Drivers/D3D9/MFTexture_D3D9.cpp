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
#include "MFRenderer_D3D9.h"

#include <d3dx9.h>


/**** Globals ****/

extern MFTexture *pNoneTexture;

extern IDirect3DDevice9 *pd3dDevice;

static D3DFORMAT gD3D9Format[ImgFmt_Max] =
{
	D3DFMT_A8R8G8B8,		// ImgFmt_A8R8G8B8
	D3DFMT_A8B8G8R8,		// ImgFmt_A8B8G8R8
	D3DFMT_UNKNOWN,			// ImgFmt_B8G8R8A8
	D3DFMT_UNKNOWN,			// ImgFmt_R8G8B8A8

	D3DFMT_R8G8B8,			// ImgFmt_R8G8B8
	D3DFMT_UNKNOWN,			// ImgFmt_B8G8R8

	D3DFMT_A2R10G10B10,		// ImgFmt_A2R10G10B10
	D3DFMT_A2B10G10R10,		// ImgFmt_A2B10G10R10

	D3DFMT_A16B16G16R16,	// ImgFmt_A16B16G16R16

	D3DFMT_R5G6B5,			// ImgFmt_R5G6B5
	D3DFMT_UNKNOWN,			// ImgFmt_R6G5B5
	D3DFMT_UNKNOWN,			// ImgFmt_B5G6R5

	D3DFMT_A1R5G5B5,		// ImgFmt_A1R5G5B5
	D3DFMT_UNKNOWN,			// ImgFmt_R5G5B5A1
	D3DFMT_UNKNOWN,			// ImgFmt_A1B5G5R5

	D3DFMT_A4R4G4B4,		// ImgFmt_A4R4G4B4
	D3DFMT_UNKNOWN,			// ImgFmt_A4B4G4R4
	D3DFMT_UNKNOWN,			// ImgFmt_R4G4B4A4

	D3DFMT_A16B16G16R16F,	// ImgFmt_ABGR_F16
	D3DFMT_A32B32G32R32F,	// ImgFmt_ABGR_F32

	D3DFMT_UNKNOWN,			// ImgFmt_R11G11B10_F
	D3DFMT_UNKNOWN,			// ImgFmt_R9G9B9_E5

	D3DFMT_P8,				// ImgFmt_I8
	D3DFMT_UNKNOWN,			// ImgFmt_I4

	D3DFMT_D16,				// ImgFmt_D16
	D3DFMT_D24X8,			// ImgFmt_D24X8
	D3DFMT_D32,				// ImgFmt_D32
	D3DFMT_D32F_LOCKABLE,	// ImgFmt_D32F

	D3DFMT_D15S1,			// ImgFmt_D15S1
	D3DFMT_D24S8,			// ImgFmt_D24S8
	D3DFMT_D24FS8,			// ImgFmt_D24FS8
	D3DFMT_UNKNOWN,			// ImgFmt_D32FS8X24

	D3DFMT_DXT1,			// ImgFmt_DXT1
	D3DFMT_DXT2,			// ImgFmt_DXT2
	D3DFMT_DXT3,			// ImgFmt_DXT3
	D3DFMT_DXT4,			// ImgFmt_DXT4
	D3DFMT_DXT5,			// ImgFmt_DXT5

	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT1
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT3
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT5

	D3DFMT_UNKNOWN,			// ImgFmt_XB_A8R8G8B8
	D3DFMT_UNKNOWN,			// ImgFmt_XB_A8B8G8R8
	D3DFMT_UNKNOWN,			// ImgFmt_XB_B8G8R8A8
	D3DFMT_UNKNOWN,			// ImgFmt_XB_R8G8B8A8

	D3DFMT_UNKNOWN,			// ImgFmt_XB_R5G6B5
	D3DFMT_UNKNOWN,			// ImgFmt_XB_R6G5B5

	D3DFMT_UNKNOWN,			// ImgFmt_XB_A1R5G5B5
	D3DFMT_UNKNOWN,			// ImgFmt_XB_R5G5B5A1

	D3DFMT_UNKNOWN,			// ImgFmt_XB_A4R4G4B4
	D3DFMT_UNKNOWN,			// ImgFmt_XB_R4G4B4A4

	D3DFMT_UNKNOWN,			// ImgFmt_PSP_A8B8G8R8s
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_B5G6R5s
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_A1B5G5R5s
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_A4B4G4R4s

	D3DFMT_UNKNOWN,			// ImgFmt_PSP_I8s
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_I4s

	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT1s
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT3s
	D3DFMT_UNKNOWN			// ImgFmt_PSP_DXT5s
};


/**** Functions ****/

void MFTexture_InitModulePlatformSpecific()
{
}

void MFTexture_DeinitModulePlatformSpecific()
{
}

void MFTexture_Release()
{
	MFResourceIterator *pI = MFResource_EnumerateFirst(MFRT_Texture);

	while(pI)
	{
		MFTexture *pTex = (MFTexture*)MFResource_Get(pI);
		MFTextureTemplateData *pTemplate = pTex->pTemplateData;

		if(pTemplate->flags & TEX_RenderTarget)
		{
			// release the old render target
			IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)pTemplate->pSurfaces[0].pImageData;
			pSurface->Release();

			IDirect3DTexture9 *pD3DTex = (IDirect3DTexture9*)pTex->pInternalData;
			pD3DTex->Release();

			pTex->pInternalData = NULL;
		}

		pI = MFResource_EnumerateNext(pI, MFRT_Texture);
	}
}

void MFTexture_Recreate()
{
	MFResourceIterator *pI = MFResource_EnumerateFirst(MFRT_Texture);

	while(pI)
	{
		MFTexture *pTex = (MFTexture*)MFResource_Get(pI);
		MFTextureTemplateData *pTemplate = pTex->pTemplateData;

		if(pTemplate->flags & TEX_RenderTarget)
		{
			// recreate a new one
			IDirect3DTexture9 *pD3DTex;
			pd3dDevice->CreateTexture(pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, 1, D3DUSAGE_RENDERTARGET, gD3D9Format[pTemplate->imageFormat], D3DPOOL_DEFAULT, &pD3DTex, NULL);
			pTex->pInternalData = pD3DTex;

			IDirect3DSurface9 *pSurface;
			pD3DTex->GetSurfaceLevel(0, &pSurface);
			pTemplate->pSurfaces[0].pImageData = (char*)pSurface;

			MFRenderer_D3D9_SetDebugName((IDirect3DTexture9*)pTex->pInternalData, pTex->pName);
		}

		pI = MFResource_EnumerateNext(pI, MFRT_Texture);
	}
}

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFCALLSTACK;

	HRESULT hr;
	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	// create texture
	pTemplate->imageFormat = MFImage_ResolveFormat(pTemplate->imageFormat, MFRD_D3D9);
	MFDebug_Assert(pTemplate->imageFormat != ImgFmt_Unknown, "Invalid texture format!");

	D3DFORMAT platformFormat = gD3D9Format[pTemplate->imageFormat];
	hr = D3DXCreateTexture(pd3dDevice, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, platformFormat, D3DPOOL_MANAGED, (IDirect3DTexture9**)&pTexture->pInternalData);

	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	MFDebug_Assert(hr != D3DXERR_INVALIDDATA, MFStr("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	MFDebug_Assert(hr == D3D_OK, MFStr("Failed to create texture '%s'.", pTexture->pName));

	IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;
	MFRenderer_D3D9_SetDebugName(pTex, pTexture->pName);

	// copy image data
	D3DLOCKED_RECT rect;
	pTex->LockRect(0, &rect, NULL, 0);
	MFCopyMemory(rect.pBits, pTemplate->pSurfaces[0].pImageData, pTemplate->pSurfaces[0].bufferLength);
	pTex->UnlockRect(0);

	// filter mip levels
	if(generateMipChain)
		D3DXFilterTexture(pTex, NULL, 0, D3DX_DEFAULT);
}

MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFImageFormat targetFormat)
{
	MFTexture *pTexture = MFTexture_Find(pName);

	if(!pTexture)
	{
		targetFormat = MFImage_ResolveFormat(targetFormat, MFRD_D3D9);
		MFDebug_Assert(targetFormat != ImgFmt_Unknown, "Invalid texture format!");

		D3DFORMAT platformFormat = gD3D9Format[targetFormat];

		IDirect3DTexture9 *pD3DTex = NULL;
		IDirect3DSurface9 *pD3DSurface;
		if(targetFormat >= ImgFmt_D16 && targetFormat <= ImgFmt_D32FS8X24)
		{
			HRESULT hr = pd3dDevice->CreateDepthStencilSurface(width, height, platformFormat, D3DMULTISAMPLE_NONE, 0, FALSE, &pD3DSurface, NULL);
			if(hr != D3D_OK)
				return NULL;

			MFRenderer_D3D9_SetDebugName(pD3DSurface, pName);
		}
		else
		{
			HRESULT hr = pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, platformFormat, D3DPOOL_DEFAULT, &pD3DTex, NULL);
			if(hr != D3D_OK)
				return NULL;

			pD3DTex->GetSurfaceLevel(0, &pD3DSurface);

			MFRenderer_D3D9_SetDebugName(pD3DTex, pName);
		}

		int nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pTexture = (MFTexture*)MFHeap_Alloc(sizeof(MFTexture) + nameLen);

		if(pName)
			pName = MFString_Copy((char*)&pTexture[1], pName);

		MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

		pTexture->pTemplateData = (MFTextureTemplateData*)MFHeap_AllocAndZero(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)&pTexture->pTemplateData[1];
		pTexture->pTemplateData->magicNumber = MFMAKEFOURCC('F','T','E','X');
		pTexture->pTemplateData->imageFormat = targetFormat;
		pTexture->pTemplateData->mipLevels = 1;
		pTexture->pTemplateData->flags = TEX_RenderTarget;

		pTexture->pTemplateData->pSurfaces[0].width = width;
		pTexture->pTemplateData->pSurfaces[0].height = height;
		pTexture->pTemplateData->pSurfaces[0].bitsPerPixel = MFImage_GetBitsPerPixel(pTexture->pTemplateData->imageFormat);
		pTexture->pTemplateData->pSurfaces[0].xBlocks = width;
		pTexture->pTemplateData->pSurfaces[0].yBlocks = height;
		pTexture->pTemplateData->pSurfaces[0].bitsPerBlock = pTexture->pTemplateData->pSurfaces[0].bitsPerPixel;
		pTexture->pTemplateData->pSurfaces[0].pImageData = NULL;
		pTexture->pTemplateData->pSurfaces[0].bufferLength = 0;
		pTexture->pTemplateData->pSurfaces[0].pPaletteEntries = NULL;
		pTexture->pTemplateData->pSurfaces[0].paletteBufferLength = 0;

		pTexture->pInternalData = pD3DTex;
		pTexture->pTemplateData->pSurfaces[0].pImageData = (char*)pD3DSurface;
	}

	return pTexture;
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	if(pTexture->pTemplateData->flags & TEX_RenderTarget)
	{
		IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)pTexture->pTemplateData->pSurfaces[0].pImageData;
		pSurface->Release();
	}

	if(pTexture->pInternalData)
	{
		IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;
		pTex->Release();
	}
}

#endif // MF_RENDERER
