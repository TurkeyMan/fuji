#include "Fuji.h"

#if MF_TRANSLATION == MF_DRIVER_XBOX

#include "MFTranslation.h"

#include <xtl.h>

MF_API MFLanguage MFTranslation_GetDefaultLanguage()
{
	MFLanguage language;

	switch(XGetLanguage())
	{
		case XC_LANGUAGE_ENGLISH:
			language = MFLang_English;
			break;
		case XC_LANGUAGE_FRENCH:
			language = MFLang_French;
			break;
		case XC_LANGUAGE_SPANISH:
			language = MFLang_Spanish;
			break;
		case XC_LANGUAGE_GERMAN:
			language = MFLang_German;
			break;
		case XC_LANGUAGE_ITALIAN:
			language = MFLang_Italian;
			break;

		case XC_LANGUAGE_JAPANESE:
			language = MFLang_Japanese;
			break;
		case XC_LANGUAGE_KOREAN:
			language = MFLang_Korean;
			break;
		case XC_LANGUAGE_TCHINESE:
			language = MFLang_Chinese;
			break;

		case XC_LANGUAGE_PORTUGUESE:
			language = MFLang_Portuguese;
			break;

		default:
			language = MFLang_English;
			break;
	}

	return language;
}

#endif // MF_TRANSLATION
