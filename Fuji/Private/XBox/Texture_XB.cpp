/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Texture.h"
#include "Display.h"

/**** Globals ****/

/**** Functions ****/

Texture* Texture::LoadTexture(const char *filename, bool generateMipChain)
{
	char *pXFilename = FixXBoxFilename(filename);

	Texture *pTexture = FindTexture(pXFilename);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create((Texture*)Heap_Alloc(sizeof(Texture)));
		pTexture->refCount = 0;
	}

	D3DSURFACE_DESC imageDesc;
	HRESULT hr;

	if(!pTexture->refCount)
	{
		hr = D3DXCreateTextureFromFileEx(pd3dDevice, pXFilename, 0, 0, generateMipChain ? 0 : 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture->texture);

		DBGASSERT(hr != D3DERR_NOTAVAILABLE, STR("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
		DBGASSERT(hr != D3DERR_OUTOFVIDEOMEMORY, STR("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
		DBGASSERT(hr != D3DERR_INVALIDCALL, STR("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
		DBGASSERT(hr != D3DXERR_INVALIDDATA, STR("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

		if(hr != D3D_OK)
		{
			LOGD(STR("Failed loading texture: %s", pXFilename));
			return NULL;
		}

		strcpy(pTexture->name, pXFilename);

		pTexture->texture->GetLevelDesc(0, &imageDesc);

		pTexture->width = imageDesc.Width;
		pTexture->height = imageDesc.Height;
		pTexture->format = imageDesc.Format;
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
	DBGASSERT(this, "Texture Not Loaded.");

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

