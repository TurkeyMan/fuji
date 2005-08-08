#include "Common.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "Input.h"
#include "Font.h"
#include "Primitive.h"
#include "PtrList.h"

#if defined(_PSP)
	#include <pspdisplay.h>
	#include <pspgu.h>
#endif

// globals
PtrListDL<MFTexture> gTextureBank;
TextureBrowser texBrowser;

char blankBuffer[8*8*4];

MFTexture *pNoneTexture;
MFTexture *pWhiteTexture;

const char * const gpMFTextureFormatStrings[TexFmt_Max] =
{
	"A8R8G8B8",
	"A8B8G8R8",
	"A2R10G10B10",
	"A2B10G10R10",
	"A16B16G16R16",
	"R5G6B5",
	"A1R5G5B5",
	"A4R4G4B4",
	"A4B4G4R4",
	"ABGR_F16",
	"ABGR_F32",
	"I8",
	"I4",
	"DXT1",
	"DXT2",
	"DXT3",
	"DXT4",
	"DXT5"
};

uint32 gMFTextureBitsPerPixel[TexFmt_Max] =
{
	32,	// TexFmt_A8R8G8B8
	32,	// TexFmt_A8B8G8R8

	32,	// TexFmt_A2R10G10B10
	32,	// TexFmt_A2B10G10R10
	64,	// TexFmt_A16B16G16R16

	16,	// TexFmt_R5G6B5
	16,	// TexFmt_A1R5G5B5

	16,	// TexFmt_A4R4G4B4
	16,	// TexFmt_A4B4G4R4

	64,	// TexFmt_ABGR_F16
	128,// TexFmt_ABGR_F32

	8,	// TexFmt_I8
	4,	// TexFmt_I4

	4,	// TexFmt_DXT1
	8,	// TexFmt_DXT2
	8,	// TexFmt_DXT3
	8,	// TexFmt_DXT4
	8	// TexFmt_DXT5
};

uint32 gMFTexturePlatformFormat[TexFmt_Max] =
{
#if defined(_WINDOWS)
	21,	// D3DFMT_A8R8G8B8		// TexFmt_A8R8G8B8
	32,	// D3DFMT_A8B8G8R8		// TexFmt_A8B8G8R8
	35,	// D3DFMT_A2R10G10B10	// TexFmt_A2R10G10B10
	31,	// D3DFMT_A2B10G10R10	// TexFmt_A2B10G10R10
	36,	// D3DFMT_A16B16G16R16	// TexFmt_A16B16G16R16
	23,	// D3DFMT_R5G6B5		// TexFmt_R5G6B5
	25,	// D3DFMT_A1R5G5B5		// TexFmt_A1R5G5B5
	26,	// D3DFMT_A4R4G4B4		// TexFmt_A4R4G4B4
	0,	//						// TexFmt_A4B4G4R4
	113,// D3DFMT_A16B16G16R16F	// TexFmt_ABGR_F16
	116,// D3DFMT_A32B32G32R32F	// TexFmt_ABGR_F32
	41,	// D3DFMT_P8			// TexFmt_I8
	0,	//						// TexFmt_I4
	MAKEFOURCC('D', 'X', 'T', '1'),	// D3DFMT_DXT1	// TexFmt_DXT1
	MAKEFOURCC('D', 'X', 'T', '2'),	// D3DFMT_DXT2	// TexFmt_DXT2
	MAKEFOURCC('D', 'X', 'T', '3'),	// D3DFMT_DXT3	// TexFmt_DXT3
	MAKEFOURCC('D', 'X', 'T', '4'),	// D3DFMT_DXT4	// TexFmt_DXT4
	MAKEFOURCC('D', 'X', 'T', '5')	// D3DFMT_DXT5	// TexFmt_DXT5
#elif defined(_XBOX)
	0, // TexFmt_A8R8G8B8
	0, // TexFmt_A8B8G8R8
	0, // TexFmt_A2R10G10B10
	0, // TexFmt_A2B10G10R10
	0, // TexFmt_A16B16G16R16
	0, // TexFmt_R5G6B5
	0, // TexFmt_A1R5G5B5
	0, // TexFmt_A4R4G4B4
	0, // TexFmt_A4B4G4R4
	0, // TexFmt_ABGR_F16
	0, // TexFmt_ABGR_F32
	0, // TexFmt_I8
	0, // TexFmt_I4
	0, // TexFmt_DXT1
	0, // TexFmt_DXT2
	0, // TexFmt_DXT3
	0, // TexFmt_DXT4
	0  // TexFmt_DXT5
#elif defined(_LNX)
	0, // TexFmt_A8R8G8B8
	0, // TexFmt_A8B8G8R8
	0, // TexFmt_A2R10G10B10
	0, // TexFmt_A2B10G10R10
	0, // TexFmt_A16B16G16R16
	0, // TexFmt_R5G6B5
	0, // TexFmt_A1R5G5B5
	0, // TexFmt_A4R4G4B4
	0, // TexFmt_A4B4G4R4
	0, // TexFmt_ABGR_F16
	0, // TexFmt_ABGR_F32
	0, // TexFmt_I8
	0, // TexFmt_I4
	0, // TexFmt_DXT1
	0, // TexFmt_DXT2
	0, // TexFmt_DXT3
	0, // TexFmt_DXT4
	0  // TexFmt_DXT5
#elif defined(_PSP)
	99,	//					TexFmt_A8R8G8B8
	3,	// SCEGU_PF8888, //	TexFmt_A8B8G8R8
	99,	//					TexFmt_A2R10G10B10
	99,	//					TexFmt_A2B10G10R10
	99,	//					TexFmt_A16B16G16R16
	0,	// SCEGU_PF5650, //	TexFmt_R5G6B5
	1,	// SCEGU_PF5551, //	TexFmt_A1R5G5B5
	99,	//					TexFmt_A4R4G4B4
	2,	// SCEGU_PF4444, //	TexFmt_A4B4G4R4
	99,	//					TexFmt_ABGR_F16
	99,	//					TexFmt_ABGR_F32
	5,	// SCEGU_PFIDX8, //	TexFmt_I8
	4,	// SCEGU_PFIDX4, //	TexFmt_I4
	8,	// SCEGU_PFDXT1, //	TexFmt_DXT1
	99,	//					TexFmt_DXT2
	9,	// SCEGU_PFDXT3, //	TexFmt_DXT3
	99,	//					TexFmt_DXT4
	10	// SCEGU_PFDXT5, //	TexFmt_DXT5
#else
	0, // TexFmt_A8R8G8B8
	0, // TexFmt_A8B8G8R8
	0, // TexFmt_A2R10G10B10
	0, // TexFmt_A2B10G10R10
	0, // TexFmt_A16B16G16R16
	0, // TexFmt_R5G6B5
	0, // TexFmt_A1R5G5B5
	0, // TexFmt_A4R4G4B4
	0, // TexFmt_A4B4G4R4
	0, // TexFmt_ABGR_F16
	0, // TexFmt_ABGR_F32
	0, // TexFmt_I8
	0, // TexFmt_I4
	0, // TexFmt_DXT1
	0, // TexFmt_DXT2
	0, // TexFmt_DXT3
	0, // TexFmt_DXT4
	0  // TexFmt_DXT5
#endif
};

// functions
void MFTexture_InitModule()
{
	gTextureBank.Init("Texture Bank", gDefaults.texture.maxTextures);

	DebugMenu_AddItem("Texture Browser", "Fuji Options", &texBrowser);

	// create white texture (used by white material)
	pNoneTexture = MFTexture_CreateBlank("_None", Vector(1.0f, 0.0f, 0.5, 1.0f));
	pWhiteTexture = MFTexture_CreateBlank("_White", Vector4::one);
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
		if(!StrCaseCmp(pName, (*ppIterator)->name)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MFTexture* MFTexture_CreateBlank(const char *pName, const Vector4 &colour)
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
	if(Input_WasPressed(IDD_Gamepad, 0, Button_XB_Y))
		pCurrentMenu = pParent;
}

#define TEX_SIZE 64.0f
float TextureBrowser::ListDraw(bool selected, const Vector3 &_pos, float maxWidth)
{
	Vector3 pos = _pos;

	MFTexture **i;
	i = gTextureBank.Begin();

	for(int a=0; a<selection; a++) i++;

	MFTexture *pTexture = *i;

	Font_DrawText(gpDebugFont, pos+Vector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? Vector(1,1,0,1) : Vector4::one, STR("%s:", name));
	Font_DrawText(gpDebugFont, pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? Vector(1,1,0,1) : Vector4::one, STR("%s", pTexture->name));
	Font_DrawText(gpDebugFont, pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? Vector(1,1,0,1) : Vector4::one, STR("%dx%d, %s Refs: %d", pTexture->pTemplateData->pSurfaces[0].width, pTexture->pTemplateData->pSurfaces[0].height, gpMFTextureFormatStrings[(int)pTexture->pTemplateData->imageFormat], pTexture->refCount));

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
	pd3dDevice->SetTexture(0, pTexture->pTexture);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
#elif defined(_XBOX)
	pd3dDevice->SetTexture(0, pTexture->pTexture);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
#elif defined(_PSP)
	int width = pTexture->pTemplateData->pSurfaces[0].width;
	int height = pTexture->pTemplateData->pSurfaces[0].height;
	char *pImageData = pTexture->pTemplateData->pSurfaces[0].pImageData;

	sceGuTexMode(pTexture->pTemplateData->platformFormat, 0, 0, 0);
	sceGuTexImage(0, width, height, width, pImageData);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f, 1.0f);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuSetMatrix(GU_TEXTURE, (ScePspFMatrix4*)&Matrix::identity);
#else
	DBGASSERT(false, "Not supported on this platform...");
#endif

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
		int texCount = gTextureBank.GetLength();

		if(Input_WasPressed(IDD_Gamepad, 0, Button_DLeft))
		{
			selection = selection <= 0 ? texCount-1 : selection-1;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(Input_WasPressed(IDD_Gamepad, 0, Button_DRight))
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


