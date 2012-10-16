#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_OSX

#include "MFDisplay_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFView.h"
#include "MFSystem.h"
#include "DebugMenu.h"
#include "MFHeap.h"
#include <stdio.h>

static void MFDisplay_ResetDisplay();


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

	MFRenderer_ResetDisplay();
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;

	FreeModes();
}

bool MFDisplay_SetDisplayMode(int width, int height, bool bFullscreen)
{
	// do any stuff the window manager needs to do to swap display modes or swap between window and fullscreen
	//...

	return MFRenderer_SetDisplayMode(width, height, bFullscreen);
}

void MFDisplay_GetNativeRes(MFRect *pRect)
{

}

void MFDisplay_GetDefaultRes(MFRect *pRect)
{

}

float MFDisplay_GetNativeAspectRatio()
{

}

bool MFDisplay_IsWidescreen()
{

}

void MFDisplay_HandleEventsX11()
{
	MFCALLSTACK;
}

#endif
