// Mount Fuji - INI file reader
//
//================================================
// USAGE
//
// MFIni *pIni = MFIni::Create("MyIniFile.ini");
// MFIniLine *pLine = pIni->GetFirstLine();
// while (pLine)
// {
//   if (pLine->IsString(0, "Group")
//   {
//     ProcessGroup(pLine);
//   }
//   pLine = pLine->Next();
// }
//================================================
#include "Vector3.h"
#include "Vector4.h"

class MFStringCache;

class MFIniLine
{
friend class MFIni;
public:
	MFIniLine *Next();
	MFIniLine *Sub();

	// find a 2 string entry (ie. "label data")
	MFIniLine *FindEntry(const char *pLabel, const char *pData);

	int GetStringCount();
	bool IsString(int index, const char *pString);
	const char *GetString(int index);
	float GetFloat(int index);
	int GetInt(int index);
	bool GetBool(int index);
	Vector3 GetVector3(int index);
	Vector4 GetVector4(int index);

	// Log the contents of this line, and following lines to the screen
	// Mainly for debugging purposes
	void DumpRecursive(int depth);

protected:
	class MFIni *pIni;			// what INI do we belong to? Allows usage of Lines as an iterator
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
	static void Destroy(MFIni *pIni);

	// get first line, all further parsing is done through the Line interface
	MFIniLine *GetFirstLine();

protected:
	char name[64];              // name of file
	char *pMem;                 // memory from File Load

	MFIniLine *pLines;          // array of lines that index the ini file
	int lineCount;

	const char **pStrings;      // array of string pointers (tokens) into the ini file
	int stringCount;

	MFStringCache *pCache;      // string cache

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
