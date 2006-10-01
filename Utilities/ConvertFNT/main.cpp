#include "Fuji.h"
#include "MFSystem.h"
#include "MFFont_Internal.h"
#include "MFStringCache.h"
#include "ConvertFNT.h"

#include "string.h"

void LOGERROR(const char *pFormat, ...)
{
	va_list arglist;
	va_start(arglist, pFormat);

	vprintf(pFormat, arglist);
	getc(stdin);
}

int main(int argc, char *argv[])
{
	MFPlatform platform = FP_Unknown;

	char fileName[256] = "";
	char outFile[256] = "";

	int a;

	// process command line
	for(a=1; a<argc; a++)
	{
		if(argv[a][0] == '-' || argv[a][0] == '/')
		{
			for(int b=0; b<FP_Max; b++)
			{
				if(!MFString_CaseCmp(&argv[a][1], MFSystem_GetPlatformString(b)))
				{
					platform = (MFPlatform)b;
					break;
				}
			}

			if(!MFString_CaseCmp(&argv[a][1], "v") || !MFString_CaseCmp(&argv[a][1], "version"))
			{
				// print version
				return 0;
			}
		}
		else
		{
			if(!fileName[0])
				MFString_Copy(fileName, argv[a]);
			else if(!outFile[0])
				MFString_Copy(outFile, argv[a]);
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
		MFString_Copy(outFile, fileName);
		for(int i=MFString_Length(outFile); --i; )
		{
			if(outFile[i] == '.')
			{
				outFile[i] = 0;
				break;
			}
		}

		MFString_Cat(outFile, ".fft");
	}

	// load image
	char *pBuffer;

	FILE *pFile = fopen(fileName, "rb");
	if(!pFile)
	{
		LOGERROR("Couldnt open source file '%s' for reading.", fileName);
		return 1;
	}

	fseek(pFile, 0, SEEK_END);
	int bytes = ftell(pFile) + 1;
	fseek(pFile, 0, SEEK_SET);

	pBuffer = (char*)malloc(bytes);
	fread(pBuffer, bytes, 1, pFile);
	fclose(pFile);

	FILE *pOutFile = fopen(outFile, "wb");

	if(!pOutFile)
	{
		LOGERROR("Couldnt open output file '%s' for writing.", outFile);
		return 2;
	}

	// parse the font file...

	// not possible for a font to be bigger than 2mb..
	char *pFontFile = (char*)malloc(2 * 1024 * 1024);
	MFZeroMemory(pFontFile, 2 * 1024 * 1024);

	MFFont *pHeader = (MFFont*)pFontFile;
	pHeader->ppPages = (MFMaterial**)&pHeader[1];

	MFStringCache *pStr = MFStringCache_Create(1024*1024);

	MFFontChar *pC = (MFFontChar*)malloc(sizeof(MFFontChar) * 65535);

	char *pToken = strtok(pBuffer, "\n\r");
	while(pToken)
	{
		int len = MFString_Length(pToken);
		pBuffer += len + 2;

		char *pT = strtok(pToken, " \t");

		if(!MFString_Compare(pT, "info"))
		{
			while(pT)
			{
				if(!MFString_CompareN(pT, "face", 4))
				{
					pHeader->pName = MFStringCache_Add(pStr, strtok(NULL, "\""));
				}
				else if(!MFString_CompareN(pT, "size", 4))
				{
					pHeader->size = atoi(&pT[5]);
				}
				else if(!MFString_CompareN(pT, "bold", 4))
				{
					pHeader->flags |= atoi(&pT[5]) ? MFFF_Bold : 0;
				}
				else if(!MFString_CompareN(pT, "italic", 6))
				{
					pHeader->flags |= atoi(&pT[7]) ? MFFF_Italic : 0;
				}
				else if(!MFString_CompareN(pT, "unicode", 7))
				{
					pHeader->flags |= atoi(&pT[8]) ? MFFF_Unicode : 0;
				}
				else if(!MFString_CompareN(pT, "smooth", 6))
				{
					pHeader->flags |= atoi(&pT[7]) ? MFFF_Smooth : 0;
				}

				pT = strtok(NULL, " \"\t");
			}
		}
		else if(!MFString_Compare(pT, "common"))
		{
			while(pT)
			{
				if(!MFString_CompareN(pT, "lineHeight", 10))
				{
					pHeader->height = atoi(&pT[11]);
				}
				else if(!MFString_CompareN(pT, "base", 4))
				{
					pHeader->base = atoi(&pT[5]);
				}
				else if(!MFString_CompareN(pT, "scaleW", 6))
				{
					pHeader->xScale = 1.0f / (float)atoi(&pT[7]);
				}
				else if(!MFString_CompareN(pT, "scaleH", 6))
				{
					pHeader->yScale = 1.0f / (float)atoi(&pT[7]);
				}
				else if(!MFString_CompareN(pT, "pages", 5))
				{
					pHeader->numPages = atoi(&pT[6]);
					pHeader->pCharacterMapping = (uint16*)&pHeader->ppPages[pHeader->numPages];
				}
				else if(!MFString_CompareN(pT, "packed", 6))
				{
//					pHeader-> = atoi(&pT[5]);
				}

				pT = strtok(NULL, " \"\t");
			}
		}
		else if(!MFString_Compare(pT, "page"))
		{
			int id = -1;

			while(pT)
			{

				if(!MFString_CompareN(pT, "id", 2))
				{
					id = atoi(&pT[3]);
				}
				else if(!MFString_CompareN(pT, "file", 4))
				{
					pT = strtok(NULL, "\"");
					pT[MFString_Length(pT)-4] = 0;
					pHeader->ppPages[id] = (MFMaterial*)MFStringCache_Add(pStr, pT);
				}

				pT = strtok(NULL, " \"\t");
			}
		}
		else if(!MFString_Compare(pT, "char"))
		{
			while(pT)
			{
				if(!MFString_CompareN(pT, "id", 2))
				{
					int id = atoi(&pT[3]);
					pHeader->pCharacterMapping[id] = pHeader->numChars;
					pHeader->maxMapping = MFMax(pHeader->maxMapping, id);
					pC[pHeader->numChars].id = id;
				}
				else if(!MFString_CompareN(pT, "x=", 2))
				{
					pC[pHeader->numChars].x = atoi(&pT[2]);
				}
				else if(!MFString_CompareN(pT, "y=", 2))
				{
					pC[pHeader->numChars].y = atoi(&pT[2]);
				}
				else if(!MFString_CompareN(pT, "width", 5))
				{
					pC[pHeader->numChars].width = atoi(&pT[6]);
				}
				else if(!MFString_CompareN(pT, "height", 6))
				{
					pC[pHeader->numChars].height = atoi(&pT[7]);
				}
				else if(!MFString_CompareN(pT, "xoffset", 7))
				{
					pC[pHeader->numChars].xoffset = atoi(&pT[8]);
				}
				else if(!MFString_CompareN(pT, "yoffset", 7))
				{
					pC[pHeader->numChars].yoffset = atoi(&pT[8]);
				}
				else if(!MFString_CompareN(pT, "xadvance", 8))
				{
					pC[pHeader->numChars].xadvance = atoi(&pT[9]);
				}
				else if(!MFString_CompareN(pT, "page", 4))
				{
					pC[pHeader->numChars].page = atoi(&pT[5]);
				}
				else if(!MFString_CompareN(pT, "chnl", 4))
				{
					pC[pHeader->numChars].channel = atoi(&pT[5]);
				}

				pT = strtok(NULL, " \"\t");
			}

			++pHeader->numChars;
		}

		pToken = strtok(pBuffer, "\n\r");
	}

	// append characters to file
	uint16 *pMapEnd = &pHeader->pCharacterMapping[pHeader->maxMapping];
	uint32 offset = MFALIGN16((uint32&)pMapEnd);
	pHeader->pChars = (MFFontChar*&)offset;
	MFCopyMemory(pHeader->pChars, pC, sizeof(MFFontChar) * pHeader->numChars);

	// append string cache to file...
	char *pStrings = (char*)&pHeader->pChars[pHeader->numChars];
	const char *pCache = MFStringCache_GetCache(pStr);
	int stringLen = MFStringCache_GetSize(pStr);
	MFCopyMemory(pStrings, pCache, stringLen);

	// byte reverse
	// TODO...

	// fix up pointers
	uint32 base = (uint32&)pFontFile;
	uint32 stringBase = (uint32&)pCache - ((uint32&)pStrings - (uint32&)pFontFile);

	for(int a=0; a<pHeader->numPages; a++)
	{
		(char*&)pHeader->ppPages[a] -= stringBase;
	}

	(char*&)pHeader->ppPages -= base;
	(char*&)pHeader->pChars -= base;
	(char*&)pHeader->pCharacterMapping -= base;
	(char*&)pHeader->pName -= stringBase;

	// write to disk
	int fileSize = ((uint32&)pStrings+stringLen) - (uint32&)pFontFile;
	fwrite(pFontFile, fileSize, 1, pOutFile);
	fclose(pOutFile);

	printf("> %s\n", outFile);

	// clean up
	MFStringCache_Destroy(pStr);
	free(pC);
	free(pFontFile);

	return 0;
}
