#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFTexture_InitModulePlatformSpecific MFTexture_InitModulePlatformSpecific_D3D9
	#define MFTexture_DeinitModulePlatformSpecific MFTexture_DeinitModulePlatformSpecific_D3D9
	#define MFTexture_Release MFTexture_Release_D3D9
	#define MFTexture_Recreate MFTexture_Recreate_D3D9
	#define MFTexture_CreatePlatformSpecific MFTexture_CreatePlatformSpecific_D3D9
	#define MFTexture_CreateRenderTarget MFTexture_CreateRenderTarget_D3D9
	#define MFTexture_DestroyPlatformSpecific MFTexture_DestroyPlatformSpecific_D3D9
	#define MFTexture_Update MFTexture_Update_D3D9
	#define MFTexture_Map MFTexture_Map_D3D9
	#define MFTexture_Unmap MFTexture_Unmap_D3D9
#endif


/**** Defines ****/

/**** Includes ****/

#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFRenderer_D3D9.h"


/**** Globals ****/

extern MFTexture *pNoneTexture;

extern IDirect3DDevice9 *pd3dDevice;

D3DFORMAT gD3D9Format[ImgFmt_Max] =
{
	D3DFMT_A8R8G8B8,		// ImgFmt_A8R8G8B8
	D3DFMT_A8B8G8R8,		// ImgFmt_A8B8G8R8
	D3DFMT_UNKNOWN,			// ImgFmt_B8G8R8A8
	D3DFMT_UNKNOWN,			// ImgFmt_R8G8B8A8

	D3DFMT_R8G8B8,			// ImgFmt_R8G8B8
	D3DFMT_UNKNOWN,			// ImgFmt_B8G8R8

	D3DFMT_UNKNOWN,			// ImgFmt_G8R8

	D3DFMT_L8,				// ImgFmt_L8
	D3DFMT_A8,				// ImgFmt_A8
	D3DFMT_A8L8,			// ImgFmt_A8L8

	D3DFMT_R5G6B5,			// ImgFmt_R5G6B5
	D3DFMT_UNKNOWN,			// ImgFmt_R6G5B5
	D3DFMT_UNKNOWN,			// ImgFmt_B5G6R5

	D3DFMT_A1R5G5B5,		// ImgFmt_A1R5G5B5
	D3DFMT_UNKNOWN,			// ImgFmt_R5G5B5A1
	D3DFMT_UNKNOWN,			// ImgFmt_A1B5G5R5

	D3DFMT_A4R4G4B4,		// ImgFmt_A4R4G4B4
	D3DFMT_UNKNOWN,			// ImgFmt_A4B4G4R4
	D3DFMT_UNKNOWN,			// ImgFmt_R4G4B4A4

	D3DFMT_A16B16G16R16,	// ImgFmt_A16B16G16R16

	D3DFMT_G16R16,			// ImgFmt_G16R16

	D3DFMT_L16,				// ImgFmt_L16
	D3DFMT_UNKNOWN,			// ImgFmt_A16
	D3DFMT_UNKNOWN,			// ImgFmt_A16L16

	D3DFMT_A2R10G10B10,		// ImgFmt_A2R10G10B10
	D3DFMT_A2B10G10R10,		// ImgFmt_A2B10G10R10

	D3DFMT_UNKNOWN,			// ImgFmt_R10G11B11
	D3DFMT_UNKNOWN,			// ImgFmt_R11G11B10

	D3DFMT_A16B16G16R16F,	// ImgFmt_ABGR_F16
	D3DFMT_A32B32G32R32F,	// ImgFmt_ABGR_F32
	D3DFMT_G16R16F,			// ImgFmt_GR_F16
	D3DFMT_G32R32F,			// ImgFmt_GR_F32
	D3DFMT_R16F,			// ImgFmt_R_F16
	D3DFMT_R32F,			// ImgFmt_R_F32

	D3DFMT_UNKNOWN,			// ImgFmt_R11G11B10_F
	D3DFMT_UNKNOWN,			// ImgFmt_R9G9B9_E5

	D3DFMT_P8,				// ImgFmt_P8
	D3DFMT_UNKNOWN,			// ImgFmt_P4

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
	D3DFMT_UNKNOWN,			// ImgFmt_ATI1
	D3DFMT_UNKNOWN,			// ImgFmt_ATI2
	D3DFMT_UNKNOWN,			// ImgFmt_BPTC_F
	D3DFMT_UNKNOWN,			// ImgFmt_BPTC
	D3DFMT_UNKNOWN,			// ImgFmt_CTX1
	D3DFMT_UNKNOWN,			// ImgFmt_ETC1
	D3DFMT_UNKNOWN,			// ImgFmt_ETC2
	D3DFMT_UNKNOWN,			// ImgFmt_EAC
	D3DFMT_UNKNOWN,			// ImgFmt_ETC2_EAC
	D3DFMT_UNKNOWN,			// ImgFmt_EACx2
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGB_2bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGB_4bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGBA_2bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGBA_4bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC2_2bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC2_4bpp
	D3DFMT_UNKNOWN,			// ImgFmt_ATCRGB
	D3DFMT_UNKNOWN,			// ImgFmt_ATCRGBA_EXPLICIT
	D3DFMT_UNKNOWN,			// ImgFmt_ATCRGBA
	D3DFMT_UNKNOWN,			// ImgFmt_ASTC

	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT1
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT3
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT5
};

static D3DFORMAT gD3D9FormatSigned[ImgFmt_Max] =
{
	D3DFMT_UNKNOWN,			// ImgFmt_A8R8G8B8
	D3DFMT_Q8W8V8U8,		// ImgFmt_A8B8G8R8
	D3DFMT_UNKNOWN,			// ImgFmt_B8G8R8A8
	D3DFMT_UNKNOWN,			// ImgFmt_R8G8B8A8

	D3DFMT_UNKNOWN,			// ImgFmt_R8G8B8
	D3DFMT_UNKNOWN,			// ImgFmt_B8G8R8

	D3DFMT_V8U8,			// ImgFmt_G8R8

	D3DFMT_UNKNOWN,			// ImgFmt_L8
	D3DFMT_UNKNOWN,			// ImgFmt_A8
	D3DFMT_UNKNOWN,			// ImgFmt_A8L8

	D3DFMT_UNKNOWN,			// ImgFmt_R5G6B5
	D3DFMT_UNKNOWN,			// ImgFmt_R6G5B5
	D3DFMT_UNKNOWN,			// ImgFmt_B5G6R5

	D3DFMT_UNKNOWN,			// ImgFmt_A1R5G5B5
	D3DFMT_UNKNOWN,			// ImgFmt_R5G5B5A1
	D3DFMT_UNKNOWN,			// ImgFmt_A1B5G5R5

	D3DFMT_UNKNOWN,			// ImgFmt_A4R4G4B4
	D3DFMT_UNKNOWN,			// ImgFmt_A4B4G4R4
	D3DFMT_UNKNOWN,			// ImgFmt_R4G4B4A4

	D3DFMT_Q16W16V16U16,	// ImgFmt_A16B16G16R16

	D3DFMT_V16U16,			// ImgFmt_G16R16

	D3DFMT_UNKNOWN,			// ImgFmt_L16
	D3DFMT_UNKNOWN,			// ImgFmt_A16
	D3DFMT_UNKNOWN,			// ImgFmt_A16L16

	D3DFMT_UNKNOWN,			// ImgFmt_A2R10G10B10
	D3DFMT_UNKNOWN,			// ImgFmt_A2B10G10R10

	D3DFMT_UNKNOWN,			// ImgFmt_R10G11B11
	D3DFMT_UNKNOWN,			// ImgFmt_R11G11B10

	D3DFMT_UNKNOWN,			// ImgFmt_ABGR_F16
	D3DFMT_UNKNOWN,			// ImgFmt_ABGR_F32
	D3DFMT_UNKNOWN,			// ImgFmt_GR_F16
	D3DFMT_UNKNOWN,			// ImgFmt_GR_F32
	D3DFMT_UNKNOWN,			// ImgFmt_R_F16
	D3DFMT_UNKNOWN,			// ImgFmt_R_F32

	D3DFMT_UNKNOWN,			// ImgFmt_R11G11B10_F
	D3DFMT_UNKNOWN,			// ImgFmt_R9G9B9_E5

	D3DFMT_UNKNOWN,			// ImgFmt_P8
	D3DFMT_UNKNOWN,			// ImgFmt_P4

	D3DFMT_UNKNOWN,			// ImgFmt_D16
	D3DFMT_UNKNOWN,			// ImgFmt_D24X8
	D3DFMT_UNKNOWN,			// ImgFmt_D32
	D3DFMT_UNKNOWN,			// ImgFmt_D32F

	D3DFMT_UNKNOWN,			// ImgFmt_D15S1
	D3DFMT_UNKNOWN,			// ImgFmt_D24S8
	D3DFMT_UNKNOWN,			// ImgFmt_D24FS8
	D3DFMT_UNKNOWN,			// ImgFmt_D32FS8X24

	D3DFMT_UNKNOWN,			// ImgFmt_DXT1
	D3DFMT_UNKNOWN,			// ImgFmt_DXT2
	D3DFMT_UNKNOWN,			// ImgFmt_DXT3
	D3DFMT_UNKNOWN,			// ImgFmt_DXT4
	D3DFMT_UNKNOWN,			// ImgFmt_DXT5
	D3DFMT_UNKNOWN,			// ImgFmt_ATI1
	D3DFMT_UNKNOWN,			// ImgFmt_ATI2
	D3DFMT_UNKNOWN,			// ImgFmt_BPTC_F
	D3DFMT_UNKNOWN,			// ImgFmt_BPTC
	D3DFMT_UNKNOWN,			// ImgFmt_CTX1
	D3DFMT_UNKNOWN,			// ImgFmt_ETC1
	D3DFMT_UNKNOWN,			// ImgFmt_ETC2
	D3DFMT_UNKNOWN,			// ImgFmt_EAC
	D3DFMT_UNKNOWN,			// ImgFmt_ETC2_EAC
	D3DFMT_UNKNOWN,			// ImgFmt_EACx2
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGB_2bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGB_4bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGBA_2bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC_RGBA_4bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC2_2bpp
	D3DFMT_UNKNOWN,			// ImgFmt_PVRTC2_4bpp
	D3DFMT_UNKNOWN,			// ImgFmt_ATCRGB
	D3DFMT_UNKNOWN,			// ImgFmt_ATCRGBA_EXPLICIT
	D3DFMT_UNKNOWN,			// ImgFmt_ATCRGBA
	D3DFMT_UNKNOWN,			// ImgFmt_ASTC

	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT1
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT3
	D3DFMT_UNKNOWN,			// ImgFmt_PSP_DXT5
};

static D3DCUBEMAP_FACES gD3DCubeFaces[6] =
{
  D3DCUBEMAP_FACE_POSITIVE_X,
  D3DCUBEMAP_FACE_NEGATIVE_X,
  D3DCUBEMAP_FACE_POSITIVE_Y,
  D3DCUBEMAP_FACE_NEGATIVE_Y,
  D3DCUBEMAP_FACE_POSITIVE_Z,
  D3DCUBEMAP_FACE_NEGATIVE_Z
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
		MFTexture *pTexture = (MFTexture*)MFResource_Get(pI);

		if(pTexture->flags & TEX_RenderTarget)
		{
			// release the old render target
			IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)(size_t)pTexture->pSurfaces[0].platformData;
			pSurface->Release();

			IDirect3DTexture9 *pD3DTex = (IDirect3DTexture9*)pTexture->pInternalData;
			pD3DTex->Release();

			pTexture->pInternalData = NULL;
		}

		pI = MFResource_EnumerateNext(pI, MFRT_Texture);
	}
}

void MFTexture_Recreate()
{
	MFResourceIterator *pI = MFResource_EnumerateFirst(MFRT_Texture);

	while(pI)
	{
		MFTexture *pTexture = (MFTexture*)MFResource_Get(pI);

		if(pTexture->flags & TEX_RenderTarget)
		{
			// recreate a new one
			IDirect3DTexture9 *pD3DTex;
			D3DFORMAT platformFormat = (pTexture->imageFormat & ImgFmt_Signed) ? gD3D9FormatSigned[pTexture->imageFormat] : gD3D9Format[pTexture->imageFormat];
			pd3dDevice->CreateTexture(pTexture->pSurfaces[0].width, pTexture->pSurfaces[0].height, 1, D3DUSAGE_RENDERTARGET, platformFormat, D3DPOOL_DEFAULT, &pD3DTex, NULL);
			pTexture->pInternalData = pD3DTex;

			IDirect3DSurface9 *pSurface;
			pD3DTex->GetSurfaceLevel(0, &pSurface);
			pTexture->pSurfaces[0].platformData = (uint64)(size_t)pSurface;

			MFRenderer_D3D9_SetDebugName((IDirect3DTexture9*)pTexture->pInternalData, pTexture->pName);
		}

		pI = MFResource_EnumerateNext(pI, MFRT_Texture);
	}
}

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	HRESULT hr = D3DERR_INVALIDCALL;

	// create texture
	D3DFORMAT platformFormat = (pTexture->imageFormat & ImgFmt_Signed) ? gD3D9FormatSigned[pTexture->imageFormat] : gD3D9Format[pTexture->imageFormat];

	bool bAutoGenMips = pTexture->numMips == 1 ? !!(pTexture->createFlags & MFTCF_GenerateMips) : false;
	UINT numMips = pTexture->numMips > 1 ? pTexture->numMips : (bAutoGenMips ? 0 : 1);

	DWORD usage = 0;
	D3DPOOL pool = D3DPOOL_DEFAULT;
	switch(pTexture->createFlags & MFTCF_TypeMask)
	{
		case MFTCF_Static:
			pool = D3DPOOL_MANAGED;
			usage = numMips == 0 ? D3DUSAGE_AUTOGENMIPMAP : 0;
			break;
		case MFTCF_Dynamic:
			usage = D3DUSAGE_DYNAMIC | (numMips == 0 ? D3DUSAGE_AUTOGENMIPMAP : 0);
			break;
		case MFTCF_Scratch:
			usage = D3DUSAGE_DYNAMIC; // DYNAMIC is supposedly only for frequent locking (per frame?)... we don't intend to lock more than once per frame.
			break;
		case MFTCF_RenderTarget:
			usage = D3DUSAGE_RENDERTARGET;
			break;
		default:
			break;
	}

	switch(pTexture->type)
	{
		case MFTexType_1D:
		{
			hr = pd3dDevice->CreateTexture(pTexture->width, 1, numMips, usage, platformFormat, pool, (IDirect3DTexture9**)&pTexture->pInternalData, NULL);
			break;
		}
		case MFTexType_2D:
		{
			if((pTexture->createFlags & MFTCF_TypeMask) == MFTCF_RenderTarget)
			{
				IDirect3DSurface9 *pD3DSurface = NULL;
				if(pTexture->imageFormat >= ImgFmt_D16 && pTexture->imageFormat <= ImgFmt_D32FS8X24)
				{
					hr = pd3dDevice->CreateDepthStencilSurface(pTexture->width, pTexture->height, platformFormat, D3DMULTISAMPLE_NONE, 0, FALSE, &pD3DSurface, NULL);
					if(hr == D3D_OK)
						MFRenderer_D3D9_SetDebugName(pD3DSurface, pTexture->pName);
				}
				else
				{
					hr = pd3dDevice->CreateTexture(pTexture->width, pTexture->height, 1, usage, platformFormat, pool, (IDirect3DTexture9**)&pTexture->pInternalData, NULL);
					if(hr == D3D_OK)
					{
						IDirect3DTexture9 *pD3DTex = (IDirect3DTexture9*)pTexture->pInternalData;
						pD3DTex->GetSurfaceLevel(0, &pD3DSurface);
					}
				}
				pTexture->pSurfaces[0].platformData = (uint64)(size_t)pD3DSurface;
			}
			else
			{
				hr = pd3dDevice->CreateTexture(pTexture->width, pTexture->height, numMips, usage, platformFormat, pool, (IDirect3DTexture9**)&pTexture->pInternalData, NULL);
			}
			break;
		}
		case MFTexType_3D:
		{
			hr = pd3dDevice->CreateVolumeTexture(pTexture->width, pTexture->height, pTexture->depth, numMips, usage, platformFormat, pool, (IDirect3DVolumeTexture9**)&pTexture->pInternalData, NULL);
			break;
		}
		case MFTexType_Cubemap:
		{
			hr = pd3dDevice->CreateCubeTexture(pTexture->width, numMips, usage, platformFormat, pool, (IDirect3DCubeTexture9**)&pTexture->pInternalData, NULL);
			break;
		}
	}

	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));

	MFDebug_Assert(hr == D3D_OK, MFStr("Failed to create texture '%s'.", pTexture->pName));

	if(hr == D3D_OK && pTexture->pInternalData)
		MFRenderer_D3D9_SetDebugName((IDirect3DResource9*)pTexture->pInternalData, pTexture->pName);

	// copy image data
	if(pTexture->pImageData)
	{
		if(!bAutoGenMips)
		{
			for(int l=0; l<pTexture->numMips; ++l)
			{
				for(int e=0; e<pTexture->numElements; ++e)
				{
					int s = l*pTexture->numElements + e;
					MFTexture_Update(pTexture, e, l, pTexture->pImageData + pTexture->pSurfaces[s].imageDataOffset);
				}
			}
		}
		else
		{
			// TODO: do something about auto-mip-generation...
			for(int e=0; e<pTexture->numElements; ++e)
				MFTexture_Update(pTexture, e, 0, pTexture->pImageData + pTexture->pSurfaces[e].imageDataOffset);
		}
	}
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	if(pTexture->flags & TEX_RenderTarget)
	{
		IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)(size_t)pTexture->pSurfaces[0].platformData;
		pSurface->Release();
	}

	if(pTexture->pInternalData)
	{
		IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;
		pTex->Release();
	}
}

MF_API bool MFTexture_Update(MFTexture *pTexture, int element, int mipLevel, const void *pData, size_t lineStride, size_t sliceStride)
{
	int numFaces = pTexture->type == MFTexType_Cubemap ? 6 : 1;
	MFDebug_Assert(element < numFaces, "Array textures not supported in D3D9!");

	int s = mipLevel*pTexture->numElements + (element > -1 ? element : 0);
	MFTextureSurface &surface = pTexture->pSurfaces[s];

	DWORD lockFlags = (pTexture->createFlags & MFTCF_TypeMask) == MFTCF_Scratch ? D3DLOCK_DISCARD : 0;

	size_t lineBytes = (surface.bitsPerPixel * surface.width) / 8;
	if(lineStride == 0)
		lineStride = lineBytes;
	if(sliceStride == 0)
		sliceStride = lineStride * surface.width;

	switch(pTexture->type)
	{
		case MFTexType_1D:
		{
			IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DSURFACE_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_RECT rect;
			pTex->LockRect(mipLevel, &rect, NULL, lockFlags);
			MFCopyMemory(rect.pBits, pData, lineBytes);
			pTex->UnlockRect(mipLevel);
			break;
		}
		case MFTexType_2D:
		{
			IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DSURFACE_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_RECT rect;
			pTex->LockRect(mipLevel, &rect, NULL, lockFlags);

			const char *pSrc = (const char*)pData;
			char *pDest = (char*)rect.pBits;
			for(int i=0; i<surface.height; ++i)
			{
				MFCopyMemory(pDest, pSrc, lineBytes);
				pDest += rect.Pitch;
				pSrc += lineStride;
			}

			pTex->UnlockRect(mipLevel);
			break;
		}
		case MFTexType_3D:
		{
			IDirect3DVolumeTexture9 *pTex = (IDirect3DVolumeTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DVOLUME_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height && (int)desc.Depth == surface.depth, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_BOX box;
			pTex->LockBox(mipLevel, &box, NULL, lockFlags);
			MFCopyMemory(box.pBits, pData, surface.bufferLength);

			const char *pSrcSlice = (const char*)pData;
			char *pDestSlice = (char*)box.pBits;
			for(int d=0; d<surface.depth; ++d)
			{
				const char *pSrcLine = pSrcSlice;
				char *pDestLine = pDestSlice;
				for(int i=0; i<surface.height; ++i)
				{
					MFCopyMemory(pDestLine, pSrcLine, lineBytes);
					pDestLine += box.RowPitch;
					pSrcLine += lineStride;
				}
				pSrcSlice += sliceStride;
				pDestSlice += box.SlicePitch;
			}

			pTex->UnlockBox(mipLevel);
			break;
		}
		case MFTexType_Cubemap:
		{
			MFDebug_Assert(element != -1, "TODO: must handle setting all surfaces at once...");

			IDirect3DCubeTexture9 *pTex = (IDirect3DCubeTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DSURFACE_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_RECT rect;
			pTex->LockRect(gD3DCubeFaces[element], mipLevel, &rect, NULL, lockFlags);

			const char *pSrc = (const char*)pData;
			char *pDest = (char*)rect.pBits;
			for(int i=0; i<surface.height; ++i)
			{
				MFCopyMemory(pDest, pSrc, lineBytes);
				pDest += rect.Pitch;
				pSrc += lineStride;
			}

			pTex->UnlockRect(gD3DCubeFaces[element], mipLevel);
			break;
		}
	}

	return true;
}

MF_API bool MFTexture_Map(MFTexture *pTexture, int element, int mipLevel, MFLockedTexture *pLock)
{
	int numFaces = pTexture->type == MFTexType_Cubemap ? 6 : 1;
	MFDebug_Assert(element < numFaces, "Array textures not supported in D3D9!");

	int s = mipLevel*pTexture->numElements + (element ? element : 0);
	MFTextureSurface &surface = pTexture->pSurfaces[s];

	DWORD lockFlags = (pTexture->createFlags & MFTCF_TypeMask) == MFTCF_Scratch ? D3DLOCK_DISCARD : 0;

	switch(pTexture->type)
	{
		case MFTexType_1D:
		{
			IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DSURFACE_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_RECT rect;
			pTex->LockRect(mipLevel, &rect, NULL, lockFlags);

			pLock->pData = rect.pBits;
			pLock->width = surface.width;
			pLock->height = surface.height;
			pLock->depth = surface.depth;
			pLock->strideInBytes = rect.Pitch;
			pLock->sliceInBytes = rect.Pitch * surface.height;
			break;
		}
		case MFTexType_2D:
		{
			IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DSURFACE_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_RECT rect;
			pTex->LockRect(mipLevel, &rect, NULL, lockFlags);

			pLock->pData = rect.pBits;
			pLock->width = surface.width;
			pLock->height = surface.height;
			pLock->depth = surface.depth;
			pLock->strideInBytes = rect.Pitch;
			pLock->sliceInBytes = rect.Pitch * surface.height;
			break;
		}
		case MFTexType_3D:
		{
			IDirect3DVolumeTexture9 *pTex = (IDirect3DVolumeTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DVOLUME_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height && (int)desc.Depth == surface.depth, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_BOX box;
			pTex->LockBox(mipLevel, &box, NULL, lockFlags);

			pLock->pData = box.pBits;
			pLock->width = surface.width;
			pLock->height = surface.height;
			pLock->depth = surface.depth;
			pLock->strideInBytes = box.RowPitch;
			pLock->sliceInBytes = box.SlicePitch;
			break;
		}
		case MFTexType_Cubemap:
		{
			IDirect3DCubeTexture9 *pTex = (IDirect3DCubeTexture9*)pTexture->pInternalData;

#if defined(MF_DEBUG)
			D3DSURFACE_DESC desc;
			pTex->GetLevelDesc(mipLevel, &desc);
			MFDebug_Assert((int)desc.Width == surface.width && (int)desc.Height == surface.height, "D3D9 mip dimensions don't match the texture template data...");
#endif

			D3DLOCKED_RECT rect;
			pTex->LockRect(gD3DCubeFaces[element], mipLevel, &rect, NULL, lockFlags);

			pLock->pData = rect.pBits;
			pLock->width = surface.width;
			pLock->height = surface.height;
			pLock->depth = surface.depth;
			pLock->strideInBytes = rect.Pitch;
			pLock->sliceInBytes = rect.Pitch * surface.height;
			break;
		}
	}

	return true;
}

MF_API void MFTexture_Unmap(MFTexture *pTexture, int element, int mipLevel)
{
	switch(pTexture->type)
	{
		case MFTexType_1D:
		case MFTexType_2D:
		{
			IDirect3DTexture9 *pTex = (IDirect3DTexture9*)pTexture->pInternalData;
			pTex->UnlockRect(mipLevel);
			break;
		}
		case MFTexType_3D:
		{
			IDirect3DVolumeTexture9 *pTex = (IDirect3DVolumeTexture9*)pTexture->pInternalData;
			pTex->UnlockBox(mipLevel);
			break;
		}
		case MFTexType_Cubemap:
		{
			IDirect3DCubeTexture9 *pTex = (IDirect3DCubeTexture9*)pTexture->pInternalData;
			pTex->UnlockRect(gD3DCubeFaces[element], mipLevel);
			break;
		}
	}
}

#endif // MF_RENDERER
