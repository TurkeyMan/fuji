#include "Fuji_Internal.h"

#if MF_SYSTEM == MF_DRIVER_WEB

#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"

extern MFInitParams gInitParams;

#if !defined(_FUJI_UTIL)
void MFSystem_InitModulePlatformSpecific()
{
	gpEngineInstance->currentPlatform = FP_Web;
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

MF_API uint64 MFSystem_ReadRTC()
{
	return 0;
}

MF_API uint64 MFSystem_GetRTCFrequency()
{
	return 1000000; // microseconds
}

MF_API const char * MFSystem_GetSystemName()
{
	// TODO: get js runtime?
	static char buffer[128] = "Web";
//	uint32 bufferSize = sizeof(buffer);
	//...
	return buffer;
}

#endif // MF_SYSTEM
