#include "Fuji.h"

#if MF_TRANSLATION == PSP

#include "MFTranslation.h"

#include <psputility_sysparam.h>

MFLanguage MFTranslation_GetDefaultLanguage()
{
	MFLanguage language;

	int lang;

	int r = sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &lang);
	MFDebug_Assert(r == 0, "sceUtilityGetSystemParamInt Failed..");

	switch(lang)
	{
		case PSP_SYSTEMPARAM_LANGUAGE_ENGLISH:
			language = MFLang_English;
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_FRENCH:
			language = MFLang_French;
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_SPANISH:
			language = MFLang_Spanish;
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_GERMAN:
			language = MFLang_German;
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_ITALIAN:
			language = MFLang_Italian;
			break;

		case PSP_SYSTEMPARAM_LANGUAGE_JAPANESE:
			language = MFLang_Japanese;
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_KOREAN:
			language = MFLang_Korean;
			break;

		case PSP_SYSTEMPARAM_LANGUAGE_DUTCH:
			language = MFLang_German;
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_PORTUGUESE:
			language = MFLang_Portuguese;
			break;

		default:
			language = MFLang_English;
			break;
	}

	return language;
}

#endif
