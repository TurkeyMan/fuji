#include "Fuji.h"
#include "MFHeap.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFTranslation_Internal.h"
#include "Asset/MFIntStringTable.h"

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


MF_API const char* MFTranslation_GetLanguageName(MFLanguage language, bool native)
{
	return native ? languageNamesNative[language] : languageNamesEnglish[language];
}

MF_API MFLanguage MFTranslation_GetLanguageByName(const char *pLanguageName)
{
	for(int a=0; a<MFLang_Max; ++a)
	{
		if(!MFString_CaseCmp(pLanguageName, languageNamesEnglish[a]))
			return (MFLanguage)a;
	}

	return MFLang_Unknown;
}

MF_API MFStringTable* MFTranslation_LoadEnumStringTable(const char *pFilename)
{
	MFStringTable *pStringTable = NULL;

	const char *pFile = MFStr("%s.Enum", pFilename);
	pStringTable = (MFStringTable*)MFFileSystem_Load(pFile);
	MFDebug_Assert(pStringTable, "Enum table unavailable.");

	MFDebug_Assert(pStringTable->magic == MFMAKEFOURCC('D','L','G','1'), "File does not appear to be a Fuji String Table");

	for(int a=0; a<pStringTable->numStrings; a++)
	{
		pStringTable->pStrings[a] += (size_t)pStringTable;
	}

	return pStringTable;
}

MF_API MFStringTable* MFTranslation_LoadStringTable(const char *pFilename, MFLanguage language, MFLanguage fallback)
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
	else
	{
		// convert from source data...
		MFIntStringTable *pIntStrings = MFIntStringTable_CreateFromFile(MFStr("%s.csv", pFilename));

		// find the one we want...
		pStringTable = MFIntStringTable_CreateRuntimeData(pIntStrings, language, MFSystem_GetCurrentPlatform(), NULL);
		if(!pStringTable)
			pStringTable = MFIntStringTable_CreateRuntimeData(pIntStrings, fallback, MFSystem_GetCurrentPlatform(), NULL);

		// free the int string table
		MFIntStringTable_Destroy(pIntStrings);
	}

	MFDebug_Assert(pStringTable, MFStr("String table '%s' does not exist", pFilename));
	MFDebug_Assert(pStringTable->magic == MFMAKEFOURCC('D','L','G','1'), "File does not appear to be a Fuji String Table");

	for(int a=0; a<pStringTable->numStrings; a++)
	{
		pStringTable->pStrings[a] += (size_t)pStringTable;
	}

	return pStringTable;
}

MF_API void MFTranslation_DestroyStringTable(MFStringTable *pTable)
{
	MFHeap_Free(pTable);
}

MF_API int MFTranslation_GetNumStrings(MFStringTable *pTable)
{
	return pTable->numStrings;
}

MF_API int MFTranslation_FindString(MFStringTable *pTable, const char *pString)
{
	for(int a=0; a<pTable->numStrings; a++)
	{
		if(!MFString_Compare(pTable->pStrings[a], pString))
			return a;
	}

	return -1;
}

MF_API const char* MFTranslation_GetString(MFStringTable *pTable, int stringID)
{
	MFDebug_Assert(stringID < pTable->numStrings, "String does not exist in the string table");

	return pTable->pStrings[stringID];
}
