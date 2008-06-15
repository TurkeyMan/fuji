#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFInput.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "MFPtrList.h"
#include "MFFileSystem_Internal.h"
#include "MFSystem.h"
#include "Asset/MFIntTexture.h"

#if defined(_PSP)
	#include <pspdisplay.h>
	#include <pspgu.h>
#endif

// globals
MFPtrListDL<MFTexture> gTextureBank;
TextureBrowser texBrowser;

char blankBuffer[8*8*4];

MFTexture *pNoneTexture;
MFTexture *pWhiteTexture;

// functions
void MFTexture_InitModule()
{
	gTextureBank.Init("Texture Bank", gDefaults.texture.maxTextures);

	DebugMenu_AddItem("Texture Browser", "Fuji Options", &texBrowser);

	// create white texture (used by white material)
	if(MFFileSystem_Exists("_None.tex"))
		pNoneTexture = MFTexture_Create("_None", true);
	else
		pNoneTexture = MFTexture_CreateBlank("_None", MakeVector(1.0f, 0.0f, 0.5, 1.0f));

	pWhiteTexture = MFTexture_CreateBlank("_White", MFVector::one);
}

void MFTexture_DeinitModule()
{
	MFTexture_Destroy(pNoneTexture);
	MFTexture_Destroy(pWhiteTexture);

	// list all non-freed textures...
	MFTexture **ppI = gTextureBank.Begin();
	bool bShowHeader = true;

	while(*ppI)
	{
		if(bShowHeader)
		{
			bShowHeader = false;
			MFDebug_Message("Un-freed textures:\n----------------------------------------------------------");
		}

		MFDebug_Message(MFStr("'%s' - x%d", (*ppI)->name, (*ppI)->refCount));

		(*ppI)->refCount = 1;
		MFTexture_Destroy(*ppI);

		ppI++;
	}

	gTextureBank.Deinit();
}

MFTexture* MFTexture_FindTexture(const char *pName)
{
	MFTexture **ppIterator = gTextureBank.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pName, (*ppIterator)->name))
			return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MFTexture* MFTexture_Create(const char *pName, bool generateMipChain)
{
	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		uint32 fileSize;

		const char *pFileName = MFStr("%s.tex", pName);

		MFTextureTemplateData *pTemplate = (MFTextureTemplateData*)MFFileSystem_Load(pFileName, &fileSize);

		if(!pTemplate)
		{
#if 0
			// try to load from source data
			const char * const pExt[] = { ".tga", ".png", ".bmp", NULL };
			const char * const *ppExt = pExt;
			MFIntTexture *pIT = NULL;
			while(!pIT && *ppExt)
			{
				pIT = MFIntTexture_CreateFromFile(MFStr("%s%s", pName, *ppExt));
				++ppExt;
			}

			if(pIT)
				MFIntTexture_CreateRuntimeData(pIT, (void**)&pTemplate, NULL, MFSystem_GetCurrentPlatform());
#endif

			if(!pTemplate)
			{
				MFDebug_Warn(2, MFStr("Texture '%s' does not exist. Using '_None'.\n", pFileName));
				return MFTexture_Create("_None");
			}
		}

		MFFixUp(pTemplate->pSurfaces, pTemplate, 1);

		for(int a=0; a<pTemplate->mipLevels; a++)
		{
			MFFixUp(pTemplate->pSurfaces[a].pImageData, pTemplate, 1);
			MFFixUp(pTemplate->pSurfaces[a].pPaletteEntries, pTemplate, 1);
		}

		pTexture = gTextureBank.Create();

		pTexture->refCount = 0;
		pTexture->pTemplateData = pTemplate;
		MFString_CopyN(pTexture->name, pName, sizeof(pTexture->name) - 1);
		pTexture->name[sizeof(pTexture->name) - 1] = 0;


		MFTexture_CreatePlatformSpecific(pTexture, generateMipChain);
	}

	pTexture->refCount++;

	return pTexture;
}

MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormat format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		MFString_CopyN(pTexture->name, pName, sizeof(pTexture->name) - 1);
		pTexture->name[sizeof(pTexture->name) - 1] = 0;

		// set this to 1 for the moment until we deal with mipmaping..
		uint32 levelCount = 1;

		int imageSize = (width * height * MFTexture_GetBitsPerPixel(format)) / 8;
#if defined(XB_XGTEXTURES)
		bool ownCopy = true;
#else
		bool ownCopy = !!(flags & TEX_CopyMemory);
#endif
		bool convertARGB = false;

		const MFPlatform currentPlatform = MFSystem_GetCurrentPlatform();

		// we guarantee ARGB on all platforms, but it is not supported natively
		if(format == TexFmt_A8R8G8B8)
		{
#if defined(MF_XBOX)
			format = TexFmt_XB_A8R8G8B8s;
#endif

			if(!MFTexture_IsAvailableOnPlatform(format, currentPlatform))
			{
				// we need to take a copy and convert the image to a native 32bit format
				ownCopy = true;
				convertARGB = true;

				if(MFTexture_IsAvailableOnPlatform(TexFmt_A8B8G8R8, currentPlatform))
					format = TexFmt_A8B8G8R8;
				else if(MFTexture_IsAvailableOnPlatform(TexFmt_B8G8R8A8, currentPlatform))
					format = TexFmt_B8G8R8A8;
				else if(MFTexture_IsAvailableOnPlatform(TexFmt_R8G8B8A8, currentPlatform))
					format = TexFmt_R8G8B8A8;
				else
					MFDebug_Assert(false, "No 32bit texture format seems to be available on the current platform.");
			}
		}

		MFDebug_Assert(MFTexture_IsAvailableOnPlatform(format, currentPlatform), MFStr("Texture format %s not supported on %s", MFTexture_GetFormatString(format), MFSystem_GetPlatformName(currentPlatform)));

		// create template data
		char *pTemplate;
		uint32 size = sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount;
		if(ownCopy)
			size = MFALIGN(size, 0x100) + imageSize;
		pTemplate = (char*)MFHeap_Alloc(size);
		MFZeroMemory(pTemplate, sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount);

		pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

		pTexture->pTemplateData->imageFormat = format;
		pTexture->pTemplateData->flags = flags;

		pTexture->pTemplateData->mipLevels = levelCount;

		// we need to take a copy of this memory if flagged to do so...
		if(ownCopy)
		{
			pTexture->pTemplateData->pSurfaces[0].pImageData = (char*)MFALIGN((char*)pTexture->pTemplateData->pSurfaces + sizeof(MFTextureSurfaceLevel)*levelCount, 0x100);
			MFCopyMemory(pTexture->pTemplateData->pSurfaces[0].pImageData, pData, imageSize);
		}
		else
		{
			pTexture->pTemplateData->pSurfaces[0].pImageData = (char*)pData;
		}

		// convert to another format if the platform requires it...
		if(convertARGB)
		{
			int numPixels = width*height;
			uint32 *pPixel = (uint32*)pTexture->pTemplateData->pSurfaces[0].pImageData;

			if(format == TexFmt_A8B8G8R8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = (*pPixel & 0xFF00FF00) | ((*pPixel >> 16) & 0xFF) | ((*pPixel << 16) & 0xFF0000);
					++pPixel;
				}
			}
			else if(format == TexFmt_B8G8R8A8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = ((*pPixel << 24) & 0xFF000000) | ((*pPixel << 8) & 0xFF0000) | ((*pPixel >> 8) & 0xFF00) | ((*pPixel >> 24) & 0xFF);
					++pPixel;
				}
			}
			else if(format == TexFmt_R8G8B8A8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = ((*pPixel << 8) & 0xFFFFFF00) | ((*pPixel >> 24) & 0xFF);
					++pPixel;
				}
			}
		}

		pTexture->pTemplateData->pSurfaces[0].bufferLength = imageSize;
		pTexture->pTemplateData->pSurfaces[0].width = width;
		pTexture->pTemplateData->pSurfaces[0].height = height;
		pTexture->pTemplateData->pSurfaces[0].bitsPerPixel = MFTexture_GetBitsPerPixel(format);

		// and call the platform specific create.
		MFTexture_CreatePlatformSpecific(pTexture, generateMipChain);
	}
	else
	{
		MFDebug_Warn(3, MFStr("Texture '%s' already exists when creating from raw data.", pName));
	}

	pTexture->refCount++;

	return pTexture;
}

MFTexture* MFTexture_CreateBlank(const char *pName, const MFVector &colour)
{
	uint32 *pPixels = (uint32*)blankBuffer;

	for(int a=0; a<8*8; a++)
	{
		pPixels[a] = colour.ToPackedColour();
	}

	return MFTexture_CreateFromRawData(pName, pPixels, 8, 8, TexFmt_A8R8G8B8, 0, false);
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
	if(MFInput_WasPressed(Button_XB_Y, IDD_Gamepad))
		pCurrentMenu = pParent;
}

#define TEX_SIZE 64.0f
float TextureBrowser::ListDraw(bool selected, const MFVector &_pos, float maxWidth)
{
	MFVector pos = _pos;

	MFTexture **i;
	i = gTextureBank.Begin();

	for(int a=0; a<selection; a++) i++;

	MFTexture *pTexture = *i;

	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s:", name));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s", pTexture->name));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%dx%d, %s Refs: %d", pTexture->pTemplateData->pSurfaces[0].width, pTexture->pTemplateData->pSurfaces[0].height, MFTexture_GetFormatString(pTexture->pTemplateData->imageFormat), pTexture->refCount));

	pos += MakeVector(maxWidth - (TEX_SIZE + 4.0f + 5.0f), 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetPosition(pos);
	MFSetPosition(pos + MakeVector(TEX_SIZE + 4.0f, 0.0f, 0.0f));
	MFSetPosition(pos + MakeVector(0.0f, TEX_SIZE + 4.0f, 0.0f));
	MFSetPosition(pos + MakeVector(TEX_SIZE + 4.0f, TEX_SIZE + 4.0f, 0.0f));
	MFEnd();

	pos += MakeVector(2.0f, 2.0f, 0.0f);

	const int numSquares = 7;
	for(int a=0; a<numSquares; a++)
	{
		for(int b=0; b<numSquares; b++)
		{
			float x, y, w, h;
			w = TEX_SIZE/(float)numSquares;
			h = TEX_SIZE/(float)numSquares;
			x = pos.x + (float)b*w;
			y = pos.y + (float)a*h;

			MFBegin(4);
			MFSetColour(((a+b)&1) ? 0xFFC0C0C0 : 0xFF303030);
			MFSetPosition(x,y,0);
			MFSetPosition(x+w,y,0);
			MFSetPosition(x,y+h,0);
			MFSetPosition(x+w,y+h,0);
			MFEnd();
		}
	}

	float xaspect, yaspect;

	if(pTexture->pTemplateData->pSurfaces[0].width > pTexture->pTemplateData->pSurfaces[0].height)
	{
		xaspect = 0.5f;
		yaspect = ((float)pTexture->pTemplateData->pSurfaces[0].height/(float)pTexture->pTemplateData->pSurfaces[0].width) * 0.5f;
	}
	else
	{
		yaspect = 0.5f;
		xaspect = ((float)pTexture->pTemplateData->pSurfaces[0].width/(float)pTexture->pTemplateData->pSurfaces[0].height) * 0.5f;
	}

#if MF_RENDERER == MF_DRIVER_D3D9
	extern IDirect3DDevice9 *pd3dDevice;
	pd3dDevice->SetTexture(0, pTexture->pTexture);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
#elif MF_RENDERER == MF_DRIVER_XBOX
	extern IDirect3DDevice8 *pd3dDevice;
	pd3dDevice->SetTexture(0, pTexture->pTexture);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
#elif MF_RENDERER == MF_DRIVER_PSP
	int width = pTexture->pTemplateData->pSurfaces[0].width;
	int height = pTexture->pTemplateData->pSurfaces[0].height;
	char *pImageData = pTexture->pTemplateData->pSurfaces[0].pImageData;

	uint32 platformFormat = MFTexture_GetPlatformFormatID(pTexture->pTemplateData->imageFormat, MFDD_PSP);
	sceGuTexMode(platformFormat, 0, 0, (pTexture->pTemplateData->flags & TEX_Swizzled) ? 1 : 0);
	sceGuTexImage(0, width, height, width, pImageData);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f, 1.0f);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuSetMatrix(GU_TEXTURE, (ScePspFMatrix4*)&MFMatrix::identity);
#elif MF_RENDERER == MF_DRIVER_OPENGL
	glBindTexture(GL_TEXTURE_2D, pTexture->textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#else
//	MFDebug_Assert(false, "Not supported on this platform...");
#endif

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetTexCoord1(0.0f,0.0f);
	MFSetPosition(pos + MakeVector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,0.0f);
	MFSetPosition(pos + MakeVector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(0.0f,1.0f);
	MFSetPosition(pos + MakeVector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,1.0f);
	MFSetPosition(pos + MakeVector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFEnd();

	return TEX_SIZE + 8.0f;
}

void TextureBrowser::ListUpdate(bool selected)
{
	if(selected)
	{
		int texCount = gTextureBank.GetLength();

		if(MFInput_WasPressed(Button_DLeft, IDD_Gamepad))
		{
			selection = selection <= 0 ? texCount-1 : selection-1;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(MFInput_WasPressed(Button_DRight, IDD_Gamepad))
		{
			selection = selection >= texCount-1 ? 0 : selection+1;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

MFVector TextureBrowser::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, TEX_SIZE + 8.0f, 0.0f);
}


