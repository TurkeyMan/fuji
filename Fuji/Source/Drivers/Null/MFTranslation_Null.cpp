#include "Fuji_Internal.h"

#if MF_TRANSLATION == MF_DRIVER_NULL

#include "MFTranslation.h"

MF_API MFLanguage MFTranslation_GetSystemLanguage()
{
	// the Null device doesnt know anything about languages ;)
	return MFLang_English;
}

#endif
