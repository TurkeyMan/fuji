#include "Fuji.h"
#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

bool isortho = false;
float fieldOfView;

extern MFVector gClearColour;

void MFDisplay_DestroyWindow()
{
	MFCALLSTACK;
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFCALLSTACK;
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;
}

void MFDisplay_BeginFrame()
{
	MFCALLSTACK;
}

void MFDisplay_EndFrame()
{
	MFCALLSTACK;
}

void MFDisplay_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void MFDisplay_ClearScreen(uint32 flags)
{
	MFCALLSTACK;
}

void MFDisplay_SetViewport(float x, float y, float width, float height)
{
	MFCALLSTACK;
}

void MFDisplay_ResetViewport()
{
	MFCALLSTACK;
}

float MFDisplay_GetNativeAspectRatio()
{
	return 4.0f/3.0f;
}

bool MFDisplay_IsWidescreen()
{
	return false;
}
