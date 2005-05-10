//
// MFIni.cpp
//

#include "Common.h"
#include "MFFileSystem.h"
#include "MFIni.h"
#include "MFStringCache.h"

//=============================================================================
MFIniLine *MFIniLine::Next()
{
	return (terminate ? NULL : this+subtreeLineCount+1);
}

MFIniLine *MFIniLine::Sub()
{
	return (subtreeLineCount > 0) ? this+1 : NULL;
}

int MFIniLine::GetStringCount()
{
	return stringCount;
}

const char *MFIniLine::GetString(int index)
{
	return pIni->pStrings[firstString+index];
}

bool MFIniLine::IsString(int index, const char *pString)
{
	return !stricmp(GetString(index), pString);
}

float MFIniLine::GetFloat(int index)
{
	return (float)atof(GetString(index));
}

int MFIniLine::GetInt(int index)
{
	return atoi(GetString(index));
}

bool MFIniLine::GetBool(int index)
{
	return atoi(GetString(index)) != 0;
}

#define MAX_LINES (32000)
#define MAX_STRINGS (64000)
#define MAX_STRINGCACHE (128000)
//=============================================================================
// Create INI file
MFIni *MFIni::Create(const char *pFilename)
{
	MFIni *pMFIni = (MFIni *)Heap_Alloc(sizeof(MFIni));

	uint32 memSize;
	char *pMem = MFFileSystem_Load(pFilename, &memSize);

	// load text file
	strcpy(pMFIni->name, pFilename);
	DBGASSERT(pMem != NULL, "Didn't load it!");

	// allocate temporary buffer for strings & lines
	pMFIni->pLines = (MFIniLine *)Heap_Alloc(sizeof(MFIniLine)*MAX_LINES);
	pMFIni->pStrings = (const char **)Heap_Alloc(4*MAX_STRINGS);
	pMFIni->pCache = MFStringCache::Create(MAX_STRINGCACHE);

	// scan though the file
	pMFIni->lineCount = 0;
	pMFIni->stringCount = 0;
	char *pIn = pMem;

	// scan text file
	pMFIni->ScanRecursive(pMem, pMem+memSize);

	// TODO: copy lines, strings & cache to save on memory

	return pMFIni;
}

void MFIni::Destroy(MFIni *pIni)
{
	Heap_Free(pIni->pLines);
	Heap_Free(pIni->pStrings);
	MFStringCache::Destroy(pIni->pCache);
	Heap_Free(pIni);
}

// returns how many lines it found
const char *MFIni::ScanRecursive(const char *pSrc, const char *pSrcEnd)
{
	bool bNewLine = true;
	int tokenLength = 0;
	char tokenBuffer[256];

	MFIniLine *pCurrLine = &pLines[lineCount];
	const char **pCurrString = &pStrings[stringCount];

	InitLine(pCurrLine);
	while (pSrc = ScanToken(pSrc, pSrcEnd, tokenBuffer))
	{
		// newline
		tokenLength = strlen(tokenBuffer);
		if (tokenLength == 1 && tokenBuffer[0] == 0xd)
		{
			bNewLine = true;
		}
		else if (tokenLength == 1 && tokenBuffer[0] == '{')
		{
			DBGASSERT(bNewLine, "open bracket must be at start of line!");

			// new sub section
			int oldLineCount = ++lineCount;
			pSrc = ScanRecursive(pSrc, pSrcEnd);
			pCurrLine->subtreeLineCount = lineCount - oldLineCount;
			lineCount--;
		}
		else if (tokenLength == 1 && tokenBuffer[0] == '}')
		{
			DBGASSERT(bNewLine, "close bracket must be at start of line!");

			if (pCurrLine->stringCount != 0 || pCurrLine->subtreeLineCount != 0)
			{
				pCurrLine->terminate = 1;
				lineCount++;
			}
			return pSrc;
		}
		else // must be a string token
		{
			if (bNewLine && (pCurrLine->stringCount != 0 || pCurrLine->subtreeLineCount != 0))
			{
				lineCount++;
				pCurrLine = &pLines[lineCount];
				InitLine(pCurrLine);
			}
			bNewLine = false;
			pStrings[stringCount++] = pCache->Add(tokenBuffer);
			pCurrLine->stringCount++;
		}
	}

	if (pCurrLine->stringCount != 0 || pCurrLine->subtreeLineCount != 0)
	{
		pCurrLine->terminate = 1;
		lineCount++;
	}
	return pSrc;
}

void MFIni::InitLine(MFIniLine *pLine)
{
	pLine->pIni = this;
	pLine->pIni = this;
	pLine->subtreeLineCount = 0;
	pLine->firstString = stringCount;
	pLine->stringCount = 0;
	pLine->terminate = 0;
}

const char *MFIni::ScanToken(const char *pSrc, const char *pSrcEnd, char *pTokenBuffer)
{
	// skip white space
	while (pSrc < pSrcEnd && (*pSrc == ' ' || *pSrc == '\t' || *pSrc == 0xa)) pSrc++;

	// end of file?
	if (pSrc == pSrcEnd)
		return NULL;

	// start of token
	char *pDst = pTokenBuffer;

	// handle special tokens (brackets and EOL)
	if (*pSrc == '{' || *pSrc == '}' || *pSrc == 0xd)
	{
		*pDst++ = *pSrc;
		*pDst++ = 0;
		return ++pSrc;
	}

	// find end of token
	bool bInQuotes = false;
	while (pSrc < pSrcEnd && *pSrc != 0xd
		    && (bInQuotes || (*pSrc != ' ' && *pSrc != '/t' && *pSrc != ',' && (pSrc[0] != '/' || pSrc[1] != '/' ))))
	{
		if (*pSrc == '"')
		{
			bInQuotes = !bInQuotes;
			pSrc++;
		}
		else
		{
			if (*pSrc == '\\')
			{
				pSrc++;
			}
			*pDst++ = *pSrc++;
		}
	}

	if (pDst != pTokenBuffer)
	{
		*pDst++ = 0;
		return pSrc;
	}

	return NULL;
}

MFIniLine *MFIni::GetFirstLine()
{
	return pLines;
}
