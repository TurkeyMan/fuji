#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFInput.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "MFFileSystem_Internal.h"
#include "MFSystem.h"
#include "Asset/MFIntTexture.h"

//#define HQX_SUPPORT

#if defined(HQX_SUPPORT)
	#include "hqx.h"
#endif

#if defined(_PSP)
	#include <pspdisplay.h>
	#include <pspgu.h>
#endif

#define ALLOW_LOAD_FROM_SOURCE_DATA

// globals
TextureBrowser texBrowser;

char blankBuffer[8*8*4];

MFTexture *pNoneTexture;
MFTexture *pWhiteTexture;

// functions
static void MFTexture_Destroy(MFResource *pRes)
{
	MFTexture *pTexture = (MFTexture*)pRes;

	MFTexture_DestroyPlatformSpecific(pTexture);

	MFHeap_Free(pTexture->pTemplateData);
	MFHeap_Free(pTexture);
}

MFInitStatus MFTexture_InitModule()
{
	MFRT_Texture = MFResource_Register("MFTexture", &MFTexture_Destroy);

	DebugMenu_AddItem("Texture Browser", "Fuji Options", &texBrowser);

	MFTexture_InitModulePlatformSpecific();

	// create white texture (used by white material)
	if(MFFileSystem_Exists("_None.tex"))
		pNoneTexture = MFTexture_Create("_None", true);
	else
		pNoneTexture = MFTexture_CreateBlank("_None", MakeVector(1.0f, 0.0f, 0.5, 1.0f));

	pWhiteTexture = MFTexture_CreateBlank("_White", MFVector::one);

	return MFIS_Succeeded;
}

void MFTexture_DeinitModule()
{
	MFTexture_Release(pNoneTexture);
	MFTexture_Release(pWhiteTexture);

	// list all non-freed textures...
	MFResourceIterator *pI = MFResource_EnumerateFirst(MFRT_Texture);
	bool bShowHeader = true;

	while(pI)
	{
		if(bShowHeader)
		{
			bShowHeader = false;
			MFDebug_Message("Un-freed textures:\n----------------------------------------------------------");
		}

		MFTexture *pT = (MFTexture*)MFResource_Get(pI);

		MFDebug_Message(MFStr("'%s' - x%d", pT->pName, pT->refCount));

		pT->refCount = 1;
		MFTexture_Release(pT);

		pI = MFResource_EnumerateNext(pI, MFRT_Texture);
	}

	MFTexture_DeinitModulePlatformSpecific();
}

MF_API MFTexture* MFTexture_Find(const char *pName)
{
	return (MFTexture*)MFResource_Find(MFUtil_HashString(pName) ^ 0x7e407e40);
}

MF_API MFTexture* MFTexture_Create(const char *pName, bool generateMipChain)
{
	MFTexture *pTexture = MFTexture_Find(pName);

	if(!pTexture)
	{
		size_t fileSize;

		const char *pFileName = MFStr("%s.tex", pName);

		MFTextureTemplateData *pTemplate = (MFTextureTemplateData*)MFFileSystem_Load(pFileName, &fileSize);

		if(!pTemplate)
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
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
			{
				size_t size;
				MFIntTexture_CreateRuntimeData(pIT, &pTemplate, &size, MFSystem_GetCurrentPlatform());

				MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.tex", pName), MFOF_Write | MFOF_Binary);
				if(pFile)
				{
					MFFile_Write(pFile, pTemplate, size, false);
					MFFile_Close(pFile);
				}

				MFIntTexture_Destroy(pIT);
			}
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

		int nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pTexture = (MFTexture*)MFHeap_Alloc(sizeof(MFTexture) + nameLen);

		if(pName)
			pName = MFString_Copy((char*)&pTexture[1], pName);

		MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

		pTexture->pTemplateData = pTemplate;

		MFTexture_CreatePlatformSpecific(pTexture, generateMipChain);
	}

	return pTexture;
}

MF_API MFTexture* MFTexture_CreateDynamic(const char *pName, int width, int height, MFImageFormat format, uint32 flags)
{
	MFDebug_Assert(false, "Not written!");

	return NULL;
}

MF_API MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFImageFormat format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_Find(pName);

	if(!pTexture)
	{
		int nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pTexture = (MFTexture*)MFHeap_Alloc(sizeof(MFTexture) + nameLen);

		if(pName)
			pName = MFString_Copy((char*)&pTexture[1], pName);

		MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

		// set this to 1 for the moment until we deal with mipmaping..
		uint32 levelCount = 1;

		int imageSize = (width * height * MFImage_GetBitsPerPixel(format)) / 8;
#if defined(XB_XGTEXTURES)
		bool ownCopy = true;
#else
		bool ownCopy = !!(flags & TEX_CopyMemory);
#endif
		bool convertARGB = false;

		// we guarantee ARGB on all platforms, but it is not supported natively
		if(format == ImgFmt_A8R8G8B8)
		{
#if defined(MF_XBOX)
			format = ImgFmt_XB_A8R8G8B8s;
#endif

			if(!MFTexture_IsFormatAvailable(format))
			{
				// we need to take a copy and convert the image to a native 32bit format
				ownCopy = true;
				convertARGB = true;

				if(MFTexture_IsFormatAvailable(ImgFmt_A8B8G8R8))
					format = ImgFmt_A8B8G8R8;
				else if(MFTexture_IsFormatAvailable(ImgFmt_B8G8R8A8))
					format = ImgFmt_B8G8R8A8;
				else if(MFTexture_IsFormatAvailable(ImgFmt_R8G8B8A8))
					format = ImgFmt_R8G8B8A8;
				else
					MFDebug_Assert(false, "No 32bit texture format seems to be available on the current platform.");
			}
		}

		MFDebug_Assert(MFTexture_IsFormatAvailable(format), MFStr("Texture format %s not supported", MFImage_GetFormatString(format)));

		// create template data
		char *pTemplate;
		size_t size = sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount;
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
			pTexture->pTemplateData->pSurfaces[0].pImageData = (char*)pTexture->pTemplateData + MFALIGN(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount, 0x100);
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

			if(format == ImgFmt_A8B8G8R8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = (*pPixel & 0xFF00FF00) | ((*pPixel >> 16) & 0xFF) | ((*pPixel << 16) & 0xFF0000);
					++pPixel;
				}
			}
			else if(format == ImgFmt_B8G8R8A8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = ((*pPixel << 24) & 0xFF000000) | ((*pPixel << 8) & 0xFF0000) | ((*pPixel >> 8) & 0xFF00) | ((*pPixel >> 24) & 0xFF);
					++pPixel;
				}
			}
			else if(format == ImgFmt_R8G8B8A8)
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
		pTexture->pTemplateData->pSurfaces[0].bitsPerPixel = MFImage_GetBitsPerPixel(format);

		// and call the platform specific create.
		MFTexture_CreatePlatformSpecific(pTexture, generateMipChain);
	}
	else
	{
		MFDebug_Warn(3, MFStr("Texture '%s' already exists when creating from raw data.", pName));
	}

	return pTexture;
}

MF_API MFTexture* MFTexture_ScaleFromRawData(const char *pName, void *pData, int sourceWidth, int sourceHeight, int destWidth, int destHeight, MFImageFormat format, MFScalingAlgorithm algorithm, uint32 flags, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_Find(pName);

	if(!pTexture)
	{
		int nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pTexture = (MFTexture*)MFHeap_Alloc(sizeof(MFTexture) + nameLen);

		if(pName)
			pName = MFString_Copy((char*)&pTexture[1], pName);

		MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

		// set this to 1 for the moment until we deal with mipmaping..
		uint32 levelCount = 1;

		int bitsPerPixel = MFImage_GetBitsPerPixel(format);
		MFDebug_Assert(bitsPerPixel == 32, "Only 32bit image formats are supported!");

		int texWidth = MFUtil_NextPowerOf2(destWidth);
		int texHeight = MFUtil_NextPowerOf2(destHeight);
		int imageSize = (texWidth * texHeight * bitsPerPixel) / 8;

		// we guarantee ARGB on all platforms, but it is not supported natively
		MFImageFormat texFormat = format;

		bool convertARGB = false;
		if(texFormat == ImgFmt_A8R8G8B8)
		{
#if defined(MF_XBOX)
			texFormat = ImgFmt_XB_A8R8G8B8s;
#endif

			if(!MFTexture_IsFormatAvailable(texFormat))
			{
				// we need to convert the image to a native 32bit format
				convertARGB = true;

				if(MFTexture_IsFormatAvailable(ImgFmt_A8B8G8R8))
					texFormat = ImgFmt_A8B8G8R8;
				else if(MFTexture_IsFormatAvailable(ImgFmt_B8G8R8A8))
					texFormat = ImgFmt_B8G8R8A8;
				else if(MFTexture_IsFormatAvailable(ImgFmt_R8G8B8A8))
					texFormat = ImgFmt_R8G8B8A8;
				else
					MFDebug_Assert(false, "No 32bit texture format seems to be available on the current platform.");
			}
		}

		MFDebug_Assert(MFTexture_IsFormatAvailable(texFormat), MFStr("Texture format %s not supported", MFImage_GetFormatString(texFormat)));

		// create template data
		char *pTemplate;
		size_t size = sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount;
		size = MFALIGN(size, 0x100) + imageSize;
		pTemplate = (char*)MFHeap_Alloc(size);
		MFZeroMemory(pTemplate, sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount);

		pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

		pTexture->pTemplateData->imageFormat = texFormat;
		pTexture->pTemplateData->flags = flags;

		pTexture->pTemplateData->mipLevels = levelCount;

		pTexture->pTemplateData->pSurfaces[0].pImageData = (char*)pTexture->pTemplateData + MFALIGN(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*levelCount, 0x100);

		// scale the image
		MFScaleImage scale;
		scale.pSourceImage = pData;
		scale.sourceWidth = sourceWidth;
		scale.sourceHeight = sourceHeight;
		scale.sourceStride = sourceWidth;
		scale.pTargetBuffer = pTexture->pTemplateData->pSurfaces[0].pImageData;
		scale.targetWidth = destWidth;
		scale.targetHeight = destHeight;
		scale.targetStride = texWidth;
		scale.format = format;
		scale.algorithm = algorithm;
		MFImage_Scale(&scale);

		// convert to another format if the platform requires it...
		if(convertARGB)
		{
			int numPixels = texWidth*texHeight;
			uint32 *pPixel = (uint32*)pTexture->pTemplateData->pSurfaces[0].pImageData;

			if(texFormat == ImgFmt_A8B8G8R8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = (*pPixel & 0xFF00FF00) | ((*pPixel >> 16) & 0xFF) | ((*pPixel << 16) & 0xFF0000);
					++pPixel;
				}
			}
			else if(texFormat == ImgFmt_B8G8R8A8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = ((*pPixel << 24) & 0xFF000000) | ((*pPixel << 8) & 0xFF0000) | ((*pPixel >> 8) & 0xFF00) | ((*pPixel >> 24) & 0xFF);
					++pPixel;
				}
			}
			else if(texFormat == ImgFmt_R8G8B8A8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = ((*pPixel << 8) & 0xFFFFFF00) | ((*pPixel >> 24) & 0xFF);
					++pPixel;
				}
			}
		}

		pTexture->pTemplateData->pSurfaces[0].bufferLength = imageSize;
		pTexture->pTemplateData->pSurfaces[0].width = texWidth;
		pTexture->pTemplateData->pSurfaces[0].height = texHeight;
		pTexture->pTemplateData->pSurfaces[0].bitsPerPixel = MFImage_GetBitsPerPixel(texFormat);

		// and call the platform specific create.
		MFTexture_CreatePlatformSpecific(pTexture, false);
	}
	else
	{
		MFDebug_Warn(3, MFStr("Texture '%s' already exists when creating from raw data.", pName));
	}

	return pTexture;
}

MF_API MFTexture* MFTexture_CreateBlank(const char *pName, const MFVector &colour)
{
	uint32 *pPixels = (uint32*)blankBuffer;

	uint32 packed = colour.ToPackedColour();
	for(int a=0; a<8*8; a++)
		pPixels[a] = packed;

	return MFTexture_CreateFromRawData(pName, pPixels, 8, 8, ImgFmt_A8R8G8B8, TEX_CopyMemory, false);
}

MF_API int MFTexture_Release(MFTexture *pTexture)
{
	return MFResource_Release(pTexture);
}

MF_API void MFTexture_GetTextureDimensions(const MFTexture *pTexture, int *pWidth, int *pHeight)
{
	if(pWidth)
		*pWidth = pTexture->pTemplateData->pSurfaces[0].width;
	if(pHeight)
		*pHeight = pTexture->pTemplateData->pSurfaces[0].height;
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

	MFResourceIterator *pI = MFResource_EnumerateFirst(MFRT_Texture);

	for(int a=0; a<selection; a++)
		pI = MFResource_EnumerateNext(pI, MFRT_Texture);

	MFTexture *pTexture = (MFTexture*)MFResource_Get(pI);

	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s:", name));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s", pTexture->pName));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%dx%d, %s Refs: %d", pTexture->pTemplateData->pSurfaces[0].width, pTexture->pTemplateData->pSurfaces[0].height, MFImage_GetFormatString(pTexture->pTemplateData->imageFormat), pTexture->refCount));

	pos += MakeVector(maxWidth - (TEX_SIZE + 4.0f + 5.0f), 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColourV(MFVector::white);
	MFSetPositionV(pos);
	MFSetPositionV(pos + MakeVector(TEX_SIZE + 4.0f, 0.0f, 0.0f));
	MFSetPositionV(pos + MakeVector(0.0f, TEX_SIZE + 4.0f, 0.0f));
	MFSetPositionV(pos + MakeVector(TEX_SIZE + 4.0f, TEX_SIZE + 4.0f, 0.0f));
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
			MFSetColourV(((a+b)&1) ? MakeVector(.75f, .75f, .75f, 1.f) : MakeVector(.2f, .2f, .2f, 1.f));
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
#if !defined(MF_OPENGL_ES)
	glBindTexture(GL_TEXTURE_2D, (size_t)pTexture->pInternalData);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif
#else
//	MFDebug_Assert(false, "Not supported on this platform...");
#endif

	MFBegin(4);
	MFSetColourV(MFVector::white);
	MFSetTexCoord1(0.0f,0.0f);
	MFSetPositionV(pos + MakeVector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,0.0f);
	MFSetPositionV(pos + MakeVector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(0.0f,1.0f);
	MFSetPositionV(pos + MakeVector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,1.0f);
	MFSetPositionV(pos + MakeVector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFEnd();

	return TEX_SIZE + 8.0f;
}

void TextureBrowser::ListUpdate(bool selected)
{
	if(selected)
	{
		int texCount = MFResource_GetNumResources(MFRT_Texture);

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


