#include "Common.h"
#include "IniFile.h"
#include "FileSystem.h"
#include "Util.h"

void IniFile::Create(char *pFilename)
{
	uint32 size = File_GetSize(pFilename);

	if(size)
	{
		strcpy(pIniFilename, pFilename);

		pIniBuffer = new char[size+1];

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
	pIniBuffer = new char[strlen(pMemory)+1];
	strcpy(pIniBuffer, pMemory);
	pCurrent = pIniBuffer;
	owned = true;
}

void IniFile::CreateFromPointer(char *pPointer)
{
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

	DBGASSERT(*pCurrent == NULL, STR("Syntax Error in %s", pIniFilename));

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

	DBGASSERT(*pCurrent == NULL, STR("Syntax Error in %s", pIniFilename));

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

bool IniFile::AsBool(int index)
{
	DBGASSERT(!IsSection(), "Cant get value of a section");

	return false;
}

int IniFile::AsInt(int index)
{
	DBGASSERT(!IsSection(), "Cant get value of a section");

	char *pEnd, *pOffset = pCurrent;

	while(IsAlphaNumeric(*pOffset)) pOffset++;

	pOffset = SkipWhite(pOffset);

	if(IsNewline(*pOffset) || *pOffset == ';' || *pOffset == '#' || (*pOffset == '/' && pOffset[1] == '/')) return 0;

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

	DBGASSERT(IsNewline(*pOffset), STR("Not enough elements in array in IniFile '%s', entry %s", pIniFilename, GetName()));
	DBGASSERT(IsNumeric(*pOffset), STR("Entry '%s' is not a number in IniFile '%s'", GetName(), pIniFilename));

	pEnd = pOffset;

	while(IsNumeric(*pEnd)) pEnd++;

	DBGASSERT(IsWhite(*pEnd) || IsNewline(*pEnd) || *pEnd == ',' || *pEnd == ';' || *pEnd == '#' || (*pEnd == '/' && pEnd[1] == '/'), STR("Syntax error in IniFile '%s', entry '%s'", pIniFilename, GetName()));

	pEnd = STRn(pOffset, pEnd-pOffset);

	return atoi(pEnd);
}

float IniFile::AsFloat(int index)
{
	DBGASSERT(!IsSection(), "Cant get value of a section");

	return 0.0f;
}

char* IniFile::AsString()
{
	DBGASSERT(!IsSection(), "Cant get value of a section");

	return "";
}

Vector3 IniFile::AsVector3()
{
	DBGASSERT(!IsSection(), "Cant get value of a section");

	return Vector3::zero;
}

Vector4 IniFile::AsVector4()
{
	DBGASSERT(!IsSection(), "Cant get value of a section");

	return Vector4::zero;
}

