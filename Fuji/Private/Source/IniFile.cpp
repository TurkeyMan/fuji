#include "Common.h"
#include "IniFile.h"
#include "FileSystem.h"
#include "Util.h"

#if !defined(_RETAIL)
#define INIASSERT(reason, rave) DBGASSERT(reason, STR("\nE:\\Projects\\Spy\\%s(%d) : %s\n", pIniFilename, iniLine, rave));
#else
#define INIASSERT(reason, rave)
#endif

void IniFile::Create(char *pFilename)
{
	uint32 size = File_GetSize(pFilename);

	if(size)
	{
#if !defined(_RETAIL)
		strcpy(pIniFilename, pFilename);
#endif
		pIniBuffer = (char*)Heap_Alloc(size+1);

		uint32 file = File_Open(pFilename);
		File_Read(pIniBuffer, size, file);
		File_Close(file);

		pIniBuffer[size] = NULL;
		pCurrent = pIniBuffer;

		owned = true;
	}
	else
		pIniBuffer = NULL;
}

void IniFile::CreateFromMemory(char *pMemory)
{
#if !defined(_RETAIL)
	strcpy(pIniFilename, "Memory");
#endif
	pIniBuffer = (char*)Heap_Alloc(strlen(pMemory)+1);
	strcpy(pIniBuffer, pMemory);
	pCurrent = pIniBuffer;
	owned = true;
}

void IniFile::CreateFromPointer(char *pPointer)
{
#if !defined(_RETAIL)
	strcpy(pIniFilename, "Pointer");
#endif
	pIniBuffer = pPointer;
	pCurrent = pIniBuffer;
	owned = false;
}

void IniFile::Release()
{
	if(pIniBuffer && owned)
	{
		delete[] pIniBuffer;
		pIniBuffer = NULL;
	}
}

bool IniFile::EndOfFile()
{
	return *pCurrent == NULL;
}

int IniFile::GetFirstLine()
{
#if !defined(_RETAIL)
	iniLine = 1;
	pLastNewline = NULL;
#endif
	pCurrent = pIniBuffer;

	for(;;pCurrent++)
	{
		if(IsWhite(*pCurrent) || IsNewline(*pCurrent)) continue;
		if(*pCurrent == ';' || *pCurrent == '#' || (*pCurrent == '/' && pCurrent[1] == '/'))
		{
			pCurrent = SeekNewline(pCurrent)-1;
			continue;
		}
		break;
	}

	if((*pCurrent == '[' && IsAlphaNumeric(pCurrent[1]) && !IsNumeric(pCurrent[1])) || (IsAlphaNumeric(*pCurrent) && !IsNumeric(*pCurrent))) return 1;

	INIASSERT(*pCurrent == NULL, "Syntax Error");

	return 0;
}

int IniFile::GetNextLine()
{
	pCurrent = SeekNewline(pCurrent);

	for(;;pCurrent++)
	{
		if(IsWhite(*pCurrent) || IsNewline(*pCurrent)) continue;
		if(*pCurrent == ';' || *pCurrent == '#' || (*pCurrent == '/' && pCurrent[1] == '/'))
		{
			pCurrent = SeekNewline(pCurrent)-1;
			continue;
		}
		break;
	}

	if((*pCurrent == '[' && IsAlphaNumeric(pCurrent[1]) && !IsNumeric(pCurrent[1])) || (IsAlphaNumeric(*pCurrent) && !IsNumeric(*pCurrent))) return 1;

	INIASSERT(*pCurrent == NULL, "Syntax Error");

	return 0;
}

int IniFile::GetNextSection()
{
	while(!EndOfFile())
	{
		GetNextLine();
		if(IsSection()) return 1;
	}

	return 0;
}

int IniFile::FindSection(char *pSection)
{
	GetFirstLine();

	while(!EndOfFile())
	{
		if(IsSection() && !stricmp(GetName(), pSection)) return 1;
		GetNextSection();
	}

	return 0;
}

int IniFile::FindLine(char *pName, char *pSection)
{
	char *pTemp;

	if(pSection)
	{
		FindSection(pSection);
		GetNextLine();

		while(!EndOfFile() && !IsSection())
		{
			pTemp = GetName();
			if(!pTemp) return 0;
			if(!stricmp(pName, pTemp)) return 1;
			GetNextLine();
		}
	}
	else
	{
		GetFirstLine();

		while(!EndOfFile())
		{
			pTemp = GetName();
			if(!pTemp) return 0;
			if(!stricmp(pName, pTemp)) return 1;
			GetNextLine();
		}
	}

	return 0;
}

bool IniFile::IsSection()
{
	return *pCurrent == '[';
}

char* IniFile::GetName()
{
	char *pEnd, *pStart = pCurrent;

	if(EndOfFile()) return NULL;

	if(IsSection()) pStart++;
	pEnd = pStart;

	while(IsAlphaNumeric(*pEnd)) pEnd++;

	return STRn(pStart, pEnd-pStart);
}

inline char* IniFile::GetFloat(char *pOffset, char **ppFloat)
{
	INIASSERT(IsNumeric(*pOffset), STR("Entry '%s' is not a number", GetName()));

	char *pEnd = pOffset;

	int dotFound = 1;
	while(IsNumeric(*pEnd) || (*pEnd == '.' && dotFound--)) pEnd++;

	*ppFloat = STRn(pOffset, pEnd-pOffset);

	if(*pEnd == 'f') pEnd++;

	INIASSERT(*pEnd == NULL || IsWhite(*pEnd) || IsNewline(*pEnd) || *pEnd == ',' || *pEnd == ';' || *pEnd == '#' || (*pEnd == '/' && pEnd[1] == '/'), STR("Syntax error in entry '%s'", GetName()));
	return pEnd;
}

bool IniFile::AsBool(int index)
{
	INIASSERT(!IsSection(), "Cant get value of a section");

	char *pNumber, *pOffset = pCurrent;

	while(IsAlphaNumeric(*pOffset)) pOffset++;

	pOffset = SkipWhite(pOffset);

	if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return true;

	if(*pOffset == '=')
	{
		pOffset++;
		pOffset = SkipWhite(pOffset);
	}

	for(int a=0; a<index; a++)
	{
		while(IsAlphaNumeric(*pOffset) || *pOffset == '.') pOffset++;

		pOffset = SkipWhite(pOffset);

		if(*pOffset == ',')
		{
			pOffset++;
			pOffset = SkipWhite(pOffset);
		}
	}

	INIASSERT(!IsNewline(*pOffset), STR("Not enough elements in array in entry %s", GetName()));

	if(!strnicmp(pOffset, "true", 4))
	{
		INIASSERT(*pOffset == NULL || IsWhite(pOffset[4]) || IsNewline(pOffset[4]) || pOffset[4] == ',' || pOffset[4] == ';' || pOffset[4] == '#' || (pOffset[4] == '/' && pOffset[5] == '/'), STR("Syntax error in entry '%s'", GetName()));
		return true;
	}
	if(!strnicmp(pOffset, "false", 5))
	{
		INIASSERT(*pOffset == NULL || IsWhite(pOffset[5]) || IsNewline(pOffset[5]) || pOffset[5] == ',' || pOffset[5] == ';' || pOffset[5] == '#' || (pOffset[5] == '/' && pOffset[6] == '/'), STR("Syntax error in entry '%s'", GetName()));
		return false;
	}

	GetFloat(pOffset, &pNumber);

	float f = (float)atof(pNumber);
	return f == 0.0f ? false : true;
}

int IniFile::AsInt(int index)
{
	INIASSERT(!IsSection(), "Cant get value of a section");

	char *pNumber, *pOffset = pCurrent;

	while(IsAlphaNumeric(*pOffset)) pOffset++;

	pOffset = SkipWhite(pOffset);

	if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return 0;

	if(*pOffset == '=')
	{
		pOffset++;
		pOffset = SkipWhite(pOffset);
	}

	for(int a=0; a<index; a++)
	{
		while(IsAlphaNumeric(*pOffset) || *pOffset == '.') pOffset++;

		pOffset = SkipWhite(pOffset);

		if(*pOffset == ',')
		{
			pOffset++;
			pOffset = SkipWhite(pOffset);
		}
	}

	INIASSERT(!IsNewline(*pOffset), STR("Not enough elements in array in entry %s", GetName()));

	GetFloat(pOffset, &pNumber);

	return atoi(pNumber);
}

float IniFile::AsFloat(int index)
{
	INIASSERT(!IsSection(), "Cant get value of a section");

	char *pNumber, *pOffset = pCurrent;

	while(IsAlphaNumeric(*pOffset)) pOffset++;

	pOffset = SkipWhite(pOffset);

	if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return 0.0f;

	if(*pOffset == '=')
	{
		pOffset++;
		pOffset = SkipWhite(pOffset);
	}

	for(int a=0; a<index; a++)
	{
		while(IsAlphaNumeric(*pOffset) || *pOffset == '.') pOffset++;

		pOffset = SkipWhite(pOffset);

		if(*pOffset == ',')
		{
			pOffset++;
			pOffset = SkipWhite(pOffset);
		}
	}

	INIASSERT(!IsNewline(*pOffset), STR("Not enough elements in array in entry %s", GetName()));

	GetFloat(pOffset, &pNumber);

	return (float)atof(pNumber);
}

char* IniFile::AsString()
{
	INIASSERT(!IsSection(), "Cant get value of a section");

	char *pEnd, *pOffset = pCurrent;

	while(IsAlphaNumeric(*pOffset)) pOffset++;

	pOffset = SkipWhite(pOffset);

	if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return NULL;

	if(*pOffset == '=')
	{
		pOffset++;
		pOffset = SkipWhite(pOffset);
	}

	INIASSERT(*pOffset == '\"', STR("Entry %s not a string", GetName()));

	pOffset++;
	pEnd = pOffset;

	while(*pEnd != NULL && *pEnd != '\"' && !IsNewline(*pEnd)) pEnd++;

	INIASSERT(!IsNewline(*pEnd), "Illegal end of line in string literal");
	INIASSERT(*pEnd != NULL, "Unexpected end of file");

	char *pString = STRn(pOffset, pEnd-pOffset);

	pOffset = pEnd+1;
	pOffset = SkipWhite(pOffset);

	INIASSERT(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/'), "Syntax Error");

	return pString;
}

Vector3 IniFile::AsVector3()
{
	INIASSERT(!IsSection(), "Cant get value of a section");

	char *pNumber, *pOffset = pCurrent;
	Vector3 output = Vector3::zero;

	while(IsAlphaNumeric(*pOffset)) pOffset++;

	pOffset = SkipWhite(pOffset);

	if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return output;

	if(*pOffset == '=')
	{
		pOffset++;
		pOffset = SkipWhite(pOffset);
	}

	for(int a=0; a<3; a++)
	{
		INIASSERT(!IsNewline(*pOffset), STR("Not enough elements in array in entry %s", GetName()));

		pOffset = GetFloat(pOffset, &pNumber);

		((float*)&output)[a] = (float)atof(pNumber);

		pOffset = SkipWhite(pOffset);

		if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return output;

		if(*pOffset == ',')
		{
			pOffset++;
			pOffset = SkipWhite(pOffset);
		}
	}

	INIASSERT(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/'), STR("Array too long in entry %s", GetName()));

	return output;
}

Vector4 IniFile::AsVector4()
{
	INIASSERT(!IsSection(), "Cant get value of a section");

	char *pNumber, *pOffset = pCurrent;
	Vector4 output = Vector4::zero;
	output.w = 1.0f;

	while(IsAlphaNumeric(*pOffset)) pOffset++;

	pOffset = SkipWhite(pOffset);

	if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return output;

	if(*pOffset == '=')
	{
		pOffset++;
		pOffset = SkipWhite(pOffset);
	}

	for(int a=0; a<4; a++)
	{
		INIASSERT(!IsNewline(*pOffset), STR("Not enough elements in array in entry %s", GetName()));

		pOffset = GetFloat(pOffset, &pNumber);

		((float*)&output)[a] = (float)atof(pNumber);

		pOffset = SkipWhite(pOffset);

		if(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return output;

		if(*pOffset == ',')
		{
			pOffset++;
			pOffset = SkipWhite(pOffset);
		}
	}

	INIASSERT(*pOffset == NULL || IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/'), STR("Array too long in IniFile '%s', entry %s", pIniFilename, GetName()));

	return output;
}

