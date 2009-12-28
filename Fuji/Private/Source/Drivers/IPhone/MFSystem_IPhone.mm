#include "Fuji.h"

#if MF_SYSTEM == MF_DRIVER_IPHONE

#import <UIKit/UIKit.h>

#include "MFSystem.h"

#include <mach/mach.h>
#include <mach/mach_time.h>

MFPlatform gCurrentPlatform = FP_IPhone;

static mach_timebase_info_data_t timebase;
static uint32 frequency;

void MFSystem_InitModulePlatformSpecific()
{
	mach_timebase_info(&timebase);
	frequency = 1000000000 * timebase.numer / timebase.denom;
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
	return (uint64)mach_absolute_time();
}

uint64 MFSystem_GetRTCFrequency()
{
	return frequency;
}

const char * MFSystem_GetSystemName()
{
	return "iPhone";
}

#endif
