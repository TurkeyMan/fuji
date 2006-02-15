#include "Fuji.h"
#include "MFTranslation.h"
#include "MFFileSystem.h"
#include "MFHeap.h"

struct MFStringTable
{
	int numStrings;
	const char *pStrings[1];
};

static const char * const languageNamesEnglish[MFLang_Max] =
{
	"English",
	"French",
	"Spanish",
	"German",
	"Italian",
	"Swedish",
	"Norwegian",
	"Danish",
	"Japanese",
	"Korean",
	"Chinese",
	"Russian",
	"Portuguese"
};

static const char * const languageNamesNative[MFLang_Max] =
{
	"English",
	"Français",
	"Español",
	"Deutsch",
	"Italiano",
	"Svenska",
	"Norsk",
	"Dansk",
	// these are a little tricky because they require unicode... :/
	"Japanese",
	"Korean",
	"Chinese",
	"Russian",
	"Português"
};


const char* MFTranslation_GetLanguageName(MFLanguage language, bool native)
{
	return native ? languageNamesNative[language] : languageNamesEnglish[language];
}

MFStringTable* MFTranslation_LoadStringTable(const char *pFilename, MFLanguage language, MFLanguage fallback)
{
	MFStringTable *pStringTable = NULL;

	const char *pFile = MFStr("%s.%s", pFilename, MFTranslation_GetLanguageName(language, false));
	pStringTable = (MFStringTable*)MFFileSystem_Load(pFile);

	if(!pStringTable && language != fallback)
	{
		MFDebug_Warn(3, MFStr("String table '%s' does not exist, attempting to use fallback language", pFile));

		pFile = MFStr("%s.%s", pFilename, MFTranslation_GetLanguageName(fallback, false));
		pStringTable = (MFStringTable*)MFFileSystem_GetSize(pFile);

		if(!pStringTable && fallback != MFLang_English)
		{
			MFDebug_Warn(3, MFStr("String table '%s' does not exist, attempting to use 'English'", pFile));

			pFile = MFStr("%s.%s", pFilename, MFTranslation_GetLanguageName(MFLang_English, false));
			pStringTable = (MFStringTable*)MFFileSystem_GetSize(pFile);
		}
	}

	MFDebug_Assert(pStringTable, MFStr("String table '%s' does not exist", pFilename));

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

const char* MFTranslation_GetString(MFStringTable *pTable, int stringID)
{
	MFDebug_Assert(stringID < pTable->numStrings, "String does not exist in the string table");

	return pTable->pStrings[stringID];
}
