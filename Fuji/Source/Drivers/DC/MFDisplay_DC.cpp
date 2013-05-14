#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_DC

#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

bool isortho = false;
float fieldOfView;

extern MFVector gClearColour;

void Display_DestroyWindow()
{
	CALLSTACK;
}

int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	CALLSTACK;

	return 0;
}

void Display_ResetDisplay()
{
	CALLSTACK;
}

void Display_DestroyDisplay()
{
	CALLSTACK;
}

void Display_BeginFrame()
{
	CALLSTACK;
}

void Display_EndFrame()
{
	CALLSTACK;
}

void Display_ClearScreen(uint32 flags)
{
	CALLSTACKc;
}

void SetViewport(float x, float y, float width, float height)
{
	CALLSTACK;
}

void ResetViewport()
{
	CALLSTACK;
}

#endif
