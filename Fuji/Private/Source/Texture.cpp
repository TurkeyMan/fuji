#include "Common.h"
#include "Texture.h"
#include "Display.h"
#include "Input.h"
#include "Font.h"
#include "Primitive.h"

std::map<std::string, Texture> gTextureBank;
TextureBrowser texBrowser;

void Texture_InitModule()
{
	DebugMenu_AddItem("Texture Browser", "Fuji Options", &texBrowser);
}

void Texture_DeinitModule()
{

}

Texture::Texture()
{
	refCount = 0;
}

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


// texture browser
TextureBrowser::TextureBrowser()
{
	selection = 0;
	type = MenuType_TextureBrowser;
}

void TextureBrowser::Draw()
{

}

void TextureBrowser::Update()
{
	if(Input_WasPressed(0, Button_XB_Y))
		pCurrentMenu = pParent;
}

#define TEX_SIZE 64.0f
float TextureBrowser::ListDraw(bool selected, const Vector3 &_pos, float maxWidth)
{
	Vector3 pos = _pos;

	std::map<std::string, Texture>::iterator i;
	i = gTextureBank.begin();

	for(int a=0; a<selection; a++) i++;

	Texture *pTexture = &i->second;

	debugFont.DrawText(pos+Vector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s:", name));
	debugFont.DrawText(pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s", pTexture->name));
	debugFont.DrawText(pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("Dimensions: %dx%d Refs: %d", pTexture->width, pTexture->height, pTexture->refCount));

	pos += Vector(maxWidth - (TEX_SIZE + 4.0f + 5.0f), 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetPosition(pos);
	MFSetPosition(pos + Vector(TEX_SIZE + 4.0f, 0.0f, 0.0f));
	MFSetPosition(pos + Vector(0.0f, TEX_SIZE + 4.0f, 0.0f));
	MFSetPosition(pos + Vector(TEX_SIZE + 4.0f, TEX_SIZE + 4.0f, 0.0f));
	MFEnd();

	pos += Vector(2.0f, 2.0f, 0.0f);

	MFBegin(4);
	MFSetColour(0xFF000000);
	MFSetPosition(pos);
	MFSetPosition(pos + Vector(TEX_SIZE, 0.0f, 0.0f));
	MFSetPosition(pos + Vector(0.0f, TEX_SIZE, 0.0f));
	MFSetPosition(pos + Vector(TEX_SIZE, TEX_SIZE, 0.0f));
	MFEnd();

	float xaspect, yaspect;

	if(pTexture->width > pTexture->height)
	{
		xaspect = 0.5f;
		yaspect = ((float)pTexture->height/(float)pTexture->width) * 0.5f;
	}
	else
	{
		yaspect = 0.5f;
		xaspect = ((float)pTexture->width/(float)pTexture->height) * 0.5f;
	}

	pTexture->SetTexture();

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetTexCoord1(0.0f,0.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,0.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(0.0f,1.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,1.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFEnd();

	return TEX_SIZE + 8.0f;
}

void TextureBrowser::ListUpdate(bool selected)
{
	if(selected)
	{
		int texCount = gTextureBank.size();

		if(Input_WasPressed(0, Button_DLeft))
		{
			selection = selection <= 0 ? texCount-1 : selection-1;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(Input_WasPressed(0, Button_DRight))
		{
			selection = selection >= texCount-1 ? 0 : selection+1;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

Vector3 TextureBrowser::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, TEX_SIZE + 8.0f, 0.0f);
}


