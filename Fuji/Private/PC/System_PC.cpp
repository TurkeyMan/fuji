#include "Common.h"
#include "System.h"

HINSTANCE apphInstance;
extern int gQuit;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmsShow)
{
	apphInstance = hInstance;

	srand(GetTickCount());

	System_GameLoop();
}

uint64 ReadPerformanceCounter()
{
	uint64 tickCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&tickCount);
	return tickCount;
}

uint64 GetPerfprmanceFrequency()
{
	uint64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return freq;
}

void DoMessageLoop()
{
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
