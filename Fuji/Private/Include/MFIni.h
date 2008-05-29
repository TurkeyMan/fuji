#if !defined(_MFINI_H)
#define _MFINI_H

// Mount Fuji - INI file reader
//
//================================================
// USAGE
//
// MFIni *pIni = MFIni::Create("MyIniFile.ini");
// MFIniLine *pLine = pIni->GetFirstLine();
// while(pLine)
// {
//   if(pLine->IsString(0, "Group"))
//   {
//     ProcessGroup(pLine);
//   }
//   pLine = pLine->Next();
// }
//================================================
#include "MFVector.h"
#include "MFMatrix.h"

struct MFStringCache;

class MFIni;

class MFIniLine
{
friend class MFIni;
public:
	MFIniLine *Next() { return (terminate ? NULL : this+subtreeLineCount+1); }
	MFIniLine *Sub() { return (subtreeLineCount > 0) ? this+1 : NULL; }

	// find a 2 string entry (ie. "label data")
	MFIniLine *FindEntry(const char *pLabel, const char *pData);

	int GetStringCount() { return stringCount; }
	bool IsString(int index, const char *pString) { const char *pToken = GetString(index); return pToken ? !MFString_CaseCmp(pToken, pString) : false; }
	const char *GetString(int index);
	float GetFloat(int index);
	int GetInt(int index);
	int GetIntString(int index, const char **ppStrings, int numStrings = -1);
	bool GetBool(int index);
	MFVector GetVector2(int index);
	MFVector GetVector3(int index);
	MFVector GetVector4(int index);
	MFMatrix GetMatrix(int index);

	// Log the contents of this line, and following lines to the screen
	// Mainly for debugging purposes
	void DumpRecursive(int depth);

protected:
	MFIni *pIni;			// what INI do we belong to? Allows usage of Lines as an iterator
	int subtreeLineCount;		// total number of sublines before next line at this level
	uint32 firstString;         // first index of MFIni::pStrings for line string data
	int16 stringCount;			// how many data strings on this line
	int16 terminate;			// is this the last subline at this level?
};

class MFIni
{
friend class MFIniLine;
public:
	static MFIni *Create(const char *pFilename);
	static MFIni *CreateFromMemory(const char *pMemory);
	static void Destroy(MFIni *pIni);

	// get first line, all further parsing is done through the Line interface
	MFIniLine *GetFirstLine();

protected:
	char name[64];              // name of file
	char *pMem;                 // memory from File Load

	MFIniLine *pLines;          // array of lines that index the ini file
	int lineCount;
	int linesAllocated;

	const char **pStrings;      // array of string pointers (tokens) into the ini file
	int stringCount;
	int stringsAllocated;

	MFStringCache *pCache;      // string cache
	int stringCacheSize;

	// scans text at "pSrc" for a single token
	// pTokenStart will point to first Char in the token
	// pTokenEnd will point to the last Char in the token
	// return TRUE if a token was found
	// '{', '}' and '/n' are also treated as tokens
	static const char *ScanToken(const char *pSrc, const char *pSrcEnd, char *pTokenBuffer, int stringCount, bool *pbIsSection);

	// use recursion to scan in the lines & strings
	const char *ScanRecursive(const char *pSrc, const char *pSrcEnd);
	void InitLine(MFIniLine *pLine);
};

#endif // _MFINI_H
