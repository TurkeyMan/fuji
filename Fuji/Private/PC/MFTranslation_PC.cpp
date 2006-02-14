#include "Fuji.h"
#include "MFTranslation.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

MFLanguage MFTranslation_GetDefaultLanguage()
{
	MFLanguage language;

	int pri, sub;
	LANGID id = GetUserDefaultLangID();

	pri = id & 0x03FF;
	sub = id & 0xFC00;

	switch(pri)
	{
		case LANG_ENGLISH:
			language = MFLang_English;
			break;
		case LANG_FRENCH:
			language = MFLang_French;
			break;
		case LANG_SPANISH:
			language = MFLang_Spanish;
			break;
		case LANG_GERMAN:
			language = MFLang_German;
			break;
		case LANG_ITALIAN:
			language = MFLang_Italian;
			break;

		case LANG_SWEDISH:
			language = MFLang_Swedish;
			break;
		case LANG_NORWEGIAN:
			language = MFLang_Norwegian;
			break;
		case LANG_DANISH:
			language = MFLang_Danish;
			break;

		case LANG_JAPANESE:
			language = MFLang_Japanese;
			break;
		case LANG_KOREAN:
			language = MFLang_Korean;
			break;
		case LANG_CHINESE:
			language = MFLang_Chinese;
			break;

		case LANG_RUSSIAN:
			language = MFLang_Russian;
			break;

		case LANG_DUTCH:
			language = MFLang_German;
			break;
		case LANG_FINNISH:
			language = MFLang_Swedish;
			break;
		case LANG_PORTUGUESE:
			language = MFLang_Portuguese;
			break;

		default:
			language = MFLang_English;
			break;
	}

	return language;
}
