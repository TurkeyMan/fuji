#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_NULL

#include "Display_Internal.h"

MFRect gCurrentViewport;

void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)gDisplay.width;
	gCurrentViewport.height = (float)gDisplay.height;

	return 0;
}

void MFRenderer_DestroyDisplay()
{
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

void MFDisplay_GetViewport(MFRect *pRect)
{
	*pRect = gCurrentViewport;
}

void MFDisplay_SetViewport(MFRect *pRect)
{
	gCurrentViewport = *pRect;
}

void MFDisplay_ResetViewport()
{
	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)gDisplay.width;
	gCurrentViewport.height = (float)gDisplay.height;
}

bool MFDisplay_IsWidescreen()
{
	return false;
}

#endif
