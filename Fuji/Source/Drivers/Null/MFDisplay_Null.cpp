#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_NULL

#include "Display_Internal.h"
#include "MFRenderer_Internal.h"

MFRect gCurrentViewport;

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)width;
	gCurrentViewport.height = (float)height;

	MFRenderer_CreateDisplay();

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFRenderer_ResetDisplay();
}

void MFDisplay_DestroyDisplay()
{
	MFRenderer_DestroyDisplay();
}

float MFDisplay_GetNativeAspectRatio()
{
	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);

	return rect.width / rect.height;
}

bool MFDisplay_IsWidescreen()
{
	return false;
}

#endif
