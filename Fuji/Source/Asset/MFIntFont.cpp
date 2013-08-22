#include "Fuji.h"

#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFStringCache.h"
#include "MFFont_Internal.h"
#include "Asset/MFIntFont.h"

#include <string.h>

#if defined(MF_COMPILER_VISUALC)
	#pragma warning(disable: 4706)
#endif

static char *GetNextLine(char *&pText)
{
	while(MFIsNewline(*pText))
		++pText;

	if(*pText == 0)
		return NULL;

	char *pLineStart = pText;
	while(*pText && !MFIsNewline(*pText))
		++pText;

	// termiante the line with a NULL
	if(*pText)
	{
		*pText = 0;
		++pText;
	}

	return pLineStart;
}

void MFIntFont_CreateFromSourceData(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform)
{
	*ppOutput = NULL;
	if(pSize)
		*pSize = 0;

	size_t bytes = 0;
	char *pBuffer = MFFileSystem_Load(pFilename, &bytes, true);

	if(!pBuffer)
		return;

	// parse the font file...

	// not possible for a font to be bigger than 2mb..
	char *pFontFile = (char*)MFHeap_AllocAndZero(2 * 1024 * 1024);

	MFFont *pHeader = (MFFont*)pFontFile;
	pHeader->ppPages = (MFMaterial**)&pHeader[1];

	MFStringCache *pStr = MFStringCache_Create(1024*1024);

	MFFontChar *pC = (MFFontChar*)MFHeap_Alloc(sizeof(MFFontChar) * 65535);

	char *pLine, *pText = pBuffer;
	while((pLine = GetNextLine(pText)) != NULL)
	{
		char *pT = strtok(pLine, " \t");

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
					pHeader->pCharacterMapping[id] = (uint16)pHeader->numChars;
					pHeader->maxMapping = MFMax(pHeader->maxMapping, id);
					pC[pHeader->numChars].id = (uint16)id;
				}
				else if(!MFString_CompareN(pT, "x=", 2))
				{
					pC[pHeader->numChars].x = (uint16)atoi(&pT[2]);
				}
				else if(!MFString_CompareN(pT, "y=", 2))
				{
					pC[pHeader->numChars].y = (uint16)atoi(&pT[2]);
				}
				else if(!MFString_CompareN(pT, "width", 5))
				{
					pC[pHeader->numChars].width = (uint16)atoi(&pT[6]);
				}
				else if(!MFString_CompareN(pT, "height", 6))
				{
					pC[pHeader->numChars].height = (uint16)atoi(&pT[7]);
				}
				else if(!MFString_CompareN(pT, "xoffset", 7))
				{
					pC[pHeader->numChars].xoffset = (int8)atoi(&pT[8]);
				}
				else if(!MFString_CompareN(pT, "yoffset", 7))
				{
					pC[pHeader->numChars].yoffset = (int8)atoi(&pT[8]);
				}
				else if(!MFString_CompareN(pT, "xadvance", 8))
				{
					pC[pHeader->numChars].xadvance = (uint16)atoi(&pT[9]);
				}
				else if(!MFString_CompareN(pT, "page", 4))
				{
					pC[pHeader->numChars].page = (uint8)atoi(&pT[5]);
				}
				else if(!MFString_CompareN(pT, "chnl", 4))
				{
					pC[pHeader->numChars].channel = (uint8)atoi(&pT[5]);
				}

				pT = strtok(NULL, " \"\t");
			}

			++pHeader->numChars;
		}
	}

	// append characters to file
	uint16 *pMapEnd = &pHeader->pCharacterMapping[pHeader->maxMapping+1];
	uintp offset = MFALIGN16(pMapEnd);
	pHeader->pChars = (MFFontChar*&)offset;
	MFCopyMemory(pHeader->pChars, pC, sizeof(MFFontChar) * pHeader->numChars);

	// append string cache to file...
	char *pStrings = (char*)&pHeader->pChars[pHeader->numChars];
	const char *pCache = MFStringCache_GetCache(pStr);
	size_t stringLen = MFStringCache_GetSize(pStr);
	MFCopyMemory(pStrings, pCache, stringLen);

	// byte reverse
	// TODO...

	// fix up pointers
	uintp base = (uintp)pFontFile;
	uintp stringBase = (uintp)pCache - ((uintp)pStrings - (uintp)pFontFile);

	for(int a=0; a<pHeader->numPages; a++)
		(char*&)pHeader->ppPages[a] -= stringBase;

	(char*&)pHeader->ppPages -= base;
	(char*&)pHeader->pChars -= base;
	(char*&)pHeader->pCharacterMapping -= base;
	(char*&)pHeader->pName -= stringBase;

	// write to disk
	int fileSize = (int)(((uintp)pStrings + stringLen) - (uintp)pFontFile);
	MFFont *pFont = (MFFont*)MFHeap_Alloc(fileSize);
	MFCopyMemory(pFont, pFontFile, fileSize);

	// clean up
	MFStringCache_Destroy(pStr);
	MFHeap_Free(pC);
	MFHeap_Free(pFontFile);
	MFHeap_Free(pBuffer);

	*ppOutput = pFont;
	*pSize = fileSize;
}
