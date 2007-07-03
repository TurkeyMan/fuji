#include "Fuji.h"
#include "Display_Internal.h"

void MFDisplay_DestroyWindow()
{
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	return 0;
}

void MFDisplay_ResetDisplay()
{
}

void MFDisplay_DestroyDisplay()
{
}

void MFDisplay_BeginFrame()
{
}

void MFDisplay_EndFrame()
{
}

void MFDisplay_SetClearColour(float r, float g, float b, float a)
{
}

void MFDisplay_ClearScreen(uint32 flags)
{
}

void MFDisplay_SetViewport(float x, float y, float width, float height)
{
}

void MFDisplay_ResetViewport()
{
}

bool MFDisplay_IsWidescreen()
{
	return false;
}
