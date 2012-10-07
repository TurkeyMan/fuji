#include "Fuji.h"

#if MF_SYSTEM == MF_DRIVER_DC

#include "System_Internal.h"

MFPlatform gCurrentPlatform = FP_DC;

void MFSystem_InitModulePlatformSpecific()
{
	kos_init_all(ALL_ENABLE, ROMDISK_NONE);
//	kos_init_all(THD_ENABLE | IRQ_ENABLE | TA_ENABLE, ROMDISK_NONE);
}

void MFSystem_DeinitModulePlatformSpecific()
{
	kos_shutdown_all();
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
	return 0;//timer_ms_gettime64();
}

MF_API uint64 MFSystem_GetRTCFrequency()
{
	return 1000;
}

MF_API const char * MFSystem_GetSystemName()
{
	return "Dreamcast";
}

#endif
