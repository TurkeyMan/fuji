#include "Fuji.h"

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
		MFDebug_Message(MFStr("Warning: %s", pWarningMessage));
	}
}

MF_API void MFDebug_Log(int level, const char *pMessage)
{
	if(level <= gLogLevel)
	{
		MFDebug_Message(pMessage);
	}
}

MF_API void MFDebug_SetMaximumWarningLevel(int maxLevel)
{
	gWarningLevel = maxLevel;
}

MF_API void MFDebug_SetMaximumLogLevel(int maxLevel)
{
	gLogLevel = maxLevel;
}

#endif // !defined(_RETAIL)
