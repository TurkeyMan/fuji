#include "Fuji.h"

#if MF_SYSTEM == MF_DRIVER_PS2

#include "MFSystem_Internal.h"

#include <kernel.h>

// Timer Registers
#define T1_COUNT	((volatile unsigned long*)0x10000800)
#define T1_MODE		((volatile unsigned long*)0x10000810)
#define T1_COMP		((volatile unsigned long*)0x10000820)
#define T1_HOLD		((volatile unsigned long*)0x10000830)

#define Tn_MODE(CLKS,GATE,GATS,GATM,ZRET,CUE,CMPE,OVFE,EQUF,OVFF) \
		(u32)((u32)(CLKS) | ((u32)(GATE) << 2) | \
		((u32)(GATS) << 3) | ((u32)(GATM) << 4) | \
		((u32)(ZRET) << 6) | ((u32)(CUE) << 7) | \
		((u32)(CMPE) << 8) | ((u32)(OVFE) << 9) | \
		((u32)(EQUF) << 10) | ((u32)(OVFF) << 11))

#define kBUSCLK			(147456000)
#define kBUSCLKBY16		(kBUSCLK / 16)
#define kBUSCLKBY256	(kBUSCLK / 256)

enum
{
	kINTC_GS,
	kINTC_SBUS,
	kINTC_VBLANK_START,
	kINTC_VBLANK_END,
	kINTC_VIF0,
	kINTC_VIF1,
	kINTC_VU0,
	kINTC_VU1,
	kINTC_IPU,
	kINTC_TIMER0,
	kINTC_TIMER1
};

MFPlatform gCurrentPlatform = FP_PS2;

// Timer statics
static int s_tnInterruptID = -1;
static u64 s_tnInterruptCount = 0;

// Timer interrupt handler
int tnTimeInterrupt(int ca)
{
	s_tnInterruptCount++;

	// A write to the overflow flag will clear the overflow flag
	*T1_MODE |= (1 << 11);
	return -1;
}

int main()
{
	MFSystem_GameLoop();

	return 0;
}

void MFSystem_InitModulePlatformSpecific()
{
	// Init the timer and register the interrupt handler
	*T1_MODE = 0x0000;

	s_tnInterruptID = AddIntcHandler(kINTC_TIMER1, tnTimeInterrupt, 0);
	EnableIntc(kINTC_TIMER1);

	// Initialize the timer registers
	// CLKS: 0x02 - 1/256 of the BUSCLK (0x01 is 1/16th)
	//  CUE: 0x01 - Start/Restart the counting
	// OVFE: 0x01 - An interrupt is generated when an overflow occurs
	// --------------------------------------------------------------
	*T1_COUNT = 0;
	*T1_MODE = Tn_MODE(0x02, 0, 0, 0, 0, 0x01, 0, 0x01, 0, 0);

	s_tnInterruptCount = 0;
}

void MFSystem_DeinitModulePlatformSpecific()
{
	// Stop the timer
	*T1_MODE = 0x0000;

	// Disable the interrupt
	if (s_tnInterruptID >= 0)
	{
		DisableIntc(kINTC_TIMER1);
		RemoveIntcHandler(kINTC_TIMER1, s_tnInterruptID);
		s_tnInterruptID = -1;
	}

	s_tnInterruptCount = 0;
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
	uint64         t;
	// Tn_COUNT is 16 bit precision. Therefore, each
	// <s_tnInterruptCount> is 65536 ticks
	// ---------------------------------------------
	t = *T1_COUNT + (s_tnInterruptCount << 16);

	t = t * 1000000 / kBUSCLKBY256;

	return t;
}

uint64 MFSystem_GetRTCFrequency()
{
	// I am using 1/256 of the BUSCLK below in the Tn_MODE register
	// which means that the timer will count at a rate of:
	//   147,456,000 / 256 = 576,000 Hz
	// This implies that the accuracy of this timer is:
	//   1 / 576,000 = 0.0000017361 seconds (~1.74 usec!)

	return 1000000;
}

const char * MFSystem_GetSystemName()
{
	return "Playstation2";
}

#endif
