#include "Fuji.h"
#include "Display.h"
#include "MFView.h"
#include "MFFont.h"
#include "MFInput.h"
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


#define CHK_BUTTON(x) \
     if (MFInput_Read(x, IDD_Gamepad)!=0.0f)  printf(#x" ");

void Game_Update()
{
   
     printf("Time is %f- %f FPS\t", gSystemTimer.GetSecondsF(), gSystemTimer.GetFPS());
     printf("Left (%1.4f, %1.4f) ", 
	     MFInput_Read(Axis_LX, IDD_Gamepad), MFInput_Read(Axis_LY, IDD_Gamepad));
     printf("Left (%1.4f, %1.4f) ", 
	     MFInput_Read(Axis_RX, IDD_Gamepad), MFInput_Read(Axis_RY, IDD_Gamepad));
     
     CHK_BUTTON(Button_P2_Cross);
     CHK_BUTTON(Button_P2_Circle);
     CHK_BUTTON(Button_P2_Box);
     CHK_BUTTON(Button_P2_Triangle);
     
     CHK_BUTTON(Button_P2_L1);
     CHK_BUTTON(Button_P2_R1);
     CHK_BUTTON(Button_P2_L2);
     CHK_BUTTON(Button_P2_R2);
     
     CHK_BUTTON(Button_P2_Start);
     CHK_BUTTON(Button_P2_Select);
     
     CHK_BUTTON(Button_P2_L3);
     CHK_BUTTON(Button_P2_R3);
     
     CHK_BUTTON(Button_DUp);
     CHK_BUTTON(Button_DDown);
     CHK_BUTTON(Button_DLeft);
     CHK_BUTTON(Button_DRight);
     printf("\n");

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
