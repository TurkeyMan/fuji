#include "Fuji_Internal.h"
#include "MFImage.h"
#include "MFVector.h"

#if defined(MF_WINDOWS)
//	#define MF_ENABLE_ATI_COMPRESSOR
#endif
#if defined(MF_ENABLE_ATI_COMPRESSOR)
	#include <windows.h>
	#include "ATI_Compress/ATI_Compress.h"
	#pragma comment(lib, "ATI_Compress/ATI_Compress.lib")
#endif


//#define HQX_SUPPORT

#if defined(HQX_SUPPORT)
	#include "hqx.h"
#endif

extern const char *gpMFImageFormatStrings[ImgFmt_Max];
extern uint8 gMFImagePlatformAvailability[4][ImgFmt_Max];
extern uint8 gMFImageBitsPerPixel[ImgFmt_Max];
extern int8 gMFImageAutoFormat[MFRD_Max][0x40];

MF_API const char * MFImage_GetFormatString(int format)
{
	return gpMFImageFormatStrings[format];
}

MF_API uint32 MFTexture_GetPlatformAvailability(int format)
{
	return gMFImagePlatformAvailability[(format>>8)&3][format];
}

MF_API bool MFImage_IsAvailableOnPlatform(int format, int platform)
{
	return (gMFImagePlatformAvailability[(format>>8)&3][format] & MFBIT(platform)) != 0;
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

static void ATICompress(const void *pSourceBuffer, int width, int height, MFImageFormat targetFormat, void *pOutputBuffer)
{
#if defined(MF_ENABLE_ATI_COMPRESSOR)
	ATI_TC_FORMAT atiFormat;
	switch(targetFormat)
	{
		case ImgFmt_DXT1:
		case ImgFmt_PSP_DXT1:
			atiFormat = ATI_TC_FORMAT_DXT1;
			break;
		case ImgFmt_DXT2:
		case ImgFmt_DXT3:
		case ImgFmt_PSP_DXT3:
			atiFormat = ATI_TC_FORMAT_DXT3;
			break;
		case ImgFmt_DXT4:
		case ImgFmt_DXT5:
		case ImgFmt_PSP_DXT5:
			atiFormat = ATI_TC_FORMAT_DXT5;
			break;
	}

	// Init source texture
	ATI_TC_Texture srcTexture;
	srcTexture.dwSize = sizeof(srcTexture);
	srcTexture.dwWidth = width;
	srcTexture.dwHeight = height;
	srcTexture.dwPitch = width*sizeof(float);
	srcTexture.format = ATI_TC_FORMAT_ARGB_32F;
	srcTexture.dwDataSize = ATI_TC_CalculateBufferSize(&srcTexture);
	srcTexture.pData = (ATI_TC_BYTE*)pSourceBuffer;

	// Init dest texture
	ATI_TC_Texture destTexture;
	destTexture.dwSize = sizeof(destTexture);
	destTexture.dwWidth = width;
	destTexture.dwHeight = height;
	destTexture.dwPitch = 0;
	destTexture.format = atiFormat;
	destTexture.dwDataSize = ATI_TC_CalculateBufferSize(&destTexture);
	destTexture.pData = (ATI_TC_BYTE*)pOutputBuffer;

	ATI_TC_CompressOptions options;
	options.dwSize = sizeof(options);
	options.bUseChannelWeighting = FALSE;
	options.fWeightingRed = 1.0;			/* Weighting of the Red or X Channel */
	options.fWeightingGreen = 1.0;			/* Weighting of the Green or Y Channel */
	options.fWeightingBlue = 1.0;			/* Weighting of the Blue or Z Channel */
	options.bUseAdaptiveWeighting = TRUE;	/* Adapt weighting on a per-block basis */
	options.bDXT1UseAlpha = TRUE;
	options.nAlphaThreshold = 128;

	// Compress
	ATI_TC_ConvertTexture(&srcTexture, &destTexture, &options, NULL, NULL, NULL);
#else
	// not supported
	MFDebug_Assert(false, "ATI's S3 Texture Compressor not available in this build..");
#endif
}

MF_API void MFImage_Convert(uint32 width, uint32 height, const void *pInput, MFImageFormat inputFormat, void *pOutput, MFImageFormat outputFormat)
{
	const uint8 *pIn = (const uint8*)pInput;
	uint8 *pOut = (uint8*)pOutput;

	bool bTosRGB = (inputFormat&ImgFmt_Linear) && !(outputFormat&ImgFmt_Linear);
	bool bToLinear = !(inputFormat&ImgFmt_Linear) && (outputFormat&ImgFmt_Linear);

	if((inputFormat | outputFormat) & ImgFmt_Swizzle)
	{
		MFDebug_Assert(false, "Swizzled formats not (yet) supported!");

		// get un-swizzled format...
//		inputFormat = ??
//		outputFormat = ??
	}

	if((inputFormat == ImgFmt_DXT1 && inputFormat <= ImgFmt_ATCRGB) ||
		(inputFormat == ImgFmt_DXT3 && inputFormat <= ImgFmt_ATCRGBA_EXPLICIT) ||
		(inputFormat == ImgFmt_DXT5 && inputFormat <= ImgFmt_ATCRGBA))
	{
		// TODO: it is possible to convert S3TC -> ATITC directly with satisfactory quality

		MFDebug_Assert(false, "DXT -> ATITC transcode not done!");
	}

	if(inputFormat >= ImgFmt_DXT1 && inputFormat <= ImgFmt_PSP_DXT5)
	{
		MFDebug_Assert(false, "Conversion FROM compressed formats not (yet) supported!");
	}

	if(outputFormat >= ImgFmt_DXT1 && outputFormat <= ImgFmt_PSP_DXT5)
	{
		MFDebug_Assert(inputFormat == ImgFmt_ABGR_F32, "Only compression from ImgFmt_ABGR_F32 is (currently) supported!");
		MFDebug_Assert(!bTosRGB && !bToLinear, "Colour space conversion not (yet) supported during compression!");

		// compress image...
		switch(outputFormat)
		{
			case ImgFmt_DXT1:
			case ImgFmt_DXT2:
			case ImgFmt_DXT3:
			case ImgFmt_DXT4:
			case ImgFmt_DXT5:
			case ImgFmt_ATI1:
			case ImgFmt_ATI2:
			case ImgFmt_PSP_DXT1:
			case ImgFmt_PSP_DXT3:
			case ImgFmt_PSP_DXT5:
			{
				// TODO: squish https://code.google.com/p/libsquish/
				//       Fuji\Fuji\Middleware\texcompress\squish
				// TODO: Fuji\Fuji\Middleware\texcompress\stb_dxt
				// TODO: Fuji\Fuji\Middleware\texcompress\ImageLib

				ATICompress(pIn, width, height, outputFormat, pOut);

				if(outputFormat >= ImgFmt_PSP_DXT1 && outputFormat <= ImgFmt_PSP_DXT5)
				{
					// we need to swizzle the PSP buffer about a bit...
					MFDebug_Assert(false, "TODO: Swizzle for PSP...\n");
				}
				break;
			}

			case ImgFmt_BPTC_F:
			case ImgFmt_BPTC:

			case ImgFmt_CTX1:
			{
				// Note: X360 lib to pack this?
			}

			case ImgFmt_ETC1:
			{
				// TODO: https://code.google.com/p/rg-etc1/
				//       Fuji\Fuji\Middleware\texcompress\rg_etc1
			}

			case ImgFmt_ETC2:
			case ImgFmt_EAC:
			case ImgFmt_ETC2_EAC:
			case ImgFmt_EACx2:

			case ImgFmt_PVRTC_RGB_2bpp:
			case ImgFmt_PVRTC_RGB_4bpp:
			case ImgFmt_PVRTC_RGBA_2bpp:
			case ImgFmt_PVRTC_RGBA_4bpp:
			case ImgFmt_PVRTC2_2bpp:
			case ImgFmt_PVRTC2_4bpp:

			case ImgFmt_ATCRGB:
			case ImgFmt_ATCRGBA_EXPLICIT:
			case ImgFmt_ATCRGBA:
			{
				// Note: Theoretically we could use a DXT packer and transcode...
			}

			case ImgFmt_ASTC:
			{
				// TODO: Fuji\Fuji\Middleware\texcompress\astc-evaluation-codec
			}

			default:
			{
				MFDebug_Assert(false, MFStr("Compression format '%s' not yet supported...\n", MFImage_GetFormatString(outputFormat)));
			}
		}
	}
	else if(outputFormat >= ImgFmt_ABGR_F16 && outputFormat <= ImgFmt_R9G9B9_E5)
	{
		// float format
		float r=0, g=0, b=0, a=0;
		for(uint32 y=0; y<height; ++y)
		{
			for(uint32 x=0; x<width; ++x)
			{
				switch(inputFormat)
				{
					case ImgFmt_A8R8G8B8:
						b = (float)pIn[0] * (1.f/255.f);
						g = (float)pIn[1] * (1.f/255.f);
						r = (float)pIn[2] * (1.f/255.f);
						a = (float)pIn[3] * (1.f/255.f);
						pIn += 4;
						break;
					case ImgFmt_A8B8G8R8:
						r = (float)pIn[0] * (1.f/255.f);
						g = (float)pIn[1] * (1.f/255.f);
						b = (float)pIn[2] * (1.f/255.f);
						a = (float)pIn[3] * (1.f/255.f);
						pIn += 4;
						break;
					case ImgFmt_B8G8R8A8:
						a = (float)pIn[0] * (1.f/255.f);
						r = (float)pIn[1] * (1.f/255.f);
						g = (float)pIn[2] * (1.f/255.f);
						b = (float)pIn[3] * (1.f/255.f);
						pIn += 4;
						break;
					case ImgFmt_R8G8B8A8:
						a = (float)pIn[0] * (1.f/255.f);
						b = (float)pIn[1] * (1.f/255.f);
						g = (float)pIn[2] * (1.f/255.f);
						r = (float)pIn[3] * (1.f/255.f);
						pIn += 4;
						break;
					case ImgFmt_R8G8B8:
						b = (float)pIn[0] * (1.f/255.f);
						g = (float)pIn[1] * (1.f/255.f);
						r = (float)pIn[2] * (1.f/255.f);
						a = 1.f;
						pIn += 3;
						break;
					case ImgFmt_B8G8R8:
						r = (float)pIn[0] * (1.f/255.f);
						g = (float)pIn[1] * (1.f/255.f);
						b = (float)pIn[2] * (1.f/255.f);
						a = 1.f;
						pIn += 3;
						break;
					case ImgFmt_A2R10G10B10:
					case ImgFmt_A2B10G10R10:
						MFDebug_Assert(false, "Not done!");
						break;
					case ImgFmt_A16B16G16R16:
					{
						const uint16 *pIn16 = (const uint16*)pIn;
						r = (float)pIn16[0] * (1.f/65535.f);
						g = (float)pIn16[1] * (1.f/65535.f);
						b = (float)pIn16[2] * (1.f/65535.f);
						a = (float)pIn16[3] * (1.f/65535.f);
						pIn += 8;
						break;
					}
					case ImgFmt_R5G6B5:
					case ImgFmt_R6G5B5:
					case ImgFmt_B5G6R5:
					case ImgFmt_A1R5G5B5:
					case ImgFmt_R5G5B5A1:
					case ImgFmt_A1B5G5R5:
					case ImgFmt_A4R4G4B4:
					case ImgFmt_A4B4G4R4:
					case ImgFmt_R4G4B4A4:
					case ImgFmt_ABGR_F16:
						MFDebug_Assert(false, "Not done!");
						break;
					case ImgFmt_ABGR_F32:
					{
						const float *pInF = (const float*)pIn;
						r = pInF[0];
						g = pInF[1];
						b = pInF[2];
						a = pInF[3];
						pIn += 16;
						break;
					}
					case ImgFmt_R11G11B10_F:
					case ImgFmt_R9G9B9_E5:
						MFDebug_Assert(false, "Not done!");
						break;
					default:
						MFDebug_Assert(false, "Shouldn't be here!");
				}

				if(bTosRGB)
				{
					r = MFImage_LinearTosRGB(r);
					g = MFImage_LinearTosRGB(g);
					b = MFImage_LinearTosRGB(b);
				}
				else if(bToLinear)
				{
					r = MFImage_sRGBToLinear(r);
					g = MFImage_sRGBToLinear(g);
					b = MFImage_sRGBToLinear(b);
				}

				switch(outputFormat)
				{
					case ImgFmt_ABGR_F16:
					{
						MFDebug_Assert(false, "This is wrong!");
						uint16 *pOut16 = (uint16*)pOut;
						uint32 c = (uint32&)r;
						pOut16[0] = (uint16)((c & 0xFC000000) >> 16 | (c & 0x007FE000) >> 13);
						c = (uint32&)g;
						pOut16[1] = (uint16)((c & 0xFC000000) >> 16 | (c & 0x007FE000) >> 13);
						c = (uint32&)b;
						pOut16[2] = (uint16)((c & 0xFC000000) >> 16 | (c & 0x007FE000) >> 13);
						c = (uint32&)a;
						pOut16[3] = (uint16)((c & 0xFC000000) >> 16 | (c & 0x007FE000) >> 13);
						pOut += 8;
						break;
					}
					case ImgFmt_ABGR_F32:
					{
						float *pOutF = (float*)pOut;
						pOutF[0] = r;
						pOutF[1] = g;
						pOutF[2] = b;
						pOutF[3] = a;
						pOut += 16;
						break;
					}
					case ImgFmt_R11G11B10_F:
					case ImgFmt_R9G9B9_E5:
						MFDebug_Assert(false, "Not done!");
						break;
					default:
						MFDebug_Assert(false, "Shouldn't be here!");
				}
			}
		}
	}
	else if(outputFormat >= ImgFmt_A2R10G10B10 && outputFormat <= ImgFmt_A16B16G16R16)
	{
		// higher fidelity
		uint16 r=0, g=0, b=0, a=0;
		for(uint32 y=0; y<height; ++y)
		{
			for(uint32 x=0; x<width; ++x)
			{
				switch(inputFormat)
				{
					case ImgFmt_A8R8G8B8:
						b = pIn[0]; b |= b << 8;
						g = pIn[1]; g |= g << 8;
						r = pIn[2]; r |= r << 8;
						a = pIn[3]; a |= a << 8;
						pIn += 4;
						break;
					case ImgFmt_A8B8G8R8:
						r = pIn[0]; r |= r << 8;
						g = pIn[1]; g |= g << 8;
						b = pIn[2]; b |= b << 8;
						a = pIn[3]; a |= a << 8;
						pIn += 4;
						break;
					case ImgFmt_B8G8R8A8:
						a = pIn[0]; a |= a << 8;
						r = pIn[1]; r |= r << 8;
						g = pIn[2]; g |= g << 8;
						b = pIn[3]; b |= b << 8;
						pIn += 4;
						break;
					case ImgFmt_R8G8B8A8:
						a = pIn[0]; a |= a << 8;
						b = pIn[1]; b |= b << 8;
						g = pIn[2]; g |= g << 8;
						r = pIn[3]; r |= r << 8;
						pIn += 4;
						break;
					case ImgFmt_R8G8B8:
						b = pIn[0]; b |= b << 8;
						g = pIn[1]; g |= g << 8;
						r = pIn[2]; r |= r << 8;
						a = 0xFFFF;
						pIn += 3;
						break;
					case ImgFmt_B8G8R8:
						r = pIn[0]; r |= r << 8;
						g = pIn[1]; g |= g << 8;
						b = pIn[2]; b |= b << 8;
						a = 0xFFFF;
						pIn += 3;
						break;
					case ImgFmt_A2R10G10B10:
					case ImgFmt_A2B10G10R10:
						MFDebug_Assert(false, "Not done!");
						break;
					case ImgFmt_A16B16G16R16:
					{
						const uint16 *pIn16 = (const uint16*)pIn;
						r = pIn16[0];
						g = pIn16[1];
						b = pIn16[2];
						a = pIn16[3];
						pIn += 8;
						break;
					}
					case ImgFmt_R5G6B5:
					case ImgFmt_R6G5B5:
					case ImgFmt_B5G6R5:
					case ImgFmt_A1R5G5B5:
					case ImgFmt_R5G5B5A1:
					case ImgFmt_A1B5G5R5:
					case ImgFmt_A4R4G4B4:
					case ImgFmt_A4B4G4R4:
					case ImgFmt_R4G4B4A4:
					case ImgFmt_ABGR_F16:
						MFDebug_Assert(false, "Not done!");
						break;
					case ImgFmt_ABGR_F32:
					{
						const float *pInF = (const float*)pIn;
						r = (uint16)(pInF[0]*65535.f + 0.5f);
						g = (uint16)(pInF[1]*65535.f + 0.5f);
						b = (uint16)(pInF[2]*65535.f + 0.5f);
						a = (uint16)(pInF[3]*65535.f + 0.5f);
						pIn += 16;
						break;
					}
					case ImgFmt_R11G11B10_F:
					case ImgFmt_R9G9B9_E5:
						MFDebug_Assert(false, "Not done!");
						break;
					default:
						MFDebug_Assert(false, "Shouldn't be here!");
				}

				if(bTosRGB)
				{
					r = (uint16)(MFImage_LinearTosRGB(r * (1.f/65535.f))*65535.f + 0.5f);
					g = (uint16)(MFImage_LinearTosRGB(g * (1.f/65535.f))*65535.f + 0.5f);
					b = (uint16)(MFImage_LinearTosRGB(b * (1.f/65535.f))*65535.f + 0.5f);
				}
				else if(bToLinear)
				{
					r = (uint16)(MFImage_sRGBToLinear(r * (1.f/65535.f))*65535.f + 0.5f);
					g = (uint16)(MFImage_sRGBToLinear(g * (1.f/65535.f))*65535.f + 0.5f);
					b = (uint16)(MFImage_sRGBToLinear(b * (1.f/65535.f))*65535.f + 0.5f);
				}

				switch(outputFormat)
				{
					case ImgFmt_A2R10G10B10:
					{
						*(uint32*)pOut = ((uint32)(a & 0xC000) << 16) |
										 ((uint32)(r & 0xFFC0) << 14) |
										 ((uint32)(g & 0xFFC0) << 4) |
										 ((uint32)(b & 0xFFC0) >> 6);
						pOut += 4;
						break;
					}
					case ImgFmt_A2B10G10R10:
					{
						*(uint32*)pOut = ((uint32)(a & 0xC000) << 16) |
										 ((uint32)(b & 0xFFC0) << 14) |
										 ((uint32)(g & 0xFFC0) << 4) |
										 ((uint32)(r & 0xFFC0) >> 6);
						pOut += 4;
						break;
					}
					case ImgFmt_A16B16G16R16:
					{
						uint16 *pOut16 = (uint16*)pOut;
						pOut16[0] = r;
						pOut16[1] = g;
						pOut16[2] = b;
						pOut16[3] = a;
						pOut += 8;
						break;
					}
					default:
						MFDebug_Assert(false, "Shouldn't be here!");
				}
			}
		}
	}
	else
	{
		uint8 r=0, g=0, b=0, a=0;
		for(uint32 y=0; y<height; ++y)
		{
			for(uint32 x=0; x<width; ++x)
			{
				switch(inputFormat)
				{
					case ImgFmt_A8R8G8B8:
						b = pIn[0];
						g = pIn[1];
						r = pIn[2];
						a = pIn[3];
						pIn += 4;
						break;
					case ImgFmt_A8B8G8R8:
						r = pIn[0];
						g = pIn[1];
						b = pIn[2];
						a = pIn[3];
						pIn += 4;
						break;
					case ImgFmt_B8G8R8A8:
						a = pIn[0];
						r = pIn[1];
						g = pIn[2];
						b = pIn[3];
						pIn += 4;
						break;
					case ImgFmt_R8G8B8A8:
						a = pIn[0];
						b = pIn[1];
						g = pIn[2];
						r = pIn[3];
						pIn += 4;
						break;
					case ImgFmt_R8G8B8:
						b = pIn[0];
						g = pIn[1];
						r = pIn[2];
						a = 0xFF;
						pIn += 3;
						break;
					case ImgFmt_B8G8R8:
						r = pIn[0];
						g = pIn[1];
						b = pIn[2];
						a = 0xFF;
						pIn += 3;
						break;
					case ImgFmt_A2R10G10B10:
					case ImgFmt_A2B10G10R10:
						MFDebug_Assert(false, "Not done!");
						break;
					case ImgFmt_A16B16G16R16:
					{
						const uint16 *pIn16 = (const uint16*)pIn;
						r = (uint8)(pIn16[0] >> 8);
						g = (uint8)(pIn16[1] >> 8);
						b = (uint8)(pIn16[2] >> 8);
						a = (uint8)(pIn16[3] >> 8);
						pIn += 8;
						break;
					}
					case ImgFmt_R5G6B5:
					case ImgFmt_R6G5B5:
					case ImgFmt_B5G6R5:
					case ImgFmt_A1R5G5B5:
					case ImgFmt_R5G5B5A1:
					case ImgFmt_A1B5G5R5:
					case ImgFmt_A4R4G4B4:
					case ImgFmt_A4B4G4R4:
					case ImgFmt_R4G4B4A4:
					case ImgFmt_ABGR_F16:
						MFDebug_Assert(false, "Not done!");
						break;
					case ImgFmt_ABGR_F32:
					{
						const float *pInF = (const float*)pIn;
						r = (uint8)(pInF[0]*255.f + 0.5f);
						g = (uint8)(pInF[1]*255.f + 0.5f);
						b = (uint8)(pInF[2]*255.f + 0.5f);
						a = (uint8)(pInF[3]*255.f + 0.5f);
						pIn += 16;
						break;
					}
					case ImgFmt_R11G11B10_F:
					case ImgFmt_R9G9B9_E5:
						MFDebug_Assert(false, "Not done!");
						break;
					default:
						MFDebug_Assert(false, "Shouldn't be here!");
				}

				if(bTosRGB)
				{
					r = (uint8)(MFImage_LinearTosRGB(r * (1.f/255.f))*255.f + 0.5f);
					g = (uint8)(MFImage_LinearTosRGB(g * (1.f/255.f))*255.f + 0.5f);
					b = (uint8)(MFImage_LinearTosRGB(b * (1.f/255.f))*255.f + 0.5f);
				}
				else if(bToLinear)
				{
					r = (uint8)(MFImage_sRGBToLinear(r * (1.f/255.f))*255.f + 0.5f);
					g = (uint8)(MFImage_sRGBToLinear(g * (1.f/255.f))*255.f + 0.5f);
					b = (uint8)(MFImage_sRGBToLinear(b * (1.f/255.f))*255.f + 0.5f);
				}

				switch(outputFormat)
				{
					case ImgFmt_A8R8G8B8:
						pOut[0] = b;
						pOut[1] = g;
						pOut[2] = r;
						pOut[3] = a;
						pOut += 4;
						break;
					case ImgFmt_A8B8G8R8:
						pOut[0] = r;
						pOut[1] = g;
						pOut[2] = b;
						pOut[3] = a;
						pOut += 4;
						break;
					case ImgFmt_B8G8R8A8:
						pOut[0] = a;
						pOut[1] = r;
						pOut[2] = g;
						pOut[3] = b;
						pOut += 4;
						break;
					case ImgFmt_R8G8B8A8:
						pOut[0] = a;
						pOut[1] = b;
						pOut[2] = g;
						pOut[3] = r;
						pOut += 4;
						break;
					case ImgFmt_R8G8B8:
						pOut[0] = b;
						pOut[1] = g;
						pOut[2] = r;
						pOut += 3;
						break;
					case ImgFmt_B8G8R8:
						pOut[0] = r;
						pOut[1] = g;
						pOut[2] = b;
						pOut += 3;
						break;
					case ImgFmt_R5G6B5:
					{
						*(uint16*)pOut = ((uint16)(r & 0xF8) << 8) |
										 ((uint16)(g & 0xFC) << 3) |
										 ((uint16)(b & 0xF8) >> 3);
						pOut += 2;
						break;
					}
					case ImgFmt_R6G5B5:
					{
						*(uint16*)pOut = ((uint16)(r & 0xFC) << 8) |
										 ((uint16)(g & 0xF8) << 2) |
										 ((uint16)(b & 0xF8) >> 3);
						pOut += 2;
						break;
					}
					case ImgFmt_B5G6R5:
					{
						*(uint16*)pOut = ((uint16)(b & 0xF8) << 8) |
										 ((uint16)(g & 0xFC) << 3) |
										 ((uint16)(r & 0xF8) >> 3);
						pOut += 2;
						break;
					}
					case ImgFmt_A1R5G5B5:
					{
						*(uint16*)pOut = ((uint16)(a & 0x80) << 8) |
										 ((uint16)(r & 0xF8) << 7) |
										 ((uint16)(g & 0xF8) << 2) |
										 ((uint16)(b & 0xF8) >> 3);
						pOut += 2;
						break;
					}
					case ImgFmt_R5G5B5A1:
					{
						*(uint16*)pOut = ((uint16)(r & 0xF8) << 8) |
										 ((uint16)(g & 0xF8) << 3) |
										 ((uint16)(b & 0xF8) >> 2) |
										 ((uint16)(a & 0x80) >> 7);
						pOut += 2;
						break;
					}
					case ImgFmt_A1B5G5R5:
					{
						*(uint16*)pOut = ((uint16)(a & 0x80) << 8) |
										 ((uint16)(b & 0xF8) << 7) |
										 ((uint16)(g & 0xF8) << 2) |
										 ((uint16)(r & 0xF8) >> 3);
						pOut += 2;
						break;
					}
					case ImgFmt_A4R4G4B4:
					{
						*(uint16*)pOut = ((uint16)(a & 0xF0) << 8) |
										 ((uint16)(r & 0xF0) << 4) |
										 ((uint16)(g & 0xF0)) |
										 ((uint16)(b & 0xF0) >> 4);
						pOut += 2;
						break;
					}
					case ImgFmt_A4B4G4R4:
					{
						*(uint16*)pOut = ((uint16)(a & 0xF0) << 8) |
										 ((uint16)(b & 0xF0) << 4) |
										 ((uint16)(g & 0xF0)) |
										 ((uint16)(r & 0xF0) >> 4);
						pOut += 2;
						break;
					}
					case ImgFmt_R4G4B4A4:
					{
						*(uint16*)pOut = ((uint16)(r & 0xF0) << 8) |
										 ((uint16)(g & 0xF0) << 4) |
										 ((uint16)(b & 0xF0)) |
										 ((uint16)(a & 0xF0) >> 4);
						pOut += 2;
						break;
					}
					default:
						MFDebug_Assert(false, "Shouldn't be here!");
				}
			}
		}
	}
}


/*** Box filter ***/

template<typename Pixel>
static void Filter_Box(MFScaleImage *pScaleData)
{
	MFDebug_Assert(sizeof(Pixel)*8 == 128, "Only 128bit image formats are supported!");
}
template<>
void Filter_Box<MFVector>(MFScaleImage *pScaleData)
{
	MFVector *pSource = (MFVector*)pScaleData->pSourceImage;
	MFVector *pDest = (MFVector*)pScaleData->pTargetBuffer;
	int sourceWidth = pScaleData->sourceWidth;
	int sourceHeight = pScaleData->sourceHeight;
	int sourceStride = pScaleData->sourceStride;
	int destWidth = pScaleData->targetWidth;
	int destHeight = pScaleData->targetHeight;
	int destStride = pScaleData->targetStride;

	// we may only be scaling in one axis...
	int hStep = sourceWidth == destWidth ? 0 : 1;
	int vStep = sourceHeight == destHeight ? 0 : sourceStride;

	if(pScaleData->format & ImgFmt_Linear)
	{
		for(int y = 0; y < destHeight; ++y)
		{
			for(int x = 0; x < destWidth; ++x)
			{
				int sourceOffset = x*(1+hStep);

				MFVector &tl = pSource[sourceOffset];
				MFVector &tr = pSource[sourceOffset + hStep];
				MFVector &bl = pSource[sourceOffset + vStep];
				MFVector &br = pSource[sourceOffset + hStep + vStep];

				pDest[x] = (tl + tr + bl + br) * MakeVector(0.25f);
			}

			pSource += sourceStride+vStep;
			pDest += destStride;
		}
	}
	else
	{
		for(int y = 0; y < destHeight; ++y)
		{
			for(int x = 0; x < destWidth; ++x)
			{
				int sourceOffset = x*(1+hStep);

				MFVector tl = pSource[sourceOffset];
				MFVector tr = pSource[sourceOffset + hStep];
				MFVector bl = pSource[sourceOffset + vStep];
				MFVector br = pSource[sourceOffset + hStep + vStep];

				// convert to linear light space
				tl.x = MFImage_sRGBToLinear(tl.x);
				tl.y = MFImage_sRGBToLinear(tl.y);
				tl.z = MFImage_sRGBToLinear(tl.z);
				tr.x = MFImage_sRGBToLinear(tr.x);
				tr.y = MFImage_sRGBToLinear(tr.y);
				tr.z = MFImage_sRGBToLinear(tr.z);
				bl.x = MFImage_sRGBToLinear(bl.x);
				bl.y = MFImage_sRGBToLinear(bl.y);
				bl.z = MFImage_sRGBToLinear(bl.z);
				br.x = MFImage_sRGBToLinear(br.x);
				br.y = MFImage_sRGBToLinear(br.y);
				br.z = MFImage_sRGBToLinear(br.z);

				MFVector t = (tl + tr + bl + br) * MakeVector(0.25f);

				// convert back to sRGB
				t.x = MFImage_LinearTosRGB(t.x);
				t.y = MFImage_LinearTosRGB(t.y);
				t.z = MFImage_LinearTosRGB(t.z);

				pDest[x] = t;
			}

			pSource += sourceStride+vStep;
			pDest += destStride;
		}
	}
}


/*** HXQ 2x,3x,4x algorithm ***/

#if defined(HQX_SUPPORT)
static bool hqxInitialised = false;
#endif
template<typename Pixel>
static void Filter_HQX(MFScaleImage *pScaleData)
{
	MFDebug_Assert(sizeof(Pixel)*8 == 32, "Only 32bit image formats are supported!");
}
template<>
void Filter_HQX<uint32>(MFScaleImage *pScaleData)
{
#if defined(HQX_SUPPORT)
	uint32 *pSource = (uint32*)pScaleData->pSourceImage;
	uint32 *pDest = (uint32*)pScaleData->pTargetBuffer;
	int sourceWidth = pScaleData->sourceWidth;
	int sourceHeight = pScaleData->sourceHeight;
	int sourceStride = pScaleData->sourceStride;
	int destWidth = pScaleData->targetWidth;
	int destHeight = pScaleData->targetHeight;
	int destStride = pScaleData->targetStride;

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
static void Filter_AdvMAME(MFScaleImage *pScaleData)
{
	T *pSource = (T*)pScaleData->pSourceImage;
	T *pDest = (T*)pScaleData->pTargetBuffer;
	int sourceWidth = pScaleData->sourceWidth;
	int sourceHeight = pScaleData->sourceHeight;
	int sourceStride = pScaleData->sourceStride;
	int destWidth = pScaleData->targetWidth;
	int destStride = pScaleData->targetStride;

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
static void Filter_Eagle(MFScaleImage *pScaleData)
{
	Pixel *pSource = (Pixel*)pScaleData->pSourceImage;
	Pixel *pDest = (Pixel*)pScaleData->pTargetBuffer;
	int sourceWidth = pScaleData->sourceWidth;
	int sourceHeight = pScaleData->sourceHeight;
	int sourceStride = pScaleData->sourceStride;
	int destStride = pScaleData->targetStride;

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
			if(sourceWidth/2 == destWidth)
				scalex = 0.5f;
			else if(sourceWidth == destWidth)
				scalex = 1.f;
			if(sourceHeight/2 == destHeight)
				scaley = 0.5f;
			else if(sourceHeight == destHeight)
				scaley = 1.f;
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
	// check that the sizes are supported by the scaling algorithm
	float scalex, scaley;
	GetScaleFactors(pScaleData->algorithm, pScaleData->sourceWidth, pScaleData->sourceHeight, pScaleData->targetWidth, pScaleData->targetHeight, scalex, scaley);

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
				Filter_Box<Pixel>(pScaleData);
				break;
			case SA_HQX:
				Filter_HQX<Pixel>(pScaleData);
				break;
			case SA_AdvMAME:
				Filter_AdvMAME<Pixel>(pScaleData);
				break;
			case SA_Eagle:
				Filter_Eagle<Pixel>(pScaleData);
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
