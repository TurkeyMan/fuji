/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Texture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"

/**** Globals ****/

extern PtrListDL<Texture> gTextureBank;
extern Texture *pNoneTexture;

/**** Functions ****/

// interface functions
Texture* Texture_Create(const char *pName, bool generateMipChain)
{
	Texture *pTexture = Texture_FindTexture(pName);

	if(!pTexture)
	{
		const char *pFileName = MFFile_SystemPath(STR("%s.tga", pName));

		MFOpenDataNative openData;
		openData.cbSize = sizeof(MFOpenDataNative);
		openData.openFlags = MFOF_Read|MFOF_Binary;
		openData.pFilename = pFileName;

		MFFileHandle hFile = MFFile_Open(hNativeFileSystem, &openData);

		if(!hFile)
		{
			LOGD(STR("Texture '%s' does not exist. Using '_None'.\n", pFileName));
			return Texture_Create("_None");
		}

		MFFile_Close(hFile);

		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		D3DSURFACE_DESC imageDesc;
		HRESULT hr;

		hr = D3DXCreateTextureFromFileEx(pd3dDevice, pFileName, 0, 0, generateMipChain ? 0 : 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture->pTexture);

		DBGASSERT(hr != D3DERR_NOTAVAILABLE, STR("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
		DBGASSERT(hr != D3DERR_OUTOFVIDEOMEMORY, STR("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
		DBGASSERT(hr != D3DERR_INVALIDCALL, STR("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));
		DBGASSERT(hr != D3DXERR_INVALIDDATA, STR("LoadTexture failed: D3DXERR_INVALIDDATA, 0x%08X", hr));

		DBGASSERT(hr == D3D_OK, STR("Failed to create texture '%s'.", pFileName));

		strcpy(pTexture->name, pName);

		pTexture->pTexture->GetLevelDesc(0, &imageDesc);

		pTexture->width = imageDesc.Width;
		pTexture->height = imageDesc.Height;
		pTexture->format = imageDesc.Format;
	}

	pTexture->refCount++;

	return pTexture;
}

Texture* Texture_CreateFromRawData(const char *pName, void *pData, int width, int height, int format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	Texture *pTexture = Texture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		HRESULT hr;
		D3DFORMAT fmt = D3DFMT_UNKNOWN;

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

		hr = pd3dDevice->CreateTexture(width, height, generateMipChain ? 0 : 1, 0, fmt, D3DPOOL_MANAGED, &pTexture->pTexture, NULL);

		DBGASSERT(SUCCEEDED(hr), STR("CreateTexture failed: hr = 0x%08X", hr));
		if(FAILED(hr))
		{
			LOGD("Couldnt Create Texture");
			return NULL;
		}

		D3DLOCKED_RECT rect;

		pTexture->pTexture->LockRect(0, &rect, NULL, 0);

		switch(format)
		{
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
		}

		pTexture->pTexture->UnlockRect(0);

		// generate mipmaps
		if(generateMipChain)
		{
			D3DXFilterTexture(pTexture->pTexture, NULL, 0, D3DX_DEFAULT);
		}

		strcpy(pTexture->name, pName);

		pTexture->width = width;
		pTexture->height = height;
		pTexture->format = fmt;
	}

	pTexture->refCount++;

	return pTexture;
}

Texture* Texture_CreateRenderTarget(const char *pName, int width, int height)
{
	DBGASSERT(false, "Not Written...");

	return NULL;
}

int Texture_Destroy(Texture *pTexture)
{
	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		pTexture->pTexture->Release();
		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}
