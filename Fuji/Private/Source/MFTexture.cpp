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
MFTexturePool gTextureBank;
TextureBrowser texBrowser;

char blankBuffer[8*8*4];

MFTexture *pNoneTexture;
MFTexture *pWhiteTexture;

// functions
void MFTexture_InitModule()
{
	gTextureBank.Init(256, 64, 64);

	DebugMenu_AddItem("Texture Browser", "Fuji Options", &texBrowser);

	MFTexture_InitModulePlatformSpecific();

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
	MFTexturePool::Iterator pI = gTextureBank.First();
	bool bShowHeader = true;

	while(pI)
	{
		if(bShowHeader)
		{
			bShowHeader = false;
			MFDebug_Message("Un-freed textures:\n----------------------------------------------------------");
		}

		MFDebug_Message(MFStr("'%s' - x%d", pI->name, pI->refCount));

		pI->refCount = 1;
		MFTexture_Destroy(pI);

		pI = gTextureBank.Next(pI);
	}

	gTextureBank.Deinit();

	MFTexture_DeinitModulePlatformSpecific();
}

MFTexture* MFTexture_FindTexture(const char *pName)
{
	return gTextureBank.Get(pName);
}

MFTexture* MFTexture_Create(const char *pName, bool generateMipChain)
{
	MFTexture *pTexture = MFTexture_FindTexture(pName);

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
				MFIntTexture_CreateRuntimeData(pIT, &pTemplate, NULL, MFSystem_GetCurrentPlatform());
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

		pTexture = &gTextureBank.Create(pName);

		pTexture->refCount = 0;
		pTexture->pTemplateData = pTemplate;
		MFString_CopyN(pTexture->name, pName, sizeof(pTexture->name) - 1);
		pTexture->name[sizeof(pTexture->name) - 1] = 0;


		MFTexture_CreatePlatformSpecific(pTexture, generateMipChain);
	}

	pTexture->refCount++;

	return pTexture;
}

MFTexture* MFTexture_CreateDynamic(const char *pName, int width, int height, MFTextureFormat format, uint32 flags)
{
	MFDebug_Assert(false, "Not written!");

	return NULL;
}

MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormat format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = &gTextureBank.Create(pName);
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

		// we guarantee ARGB on all platforms, but it is not supported natively
		if(format == TexFmt_A8R8G8B8)
		{
#if defined(MF_XBOX)
			format = TexFmt_XB_A8R8G8B8s;
#endif

			if(!MFTexture_IsAvailable(format))
			{
				// we need to take a copy and convert the image to a native 32bit format
				ownCopy = true;
				convertARGB = true;

				if(MFTexture_IsAvailable(TexFmt_A8B8G8R8))
					format = TexFmt_A8B8G8R8;
				else if(MFTexture_IsAvailable(TexFmt_B8G8R8A8))
					format = TexFmt_B8G8R8A8;
				else if(MFTexture_IsAvailable(TexFmt_R8G8B8A8))
					format = TexFmt_R8G8B8A8;
				else
					MFDebug_Assert(false, "No 32bit texture format seems to be available on the current platform.");
			}
		}

		MFDebug_Assert(MFTexture_IsAvailable(format), MFStr("Texture format %s not supported", MFTexture_GetFormatString(format)));

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

void MFTexture_ScaleImage(MFScaleImage *pScaleData)
{
	uint32 *pSource = (uint32*)pScaleData->pSourceImage;
	uint32 *pDest = (uint32*)pScaleData->pTargetBuffer;
	int sourceWidth = pScaleData->sourceWidth;
	int sourceHeight = pScaleData->sourceHeight;
	int sourceStride = pScaleData->sourceStride;
	int destWidth = pScaleData->targetWidth;
	int destHeight = pScaleData->targetHeight;
	int destStride = pScaleData->targetStride;

	// check that the sizes are supported by the scaling algorithm
	float scalex = 0.f;
	float scaley = 0.f;
	switch(pScaleData->algorithm)
	{
		case SA_Nearest:
		case SA_Bilinear:
			// any size supported
			scalex = (float)destWidth / (float)sourceWidth;
			scaley = (float)destHeight / (float)sourceHeight;
			break;
		case SA_None:
			// texture must be the same size as the source
			if(destWidth == sourceWidth && destHeight == sourceHeight)
				scalex = scaley = 1.f;
			break;
		case SA_Box:
			// texture should be half the size of the source
			if((destWidth & 1) == 0 && (destHeight & 1) == 0 && destWidth == (sourceWidth >> 1) && destHeight == (sourceHeight >> 1))
				scalex = scaley = 0.5f;
			break;
		case SA_HQX:
		case SA_AdvMAME:
			// texture should be 2x, 3x, 4x the size of the source
			if(destWidth == sourceWidth*2 && destHeight == sourceHeight*2)
				scalex = scaley = 2.f;
			else if(destWidth == sourceWidth*3 && destHeight == sourceHeight*3)
				scalex = scaley = 3.f;
			else if(destWidth == sourceWidth*4 && destHeight == sourceHeight*4)
				scalex = scaley = 4.f;
			break;
		case SA_Eagle:
		case SA_SuperEagle:
		case SA_2xSaI:
		case SA_Super2xSaI:
			// texture should be twice the size of the source
			if(destWidth == (sourceWidth << 1) && destHeight == (sourceHeight << 1))
				scalex = scaley = 2.f;
			break;
	}

	if(scalex == 0.f || scaley == 0.f)
	{
		MFDebug_Assert(false, "Invalid scale factor!");
		return;
	}

	int bitsPerPixel = MFTexture_GetBitsPerPixel(pScaleData->sourceFormat);
	MFDebug_Assert(bitsPerPixel == 32, "Only 32bit image formats are supported!");

	// scale the image into the target buffer...
	if(scalex == 1.f && scaley == 1.f)
	{
		// copy each line separately, since the source and target could have separate stride
//		MFCopyMemory(pSource, pDest, imageSize);
	}
	else
	{
		static bool hqxInitialised = false;

		switch(pScaleData->algorithm)
		{
			case SA_Nearest:
			case SA_Bilinear:
			case SA_Box:
				MFDebug_Assert(false, "Not implemented!");
				break;
			case SA_HQX:
				if(scalex == 2.f)
				{
#if defined(HQX_SUPPORT)
					if(!hqxInitialised)
					{
						hqxInit();
						hqxInitialised = true;
					}

					hq2x_32(pSource, pDest, sourceWidth, sourceHeight);
#endif
				}
				else if(scalex == 3.f)
				{
#if defined(HQX_SUPPORT)
					if(!hqxInitialised)
					{
						hqxInit();
						hqxInitialised = true;
					}

					hq3x_32(pSource, pDest, sourceWidth, sourceHeight);
#endif
				}
				else if(scalex == 4.f)
				{
#if defined(HQX_SUPPORT)
					if(!hqxInitialised)
					{
						hqxInit();
						hqxInitialised = true;
					}

					hq4x_32(pSource, pDest, sourceWidth, sourceHeight);
#endif
				}
				break;
			case SA_AdvMAME:
				if(scalex == 2.f)
				{
					// http://en.wikipedia.org/wiki/Pixel_art_scaling_algorithms
					//   A    --\ 1 2
					// C P B  --/ 3 4
					//   D 
					//  1=P; 2=P; 3=P; 4=P;
					//  IF C==A AND C!=D AND A!=B => 1=A
					//  IF A==B AND A!=C AND B!=D => 2=B
					//  IF B==D AND B!=A AND D!=C => 4=D
					//  IF D==C AND D!=B AND C!=A => 3=C

					uint32 *pSourceLines[3];
					uint32 *pDestLine = pDest;
					for(int y=0; y<sourceHeight; ++y)
					{
						pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1) : pSource;
						pSourceLines[1] = pSource + sourceStride * y;
						pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1): pSource + sourceStride*y;
						uint32 *pNextLine = pDestLine + destStride*2;

						for(int x=0; x<sourceWidth; ++x)
						{
							int l = x > 0 ? x - 1 : x;
							int r = x < sourceWidth-1 ? x + 1 : x;

							// get pixels
							uint32 A = pSourceLines[0][x];
							uint32 C = pSourceLines[1][l];
							uint32 P = pSourceLines[1][x];
							uint32 B = pSourceLines[1][r];
							uint32 D = pSourceLines[2][x];

							// scale block
							pDestLine[0]			= C==A && C!=D && A!=B ? A : P;
							pDestLine[1]			= A==B && A!=C && B!=D ? B : P;
							pDestLine[destStride]	= D==C && D!=B && C!=A ? C : P;
							pDestLine[destStride+1]	= B==D && B!=A && D!=C ? D : P;

							// next pixel
							pDestLine += 2;
						}

						// skip the next line
						pDestLine = pNextLine;
					}
				}
				else if(scalex == 3.f)
				{
					// http://en.wikipedia.org/wiki/Pixel_art_scaling_algorithms
					// A B C --\  1 2 3
					// D E F    > 4 5 6
					// G H I --/  7 8 9
					//  1=E; 2=E; 3=E; 4=E; 5=E; 6=E; 7=E; 8=E; 9=E;
					//  IF D==B AND D!=H AND B!=F => 1=D
					//  IF (D==B AND D!=H AND B!=F AND E!=C) OR (B==F AND B!=D AND F!=H AND E!=A) 2=B
					//  IF B==F AND B!=D AND F!=H => 3=F
					//  IF (H==D AND H!=F AND D!=B AND E!=A) OR (D==B AND D!=H AND B!=F AND E!=G) 4=D
					//  5=E
					//  IF (B==F AND B!=D AND F!=H AND E!=I) OR (F==H AND F!=B AND H!=D AND E!=C) 6=F
					//  IF H==D AND H!=F AND D!=B => 7=D
					//  IF (F==H AND F!=B AND H!=D AND E!=G) OR (H==D AND H!=F AND D!=B AND E!=I) 8=H
					//  IF F==H AND F!=B AND H!=D => 9=F

					uint32 *pSourceLines[3];
					uint32 *pDestLine = pDest;
					for(int y=0; y<sourceHeight; ++y)
					{
						pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1): pSource;
						pSourceLines[1] = pSource + sourceStride * y;
						pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1) : pSource + sourceStride*y;
						uint32 *pNextLine = pDestLine + destStride*3;

						for(int x=0; x<sourceWidth; ++x)
						{
							int l = x > 0 ? x - 1 : x;
							int r = x < sourceWidth-1 ? x + 1 : x;

							// get pixels
							uint32 A = pSourceLines[0][l];
							uint32 B = pSourceLines[0][x];
							uint32 C = pSourceLines[0][r];
							uint32 D = pSourceLines[1][l];
							uint32 E = pSourceLines[1][x];
							uint32 F = pSourceLines[1][r];
							uint32 G = pSourceLines[2][l];
							uint32 H = pSourceLines[2][x];
							uint32 I = pSourceLines[2][r];

							// scale block
							pDestLine[0]				=  D==B && D!=H && B!=F                                            ? D : E;
							pDestLine[1]				= (D==B && D!=H && B!=F && E!=C) || (B==F && B!=D && F!=H && E!=A) ? B : E;
							pDestLine[2]				=  B==F && B!=D && F!=H                                            ? F : E;
							pDestLine[destStride]		= (H==D && H!=F && D!=B && E!=A) || (D==B && D!=H && B!=F && E!=G) ? D : E;
							pDestLine[destStride+1]		=                                                                        E;
							pDestLine[destStride+2]		= (B==F && B!=D && F!=H && E!=I) || (F==H && F!=B && H!=D && E!=C) ? F : E;
							pDestLine[destStride*2]		=  H==D && H!=F && D!=B                                            ? D : E;
							pDestLine[destStride*2+1]	= (F==H && F!=B && H!=D && E!=G) || (H==D && H!=F && D!=B && E!=I) ? H : E;
							pDestLine[destStride*2+2]	=  F==H && F!=B && H!=D                                            ? F : E;

							// next pixel
							pDestLine += 3;
						}

						// skip the next 2 lines
						pDestLine = pNextLine;
					}
				}
				else if(scalex == 4.f)
				{
					// 4x is just AdvMAME 2x twice!
/*
					uint32 *pSourceLines[3];
					uint32 *pDestLine = pDest;
					for(int y=0; y<sourceHeight; ++y)
					{
						pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1) : pSource;
						pSourceLines[1] = pSource + sourceStride * y;
						pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1): pSource + sourceStride*y;
						uint32 *pNextLine = pDestLine + destStride*2;

						for(int x=0; x<sourceWidth; ++x)
						{
							int l = x > 0 ? x - 1 : x;
							int r = x < sourceWidth-1 ? x + 1 : x;

							// get pixels
							uint32 A = pSourceLines[0][x];
							uint32 C = pSourceLines[1][l];
							uint32 P = pSourceLines[1][x];
							uint32 B = pSourceLines[1][r];
							uint32 D = pSourceLines[2][x];

							// scale block
							pDestLine[0]				= C==A && C!=D && A!=B ? A : P;
							pDestLine[2]				= A==B && A!=C && B!=D ? B : P;
							pDestLine[destStride*2]		= D==C && D!=B && C!=A ? C : P;
							pDestLine[destStride*2+2]	= B==D && B!=A && D!=C ? D : P;

							// next pixel
							pDestLine += 3;
						}

						// skip the next line
						pDestLine = pNextLine * 3;
					}

					// scale it again!
					uint32 *pDestLine = pDest;
					for(int y=0; y<sourceHeight; ++y)
					{
						pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1) : pSource;
						pSourceLines[1] = pSource + sourceStride * y;
						pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1): pSource + sourceStride*y;
						uint32 *pNextLine = pDestLine + destStride*2;

						for(int x=0; x<sourceWidth; ++x)
						{
							int l = x > 0 ? x - 1 : x;
							int r = x < sourceWidth-1 ? x + 1 : x;

							// get pixels
							uint32 A = pSourceLines[0][x];
							uint32 C = pSourceLines[1][l];
							uint32 P = pSourceLines[1][x];
							uint32 B = pSourceLines[1][r];
							uint32 D = pSourceLines[2][x];

							// scale block
							pDestLine[0]				= C==A && C!=D && A!=B ? A : P;
							pDestLine[2]				= A==B && A!=C && B!=D ? B : P;
							pDestLine[destStride*2]		= D==C && D!=B && C!=A ? C : P;
							pDestLine[destStride*2+2]	= B==D && B!=A && D!=C ? D : P;

							// next pixel
							pDestLine += 3;
						}

						// skip the next line
						pDestLine = pNextLine * 3;
					}
*/
				}
				break;
			case SA_Eagle:
			{
				// http://en.wikipedia.org/wiki/Pixel_art_scaling_algorithms
				// First:        |Then 
				// . . . --\ CC  |S T U  --\ 1 2
				// . C . --/ CC  |V C W  --/ 3 4
				// . . .         |X Y Z
          		//               | IF V==S==T => 1=S
				//               | IF T==U==W => 2=U
				//               | IF V==X==Y => 3=X
				//               | IF W==Z==Y => 4=Z

				uint32 *pSourceLines[3];
				uint32 *pDestLine = pDest;
				for(int y=0; y<sourceHeight; ++y)
				{
					pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1): pSource;
					pSourceLines[1] = pSource + sourceStride * y;
					pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1) : pSource + sourceStride*y;
					uint32 *pNextLine = pDestLine + destStride*2;

					for(int x=0; x<sourceWidth; ++x)
					{
						int l = x > 0 ? x - 1 : x;
						int r = x < sourceWidth-1 ? x + 1 : x;

						// get pixels
						uint32 S = pSourceLines[0][l];
						uint32 T = pSourceLines[0][x];
						uint32 U = pSourceLines[0][r];
						uint32 V = pSourceLines[1][l];
						uint32 C = pSourceLines[1][x];
						uint32 W = pSourceLines[1][r];
						uint32 X = pSourceLines[2][l];
						uint32 Y = pSourceLines[2][x];
						uint32 Z = pSourceLines[2][r];

						// scale block
						pDestLine[0]			= V == S && S == T ? S : C;
						pDestLine[1]			= T == U && U == W ? U : C;
						pDestLine[destStride]	= V == X && X == Y ? X : C;
						pDestLine[destStride+1]	= W == Z && Z == Y ? Z : C;

						// next pixel
						pDestLine += 2;
					}

					// skip the next line
					pDestLine = pNextLine;
				}
				break;
			}
			case SA_SuperEagle:
				break;
			case SA_2xSaI:
				break;
			case SA_Super2xSaI:
				break;
		}
	}
}

MFTexture* MFTexture_ScaleFromRawData(const char *pName, void *pData, int sourceWidth, int sourceHeight, int destWidth, int destHeight, MFTextureFormat format, MFScalingAlgorithm algorithm, uint32 flags, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = &gTextureBank.Create(pName);
		pTexture->refCount = 0;

		MFString_CopyN(pTexture->name, pName, sizeof(pTexture->name) - 1);
		pTexture->name[sizeof(pTexture->name) - 1] = 0;

		// set this to 1 for the moment until we deal with mipmaping..
		uint32 levelCount = 1;

		int bitsPerPixel = MFTexture_GetBitsPerPixel(format);
		MFDebug_Assert(bitsPerPixel == 32, "Only 32bit image formats are supported!");

		int texWidth = MFUtil_NextPowerOf2(destWidth);
		int texHeight = MFUtil_NextPowerOf2(destHeight);
		int imageSize = (texWidth * texHeight * bitsPerPixel) / 8;

		// we guarantee ARGB on all platforms, but it is not supported natively
		MFTextureFormat texFormat = format;

		bool convertARGB = false;
		if(texFormat == TexFmt_A8R8G8B8)
		{
#if defined(MF_XBOX)
			texFormat = TexFmt_XB_A8R8G8B8s;
#endif

			if(!MFTexture_IsAvailable(texFormat))
			{
				// we need to convert the image to a native 32bit format
				convertARGB = true;

				if(MFTexture_IsAvailable(TexFmt_A8B8G8R8))
					texFormat = TexFmt_A8B8G8R8;
				else if(MFTexture_IsAvailable(TexFmt_B8G8R8A8))
					texFormat = TexFmt_B8G8R8A8;
				else if(MFTexture_IsAvailable(TexFmt_R8G8B8A8))
					texFormat = TexFmt_R8G8B8A8;
				else
					MFDebug_Assert(false, "No 32bit texture format seems to be available on the current platform.");
			}
		}

		MFDebug_Assert(MFTexture_IsAvailable(texFormat), MFStr("Texture format %s not supported", MFTexture_GetFormatString(texFormat)));

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
		scale.sourceFormat = format;
		scale.algorithm = algorithm;
		MFTexture_ScaleImage(&scale);

		// convert to another format if the platform requires it...
		if(convertARGB)
		{
			int numPixels = texWidth*texHeight;
			uint32 *pPixel = (uint32*)pTexture->pTemplateData->pSurfaces[0].pImageData;

			if(texFormat == TexFmt_A8B8G8R8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = (*pPixel & 0xFF00FF00) | ((*pPixel >> 16) & 0xFF) | ((*pPixel << 16) & 0xFF0000);
					++pPixel;
				}
			}
			else if(texFormat == TexFmt_B8G8R8A8)
			{
				for(int a=0; a<numPixels; ++a)
				{
					*pPixel = ((*pPixel << 24) & 0xFF000000) | ((*pPixel << 8) & 0xFF0000) | ((*pPixel >> 8) & 0xFF00) | ((*pPixel >> 24) & 0xFF);
					++pPixel;
				}
			}
			else if(texFormat == TexFmt_R8G8B8A8)
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
		pTexture->pTemplateData->pSurfaces[0].bitsPerPixel = MFTexture_GetBitsPerPixel(texFormat);

		// and call the platform specific create.
		MFTexture_CreatePlatformSpecific(pTexture, false);
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

	uint32 packed = colour.ToPackedColour();
	for(int a=0; a<8*8; a++)
		pPixels[a] = packed;

	return MFTexture_CreateFromRawData(pName, pPixels, 8, 8, TexFmt_A8R8G8B8, TEX_CopyMemory, false);
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	MFCALLSTACK;

	--pTexture->refCount;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		MFTexture_DestroyPlatformSpecific(pTexture);

		MFHeap_Free(pTexture->pTemplateData);
		gTextureBank.DestroyItem(*pTexture);
		return 0;
	}

	return pTexture->refCount;
}

void MFTexture_GetTextureDimensions(MFTexture *pTexture, int *pWidth, int *pHeight)
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

	MFTexturePool::Iterator pTexture = gTextureBank.First();

	for(int a=0; a<selection; a++)
		pTexture = gTextureBank.Next(pTexture);

	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s:", name));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s", pTexture->name));
	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%dx%d, %s Refs: %d", pTexture->pTemplateData->pSurfaces[0].width, pTexture->pTemplateData->pSurfaces[0].height, MFTexture_GetFormatString(pTexture->pTemplateData->imageFormat), pTexture->refCount));

	pos += MakeVector(maxWidth - (TEX_SIZE + 4.0f + 5.0f), 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(MFVector::white);
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
			MFSetColour(((a+b)&1) ? MakeVector(.75f, .75f, .75f, 1.f) : MakeVector(.2f, .2f, .2f, 1.f));
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
	MFSetColour(MFVector::white);
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
		int texCount = gTextureBank.GetNumItems();

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


