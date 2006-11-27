#include "Fuji.h"
#include "Display.h"
#include "MFView.h"
#include "MFFont.h"
#include <stdio.h>
#include "Timer.h"

/**** Globals ****/


/**** Functions ****/

void Game_InitSystem()
{
	MFCALLSTACK;
}

void Game_Init()
{
	MFCALLSTACK;
}

void Game_Update()
{
    printf("Time is %f \t %f FPS\n", gSystemTimer.GetSecondsF(), gSystemTimer.GetFPS());
   
	MFCALLSTACK;
}

void Game_Draw()
{
	MFCALLSTACK;

	// set clear colour and clear the screen
	MFDisplay_SetClearColour(0.f, 0.f, 0.2f, 1.f);
	MFDisplay_ClearScreen();

	// push current view onto the stack
	MFView_Push();

	// set orthographic projection
	MFView_SetOrtho();

	// render some text
	MFFont_DrawText(MFFont_GetDebugFont(), 200.f, 200.f, 50.f, MFVector::one, "Hello World!");

	// pop the current view
	MFView_Pop();
}

void Game_Deinit()
{
	MFCALLSTACK;
}
