#include "Fuji.h"
#include "MFSystem_Internal.h"

#include <time.h>

MFPlatform gCurrentPlatform = FP_Linux;

int main(int argc, char *argv[])
{
	MFCALLSTACK;

	MFSystem_GameLoop();

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
	return (uint64)clock();
}

uint64 MFSystem_GetRTCFrequency()
{
	return CLOCKS_PER_SEC;
}

const char * MFSystem_GetSystemName()
{
	return "Insert code to find system name here...";
}
