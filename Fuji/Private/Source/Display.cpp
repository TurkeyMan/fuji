#include "Common.h"
#include "Display.h"
#include "DebugMenu.h"
#include "View.h"

DisplaySettings display;

Vector4 gClearColour = Vector(0.f,0.f,0.22f,1.f);

void Display_InitModule()
{
	CALLSTACK;

	int error;

	DebugMenu_AddMenu("Display Options", "Fuji Options");

	// create the display
	error = Display_CreateDisplay(640, 480, 32, 60, true, false, false, false);
	if(error) return;

	View::defaultView.view.SetIdentity();
	View::defaultView.SetProjection((PI*2.0f)*0.16666f);
	View::defaultView.viewProj = View::defaultView.projection;
	View::defaultView.viewProjDirty = false;
	View::defaultView.isOrtho = false;
	View::UseDefault();
}

void Display_DeinitModule()
{
	CALLSTACK;

	Display_DestroyDisplay();
}
