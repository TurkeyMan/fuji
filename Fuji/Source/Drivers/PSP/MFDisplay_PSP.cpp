#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_PSP

#include "MFRenderer_Internal.h"

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	return MFRenderer_CreateDisplay();
}

void MFDisplay_DestroyWindow()
{
	MFRenderer_DestroyDisplay();
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
	return 16.0f/9.0f;
}

bool MFDisplay_IsWidescreen()
{
	return true;
}

#endif
