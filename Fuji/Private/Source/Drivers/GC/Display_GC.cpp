#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_GC

#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

bool isortho = false;
float fieldOfView;

extern MFVector gClearColour;

void Display_DestroyWindow()
{
	MFCALLSTACK;
}

int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	return 0;
}

void Display_ResetDisplay()
{
	MFCALLSTACK;
}

void Display_DestroyDisplay()
{
	MFCALLSTACK;
}

void Display_BeginFrame()
{
	MFCALLSTACK;
}

void Display_EndFrame()
{
	MFCALLSTACK;
}

void Display_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void Display_ClearScreen(uint32 flags)
{
	MFCALLSTACK;
}

void SetViewport(float x, float y, float width, float height)
{
	MFCALLSTACK;
}

void ResetViewport()
{
	MFCALLSTACK;
}

#endif
