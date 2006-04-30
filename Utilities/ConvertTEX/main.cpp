#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "MFSystem.h"

#include "ConvertTex.h"
#include "IntImage.h"
#include "LoadTarga.h"
#include "LoadBMP.h"

#if defined(WIN32)
#include <d3d8.h>
#include <xgraphics.h>
#endif

void LOGERROR(const char *pFormat, ...)
{
	va_list arglist;
	va_start(arglist, pFormat);

	vprintf(pFormat, arglist);
	getc(stdin);
}

int ConvertSurface(SourceImageLevel *pSourceSurface, MFTextureSurfaceLevel *pOutputSurface, MFTextureFormat targetFormat);
void Swizzle_PSP(char* out, const char* in, uint32 width, uint32 height, MFTextureFormat format);

int main(int argc, char *argv[])
{
	MFPlatform platform = FP_Unknown;
	MFTextureFormat targetFormat = TexFmt_Unknown;

	char fileName[256] = "";
	char outFile[256] = "";

	SourceImage *pImage = NULL;

	int a;

	// process command line
	for(a=1; a<argc; a++)
	{
		if(argv[a][0] == '-' || argv[a][0] == '/')
		{
			for(int b=0; b<FP_Max; b++)
			{
				if(!stricmp(&argv[a][1], MFSystem_GetPlatformString(b)))
				{
					platform = (MFPlatform)b;
					break;
				}
			}

			if(!strnicmp(&argv[a][1], "format", 6))
			{
				const char *pFormatString = &argv[a][7];

				while(pFormatString[0] == ' ' || pFormatString[0] == '\t' || pFormatString[0] == '=')
				 ++pFormatString;

				int b = 0;
				for(; b<TexFmt_Max; b++)
				{
					if(!stricmp(pFormatString, MFTexture_GetFormatString(b)))
					{
						(int&)targetFormat = b;
						break;
					}
				}

				if(b == TexFmt_Max)
				{
					printf("Unknown texture format '%s'..\n", pFormatString);
					return 1;
				}
			}
			else if(!stricmp(&argv[a][1], "v") || !stricmp(&argv[a][1], "version"))
			{
				// print version
				return 0;
			}
			else if(!stricmp(&argv[a][1], "l") || !stricmp(&argv[a][1], "listavailable"))
			{
				// list formats for platform
				for(int f=0; f<TexFmt_Max; f++)
				{
					if(MFTexture_IsAvailableOnPlatform(f, platform))
					{
						printf("%s\n", MFTexture_GetFormatString(f));
					}
				}
				gets(outFile);
				return 0;
			}
			else if(!stricmp(&argv[a][1], "a") || !stricmp(&argv[a][1], "listall"))
			{
				// list all available formats
				for(int f=0; f<TexFmt_Max; f++)
				{
					printf("%s\t", MFTexture_GetFormatString(f));

					bool printPipe = false;

					for(int g = 0; g<FP_Max; g++)
					{
						if(MFTexture_IsAvailableOnPlatform(f, g))
						{
							printf("%s%s", printPipe ? "|" : "", MFSystem_GetPlatformString(g));
							printPipe = true;
						}
					}

					printf("\n");
				}
				gets(outFile);
				return 0;
			}
		}
		else
		{
			if(!fileName[0])
				strcpy(fileName, argv[a]);
			else if(!outFile[0])
				strcpy(outFile, argv[a]);
		}
	}

	if(platform == FP_Unknown)
	{
		LOGERROR("No platform specified...\n");
		return 1;
	}

	if(!fileName[0])
	{
		LOGERROR("No file specified...\n");
		return 1;
	}

	if(!outFile[0])
	{
		// generate output filename
		strcpy(outFile, fileName);
		for(int i=(int)strlen(outFile); --i; )
		{
			if(outFile[i] == '.')
			{
				outFile[i] = 0;
				break;
			}
		}

		strcat(outFile, ".tex");
	}

	// load image
	int fileNameLen = (int)strlen(fileName);

	if(!stricmp(&fileName[fileNameLen-3], "tga"))
	{
		pImage = LoadTarga(fileName);
	}
	else if(!stricmp(&fileName[fileNameLen-3], "bmp"))
	{
		pImage = LoadBMP(fileName);
	}
	else
	{
		LOGERROR("Unsupported source image format..\n");
		return 1;
	}

	if(!pImage)
	{
		LOGERROR("Unable to load source image...\n");
		return 2;
	}

	// chuck some source image warnings
	if(pImage->pLevels[0].width < 8 || pImage->pLevels[0].height < 8)
	{
		LOGERROR("Textures should be a minimum of 8x8 pixels.\n");
	}

	// choose target image format
	if(targetFormat == TexFmt_Unknown)
	{
		// choose target format..
		switch(platform)
		{
			case FP_PC:
				targetFormat = TexFmt_A8R8G8B8;
				break;

			case FP_XBox:
				targetFormat = TexFmt_A8R8G8B8;
				break;

			case FP_Linux:
				targetFormat = TexFmt_A8B8G8R8;
				break;

			case FP_PSP:
				if(pImage->opaque)
					targetFormat = TexFmt_PSP_B5G6R5s;
				else if(pImage->oneBitAlpha)
					targetFormat = TexFmt_PSP_A1B5G5R5s;
				else
					targetFormat = TexFmt_PSP_A4B4G4R4s;
				break;

			default:
				targetFormat = TexFmt_A8B8G8R8;
				break;
		};
	}

	// check minimum pitch
	if((pImage->pLevels[0].width*MFTexture_GetBitsPerPixel(targetFormat)) / 8 < 64)
	{
		LOGERROR("Textures should have a minimum pitch of 64 bytes.\n");
		return 1;
	}

	// check power of 2 dimensions
	if(0)
	{
		LOGERROR("Texture dimensions are not a power of 2.\n");
	}

	//
	// begin processing...
	//

	// calculate texture data size..
	uint32 imageBytes = sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pImage->mipLevels;

	for(a=0; a<pImage->mipLevels; a++)
	{
		imageBytes += (pImage->pLevels[a].width*pImage->pLevels[a].height * MFTexture_GetBitsPerPixel(targetFormat)) / 8;

		// add palette
		uint32 paletteBytes = 0;

		if(targetFormat == TexFmt_I8)
			paletteBytes = 4*256;
		if(targetFormat == TexFmt_I4)
			paletteBytes = 4*16;

		imageBytes += paletteBytes;
	}

	// allocate buffer
	char *pOutputBuffer = (char*)malloc(imageBytes);

	MFTextureTemplateData *pTemplate = (MFTextureTemplateData*)pOutputBuffer;
	memset(pTemplate, 0, sizeof(MFTextureTemplateData));

	pTemplate->magicNumber = MFMAKEFOURCC('F','T','E','X');

	pTemplate->imageFormat = targetFormat;
	pTemplate->platformFormat = gMFTexturePlatformFormat[(int)platform][(int)targetFormat];

	if(targetFormat >= TexFmt_XB_A8R8G8B8s)
		pTemplate->swizzled = 1;

	if(pImage->opaque = true)
		pTemplate->alpha = 0;
	else if(pImage->oneBitAlpha)
		pTemplate->alpha = 3;
	else
		pTemplate->alpha = 1;

	pTemplate->mipLevels = pImage->mipLevels;
	pTemplate->pSurfaces = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	MFTextureSurfaceLevel *pSurfaceLevels = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	char *pDataPointer = pOutputBuffer + sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pImage->mipLevels;

	for(a=0; a<pImage->mipLevels; a++)
	{
		memset(&pSurfaceLevels[a], 0, sizeof(MFTextureSurfaceLevel));

		pSurfaceLevels[a].width = pImage->pLevels[a].width;
		pSurfaceLevels[a].height = pImage->pLevels[a].height;
		pSurfaceLevels[a].bitsPerPixel = MFTexture_GetBitsPerPixel(targetFormat);

		pSurfaceLevels[a].xBlocks = -1;
		pSurfaceLevels[a].yBlocks = -1;
		pSurfaceLevels[a].bitsPerBlock = -1;

		pSurfaceLevels[a].pImageData = pDataPointer;
		pSurfaceLevels[a].bufferLength = (pImage->pLevels[a].width*pImage->pLevels[a].height * MFTexture_GetBitsPerPixel(targetFormat)) / 8;
		pDataPointer += pSurfaceLevels[a].bufferLength;

		uint32 paletteBytes = 0;

		if(targetFormat == TexFmt_I8)
			paletteBytes = 4*256;
		if(targetFormat == TexFmt_I4)
			paletteBytes = 4*16;

		if(paletteBytes)
		{
			pSurfaceLevels[a].pImageData = pDataPointer;
			pSurfaceLevels[a].paletteBufferLength = paletteBytes;
			pDataPointer += paletteBytes;
		}

		// convert surface
		ConvertSurface(&pImage->pLevels[a], &pSurfaceLevels[a], targetFormat);
	}

	// destroy source
	DestroyImage(pImage);

	// fix up pointers
	for(a=0; a<pTemplate->mipLevels; a++)
	{
		FixUp(pTemplate->pSurfaces[a].pImageData, pOutputBuffer, 0);
		FixUp(pTemplate->pSurfaces[a].pPaletteEntries, pOutputBuffer, 0);
	}
	FixUp(pTemplate->pSurfaces, pOutputBuffer, 0);

	// write out texture..
	FILE *pFile = fopen(outFile, "wb");

	if(!pFile)
	{
		LOGERROR("Couldnt open output file...\n");
		return 1;
	}

	fwrite(pOutputBuffer, 1, imageBytes, pFile);
	fclose(pFile);

	free(pOutputBuffer);

	printf(MFStr("> %s\n", outFile));
	// done! :)

	return 0;
}

int ConvertSurface(SourceImageLevel *pSourceSurface, MFTextureSurfaceLevel *pOutputSurface, MFTextureFormat targetFormat)
{
	// convert image...
	int width = pSourceSurface->width;
	int height = pSourceSurface->height;

	int x, y;
	Pixel *pSource = pSourceSurface->pData;

	switch(targetFormat)
	{
		case TexFmt_A8R8G8B8:
		case TexFmt_XB_A8R8G8B8s:
		{
			uint32 *pTarget = (uint32*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint32)(pSource->a*255.0f) & 0xFF) << 24 |
								((uint32)(pSource->r*255.0f) & 0xFF) << 16 |
								((uint32)(pSource->g*255.0f) & 0xFF) << 8 |
								((uint32)(pSource->b*255.0f) & 0xFF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A8B8G8R8:
		case TexFmt_XB_A8B8G8R8s:
		case TexFmt_PSP_A8B8G8R8s:
		{
			uint32 *pTarget = (uint32*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint32)(pSource->a*255.0f) & 0xFF) << 24 |
								((uint32)(pSource->b*255.0f) & 0xFF) << 16 |
								((uint32)(pSource->g*255.0f) & 0xFF) << 8 |
								((uint32)(pSource->r*255.0f) & 0xFF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_B8G8R8A8:
		case TexFmt_XB_B8G8R8A8s:
		{
			uint32 *pTarget = (uint32*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint32)(pSource->b*255.0f) & 0xFF) << 24 |
								((uint32)(pSource->g*255.0f) & 0xFF) << 16 |
								((uint32)(pSource->r*255.0f) & 0xFF) << 8 |
								((uint32)(pSource->a*255.0f) & 0xFF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_R8G8B8A8:
		case TexFmt_XB_R8G8B8A8s:
		{
			uint32 *pTarget = (uint32*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint32)(pSource->r*255.0f) & 0xFF) << 24 |
								((uint32)(pSource->g*255.0f) & 0xFF) << 16 |
								((uint32)(pSource->b*255.0f) & 0xFF) << 8 |
								((uint32)(pSource->a*255.0f) & 0xFF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A2R10G10B10:
		{
			uint32 *pTarget = (uint32*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint32)(pSource->a*3.0f) & 0x3) << 30 |
								((uint32)(pSource->r*1023.0f) & 0x3FF) << 20 |
								((uint32)(pSource->g*1023.0f) & 0x3FF) << 10 |
								((uint32)(pSource->b*1023.0f) & 0x3FF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A2B10G10R10:
		{
			uint32 *pTarget = (uint32*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint32)(pSource->a*3.0f) & 0x3) << 30 |
								((uint32)(pSource->b*1023.0f) & 0x3FF) << 20 |
								((uint32)(pSource->g*1023.0f) & 0x3FF) << 10 |
								((uint32)(pSource->r*1023.0f) & 0x3FF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A16B16G16R16:
		{
			uint64 *pTarget = (uint64*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint64)(pSource->a*65535.0f) & 0xFFFF) << 48 |
								((uint64)(pSource->b*65535.0f) & 0xFFFF) << 32 |
								((uint64)(pSource->g*65535.0f) & 0xFFFF) << 16 |
								((uint64)(pSource->r*65535.0f) & 0xFFFF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_R5G6B5:
		case TexFmt_XB_R5G6B5s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->r*31.0f) & 0x1F) << 11 |
								((uint16)(pSource->g*63.0f) & 0x3F) << 5 |
								((uint16)(pSource->b*31.0f) & 0x1F);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_R6G5B5:
		case TexFmt_XB_R6G5B5s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->r*63.0f) & 0x3F) << 10 |
								((uint16)(pSource->g*31.0f) & 0x1F) << 5 |
								((uint16)(pSource->b*31.0f) & 0x1F);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_B5G6R5:
		case TexFmt_PSP_B5G6R5s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->b*31.0f) & 0x1F) << 11 |
								((uint16)(pSource->g*63.0f) & 0x3F) << 5 |
								((uint16)(pSource->r*31.0f) & 0x1F);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A1R5G5B5:
		case TexFmt_XB_A1R5G5B5s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->a*1.0f) & 0x1) << 15 |
								((uint16)(pSource->r*31.0f) & 0x1F) << 10 |
								((uint16)(pSource->g*31.0f) & 0x1F) << 5 |
								((uint16)(pSource->b*31.0f) & 0x1F);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_R5G5B5A1:
		case TexFmt_XB_R5G5B5A1s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->a*1.0f) & 0x1) |
								((uint16)(pSource->r*31.0f) & 0x1F) << 11 |
								((uint16)(pSource->g*31.0f) & 0x1F) << 6 |
								((uint16)(pSource->b*31.0f) & 0x1F) << 1;
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A1B5G5R5:
		case TexFmt_PSP_A1B5G5R5s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->a*1.0f) & 0x1) << 15 |
								((uint16)(pSource->b*31.0f) & 0x1F) << 10 |
								((uint16)(pSource->g*31.0f) & 0x1F) << 5 |
								((uint16)(pSource->r*31.0f) & 0x1F);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A4R4G4B4:
		case TexFmt_XB_A4R4G4B4s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->a*15.0f) & 0xF) << 12 |
								((uint16)(pSource->r*15.0f) & 0xF) << 8 |
								((uint16)(pSource->g*15.0f) & 0xF) << 4 |
								((uint16)(pSource->b*15.0f) & 0xF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_A4B4G4R4:
		case TexFmt_PSP_A4B4G4R4s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->a*15.0f) & 0xF) << 12 |
								((uint16)(pSource->b*15.0f) & 0xF) << 8 |
								((uint16)(pSource->g*15.0f) & 0xF) << 4 |
								((uint16)(pSource->r*15.0f) & 0xF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_R4G4B4A4:
		case TexFmt_XB_R4G4B4A4s:
		{
			uint16 *pTarget = (uint16*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = ((uint16)(pSource->r*15.0f) & 0xF) << 12 |
								((uint16)(pSource->g*15.0f) & 0xF) << 8 |
								((uint16)(pSource->b*15.0f) & 0xF) << 4 |
								((uint16)(pSource->a*15.0f) & 0xF);
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_ABGR_F16:
		{
			uint64 *pTarget = (uint64*)pOutputSurface->pImageData;
			uint32 c;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					c = (uint32&)pSource->a;
					*pTarget = (uint64)((c & 0xFC000000) >> 16 | (c & 0x007FC000) >> 13) << 48;
					c = (uint32&)pSource->b;
					*pTarget |= (uint64)((c & 0xFC000000) >> 16 | (c & 0x007FC000) >> 13) << 32;
					c = (uint32&)pSource->g;
					*pTarget |= (uint64)((c & 0xFC000000) >> 16 | (c & 0x007FE000) >> 13) << 16;
					c = (uint32&)pSource->r;
					*pTarget |= (uint64)((c & 0xFC000000) >> 16 | (c & 0x007FC000) >> 13);

					++pTarget;
					++pSource;
				}
			}
			break;
		}

		case TexFmt_ABGR_F32:
		{
			float *pTarget = (float*)pOutputSurface->pImageData;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					*pTarget = pSource->r;
					++pTarget;
					*pTarget = pSource->g;
					++pTarget;
					*pTarget = pSource->b;
					++pTarget;
					*pTarget = pSource->a;
					++pTarget;
					++pSource;
				}
			}
			break;
		}

		default:
		{
			LOGERROR("Conversion for target type '%s' not yet support...\n", MFTexture_GetFormatString(targetFormat));
			return 1;
		}
	}

	// test for swizzled format..
	if(targetFormat >= TexFmt_XB_A8R8G8B8s)
	{
		uint32 imageBytes = (width * height * MFTexture_GetBitsPerPixel(targetFormat)) / 8;
		uint32 bytesperpixel = MFTexture_GetBitsPerPixel(targetFormat) / 8;

		char *pBuffer = (char*)malloc(imageBytes);

#if defined(WIN32)
		if(targetFormat >= TexFmt_XB_A8R8G8B8s && targetFormat <= TexFmt_XB_R4G4B4A4s)
		{
			// swizzle for xbox
			XGSwizzleRect(pOutputSurface->pImageData, 0, NULL, pBuffer, width, height, NULL, bytesperpixel);
		}
		else
#endif
		if(targetFormat >= TexFmt_PSP_A8B8G8R8s && targetFormat <= TexFmt_PSP_DXT5s)
		{
			// swizzle for PSP
			Swizzle_PSP(pBuffer, pOutputSurface->pImageData, width, height, targetFormat);
		}

		memcpy(pOutputSurface->pImageData, pBuffer, imageBytes);
		free(pBuffer);
	}

	return 0;
}

void Swizzle_PSP(char* out, const char* in, uint32 width, uint32 height, MFTextureFormat format)
{
	uint32 blockx, blocky;
	uint32 j;

	// calculate width in bytes
	width = (width * MFTexture_GetBitsPerPixel(format)) / 8;

	uint32 width_blocks = (width / 16);
	uint32 height_blocks = (height / 8);

	uint32 src_pitch = (width-16)/4;
	uint32 src_row = width * 8;

	const char* ysrc = in;
	uint32* dst = (uint32*)out;

	for(blocky = 0; blocky < height_blocks; ++blocky)
	{
		const char* xsrc = ysrc;
		for(blockx = 0; blockx < width_blocks; ++blockx)
		{
			const uint32* src = (uint32*)xsrc;
			for(j = 0; j < 8; ++j)
			{
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				src += src_pitch;
			}
			xsrc += 16;
		}
		ysrc += src_row;
	}
}
