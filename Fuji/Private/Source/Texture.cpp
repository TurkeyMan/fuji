#include "Common.h"
#include "Texture.h"
#include "Display.h"

int Texture::LoadTexture(char *filename, bool generateMipChain)
{
	D3DSURFACE_DESC fontdesc;
	HRESULT hr;

	hr = D3DXCreateTextureFromFileEx(pd3dDevice, filename, 0, 0, generateMipChain ? 0 : 1, 0, D3DFMT_UNKNOWN, 0, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);

	DBGASSERT(hr != D3DERR_NOTAVAILABLE, STR("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
	DBGASSERT(hr != D3DERR_OUTOFVIDEOMEMORY, STR("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
	DBGASSERT(hr != D3DERR_INVALIDCALL, STR("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
	DBGASSERT(hr != D3DXERR_INVALIDDATA, STR("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

	texture->GetLevelDesc(0, &fontdesc);

	width = fontdesc.Width;
	height = fontdesc.Height;
	format = fontdesc.Format;

	return 0;
}

void Texture::ReleaseTexture()
{
	texture->Release();
}

void Texture::SetTexture()
{
	pd3dDevice->SetTexture(0, texture);

	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
}

void Texture::UseNone()
{
	pd3dDevice->SetTexture(0, NULL);
}

