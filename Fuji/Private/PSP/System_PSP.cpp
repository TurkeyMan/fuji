#include "Common.h"
#include "System_Internal.h"

#include "MFFileSystem_Internal.h"
#include "DebugMenu_Internal.h"
#include "View_Internal.h"
#include "Display_Internal.h"
#include "Input_Internal.h"

#include <pspkernel.h>

#include "sceRtc.h"

/* Define the module info section */
PSP_MODULE_INFO("FUJIPSP", 0, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

void dump_threadstatus(void);

extern int gQuit;
extern int gRestart;

/* Exit callback */
int ExitCallback(int count, int arg, void *common)
{
	// terminate the app
	gQuit = 1;
	gRestart = 0;

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize size, void *arg)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (void*)ExitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("Update Thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

int main()
{
//	srand(GetTickCount());

	SetupCallbacks();

	System_GameLoop();

	sceKernelExitGame();

	return 0;
}

uint64 RDTSC()
{
//	ScePspDateTime time;
//	sceRtcGetCurrentClockLocalTime(&time);
//	return time.microsecond + time.second * 1000000 + time.minute * 60000000;
	return gFrameCount;
}

uint64 GetTSCFrequency()
{
	return 60;//(uint64)sceRtcGetTickResolution();
}
