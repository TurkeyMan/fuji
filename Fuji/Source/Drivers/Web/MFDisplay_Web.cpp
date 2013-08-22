#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_WEB

#include "MFDisplay_Internal.h"
#include "MFRenderer_Internal.h"
#include "DebugMenu_Internal.h"
#include "MFSystem.h"

void MFDisplay_ResetDisplay();

extern MFSystemCallbackFunction pSystemCallbacks[MFCB_Max];

bool isortho = false;
float fieldOfView;

extern MFInitParams gInitParams;

bool initialised = false;


void MFDisplay_DestroyWindow()
{
	MFCALLSTACK;

	MFRenderer_DestroyDisplay();
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	gDisplay.fullscreenWidth = gDisplay.width = width;
	gDisplay.fullscreenHeight = gDisplay.height = height;
	gDisplay.refreshRate = rate;
	gDisplay.colourDepth = 32;
	gDisplay.windowed = true;
	gDisplay.wide = false;
	gDisplay.progressive = true;

	MFRenderer_CreateDisplay();

	initialised = true;

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

	MFRenderer_DestroyDisplay();
}

bool MFDisplay_SetDisplayMode(int width, int height, bool bFullscreen)
{

	return MFRenderer_SetDisplayMode(width, height, bFullscreen);
}

void MFDisplay_GetNativeRes(MFRect *pRect)
{
	pRect->x = pRect->y = 0.f;
	pRect->width = 800.f;
	pRect->height = 480.f;
}

void MFDisplay_GetDefaultRes(MFRect *pRect)
{
	return MFDisplay_GetNativeRes(pRect);
}

float MFDisplay_GetNativeAspectRatio()
{
	MFRect rect;
	MFDisplay_GetNativeRes(&rect);

	return rect.width / rect.height;
}

bool MFDisplay_IsWidescreen()
{
	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);

	return rect.width / rect.height >= 1.6f;
}

#endif // MF_DISPLAY
