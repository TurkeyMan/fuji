/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Texture.h"
#include "Display.h"

/**** Globals ****/

/**** Functions ****/

Texture* Texture::LoadTexture(const char *filename, bool generateMipChain)
{
	Texture *pTexture = &gTextureBank[filename];

	D3DSURFACE_DESC fontdesc;
	HRESULT hr;

	if(!pTexture->refCount)
	{
		hr = D3DXCreateTextureFromFileEx(pd3dDevice, filename, 0, 0, generateMipChain ? 0 : 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture->texture);

		DBGASSERT(hr != D3DERR_NOTAVAILABLE, STR("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
		DBGASSERT(hr != D3DERR_OUTOFVIDEOMEMORY, STR("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
		DBGASSERT(hr != D3DERR_INVALIDCALL, STR("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
		DBGASSERT(hr != D3DXERR_INVALIDDATA, STR("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

		if(hr != D3D_OK)
		{
			LOGD(STR("Failed loading texture: %s", filename));
			return NULL;
		}

		strcpy(pTexture->name, filename);

		pTexture->texture->GetLevelDesc(0, &fontdesc);

		pTexture->width = fontdesc.Width;
		pTexture->height = fontdesc.Height;
		pTexture->format = fontdesc.Format;
	}

	pTexture->refCount++;

	return pTexture;
}

void Texture::Release()
{
	refCount--;

	if(!refCount)
	{
		texture->Release();
	}
}

void Texture::SetTexture(int texUnit)
{
	pd3dDevice->SetTexture(texUnit, texture);

#if defined(_XBOX)
	pd3dDevice->SetTextureStageState(texUnit, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(texUnit, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(texUnit, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
#elif defined(_WINDOWS)
	pd3dDevice->SetSamplerState(texUnit, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(texUnit, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(texUnit, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
#endif
}

void Texture::UseNone(int texUnit)
{
	pd3dDevice->SetTexture(texUnit, NULL);
}

