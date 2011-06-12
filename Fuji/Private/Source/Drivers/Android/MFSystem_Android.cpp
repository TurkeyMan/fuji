#include "Fuji.h"

#if MF_SYSTEM == MF_DRIVER_ANDROID

#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"

extern MFInitParams gInitParams;

extern int gQuit;

MFPlatform gCurrentPlatform = FP_Android;

char *gpCommandLineBuffer = NULL;

#if !defined(_FUJI_UTIL)
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
#endif

uint64 MFSystem_ReadRTC()
{
	uint64 tickCount = 0;
	return tickCount;
}

uint64 MFSystem_GetRTCFrequency()
{
	uint64 freq = 1;
	return freq;
}

const char * MFSystem_GetSystemName()
{
	static char buffer[128] = "Android";
	DWORD bufferSize = sizeof(buffer);
	//...
	return buffer;
}

#endif // MF_SYSTEM
