#include "Common.h"
#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

bool isortho = false;
float fieldOfView;

extern Vector4 gClearColour;

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

void Display_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
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
