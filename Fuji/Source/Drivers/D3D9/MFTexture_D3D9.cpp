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

static D3DFORMAT gD3D9Format[] =
{
	D3DFMT_UNKNOWN,
	D3DFMT_A8R8G8B8,			// ImgFmt_A8R8G8B8
	D3DFMT_A8B8G8R8,			// ImgFmt_A8B8G8R8
	D3DFMT_R8G8B8,				// ImgFmt_R8G8B8
	D3DFMT_A2R10G10B10,			// ImgFmt_A2R10G10B10
	D3DFMT_A2B10G10R10,			// ImgFmt_A2B10G10R10
	D3DFMT_A16B16G16R16,		// ImgFmt_A16B16G16R16
	D3DFMT_R5G6B5,				// ImgFmt_R5G6B5
	D3DFMT_A1R5G5B5,			// ImgFmt_A1R5G5B5
	D3DFMT_A4R4G4B4,			// ImgFmt_A4R4G4B4
	D3DFMT_A16B16G16R16F,		// ImgFmt_ABGR_F16
	D3DFMT_A32B32G32R32F,		// ImgFmt_ABGR_F32
	D3DFMT_P8,					// ImgFmt_I8
	D3DFMT_D16,					// ImgFmt_D16
	D3DFMT_D15S1,				// ImgFmt_D15S1
	D3DFMT_D24X8,				// ImgFmt_D24X8
	D3DFMT_D24S8,				// ImgFmt_D24S8
	D3DFMT_D24FS8,				// ImgFmt_D24FS8
	D3DFMT_D32,					// ImgFmt_D32
	D3DFMT_D32F_LOCKABLE,		// ImgFmt_D32F
	(D3DFORMAT)MFMAKEFOURCC('D', 'X', 'T', '1'),	// D3DFMT_DXT1	// ImgFmt_DXT1
	(D3DFORMAT)MFMAKEFOURCC('D', 'X', 'T', '2'),	// D3DFMT_DXT2	// ImgFmt_DXT2
	(D3DFORMAT)MFMAKEFOURCC('D', 'X', 'T', '3'),	// D3DFMT_DXT3	// ImgFmt_DXT3
	(D3DFORMAT)MFMAKEFOURCC('D', 'X', 'T', '4'),	// D3DFMT_DXT4	// ImgFmt_DXT4
	(D3DFORMAT)MFMAKEFOURCC('D', 'X', 'T', '5')		// D3DFMT_DXT5	// ImgFmt_DXT5
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
			D3DFORMAT platformFormat = gD3D9Format[MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFRD_D3D9)];
			IDirect3DTexture9 *pD3DTex;
			pd3dDevice->CreateTexture(pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, 1, D3DUSAGE_RENDERTARGET, platformFormat, D3DPOOL_DEFAULT, &pD3DTex, NULL);
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
	D3DFORMAT platformFormat = gD3D9Format[MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFRD_D3D9)];
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

int8 gAutoFormats[0x40] =
{
	ImgFmt_A8R8G8B8, // ImgFmt_SelectDefault
	ImgFmt_A8R8G8B8, // ImgFmt_SelectNicest
	ImgFmt_A8R8G8B8, // ImgFmt_SelectFastest
	ImgFmt_ABGR_F16, // ImgFmt_SelectHDR
	ImgFmt_R8G8B8, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
	ImgFmt_A2R10G10B10, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
	ImgFmt_R8G8B8, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
	ImgFmt_ABGR_F16, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
	ImgFmt_A8R8G8B8, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
	ImgFmt_A2R10G10B10, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
	ImgFmt_A8R8G8B8, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
	ImgFmt_ABGR_F16, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
	-1,
	-1,
	-1,
	-1,
	ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
	ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
	ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
	ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
	ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
	ImgFmt_A2R10G10B10, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
	ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
	ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
	ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
	ImgFmt_A2R10G10B10, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
	ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
	ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
	-1,
	-1,
	-1,
	-1,
	ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
	ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
	ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
	ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
	ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
	ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
	ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
	ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
	ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
	ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
	ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
	ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
	-1,
	-1,
	-1,
	-1,
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
	ImgFmt_D24FS8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
	ImgFmt_D24FS8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
	ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
	ImgFmt_D15S1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
	ImgFmt_D24FS8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
	-1,
	-1,
	-1,
	-1
};

MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFImageFormat targetFormat)
{
	MFTexture *pTexture = MFTexture_Find(pName);

	if(!pTexture)
	{
		if(targetFormat & ImgFmt_SelectDefault)
		{
			targetFormat = (MFImageFormat)gAutoFormats[targetFormat & 0x3F];
			MFDebug_Assert(targetFormat != -1, "Invalid texture format!");
		}

		D3DFORMAT platformFormat = gD3D9Format[MFTexture_GetPlatformFormatID(targetFormat, MFRD_D3D9)];
		IDirect3DTexture9 *pD3DTex;
		HRESULT hr = pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, platformFormat, D3DPOOL_DEFAULT, &pD3DTex, NULL);
		if(hr != D3D_OK)
			return NULL;

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

		IDirect3DSurface9 *pSurface;
		pD3DTex->GetSurfaceLevel(0, &pSurface);
		pTexture->pTemplateData->pSurfaces[0].pImageData = (char*)pSurface;

		MFRenderer_D3D9_SetDebugName((IDirect3DTexture9*)pTexture->pInternalData, pTexture->pName);
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

	IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;
	pTex->Release();
}

#endif // MF_RENDERER
