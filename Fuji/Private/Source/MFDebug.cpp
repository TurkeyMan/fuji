#include "Fuji.h"

static int gWarningLevel = 4;

void MFDebug_Error(const char *pErrorMessage)
{
	MFDebug_Message(MFStr("Error: %s", pErrorMessage));
	MFDebug_Breakpoint();
}

void MFDebug_Warn(int level, const char *pWarningMessage)
{
	if(level <= gWarningLevel)
	{
		MFDebug_Message(MFStr("Warning: %s", pWarningMessage));
	}
}

void MFDebug_SetMaximumWarningLevel(int maxLevel)
{
	gWarningLevel = maxLevel;
}
