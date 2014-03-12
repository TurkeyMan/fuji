#include "Fuji_Internal.h"
#include "MFImage.h"
#include "MFVector.h"

//#define HQX_SUPPORT

#if defined(HQX_SUPPORT)
	#include "hqx.h"
#endif

extern const char *gpMFImageFormatStrings[ImgFmt_Max];
extern uint32 gMFImagePlatformAvailability[ImgFmt_Max];
extern uint8 gMFImageBitsPerPixel[ImgFmt_Max];
extern int8 gMFImageAutoFormat[MFRD_Max][0x40];

MF_API const char * MFImage_GetFormatString(int format)
{
	return gpMFImageFormatStrings[format];
}

MF_API uint32 MFTexture_GetPlatformAvailability(int format)
{
	return gMFImagePlatformAvailability[format];
}

MF_API bool MFImage_IsAvailableOnPlatform(int format, int platform)
{
	return (gMFImagePlatformAvailability[format] & MFBIT(platform)) != 0;
}

MF_API MFImageFormat MFImage_ResolveFormat(int format, MFRendererDrivers driver)
{
	if(format & ImgFmt_SelectDefault)
		return (MFImageFormat)gMFImageAutoFormat[driver][format & 0x3f];
	return (MFImageFormat)format;
}

MF_API int MFImage_GetBitsPerPixel(int format)
{
	return (int)gMFImageBitsPerPixel[format];
}


/*** Box filter ***/

template<typename Pixel>
static void Filter_Box(Pixel *pSource, Pixel *pDest, int sourceWidth, int sourceHeight, int sourceStride, int destWidth, int destHeight, int destStride)
{
	MFDebug_Assert(sizeof(Pixel)*8 == 128, "Only 128bit image formats are supported!");
}
template<>
void Filter_Box<MFVector>(MFVector *pSource, MFVector *pDest, int sourceWidth, int sourceHeight, int sourceStride, int destWidth, int destHeight, int destStride)
{
	for(int y = 0; y < destHeight; ++y)
	{
		for(int x = 0; x < destWidth; ++x)
		{
			int sourceOffset = x*2;

			MFVector &tl = pSource[sourceOffset];
			MFVector &tr = pSource[sourceOffset + 1];
			MFVector &bl = pSource[sourceOffset + sourceStride];
			MFVector &br = pSource[sourceOffset + 1 + sourceStride];

			pDest[x] = (tl + tr + bl + br) * MakeVector(0.25f);
		}

		pSource += sourceStride*2;
		pDest += destStride;
	}
}


/*** HXQ 2x,3x,4x algorithm ***/

#if defined(HQX_SUPPORT)
static bool hqxInitialised = false;
#endif
template<typename Pixel>
static void Filter_HQX(Pixel *pSource, Pixel *pDest, int sourceWidth, int sourceHeight, int sourceStride, int destWidth, int destHeight, int destStride)
{
	MFDebug_Assert(sizeof(Pixel)*8 == 32, "Only 32bit image formats are supported!");
}
template<>
void Filter_HQX<uint32>(uint32 *pSource, uint32 *pDest, int sourceWidth, int sourceHeight, int sourceStride, int destWidth, int destHeight, int destStride)
{
#if defined(HQX_SUPPORT)
	float scale = (float)destWidth / (float)sourceWidth;

	if(scale == 2.f)
	{
		if(!hqxInitialised)
		{
			hqxInit();
			hqxInitialised = true;
		}

		hq2x_32(pSource, pDest, sourceWidth, sourceHeight);
	}
	else if(scale == 3.f)
	{
		if(!hqxInitialised)
		{
			hqxInit();
			hqxInitialised = true;
		}

		hq3x_32(pSource, pDest, sourceWidth, sourceHeight);
	}
	else if(scale == 4.f)
	{
		if(!hqxInitialised)
		{
			hqxInit();
			hqxInitialised = true;
		}

		hq4x_32(pSource, pDest, sourceWidth, sourceHeight);
	}
#else
	MFDebug_Assert(false, "HQX scaler support was not built in!");
#endif
}


/*** Advance Mame 2x,3x,4x algorithm ***/

template<typename T>
static void Filter_AdvMAME(T *pSource, T *pDest, int sourceWidth, int sourceHeight, int sourceStride, int destWidth, int destHeight, int destStride)
{
	float scale = (float)destWidth / (float)sourceWidth;

	if(scale == 2.f)
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

		T *pSourceLines[3];
		T *pDestLine = pDest;
		for(int y=0; y<sourceHeight; ++y)
		{
			pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1) : pSource;
			pSourceLines[1] = pSource + sourceStride * y;
			pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1): pSource + sourceStride*y;
			T *pNextLine = pDestLine + destStride*2;

			for(int x=0; x<sourceWidth; ++x)
			{
				int l = x > 0 ? x - 1 : x;
				int r = x < sourceWidth-1 ? x + 1 : x;

				// get pixels
				T A = pSourceLines[0][x];
				T C = pSourceLines[1][l];
				T P = pSourceLines[1][x];
				T B = pSourceLines[1][r];
				T D = pSourceLines[2][x];

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
	else if(scale == 3.f)
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

		T *pSourceLines[3];
		T *pDestLine = pDest;
		for(int y=0; y<sourceHeight; ++y)
		{
			pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1): pSource;
			pSourceLines[1] = pSource + sourceStride * y;
			pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1) : pSource + sourceStride*y;
			T *pNextLine = pDestLine + destStride*3;

			for(int x=0; x<sourceWidth; ++x)
			{
				int l = x > 0 ? x - 1 : x;
				int r = x < sourceWidth-1 ? x + 1 : x;

				// get pixels
				T A = pSourceLines[0][l];
				T B = pSourceLines[0][x];
				T C = pSourceLines[0][r];
				T D = pSourceLines[1][l];
				T E = pSourceLines[1][x];
				T F = pSourceLines[1][r];
				T G = pSourceLines[2][l];
				T H = pSourceLines[2][x];
				T I = pSourceLines[2][r];

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
	else if(scale == 4.f)
	{
		// 4x is just AdvMAME 2x twice!
/*
		T *pSourceLines[3];
		T *pDestLine = pDest;
		for(int y=0; y<sourceHeight; ++y)
		{
			pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1) : pSource;
			pSourceLines[1] = pSource + sourceStride * y;
			pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1): pSource + sourceStride*y;
			T *pNextLine = pDestLine + destStride*2;

			for(int x=0; x<sourceWidth; ++x)
			{
				int l = x > 0 ? x - 1 : x;
				int r = x < sourceWidth-1 ? x + 1 : x;

				// get pixels
				T A = pSourceLines[0][x];
				T C = pSourceLines[1][l];
				T P = pSourceLines[1][x];
				T B = pSourceLines[1][r];
				T D = pSourceLines[2][x];

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
		T *pDestLine = pDest;
		for(int y=0; y<sourceHeight; ++y)
		{
			pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1) : pSource;
			pSourceLines[1] = pSource + sourceStride * y;
			pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1): pSource + sourceStride*y;
			T *pNextLine = pDestLine + destStride*2;

			for(int x=0; x<sourceWidth; ++x)
			{
				int l = x > 0 ? x - 1 : x;
				int r = x < sourceWidth-1 ? x + 1 : x;

				// get pixels
				T A = pSourceLines[0][x];
				T C = pSourceLines[1][l];
				T P = pSourceLines[1][x];
				T B = pSourceLines[1][r];
				T D = pSourceLines[2][x];

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
}


/*** Eagle 2x algorithm ***/

template<typename Pixel>
static void Filter_Eagle(Pixel *pSource, Pixel *pDest, int sourceWidth, int sourceHeight, int sourceStride, int destWidth, int destHeight, int destStride)
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

	Pixel *pSourceLines[3];
	Pixel *pDestLine = pDest;
	for(int y=0; y<sourceHeight; ++y)
	{
		pSourceLines[0] = y > 0 ? pSource + sourceStride*(y-1): pSource;
		pSourceLines[1] = pSource + sourceStride * y;
		pSourceLines[2] = y < sourceHeight-1 ? pSource + sourceStride*(y+1) : pSource + sourceStride*y;
		Pixel *pNextLine = pDestLine + destStride*2;

		for(int x=0; x<sourceWidth; ++x)
		{
			int l = x > 0 ? x - 1 : x;
			int r = x < sourceWidth-1 ? x + 1 : x;

			// get pixels
			Pixel S = pSourceLines[0][l];
			Pixel T = pSourceLines[0][x];
			Pixel U = pSourceLines[0][r];
			Pixel V = pSourceLines[1][l];
			Pixel C = pSourceLines[1][x];
			Pixel W = pSourceLines[1][r];
			Pixel X = pSourceLines[2][l];
			Pixel Y = pSourceLines[2][x];
			Pixel Z = pSourceLines[2][r];

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
}

static void GetScaleFactors(MFScalingAlgorithm algorithm, int sourceWidth, int sourceHeight, int destWidth, int destHeight, float &scalex, float &scaley)
{
	// check that the sizes are supported by the scaling algorithm
	scalex = 0.f;
	scaley = 0.f;

	switch(algorithm)
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
			if(destWidth*2 == sourceWidth && destHeight*2 == sourceHeight)
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
			if(destWidth == sourceWidth*2 && destHeight == sourceHeight*2)
				scalex = scaley = 2.f;
			break;
		default:
			MFUNREACHABLE;
	}

	if(scalex == 0.f || scaley == 0.f)
	{
		MFDebug_Assert(false, "Invalid scale factor!");
		return;
	}
}

template<typename Pixel>
static void MFImage_ScaleInternal(MFScaleImage *pScaleData)
{
	Pixel *pSource = (Pixel*)pScaleData->pSourceImage;
	Pixel *pDest = (Pixel*)pScaleData->pTargetBuffer;
	int sourceWidth = pScaleData->sourceWidth;
	int sourceHeight = pScaleData->sourceHeight;
	int sourceStride = pScaleData->sourceStride;
	int destWidth = pScaleData->targetWidth;
	int destHeight = pScaleData->targetHeight;
	int destStride = pScaleData->targetStride;

	// check that the sizes are supported by the scaling algorithm
	float scalex, scaley;
	GetScaleFactors(pScaleData->algorithm, sourceWidth, sourceHeight, destWidth, destHeight, scalex, scaley);

	// scale the image into the target buffer...
	if(scalex == 1.f && scaley == 1.f)
	{
		// copy each line separately, since the source and target could have separate stride
//		MFCopyMemory(pSource, pDest, imageSize);
		MFDebug_Assert(false, "Not implemented!");
	}
	else
	{
		switch(pScaleData->algorithm)
		{
			case SA_Nearest:
			case SA_Bilinear:
				MFDebug_Assert(false, "Not implemented!");
				break;
			case SA_Box:
				Filter_Box(pSource, pDest, sourceWidth, sourceHeight, sourceStride, destWidth, destHeight, destStride);
				break;
			case SA_HQX:
				Filter_HQX(pSource, pDest, sourceWidth, sourceHeight, sourceStride, destWidth, destHeight, destStride);
				break;
			case SA_AdvMAME:
				Filter_AdvMAME(pSource, pDest, sourceWidth, sourceHeight, sourceStride, destWidth, destHeight, destStride);
				break;
			case SA_Eagle:
				Filter_Eagle(pSource, pDest, sourceWidth, sourceHeight, sourceStride, destWidth, destHeight, destStride);
				break;
			case SA_SuperEagle:
				break;
			case SA_2xSaI:
				break;
			case SA_Super2xSaI:
				break;
			default:
				MFUNREACHABLE;
		}
	}
}

MF_API void MFImage_Scale(MFScaleImage *pScaleData)
{
	int bitsPerPixel = MFImage_GetBitsPerPixel(pScaleData->format);

	if(bitsPerPixel == 128)
		MFImage_ScaleInternal<MFVector>(pScaleData);
	else if(bitsPerPixel == 32)
		MFImage_ScaleInternal<uint32>(pScaleData);
	else
		MFDebug_Assert(false, "Image format not supported!");
}
