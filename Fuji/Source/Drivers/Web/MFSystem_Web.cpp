#include "Fuji_Internal.h"

#if MF_SYSTEM == MF_DRIVER_WEB

#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"
#include "MFFileSystem.h"

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

MF_API void MFSystem_SystemTime(MFSystemTime* pSystemTime)
{
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_SystemTimeToFileTime(const MFSystemTime *pSystemTime, MFFileTime *pFileTime)
{
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_FileTimeToSystemTime(const MFFileTime *pFileTime, MFSystemTime *pSystemTime)
{
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_SystemTimeToLocalTime(const MFSystemTime *pSystemTime, MFSystemTime *pLocalTime)
{
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_LocalTimeToSystemTime(const MFSystemTime *pLocalTime, MFSystemTime *pSystemTime)
{
	MFDebug_Assert(false, "todo");
}

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
