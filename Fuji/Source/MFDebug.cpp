#include "Fuji_Internal.h"

#if !defined(_RETAIL)

static int gWarningLevel = 4;
static int gLogLevel = 2;

MF_API void MFDebug_Error(const char *pErrorMessage)
{
	MFDebug_Message(MFStr("Error: %s", pErrorMessage));
	MFDebug_Breakpoint();
}

MF_API void MFDebug_Warn(int level, const char *pWarningMessage)
{
	if(level <= gWarningLevel)
	{
		MFDebug_Message(MFStr("Warning(%d): %s", level, pWarningMessage));
	}
}

MF_API void MFDebug_Log(int level, const char *pMessage)
{
	if(level <= gLogLevel)
	{
		MFDebug_Message(pMessage);
	}
}

MF_API int MFDebug_SetMaximumWarningLevel(int maxLevel)
{
	int old = gWarningLevel;
	gWarningLevel = maxLevel;
	return old;
}

MF_API int MFDebug_SetMaximumLogLevel(int maxLevel)
{
	int old = gLogLevel;
	gLogLevel = maxLevel;
	return old;
}

#endif // !defined(_RETAIL)
