#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFInput.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "MFPtrList.h"
#include "MFFileSystem_Internal.h"
#include "MFSystem.h"

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

	gTextureBank.Deinit();
}

MFTexture* MFTexture_FindTexture(const char *pName)
{
	MFTexture **ppIterator = gTextureBank.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pName, (*ppIterator)->name)) return *ppIterator;

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
			MFDebug_Warn(2, MFStr("Texture '%s' does not exist. Using '_None'.\n", pFileName));
			return MFTexture_Create("_None");
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
		MFString_Copy(pTexture->name, pName);

		MFTexture_CreatePlatformSpecific(pTexture, generateMipChain);
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

	return MFTexture_CreateFromRawData(pName, pPixels, 8, 8, TexFmt_A8R8G8B8, 0);
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

#if defined(_WINDOWS)
	extern IDirect3DDevice9 *pd3dDevice;
	pd3dDevice->SetTexture(0, pTexture->pTexture);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
#elif defined(_MFXBOX)
	extern IDirect3DDevice8 *pd3dDevice;
	pd3dDevice->SetTexture(0, pTexture->pTexture);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
#elif defined(_PSP)
	int width = pTexture->pTemplateData->pSurfaces[0].width;
	int height = pTexture->pTemplateData->pSurfaces[0].height;
	char *pImageData = pTexture->pTemplateData->pSurfaces[0].pImageData;

	sceGuTexMode(pTexture->pTemplateData->platformFormat, 0, 0, pTexture->pTemplateData->swizzled);
	sceGuTexImage(0, width, height, width, pImageData);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f, 1.0f);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuSetMatrix(GU_TEXTURE, (ScePspFMatrix4*)&MFMatrix::identity);
#else
	MFDebug_Assert(false, "Not supported on this platform...");
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


