#include "Fuji_Internal.h"

#if MF_TRANSLATION == MF_DRIVER_LINUX

#include "MFTranslation.h"

MF_API MFLanguage MFTranslation_GetDefaultLanguage()
{
	char *pLang = getenv("LANG");

	MFLanguage lang = MFLang_English;

	if(pLang)
	{
		if(!MFString_CompareN(pLang, "en", 2))
			lang = MFLang_English;
		else if(!MFString_CompareN(pLang, "fr", 2))
			lang = MFLang_French;
		else if(!MFString_CompareN(pLang, "es", 2))
			lang = MFLang_Spanish;
		else if(!MFString_CompareN(pLang, "de", 2))
			lang = MFLang_German;
		else if(!MFString_CompareN(pLang, "it", 2))
			lang = MFLang_Italian;
		else if(!MFString_CompareN(pLang, "sv", 2))
			lang = MFLang_Swedish;
		else if(!MFString_CompareN(pLang, "no", 2))
			lang = MFLang_Norwegian;
		else if(!MFString_CompareN(pLang, "da", 2))
			lang = MFLang_Danish;
		else if(!MFString_CompareN(pLang, "ja", 2))
			lang = MFLang_Japanese;
		else if(!MFString_CompareN(pLang, "ko", 2))
			lang = MFLang_Korean;
		else if(!MFString_CompareN(pLang, "zh", 2))
			lang = MFLang_Chinese;
		else if(!MFString_CompareN(pLang, "ru", 2))
			lang = MFLang_Russian;
		else if(!MFString_CompareN(pLang, "pt", 2))
			lang = MFLang_Portuguese;
		else if(!MFString_CompareN(pLang, "el", 2))
			lang = MFLang_Greek;
		else
			MFDebug_Warn(2, MFStr("Unsupported language '%s'", pLang));
	}
	else
		MFDebug_Warn(2, "Language not defined ($LANG not set)");

	return lang;
}

#endif
