#include "Common.h"
#include "Texture.h"
#include "Display.h"

int Texture::LoadTexture(char *filename, bool generateMipChain)
{
	D3DSURFACE_DESC fontdesc;
	HRESULT hr;

	hr = D3DXCreateTextureFromFileEx(pd3dDevice, filename, 0, 0, generateMipChain ? 0 : 1, 0, D3DFMT_UNKNOWN, 0, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
	if(hr != D3D_OK) return 1;

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

