#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFTexture_InitModulePlatformSpecific MFTexture_InitModulePlatformSpecific_D3D11
	#define MFTexture_DeinitModulePlatformSpecific MFTexture_DeinitModulePlatformSpecific_D3D11
	#define MFTexture_Release MFTexture_Release_D3D11
	#define MFTexture_Recreate MFTexture_Recreate_D3D11
	#define MFTexture_CreatePlatformSpecific MFTexture_CreatePlatformSpecific_D3D11
	#define MFTexture_CreateRenderTarget MFTexture_CreateRenderTarget_D3D11
	#define MFTexture_Destroy MFTexture_Destroy_D3D11
#endif

/**** Defines ****/

/**** Includes ****/

#include "MFTexture_Internal.h"
#include "MFDebug.h"
#include "MFHeap.h"
#include "MFPtrList.h"

#include <D3D11.h>
#include <D3DX11.h>

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;

/**** Functions ****/

void MFTexture_InitModulePlatformSpecific()
{
}

void MFTexture_DeinitModulePlatformSpecific()
{
}

void MFTexture_Release()
{
	//MFTexture **ppTI = gTextureBank.Begin();
	//while (*ppTI)
	//{
	//	MFTexture *pTexture = *ppTI;
	//	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	//	if (pTemplate->flags & TEX_RenderTarget)
	//	{
	//		ID3D11Texture2D *pD3DTex = (ID3D11Texture2D**)&pTexture->pInternalData;

	//		// release the old render target
	//		pD3DTex->Release();

	//		pTexture->pInternalData = NULL;
	//	}

	//	++ppTI;
	//}
}

void MFTexture_Recreate()
{

}

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFCALLSTACK;

	HRESULT hr;
	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	// create texture
	DXGI_FORMAT platformFormat = (DXGI_FORMAT)MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFDD_D3D11);
	//hr = D3DX11CreateTextureFromMemory(pd3dDevice, pTemplate->pSurfaces[0].width, pTemplate->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, platformFormat, D3DPOOL_MANAGED, (IDirect3DTexture9**)&pTexture->pInternalData);

	int pitch = MFTexture_GetBitsPerPixel(pTemplate->imageFormat);

	D3D11_TEXTURE2D_DESC desc;
	MFZeroMemory(&desc, sizeof(desc));
	desc.Width = pTemplate->pSurfaces[0].width;
	desc.Height = pTemplate->pSurfaces[0].height;
	desc.MipLevels = 1; //generateMipChain ? 0 : 1;
	desc.ArraySize = 1;
	desc.Format = platformFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT; // = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = generateMipChain ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	D3D11_SUBRESOURCE_DATA data[16];
	MFZeroMemory(&data, sizeof(data));
	data[0].pSysMem = pTemplate->pSurfaces[0].pImageData;
	data[0].SysMemPitch = pitch;

	hr = g_pd3dDevice->CreateTexture2D(&desc, data, (ID3D11Texture2D**)&pTexture->pInternalData);

	//MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	//MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	//MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	//MFDebug_Assert(hr != D3DXERR_INVALIDDATA, MFStr("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	ID3D11Texture2D* pTex = (ID3D11Texture2D*)pTexture->pInternalData;

	MFDebug_Assert(hr == S_OK, MFStr("Failed to create texture '%s'.", pTexture->name));

	if (pTex)
	{
		hr = pTex->SetPrivateData( WKPDID_D3DDebugObjectName, MFString_Length(pTexture->name), pTexture->name );
	}
	
	//// filter mip levels
	//if (generateMipChain)
	//	D3DX11FilterTexture(NULL, pTex, 0, D3DX11_FILTER_BOX);
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFTextureFormat targetFormat)
{
	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	MFCALLSTACK;

	--pTexture->refCount;

	// if no references left, destroy texture
	if (!pTexture->refCount)
	{
		MFHeap_Free(pTexture->pTemplateData);

		ID3D11Texture2D *pTex = (ID3D11Texture2D*)pTexture->pInternalData;
		pTex->Release();

		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}

#endif // MF_RENDERER
