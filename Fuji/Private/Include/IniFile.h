#if !defined(_INIFILE_H)
#define _INIFILE_H

#include "Vector3.h"
#include "Vector4.h"

class IniFile
{
public:
	// create an IniFile from a file
	void Create(char *pFilename);
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
#endif
	char *pIniBuffer;
	char *pCurrent;
	bool owned;
};

#endif
