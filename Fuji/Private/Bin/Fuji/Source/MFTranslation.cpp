#include "Fuji.h"
#include "MFTranslation.h"
#include "MFFileSystem.h"
#include "MFHeap.h"

struct MFStringTable
{
	uint32 magic;
	int numStrings;
	const char *pStrings[1];
};

static const char * const languageNamesEnglish[MFLang_Max] =
{
	"English",
	"French",
	"German",
	"Dutch",
	"Italian",
	"Spanish",
	"Portuguese",
	"Swedish",
	"Norwegian",
	"Finnish",
	"Danish",
	"Russian",
	"Greek",
	"Japanese",
	"Korean",
	"Chinese"
};

static const char * const languageNamesNative[MFLang_Max] =
{
	"English",
	"Français",
	"Deutsch",
	"Nederlands",
	"Italiano",
	"Español",
	"Português",
	"Svenska",
	"Norsk",
	"Suomi",
	"Dansk",
	"Русский",	// Russkiy
	"Ελληνικά",	// Εlliniká
	"日本語",	// Nihongo
	"한국어",	// Hangugeo
	"中文"		// Zhōngwén
};


const char* MFTranslation_GetLanguageName(MFLanguage language, bool native)
{
	return native ? languageNamesNative[language] : languageNamesEnglish[language];
}

MFStringTable* MFTranslation_LoadEnumStringTable(const char *pFilename)
{
	MFStringTable *pStringTable = NULL;

	const char *pFile = MFStr("%s.Enum", pFilename);
	pStringTable = (MFStringTable*)MFFileSystem_Load(pFile);
	MFDebug_Assert(pStringTable, "Enum table unavailable.");

	MFDebug_Assert(pStringTable->magic == MFMAKEFOURCC('D','L','G','1'), "File does not appear to be a Fuji String Table");

	for(int a=0; a<pStringTable->numStrings; a++)
	{
		pStringTable->pStrings[a] += (uint32)pStringTable;
	}

	return pStringTable;
}

MFStringTable* MFTranslation_LoadStringTable(const char *pFilename, MFLanguage language, MFLanguage fallback)
{
	MFStringTable *pStringTable = NULL;

	const char *pFile = MFStr("%s.%s", pFilename, MFTranslation_GetLanguageName(language, false));
	pStringTable = (MFStringTable*)MFFileSystem_Load(pFile);

	if(!pStringTable && fallback == MFLang_Unknown)
	{
		// we have requested to fail if the language could not be loaded
		return NULL;
	}
	else if(!pStringTable && language != fallback)
	{
		MFDebug_Warn(3, MFStr("String table '%s' does not exist, attempting to use fallback language", pFile));

		pFile = MFStr("%s.%s", pFilename, MFTranslation_GetLanguageName(fallback, false));
		pStringTable = (MFStringTable*)MFFileSystem_Load(pFile);

		if(!pStringTable && fallback != MFLang_English)
		{
			MFDebug_Warn(3, MFStr("String table '%s' does not exist, attempting to use 'English'", pFile));

			pFile = MFStr("%s.%s", pFilename, MFTranslation_GetLanguageName(MFLang_English, false));
			pStringTable = (MFStringTable*)MFFileSystem_Load(pFile);
		}
	}

	MFDebug_Assert(pStringTable, MFStr("String table '%s' does not exist", pFilename));
	MFDebug_Assert(pStringTable->magic == MFMAKEFOURCC('D','L','G','1'), "File does not appear to be a Fuji String Table");

	for(int a=0; a<pStringTable->numStrings; a++)
	{
		pStringTable->pStrings[a] += (uint32)pStringTable;
	}

	return pStringTable;
}

void MFTranslation_DestroyStringTable(MFStringTable *pTable)
{
	MFHeap_Free(pTable);
}

int MFTranslation_GetNumStrings(MFStringTable *pTable)
{
	return pTable->numStrings;
}

int MFTranslation_FindString(MFStringTable *pTable, const char *pString)
{
	for(int a=0; a<pTable->numStrings; a++)
	{
		if(!MFString_Compare(pTable->pStrings[a], pString))
			return a;
	}

	return -1;
}

const char* MFTranslation_GetString(MFStringTable *pTable, int stringID)
{
	MFDebug_Assert(stringID < pTable->numStrings, "String does not exist in the string table");

	return pTable->pStrings[stringID];
}
