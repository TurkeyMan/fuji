#include "Fuji.h"

#if MF_SYSTEM == MF_DRIVER_GC

#include "MFSystem.h"

MFPlatform gCurrentPlatform = FP_GC;

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

MF_API uint64 MFSystem_ReadRTC()
{
	return 0;
}

MF_API uint64 MFSystem_GetRTCFrequency()
{
	return 1000;
}

MF_API const char * MFSystem_GetSystemName()
{
#if defined(MF_GC)
	return "Nintendo Gamecube";
#elif defined(MF_WII)
	return "Nintendo Wii";
#endif
}

#endif
