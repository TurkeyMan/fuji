/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Texture.h"
#include "Display.h"

/**** Globals ****/

/**** Functions ****/

Texture* Texture_CreateFromRawData(void *pData, uint32 width, uint32 height, uint32 format, uint32 flags, uint32 *pPalette)
{
	HRESULT hr;
	D3DFORMAT fmt;

	Texture *pTexture = gTextureBank.Create((Texture*)Heap_Alloc(sizeof(Texture)));
	pTexture->refCount = 0;

	switch(format)
	{
		case TEXF_Unknown:
			DBGASSERT(false, "Invalid Texture format: 'TEXF_Unknown'");
			break;

		case TEXF_A8R8G8B8:
			fmt = D3DFMT_A8R8G8B8;
			break;

		default:
			DBGASSERT(false, "Texture format not yet supported..");
	}

	hr = pd3dDevice->CreateTexture(width, height, 1, 0, fmt, D3DPOOL_MANAGED, &pTexture->texture, NULL);

	DBGASSERT(SUCCEEDED(hr), STR("CreateTexture failed: hr = 0x%08X", hr));

	if(FAILED(hr))
	{
		LOGD("Couldnt Create Texture");
		return NULL;
	}

	D3DLOCKED_RECT rect;

	pTexture->texture->LockRect(0, &rect, NULL, 0);

	switch(format)
	{
		case TEXF_Unknown:
			DBGASSERT(false, "Invalid Texture format: 'TEXF_Unknown'");
			break;

		case TEXF_A8R8G8B8:
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

		default:
			DBGASSERT(false, "Texture format not yet supported..");
	}

	pTexture->texture->UnlockRect(0);

	static int createdCount = 0;

	strcpy(pTexture->name, STR("CreateFromRawData%d", createdCount++));

	pTexture->width = width;
	pTexture->height = height;
	pTexture->format = fmt;

	pTexture->refCount++;

	return pTexture;
}

Texture* Texture_CreateBlank(const Vector4 &colour, uint32 width, uint32 height, uint32 format)
{
	HRESULT hr;
	D3DFORMAT fmt;

	Texture *pTexture = gTextureBank.Create((Texture*)Heap_Alloc(sizeof(Texture)));
	pTexture->refCount = 0;

	switch(format)
	{
		case TEXF_Unknown:
			DBGASSERT(false, "Invalid Texture format: 'TEXF_Unknown'");
			break;

		case TEXF_A8R8G8B8:
			fmt = D3DFMT_A8R8G8B8;
			break;

		default:
			DBGASSERT(false, "Texture format not yet supported..");
	}

	hr = pd3dDevice->CreateTexture(width, height, 1, 0, fmt, D3DPOOL_DEFAULT, &pTexture->texture, NULL);

	DBGASSERT(SUCCEEDED(hr), STR("CreateTexture failed: hr = 0x%08X", hr));

	if(FAILED(hr))
	{
		LOGD("Couldnt Create Texture");
		return NULL;
	}

	D3DLOCKED_RECT rect;

	pTexture->texture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);

	switch(format)
	{
		case TEXF_Unknown:
			DBGASSERT(false, "Invalid Texture format: 'TEXF_Unknown'");
			break;

		case TEXF_A8R8G8B8:
		{
			memset(rect.pBits, colour.ToARGB(), width*height*sizeof(uint32));
			break;
		}

		default:
			DBGASSERT(false, "Texture format not yet supported..");
	}

	pTexture->texture->UnlockRect(0);

	static int blankCount = 0;

	strcpy(pTexture->name, STR("CreateBlank%d", blankCount++));

	pTexture->width = width;
	pTexture->height = height;
	pTexture->format = fmt;

	pTexture->refCount++;

	return pTexture;
}

Texture* Texture::LoadTexture(const char *filename, bool generateMipChain)
{
	Texture *pTexture = FindTexture(filename);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create((Texture*)Heap_Alloc(sizeof(Texture)));
		pTexture->refCount = 0;
	}

	D3DSURFACE_DESC imageDesc;
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
  
	pd3dDevice->SetSamplerState(texUnit, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(texUnit, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(texUnit, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

void Texture::UseNone(int texUnit)
{
	pd3dDevice->SetTexture(texUnit, NULL);
}

