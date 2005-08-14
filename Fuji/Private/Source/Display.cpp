#include "Common.h"
#include "Display_Internal.h"
#include "DebugMenu.h"
#include "View.h"

float AspectRatio[Aspect_Max] = 
{
	1.0f,
	1.3333333333333333333333333333333f,
	1.7777777777777777777777777777778f,
	1.6f
};

DisplaySettings display;

MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);

void Display_InitModule()
{
	CALLSTACK;

	int error;

	DebugMenu_AddMenu("Display Options", "Fuji Options");

	// create the display
	error = Display_CreateDisplay(640, 480, 32, 60, true, false, false, false);
	if(error) return;
}

void Display_DeinitModule()
{
	CALLSTACK;

	Display_DestroyDisplay();
}
