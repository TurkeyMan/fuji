#include "Fuji.h"
#include "MFSystem_Internal.h"
#include "MFHeap.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HINSTANCE apphInstance;
extern int gQuit;

MFPlatform gCurrentPlatform = FP_PC;

char *gpCommandLineBuffer = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmsShow)
{
	MFCALLSTACK;

	apphInstance = hInstance;
	gpCommandLineBuffer = lpCmdLine;

	// HACK: this is to force dual-core or multi-processor systems to use the first cpu for timing.
//	DWORD proc, system;
//	GetProcessAffinityMask(GetCurrentProcess(), &proc, &system);
	SetProcessAffinityMask(GetCurrentProcess(), 1);

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
	MFCALLSTACK;

	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(!GetMessage(&msg, NULL, 0, 0)) gQuit=true;
		else
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
	}
}

void MFSystem_UpdatePlatformSpecific()
{
}

void MFSystem_DrawPlatformSpecific()
{
}

uint64 MFSystem_ReadRTC()
{
	uint64 tickCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&tickCount);
	return tickCount;
}

uint64 MFSystem_GetRTCFrequency()
{
	uint64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return freq;
}

const char * MFSystem_GetSystemName()
{
	static char buffer[128];
	DWORD bufferSize = sizeof(buffer);
	GetComputerName(buffer, &bufferSize);
	return buffer;
}
