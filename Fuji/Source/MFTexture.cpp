#include "Fuji_Internal.h"
#include "MFTexture_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFAsset.h"
#include "MFInput.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "MFFileSystem.h"
#include "MFSystem.h"
#include "Util.h"
#include "MFRenderer.h"

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
	if(pTexture->pImageData && (pTexture->flags & TEX_FreeImageData))
		MFHeap_Free(pTexture->pImageData);
	MFHeap_Free(pTexture);
}

MFInitStatus MFTexture_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFRT_Texture = MFResource_Register("MFTexture", &MFTexture_Destroy);

	DebugMenu_AddItem("Texture Browser", "Fuji Options", &texBrowser);

	MFTexture_InitModulePlatformSpecific();

	// create default textures
	pNoneTexture = MFTexture_CreateFromFile("_None");
	if(!pNoneTexture)
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

static int GetNumMipLevels(int width, int height, int depth, int targetSize = 1)
{
	int numMips = 1;
	while(width > targetSize || height > targetSize || depth > targetSize)
	{
		width >>= 1;
		height >>= 1;
		depth >>= 1;
		++numMips;
	}
	return numMips;
}

MFTexture* MFTexture_InitTexture(const MFTextureDesc *pDesc, MFRendererDrivers renderer, size_t *pExtraBytes)
{
	switch(pDesc->type)
	{
		case MFTexType_1D:		MFDebug_Assert(pDesc->width > 0 && pDesc->height == 0 && pDesc->depth == 0, "1D textures must have: width > 0 && height == 0 && depth == 0");
								MFDebug_Assert((pDesc->flags & MFTCF_TypeMask) != MFTCF_RenderTarget, "1D textures may not be render targets"); break;
		case MFTexType_2D:		MFDebug_Assert(pDesc->width > 0 && pDesc->height > 0 && pDesc->depth == 0, "2D textures must have: width > 0 && height > 0 && depth == 0"); break;
		case MFTexType_Cubemap:	MFDebug_Assert(pDesc->width > 0 && pDesc->height == pDesc->width && pDesc->depth == 0, "Cubemap textures must have: width > 0 && height == width && depth == 0"); break;
		case MFTexType_3D:		MFDebug_Assert(pDesc->width > 0 && pDesc->height > 0 && pDesc->depth > 0, "1D textures must have: width > 0 && height > 0 && depth > 0");
								MFDebug_Assert(pDesc->arrayElements == 0, "Arrays of 3D textures are not supported");
								MFDebug_Assert((pDesc->flags & MFTCF_TypeMask) != MFTCF_RenderTarget, "3D textures may not be render targets"); break;
	}

	MFImageFormat format = MFImage_ResolveFormat(pDesc->format, renderer);
	MFDebug_Assert(format != ImgFmt_Unknown, "Invalid texture format!");
	MFDebug_Assert(MFTexture_IsFormatAvailable(format), MFStr("Texture format %s not supported", MFImage_GetFormatString(pDesc->format)));

	int maxMipDepth = GetNumMipLevels(pDesc->width, pDesc->height, pDesc->depth);
	MFDebug_Assert(!pDesc->mipLevels || pDesc->mipLevels <= maxMipDepth, MFStr("Too many mip levels for texture. %d levels requested, but 1x1 mip is level %d", pDesc->mipLevels, maxMipDepth));

	int numFaces = pDesc->type == MFTexType_Cubemap ? 6 : 1;
	int numMips = pDesc->mipLevels ? pDesc->mipLevels : maxMipDepth;
	int numElements = (pDesc->arrayElements ? pDesc->arrayElements : 1) * numFaces;
	int numSurfaces = numElements * numMips;

	MFTexture *pTemplate;
	size_t size = sizeof(MFTexture) + sizeof(MFTextureSurface)*numSurfaces + (pExtraBytes ? *pExtraBytes : 0);
	if(pExtraBytes)
		*pExtraBytes = sizeof(MFTexture) + sizeof(MFTextureSurface)*numSurfaces;
	pTemplate = (MFTexture*)MFHeap_AllocAndZero(size);
	pTemplate->pSurfaces = (MFTextureSurface*)&pTemplate[1];

	pTemplate->hash = MFMAKEFOURCC('F','T','E','X');
	pTemplate->type = pDesc->type;
	pTemplate->imageFormat = pDesc->format;
	pTemplate->createFlags = pDesc->flags;

	pTemplate->width = pDesc->width;
	pTemplate->height = pDesc->height;
	pTemplate->depth = pDesc->depth;
	pTemplate->numElements = numElements;
	pTemplate->numMips = numMips;

	if(pDesc->flags & MFTCF_PremultipliedAlpha)
		pTemplate->flags |= TEX_PreMultipliedAlpha;
	if((pDesc->flags & MFTCF_TypeMask) == MFTCF_RenderTarget)
		pTemplate->flags |= TEX_RenderTarget;

	// populate the surfaces
	MFTextureSurface *pSurface = pTemplate->pSurfaces;
	for(int mip=0; mip<numMips; ++mip)
	{
		int width = pDesc->width >> mip;
		int height = pDesc->height >> mip;
		int depth = pDesc->depth >> mip;
		width = width ? width : 1;
		height = height ? height : 1;
		depth = depth ? depth : 1;

		for(int e=0; e<numElements; ++e)
		{
//			int i = e / numFaces; // array element
//			int f = e % numFaces; // cube face

			for(int d=0; d<depth; ++d)
			{
				pSurface->width = width;
				pSurface->height = height;
				pSurface->depth = depth;
				pSurface->bitsPerPixel = (uint8)MFImage_GetBitsPerPixel(pDesc->format);

				if(pTemplate->imageFormat & 0xFF)
				{
					// TODO: calculate from compression format...
					//...
				}
				else
				{
					pSurface->xBlocks = pSurface->width;
					pSurface->yBlocks = pSurface->height;
					pSurface->bitsPerBlock = pSurface->bitsPerPixel;
				}

				pSurface++;
			}
		}
	}

	return pTemplate;
}

MF_API MFTexture* MFTexture_Create(const char *pName, const MFTextureDesc *pDesc)
{
	MFTexture *pTexture = MFTexture_Find(pName);
	if(pTexture)
	{
		MFDebug_Warn(3, MFStr("Texture '%s' already exists.", pName));
		MFTexture_Release(pTexture);
		return NULL;
	}

	// create the texture template data
	size_t nameLen = pName ? MFString_Length(pName) + 1 : 0;
	pTexture = MFTexture_InitTexture(pDesc, MFRenderer_GetCurrentRenderDriver(), &nameLen);
	pName = MFString_Copy((char*)pTexture + nameLen, pName);

	MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

	// allocate the texture memory...
	MFTexture_CreatePlatformSpecific(pTexture);

	return pTexture;
}

MF_API MFTexture* MFTexture_CreateFromFile(const char *pName, uint32 flags)
{
	MFTexture *pTexture = MFTexture_Find(pName);

	if(!pTexture)
	{
		size_t fileSize;

		const char *pFileName = MFStr("%s.tex", pName);

		pTexture = (MFTexture*)MFFileSystem_Load(pFileName, &fileSize);

		if(!pTexture)
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
			const char *pExt = MFString_GetFileExtension(pName);
			if(pExt && MFAsset_IsImageFile(pExt))
			{
				MFAsset_ConvertTextureFromFile(pName, (void**)&pTexture, &fileSize, MFSystem_GetCurrentPlatform());
			}
			else
			{
				// try each extension...
				for(const char **ppExt = MFAsset_GetImageFileTypes(); *ppExt != NULL; ++ppExt)
				{
					MFString fileName = MFString::Format("%s%s", pName, *ppExt);
					if(MFAsset_ConvertTextureFromFile(fileName.CStr(), (void**)&pTexture, &fileSize, MFSystem_GetCurrentPlatform()))
						break;
				}
			}
#endif

			if(!pTexture)
			{
				MFDebug_Warn(2, MFStr("Texture '%s' does not exist. Using '_None'.\n", pFileName));
				return MFTexture_Find("_None");
			}
		}

		// fixup raw data
		pTexture->pSurfaces = (MFTextureSurface*)(pTexture->surfacesOffset + (size_t)pTexture);
		pTexture->pImageData = (char*)(pTexture->imageDataOffset + (size_t)pTexture);
		pTexture->pName = MFString_Dup(pName);

		MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pTexture->pName);

		MFTexture_CreatePlatformSpecific(pTexture);
	}

	return pTexture;
}

MF_API MFTexture* MFTexture_CreateFromRawData(const char *pName, const void *pData, int width, int height, MFImageFormat format, uint32 flags, const uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_Find(pName);
	if(pTexture)
	{
		MFDebug_Warn(3, MFStr("Texture '%s' already exists.", pName));
		MFTexture_Release(pTexture);
		return NULL;
	}

	// we guarantee ARGB on all platforms, but it is not supported natively
	bool bConvertARGB = false;
	if(format == ImgFmt_A8R8G8B8)
	{
#if defined(MF_XBOX)
		format = ImgFmt_XB_A8R8G8B8s;
#endif

		if(!MFTexture_IsFormatAvailable(format))
		{
			// we need to take a copy and convert the image to a native 32bit format
			bConvertARGB = true;

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

	// set this to 1 for the moment until we deal with mipmaping..
	int mipLevels = 1;//((flags & MFTCF_GenerateMips) ? 0 : 1)

	// initialise the texture structure
	MFTextureDesc desc = { MFTexType_2D, format, width, height, 0, 0, mipLevels, flags };

	size_t nameLen = pName ? MFString_Length(pName) + 1 : 0;
	pTexture = MFTexture_InitTexture(&desc, MFRenderer_GetCurrentRenderDriver(), &nameLen);
	if(pName)
		pName = MFString_Copy((char*)pTexture + nameLen, pName);

	int imageSize = (pTexture->pSurfaces[0].width * pTexture->pSurfaces[0].height * pTexture->pSurfaces[0].bitsPerPixel) / 8;
	pTexture->pImageData = (char*)MFHeap_Alloc(imageSize);
	pTexture->flags |= TEX_FreeImageData;

	// copy or transform
	if(bConvertARGB)
	{
		int numPixels = width*height;
		uint32 *pSrc = (uint32*)pData;
		uint32 *pDest = (uint32*)pTexture->pImageData;

		if(format == ImgFmt_A8B8G8R8)
		{
			for(int a=0; a<numPixels; ++a)
			{
				*pDest = (*pSrc & 0xFF00FF00) | ((*pSrc >> 16) & 0xFF) | ((*pSrc << 16) & 0xFF0000);
				++pSrc;
				++pDest;
			}
		}
		else if(format == ImgFmt_B8G8R8A8)
		{
			for(int a=0; a<numPixels; ++a)
			{
				*pDest = ((*pSrc << 24) & 0xFF000000) | ((*pSrc << 8) & 0xFF0000) | ((*pSrc >> 8) & 0xFF00) | ((*pSrc >> 24) & 0xFF);
				++pSrc;
				++pDest;
			}
		}
		else if(format == ImgFmt_R8G8B8A8)
		{
			for(int a=0; a<numPixels; ++a)
			{
				*pDest = ((*pSrc << 8) & 0xFFFFFF00) | ((*pSrc >> 24) & 0xFF);
				++pSrc;
				++pDest;
			}
		}
	}
	else
	{
		MFCopyMemory(pTexture->pImageData, pData, imageSize);
	}

	pTexture->pSurfaces[0].bufferLength = imageSize;

	MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

	// and call the platform specific create.
	MFTexture_CreatePlatformSpecific(pTexture);

	return pTexture;
}

MF_API MFTexture* MFTexture_ScaleFromRawData(const char *pName, void *pData, int sourceWidth, int sourceHeight, int destWidth, int destHeight, MFImageFormat format, MFScalingAlgorithm algorithm, uint32 flags, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_Find(pName);
	if(pTexture)
	{
		MFDebug_Warn(3, MFStr("Texture '%s' already exists.", pName));
		MFTexture_Release(pTexture);
		return NULL;
	}

	// we guarantee ARGB on all platforms, but it is not supported natively
	MFImageFormat srcFormat = format;
	bool bConvertARGB = false;
	if(format == ImgFmt_A8R8G8B8)
	{
#if defined(MF_XBOX)
		format = ImgFmt_XB_A8R8G8B8s;
#endif

		if(!MFTexture_IsFormatAvailable(format))
		{
			// we need to take a copy and convert the image to a native 32bit format
			bConvertARGB = true;

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

	int bitsPerPixel = MFImage_GetBitsPerPixel(format);
	MFDebug_Assert(bitsPerPixel == 32, "Only 32bit image formats are supported!");

	// set this to 1 for the moment until we deal with mipmaping..
	int mipLevels = 1;//((flags & MFTCF_GenerateMips) ? 0 : 1)

	// initialise the texture structure
	MFTextureDesc desc = { MFTexType_2D, format, destWidth, destHeight, 0, 0, mipLevels, flags };

	size_t nameLen = pName ? MFString_Length(pName) + 1 : 0;
	pTexture = MFTexture_InitTexture(&desc, MFRenderer_GetCurrentRenderDriver(), &nameLen);
	if(pName)
		pName = MFString_Copy((char*)pTexture + nameLen, pName);

	// TODO: shall we scale the dest width/height to pow-of-2 for platforms that require it?

	int imageSize = (destWidth * destHeight * bitsPerPixel) / 8;
	pTexture->pImageData = (char*)MFHeap_Alloc(imageSize);
	pTexture->flags |= TEX_FreeImageData;

	// scale the image
	MFScaleImage scale;
	scale.pSourceImage = pData;
	scale.sourceWidth = sourceWidth;
	scale.sourceHeight = sourceHeight;
	scale.sourceStride = sourceWidth;
	scale.pTargetBuffer = pTexture->pImageData;
	scale.targetWidth = destWidth;
	scale.targetHeight = destHeight;
	scale.targetStride = destWidth;
	scale.format = srcFormat;
	scale.algorithm = algorithm;
	MFImage_Scale(&scale);

	// convert to another format if the platform requires it...
	if(bConvertARGB)
	{
		int numPixels = destWidth*destHeight;
		uint32 *pPixel = (uint32*)pTexture->pImageData;

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

	MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

	// and call the platform specific create.
	MFTexture_CreatePlatformSpecific(pTexture);

	return pTexture;
}

MF_API MFTexture* MFTexture_CreateBlank(const char *pName, const MFVector &colour)
{
	uint32 *pPixels = (uint32*)blankBuffer;

	uint32 packed = colour.ToPackedColour();
	for(int a=0; a<8*8; a++)
		pPixels[a] = packed;

	return MFTexture_CreateFromRawData(pName, pPixels, 8, 8, ImgFmt_A8R8G8B8);
}

MF_API int MFTexture_Release(MFTexture *pTexture)
{
	return MFResource_Release(pTexture);
}

MF_API void MFTexture_GetTextureDimensions(const MFTexture *pTexture, int *pWidth, int *pHeight, int *pDepth)
{
	if(pWidth)
		*pWidth = pTexture->width;
	if(pHeight)
		*pHeight = pTexture->height;
	if(pDepth)
		*pDepth = pTexture->depth;
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
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%dx%d, %s Refs: %d", pTexture->width, pTexture->height, MFImage_GetFormatString(pTexture->imageFormat), pTexture->refCount));

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


