#include "Common.h"
#include "MFTexture_Internal.h"

#include "ConvertTex.h"
#include "IntImage.h"
#include "LoadTarga.h"

#include <d3d8.h>
#include <xgraphics.h>

int ConvertSurface(SourceImageLevel *pSourceSurface, MFTextureSurfaceLevel *pOutputSurface, MFTextureFormats targetFormat);

int main(int argc, char *argv[])
{
	FujiPlatforms platform = FP_Unknown;
	MFTextureFormats targetFormat = TexFmt_Unknown;

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
				if(!stricmp(&argv[a][1], gPlatformStrings[b]))
				{
					platform = (FujiPlatforms)b;
					break;
				}
			}

			if(!strnicmp(&argv[a][1], "format", 6))
			{
				const char *pFormatString = &argv[a][7];

				while(pFormatString[0] == ' ' || pFormatString[0] == '\t' || pFormatString[0] == '=')
				 ++pFormatString;

				for(int b=0; b<TexFmt_Max; b++)
				{
					if(!stricmp(pFormatString, gpMFTextureFormatStrings[b]))
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
			else if(!stricmp(&argv[a][1], "l") || !stricmp(&argv[a][1], "list"))
			{
				// list formats for platform
				for(int f=0; f<TexFmt_Max; f++)
				{
					if(gMFTexturePlatformAvailability[f] & 1<<(uint32)platform)
					{
						printf("%s\n", gpMFTextureFormatStrings[f]);
					}
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
		printf("No platform specified...\n");
		return 1;
	}

	if(!fileName[0])
	{
		printf("No file specified...\n");
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
	else
	{
		printf("Unsupported source image format..\n");
		return 1;
	}

	if(!pImage)
	{
		printf("Unable to load source image...\n");
		return 2;
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
				targetFormat = TexFmt_A4B4G4R4;
				break;
		};
	}

	// verify format is available on target platform
	if((gMFTexturePlatformAvailability[(int)targetFormat] & (uint32)BIT(platform)) == 0)
	{
		printf("Desired texture format is unavailable on target platform..\n");
		return 1;
	}

	//
	// begin processing...
	//

	// calculate texture data size..
	uint32 imageBytes = sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pImage->mipLevels;

	for(a=0; a<pImage->mipLevels; a++)
	{
		imageBytes += (pImage->pLevels[a].width*pImage->pLevels[a].height * gMFTextureBitsPerPixel[(int)targetFormat]) / 8;

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

	pTemplate->magicNumber = MAKEFOURCC('F','T','E','X');

	pTemplate->imageFormat = targetFormat;
	pTemplate->platformFormat = gMFTexturePlatformFormat[(int)platform][(int)targetFormat];

	pTemplate->mipLevels = pImage->mipLevels;
	pTemplate->pSurfaces = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	MFTextureSurfaceLevel *pSurfaceLevels = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	char *pDataPointer = pOutputBuffer + sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pImage->mipLevels;

	for(a=0; a<pImage->mipLevels; a++)
	{
		memset(&pSurfaceLevels[a], 0, sizeof(MFTextureSurfaceLevel));

		pSurfaceLevels[a].width = pImage->pLevels[a].width;
		pSurfaceLevels[a].height = pImage->pLevels[a].height;
		pSurfaceLevels[a].bitsPerPixel = gMFTextureBitsPerPixel[(int)targetFormat];

		pSurfaceLevels[a].xBlocks = -1;
		pSurfaceLevels[a].yBlocks = -1;
		pSurfaceLevels[a].bitsPerBlock = -1;

		pSurfaceLevels[a].pImageData = pDataPointer;
		pSurfaceLevels[a].bufferLength = (pImage->pLevels[a].width*pImage->pLevels[a].height * gMFTextureBitsPerPixel[(int)targetFormat]) / 8;
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
		printf("Couldnt open output file...\n");
		return 1;
	}

	fwrite(pOutputBuffer, 1, imageBytes, pFile);
	fclose(pFile);

	free(pOutputBuffer);

	printf(STR("> %s\n", outFile));
	// done! :)

	return 0;
}

int ConvertSurface(SourceImageLevel *pSourceSurface, MFTextureSurfaceLevel *pOutputSurface, MFTextureFormats targetFormat)
{
	// convert image...
	int width = pSourceSurface->width;
	int height = pSourceSurface->height;

	int x, y;
	Pixel *pSource = pSourceSurface->pData;

	switch(targetFormat)
	{
		case TexFmt_A8R8G8B8:
		case TexFmt_XB_A8R8G8B8:
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
		case TexFmt_XB_A8B8G8R8:
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
		case TexFmt_XB_R5G6B5:
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

		case TexFmt_A1R5G5B5:
		case TexFmt_XB_A1R5G5B5:
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

		case TexFmt_A4R4G4B4:
		case TexFmt_XB_A4R4G4B4:
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

		case TexFmt_ABGR_F16:
		{
			uint64 *pTarget = (uint64*)pOutputSurface->pImageData;
			uint32 c;

			for(y=0; y<height; y++)
			{
				for(x=0; x<width; x++)
				{
					c = (uint32&)pSource->a;
					*pTarget = ((c & 0xFC000000) >> 16 | (c & 0x007FC000) >> 13) << 48;
					c = (uint32&)pSource->b;
					*pTarget |= ((c & 0xFC000000) >> 16 | (c & 0x007FC000) >> 13) << 32;
					c = (uint32&)pSource->g;
					*pTarget |= ((c & 0xFC000000) >> 16 | (c & 0x007FE000) >> 13) << 16;
					c = (uint32&)pSource->r;
					*pTarget |= ((c & 0xFC000000) >> 16 | (c & 0x007FC000) >> 13);

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
			printf("Conversion for target type not yet support...\n");
			return 1;
		}
	}

	// xbox supports swizzled formats..
	if(targetFormat >= TexFmt_XB_A8R8G8B8)
	{
		uint32 bytesperpixel = gMFTextureBitsPerPixel[targetFormat] / 8;
		uint32 imageBytes = width * height * bytesperpixel;

		char *pBuffer = (char*)malloc(imageBytes);
		XGSwizzleRect(pOutputSurface->pImageData, 0, NULL, pBuffer, width, height, NULL, bytesperpixel);
		memcpy(pOutputSurface->pImageData, pBuffer, imageBytes);
		free(pBuffer);
	}

	return 0;
}

