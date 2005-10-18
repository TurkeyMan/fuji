//
// MFIni.cpp
//

#include "Fuji.h"
#include "MFFileSystem.h"
#include "MFIni.h"
#include "MFStringCache.h"

#if defined(_FUJI_UTIL)
#include <stdio.h>
#endif

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
	if (index >= this->stringCount)
	{
		return 0;
	}
	return atoi(GetString(index));
}

bool MFIniLine::GetBool(int index)
{
	if (index >= this->stringCount)
	{
		return false;
	}
	return atoi(GetString(index)) != 0;
}

MFVector MFIniLine::GetVector2(int index)
{
	return MakeVector(GetFloat(index), GetFloat(index+1));
}

MFVector MFIniLine::GetVector3(int index)
{
	return MakeVector(GetFloat(index), GetFloat(index+1), GetFloat(index+2));
}

MFVector MFIniLine::GetVector4(int index)
{
	return MakeVector(GetFloat(index), GetFloat(index+1), GetFloat(index+2), GetFloat(index+3));
}

// find a 2 string entry (ie. "label data")
MFIniLine *MFIniLine::FindEntry(const char *pLabel, const char *pData)
{
	MFIniLine *pLine = this;
	while (pLine)
	{
		if (pLine->IsString(0, pLabel) && pLine->IsString(1, pData))
			return pLine;
		pLine = pLine->Next();
	}
	return NULL;
}


#define MAX_LINES (32000)
#define MAX_STRINGS (64000)
#define MAX_STRINGCACHE (128000)
//=============================================================================
// Create INI file
MFIni *MFIni::Create(const char *pFilename)
{
	uint32 memSize;

	// load text file
#if !defined(_FUJI_UTIL)
	char *pMem = MFFileSystem_Load(pFilename, &memSize);
#else
	FILE *pFile = fopen(pFilename, "rb");

	if(!pFile)
		return NULL;

	fseek(pFile, 0, SEEK_END);
	memSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *pMem = (char*)Heap_Alloc(memSize);
	fread(pMem, 1, memSize, pFile);
	fclose(pFile);
#endif

	DBGASSERT(pMem != NULL, "Didn't load it!");

	// allocate ini file
	MFIni *pMFIni = (MFIni *)Heap_Alloc(sizeof(MFIni));
	strcpy(pMFIni->name, pFilename);

	// allocate temporary buffer for strings & lines
	pMFIni->pLines = (MFIniLine *)Heap_Alloc(sizeof(MFIniLine)*MAX_LINES);
	pMFIni->pStrings = (const char **)Heap_Alloc(4*MAX_STRINGS);
	pMFIni->pCache = MFStringCache::Create(MAX_STRINGCACHE);

	// scan though the file
	pMFIni->lineCount = 0;
	pMFIni->stringCount = 0;
//	char *pIn = pMem;

	// scan text file
	pMFIni->ScanRecursive(pMem, pMem+memSize);

	// TODO: copy lines, strings & cache to save on memory

	return pMFIni;
}

MFIni *MFIni::CreateFromMemory(const char *pMemory)
{
	DBGASSERT(pMemory, "Cant create ini from NULL buffer");

	uint32 memSize = (uint32)strlen(pMemory);
	const char *pMem = pMemory;

	// allocate ini file
	MFIni *pMFIni = (MFIni *)Heap_Alloc(sizeof(MFIni));
	strcpy(pMFIni->name, "Memory Ini");

	// allocate temporary buffer for strings & lines
	pMFIni->pLines = (MFIniLine *)Heap_Alloc(sizeof(MFIniLine)*MAX_LINES);
	pMFIni->pStrings = (const char **)Heap_Alloc(4*MAX_STRINGS);
	pMFIni->pCache = MFStringCache::Create(MAX_STRINGCACHE);

	// scan though the file
	pMFIni->lineCount = 0;
	pMFIni->stringCount = 0;
//	const char *pIn = pMem;

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
//	const char **pCurrString = &pStrings[stringCount];

	InitLine(pCurrLine);
	bool bIsSection;
	while (pSrc && (pSrc = ScanToken(pSrc, pSrcEnd, tokenBuffer, pCurrLine->stringCount, &bIsSection)) != NULL)
	{
		// newline
		tokenLength = (int)strlen(tokenBuffer);
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

			if (bIsSection)
			{
				pStrings[stringCount++] = pCache->Add("section");
				pCurrLine->stringCount++;
			}
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

const char *MFIni::ScanToken(const char *pSrc, const char *pSrcEnd, char *pTokenBuffer, int stringCount, bool *pbIsSection)
{
	// skip white space
	while (pSrc < pSrcEnd)
	{
		// skip comment lines
		if ((pSrc[0] == '/' && pSrc[1] == '/') || (pSrc[0] == ';') || (pSrc[0] == '#'))
		{
			while (pSrc < pSrcEnd && pSrc[0] != 0xd)
			{
				pSrc++;
			}
			if (pSrc == pSrcEnd)
				return NULL;
		}

		// check if we have found some non-whitespace
		if (pSrc[0] != ' ' && pSrc[0] != '\t' && pSrc[0] != 0xa && (stringCount!=1 || pSrc[0] != '='))
//		if (pSrc[0] != ' ' && pSrc[0] != '\t' && pSrc[0] != 0xd && pSrc[0] != 0xa && (stringCount!=1 || pSrc[0] != '='))
			break;

		pSrc++;
	}

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
	int sectionDepth = 0;
	*pbIsSection = false;
	while (pSrc < pSrcEnd && *pSrc != 0xd
		    && (bInQuotes || ((stringCount!=0 || *pSrc != '=') && *pSrc != ' ' && *pSrc != '\t' && *pSrc != ',' && (pSrc[0] != '/' || pSrc[1] != '/' ))))
	{
		if (!bInQuotes && *pSrc == '[')
		{
			sectionDepth++;
			*pbIsSection = true;
			pSrc++;
		}
		else if (!bInQuotes && *pSrc == ']')
		{
			sectionDepth--;
			if (sectionDepth < 0)
				LOGD("Error - Missing '['");
			pSrc++;
		}
		else if (*pSrc == '"')
		{
			bInQuotes = !bInQuotes;
			pSrc++;
		}
		else
		{
			*pDst++ = *pSrc++;
		}
	}

	if (pDst != pTokenBuffer)
	{
		*pDst++ = 0;
		if (*pSrc == ',')
			pSrc++;
		return pSrc;
	}

	return NULL;
}

MFIniLine *MFIni::GetFirstLine()
{
	return pLines;
}

// Log the contents of this line, and following lines to the screen
// Mainly for debugging purposes
void MFIniLine::DumpRecursive(int depth)
{
	char prefix[256];
	int c;
	for (c=0; c<depth*2; c++)
	{
		prefix[c] = ' ';
	}
	prefix[c]=0;

	MFIniLine *pLine = this;

	char buffer[256];
	while (pLine)
	{
		strcpy(buffer,prefix);
		for (int i=0; i<pLine->GetStringCount(); i++)
		{
			strcat(buffer, STR("'%s'",pLine->GetString(i)));
			strcat(buffer, "  ");
		}
		LOGD(buffer);
		if (pLine->Sub())
		{
			LOGD(STR("%s{",prefix));
			pLine->Sub()->DumpRecursive(depth+1);
			LOGD(STR("%s}",prefix));
		}
		pLine = pLine->Next();
	}
}

