#include "Fuji.h"

#if MF_SYSTEM == NULL

#include <time.h>

MFPlatform gCurrentPlatform = FP_Unknown;

void MFSystem_InitModulePlatformSpecific()
{
}

void MFSystem_DeinitModulePlatformSpecific()
{
}

void MFSystem_HandleEventsPlatformSpecific()
{
}

void MFSystem_UpdatePlatformSpecific()
{
}

void MFSystem_DrawPlatformSpecific()
{
}

uint64 MFSystem_ReadRTC()
{
	return (uint64)clock();
}

uint64 MFSystem_GetRTCFrequency()
{
	return CLOCKS_PER_SEC;
}

const char * MFSystem_GetSystemName()
{
	return "null";
}

#endif
