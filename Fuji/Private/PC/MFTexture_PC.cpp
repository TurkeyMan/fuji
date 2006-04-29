/**** Defines ****/

/**** Includes ****/

#include "Fuji.h"
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
	hr = D3DXCreateTexture(pd3dDevice, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, (D3DFORMAT)pTemplate->platformFormat, D3DPOOL_MANAGED, &pTexture->pTexture);

	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	MFDebug_Assert(hr != D3DXERR_INVALIDDATA, MFStr("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	MFDebug_Assert(hr == D3D_OK, MFStr("Failed to create texture '%s'.", pTexture->name));

	// copy image data
	D3DLOCKED_RECT rect;
	pTexture->pTexture->LockRect(0, &rect, NULL, 0);
	memcpy(rect.pBits, pTemplate->pSurfaces[0].pImageData, pTemplate->pSurfaces[0].bufferLength);
	pTexture->pTexture->UnlockRect(0);

	// filter mip levels
	D3DXFilterTexture(pTexture->pTexture, NULL, 0, D3DX_DEFAULT);
}

MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormat format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		D3DSURFACE_DESC imageDesc;
		HRESULT hr;
		D3DFORMAT fmt = D3DFMT_UNKNOWN;

		switch(format)
		{
			case TexFmt_Unknown:
				MFDebug_Assert(false, "Invalid Texture format: 'TEXF_Unknown'");
				break;
			case TexFmt_A8R8G8B8:
				fmt = D3DFMT_A8R8G8B8;
				break;
			default:
				MFDebug_Assert(false, "Texture format not yet supported..");
		}

		hr = pd3dDevice->CreateTexture(width, height, generateMipChain ? 0 : 1, 0, fmt, D3DPOOL_MANAGED, &pTexture->pTexture, NULL);

		MFDebug_Assert(SUCCEEDED(hr), MFStr("CreateTexture failed: hr = 0x%08X", hr));
		if(FAILED(hr))
		{
			MFDebug_Warn(2, "Couldnt Create Texture");
			return NULL;
		}

		D3DLOCKED_RECT rect;

		pTexture->pTexture->LockRect(0, &rect, NULL, 0);

		switch(format)
		{
			case TexFmt_A8R8G8B8:
			{
				if(flags & TEX_VerticalMirror)
				{
					(char*&)pData += width*height*sizeof(uint32);

					for(int a=0; a<(int)height; a++)
					{
						(char*&)pData -= width*sizeof(uint32);
						memcpy(rect.pBits, pData, width*sizeof(uint32));
						(char*&)rect.pBits += width*sizeof(uint32);
					}
				}
				else
				{
					memcpy(rect.pBits, pData, width*height*sizeof(uint32));
				}
				break;
			}
		}

		pTexture->pTexture->UnlockRect(0);

		// generate mipmaps
		if(generateMipChain)
		{
			D3DXFilterTexture(pTexture->pTexture, NULL, 0, D3DX_DEFAULT);
		}

		MFString_Copy(pTexture->name, pName);

		// create template data
		uint32 levelCount = pTexture->pTexture->GetLevelCount();

		char *pTemplate;
		pTemplate = (char*)MFHeap_Alloc(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount);

		pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

		pTexture->pTexture->GetLevelDesc(0, &imageDesc);
		pTexture->pTemplateData->imageFormat = format;
		pTexture->pTemplateData->platformFormat = imageDesc.Format;

		pTexture->pTemplateData->mipLevels = levelCount;

		for(int a=0; a<(int)levelCount; a++)
		{
			pTexture->pTexture->GetLevelDesc(a, &imageDesc);

			pTexture->pTemplateData->pSurfaces[a].width = imageDesc.Width;
			pTexture->pTemplateData->pSurfaces[a].height = imageDesc.Height;
		}
	}

	pTexture->refCount++;

	return pTexture;
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

	pTexture->refCount--;

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
