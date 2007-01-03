#include "Fuji.h"
#include "System_Internal.h"

MFPlatform gCurrentPlatform = FP_DC;

int main()
{
	kos_init_all(ALL_ENABLE, ROMDISK_NONE);
//	kos_init_all(THD_ENABLE | IRQ_ENABLE | TA_ENABLE, ROMDISK_NONE);

	System_GameLoop();

	kos_shutdown_all();

	return 0;
}

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
	return 0;//timer_ms_gettime64();
}

uint64 MFSystem_GetRTCFrequency()
{
	return 1000;
}

const char * MFSystem_GetSystemName()
{
	return "Dreamcast";
}
