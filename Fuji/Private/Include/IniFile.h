#if !defined(_INIFILE_H)
#define _INIFILE_H

#include "Vector3.h"
#include "Vector4.h"

class IniFile
{
public:
	// create an IniFile from a file
	int Create(char *pFilename);
	// create an IniFile from a *copy* of a memory location
	void CreateFromMemory(char *pMemory);
	// create an IniFile from a *direct reference* a memoryLocation
	void CreateFromPointer(char *pPointer);
	// release the IniFile
	void Release();

	bool EndOfFile();

	int GetFirstLine();

	int GetNextLine();
	int GetNextSection();

	void PushMarker();
	void PopMarker();

	int FindSection(char *pSection);
	int FindLine(char *pName, char *pSection = NULL);

	bool IsSection();
	char* GetName();

	bool AsBool(int index);
	int AsInt(int index);
	float AsFloat(int index);
	char* AsString();
	Vector3 AsVector3();
	Vector4 AsVector4();

protected:
#if !defined(_RETAIL)
	char pIniFilename[64];
	int iniLine;
	char *pLastNewline;
	inline bool IsNewline(char c)
	{
		if(c=='\n' && pCurrent!=pLastNewline)
		{
			iniLine++;
			pLastNewline = pCurrent;
		}

		return c=='\n' || c=='\r';
	}
	inline char* SeekNewline(char *pC)
	{
		while(!IsNewline(*pC) && *pC != 0) pC++;
		while(IsNewline(*pC)) pC++;
		return pC;
	}
#endif
	char *pIniBuffer;
	char *pCurrent;
	char *pCurrentStack[10];
	int currentHeight;
	bool owned;

	inline char* GetFloat(char *pOffset, char **ppFloat);
};

#endif
