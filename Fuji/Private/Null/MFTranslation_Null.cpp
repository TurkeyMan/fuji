#include "Fuji.h"
#include "MFTranslation.h"

MFLanguage MFTranslation_GetDefaultLanguage()
{
	// the Null device doesnt know anything about languages ;)
	return MFLang_English;
}
