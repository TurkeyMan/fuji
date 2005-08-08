#include "Common.h"

#include "ConvertTex.h"
#include "IntImage.h"
#include "LoadTarga.h"

#include <d3d8.h>
#include <xgraphics.h>


const char * const pFormatStrings[TexFmt_Max] =
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

uint32 platformAvailability[TexFmt_Max] =
{
	BIT(TP_PC)|BIT(TP_XBox),	// TexFmt_A8R8G8B8
	BIT(TP_PSP),				// TexFmt_A8B8G8R8

	BIT(TP_PC),					// TexFmt_A2R10G10B10
	BIT(TP_PC),					// TexFmt_A2B10G10R10
	BIT(TP_PC),					// TexFmt_A16B16G16R16

	BIT(TP_PC),					// TexFmt_R5G6B5
	BIT(TP_PC),					// TexFmt_A1R5G5B5

	BIT(TP_PC),					// TexFmt_A4R4G4B4
	BIT(TP_PSP),				// TexFmt_A4B4G4R4

	BIT(TP_PC),					// TexFmt_ABGR_F16
	BIT(TP_PC),					// TexFmt_ABGR_F32

	BIT(TP_PSP),				// TexFmt_I8
	BIT(TP_PSP),				// TexFmt_I4

	BIT(TP_PC)|BIT(TP_XBox)|BIT(TP_PSP),	// TexFmt_DXT1
	BIT(TP_PC)|BIT(TP_XBox),				// TexFmt_DXT2
	BIT(TP_PC)|BIT(TP_XBox)|BIT(TP_PSP),	// TexFmt_DXT3
	BIT(TP_PC)|BIT(TP_XBox),				// TexFmt_DXT4
	BIT(TP_PC)|BIT(TP_XBox)|BIT(TP_PSP)		// TexFmt_DXT5
};

uint32 bitsPerPixel[TexFmt_Max] =
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

uint32 platformFormat[TP_Max][TexFmt_Max] =
{
	{ // PC
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
	},

	{ // XBox
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
	},

	{ // Linux
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
	},

	{ // PSP
		0,	//					TexFmt_A8R8G8B8
		3,	// SCEGU_PF8888, //	TexFmt_A8B8G8R8
		0,	//					TexFmt_A2R10G10B10
		0,	//					TexFmt_A2B10G10R10
		0,	//					TexFmt_A16B16G16R16
		0,	// SCEGU_PF5650, //	TexFmt_R5G6B5
		1,	// SCEGU_PF5551, //	TexFmt_A1R5G5B5
		0,	//					TexFmt_A4R4G4B4
		2,	// SCEGU_PF4444, //	TexFmt_A4B4G4R4
		0,	//					TexFmt_ABGR_F16
		0,	//					TexFmt_ABGR_F32
		5,	// SCEGU_PFIDX8, //	TexFmt_I8
		4,	// SCEGU_PFIDX4, //	TexFmt_I4
		8,	// SCEGU_PFDXT1, //	TexFmt_DXT1
		0,	//					TexFmt_DXT2
		9,	// SCEGU_PFDXT3, //	TexFmt_DXT3
		0,	//					TexFmt_DXT4
		10	// SCEGU_PFDXT5, //	TexFmt_DXT5
	}
};

// texture TemplateData

struct MFTextureSurfaceLevel;

struct MFTextureTemplateData
{
	uint32 magicNumber;

	ImageFormats imageFormat;
	uint32 platformFormat;

	int mipLevels;
	int opaque;

	uint32 res[2];

	MFTextureSurfaceLevel *pSurfaces;
};

struct MFTextureSurfaceLevel
{
	int width, height;
	int bitsPerPixel;

	int xBlocks, yBlocks;
	int bitsPerBlock;

	char *pImageData;
	int bufferLength;

	char *pPaletteEntries;
	int paletteBufferLength;

	uint32 res[2];
};

////////////////////////////////

int ConvertSurface(SourceImageLevel *pSourceSurface, MFTextureSurfaceLevel *pOutputSurface, ImageFormats targetFormat);

////////////////////////////////

int main(int argc, char *argv[])
{
	TargetPlatform platform = TP_Unknown;
	ImageFormats targetFormat = TexFmt_Unknown;

	char fileName[256] = "";
	char outFile[256] = "";

	SourceImage *pImage = NULL;

	int a;

	// process command line
	for(a=1; a<argc; a++)
	{
		if(argv[a][0] == '-' || argv[a][0] == '/')
		{
			if(!stricmp(&argv[a][1], "pc"))
			{
				platform = TP_PC;
			}
			else if(!stricmp(&argv[a][1], "linux") || !stricmp(&argv[a][1], "lnx"))
			{
				platform = TP_Linux;
			}
			else if(!stricmp(&argv[a][1], "xbox") || !stricmp(&argv[a][1], "xb"))
			{
				platform = TP_XBox;
			}
			else if(!stricmp(&argv[a][1], "psp"))
			{
				platform = TP_PSP;
			}
			else if(!stricmp(&argv[a][1], "format"))
			{
				const char *pFormatString = &argv[a][7];

				while(pFormatString[0] != 0 || pFormatString[0] == ' ' || pFormatString[0] == '\t' || pFormatString[0] == '=')
				 ++pFormatString;

				for(int b=0; b<TexFmt_Max; b++)
				{
					if(!stricmp(pFormatString, pFormatStrings[b]))
					{
						(int&)targetFormat = b;
						break;
					}
				}

				if(b == TexFmt_Max)
				{
					printf("Unknown texture format '%s'..", pFormatString);
					return 1;
				}
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

	if(platform == TP_Unknown)
	{
		printf("No platform specified...");
		return 1;
	}

	if(!fileName[0])
	{
		printf("No file specified...");
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
		printf("Unsupported source image format..");
		return 1;
	}

	if(!pImage)
	{
		printf("Unable to load source image...");
		return 2;
	}

	// choose target image format
	if(targetFormat == TexFmt_Unknown)
	{
		// choose target format..
		switch(platform)
		{
			case TP_PC:
				targetFormat = TexFmt_A8R8G8B8;
				break;

			case TP_XBox:
				targetFormat = TexFmt_A8R8G8B8;
				break;

			case TP_Linux:
				targetFormat = TexFmt_A8B8G8R8;
				break;

			case TP_PSP:
				targetFormat = TexFmt_A4B4G4R4;
				break;
		};
	}

	// verify format is available on target platform
	if((platformAvailability[(int)targetFormat] & (uint32)BIT(platform)) == 0)
	{
		printf("Desired texture format is unavailable on target platform..");
		return 1;
	}

	//
	// begin processing...
	//

	// calculate texture data size..
	uint32 imageBytes = sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pImage->mipLevels;

	for(a=0; a<pImage->mipLevels; a++)
	{
		imageBytes += (pImage->pLevels[a].width*pImage->pLevels[a].height * bitsPerPixel[(int)targetFormat]) / 8;

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
	pTemplate->platformFormat = platformFormat[(int)platform][(int)targetFormat];

	pTemplate->mipLevels = pImage->mipLevels;
	pTemplate->pSurfaces = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	MFTextureSurfaceLevel *pSurfaceLevels = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	char *pDataPointer = pOutputBuffer + sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pImage->mipLevels;

	for(a=0; a<pImage->mipLevels; a++)
	{
		memset(&pSurfaceLevels[a], 0, sizeof(MFTextureSurfaceLevel));

		pSurfaceLevels[a].width = pImage->pLevels[a].width;
		pSurfaceLevels[a].height = pImage->pLevels[a].height;
		pSurfaceLevels[a].bitsPerPixel = bitsPerPixel[(int)targetFormat];

		pSurfaceLevels[a].xBlocks = -1;
		pSurfaceLevels[a].yBlocks = -1;
		pSurfaceLevels[a].bitsPerBlock = -1;

		pSurfaceLevels[a].pImageData = pDataPointer;
		pSurfaceLevels[a].bufferLength = (pImage->pLevels[a].width*pImage->pLevels[a].height * bitsPerPixel[(int)targetFormat]) / 8;
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

	printf(STR("> %s", outFile));
	// done! :)

	return 0;
}

int ConvertSurface(SourceImageLevel *pSourceSurface, MFTextureSurfaceLevel *pOutputSurface, ImageFormats targetFormat)
{
	// convert image...
	int width = pSourceSurface->width;
	int height = pSourceSurface->height;

	int x, y;
	Pixel *pSource = pSourceSurface->pData;

	switch(targetFormat)
	{
		case TexFmt_A8R8G8B8:
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
			printf("Conversion for target type not yet support...");
			return 1;
		}
	}

	return 0;
}

