#include "Fuji.h"
#include "Display_Internal.h"
#include "DebugMenu.h"
#include "MFView.h"
#include "MFSystem.h"

MFDisplaySettings gDisplay;

void MFDisplay_InitModule()
{
	MFCALLSTACK;

	int error;

	DebugMenu_AddMenu("Display Options", "Fuji Options");

	// create the display
	error = MFDisplay_CreateDisplay(gDefaults.display.displayWidth, gDefaults.display.displayHeight, 32, 60, true, false, false, false);
	if(error) return;
}

void MFDisplay_DeinitModule()
{
	MFCALLSTACK;

	MFDisplay_DestroyDisplay();
}

void MFDisplay_GetDisplayRect(MFRect *pRect)
{
	pRect->x = 0.0f;
	pRect->y = 0.0f;
	pRect->width = (float)gDisplay.width;
	pRect->height = (float)gDisplay.height;
}
