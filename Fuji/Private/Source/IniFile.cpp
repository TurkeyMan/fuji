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
	}
	else
		pIniBuffer = NULL;
}

void IniFile::Release()
{
	if(pIniBuffer)
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

	if(IsAlphaNumeric(*pCurrent) && !IsNumeric(*pCurrent)) return 1;

	DBGASSERT(*pCurrent == NULL, STR("Syntax Error in %s", pIniFilename));

	return 0;
}

int IniFile::GetNextLine()
{
	SeekNewline(pCurrent);

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

	if(*pCurrent == '[' || (IsAlphaNumeric(*pCurrent) && !IsNumeric(*pCurrent))) return 1;

	DBGASSERT(*pCurrent == NULL, STR("Syntax Error in %s", pIniFilename));

	return 0;
}

int IniFile::GetNextSection()
{
	while(!EndOfFile());
	{
		GetNextLine();
		if(IsSection()) return 1;
	}

	return 0;
}

int IniFile::FindSection(char *pSection)
{
	GetFirstLine();

	while(!EndOfFile());
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
	return false;
}

int IniFile::AsInt(int index)
{
	return 0;
}

float IniFile::AsFloat(int index)
{
	return 0.0f;
}

char* IniFile::AsString()
{
	return "";
}

Vector3 IniFile::AsVector3()
{
	return Vector3::zero;
}

Vector4 IniFile::AsVector4()
{
	return Vector4::zero;
}

