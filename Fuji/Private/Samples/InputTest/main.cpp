#include "Fuji.h"
#include "MFSystem.h"
#include "Display.h"
#include "MFView.h"
#include "MFFont.h"
#include "MFInput.h"

/**** Globals ****/

/**** Functions ****/

void Game_InitSystem()
{
	MFCALLSTACK;

	gDefaults.input.useXInput = false;
}

void Game_Init()
{
	MFCALLSTACK;
}

void Game_Update()
{
	MFCALLSTACK;
}

void Game_Draw()
{
	MFCALLSTACK;

	MFDisplay_SetClearColour(0.f, 0.f, 0.2f, 1.f);
	MFDisplay_ClearScreen();

	MFView_Push();
	MFView_SetOrtho();

	const char *pText;
	float x, y;
	int a, b;

	x = 30.0f;
	for(a=0; a<MFInput_GetNumGamepads(); a++)
	{
		if(MFInput_IsAvailable(IDD_Gamepad, a))
		{
			MFFont_DrawText(MFFont_GetDebugFont(), x-10.0f, 15.0f, 15.0f, MFVector::one, MFStr("%s (%d):", MFInput_GetDeviceName(IDD_Gamepad,a), a));

			if(MFInput_IsReady(IDD_Gamepad, a))
			{
				y = 30.0f;
				for(b=0; b<GamepadType_Max; b++)
				{
					MFFont_DrawText(MFFont_GetDebugFont(), x, y, 15.0f, MFVector::one, MFStr("%s: %.3g", MFInput_EnumerateString(b, IDD_Gamepad, a), MFInput_Read(b, IDD_Gamepad, a)));
					y += 15.0f;
				}
			}
			else
			{
				MFFont_DrawText(MFFont_GetDebugFont(), x, 30.0f, 15.0f, MakeVector(1.0f, 0.0f, 0.0f, 1.0f), "Disconnected...");
			}

			x += 170.0f;
		}
	}

	int numMouses = MFInput_GetNumPointers();

	x = 80.0f;
	for(a=0; a<numMouses; a++)
	{
		MFFont_DrawText(MFFont_GetDebugFont(), x, 355.0f, 15.0f, MFVector::one, MFStr("Mouse Pos: %g, %g", MFInput_Read(Mouse_XPos, IDD_Mouse, a), MFInput_Read(Mouse_YPos, IDD_Mouse, a)));
		MFFont_DrawText(MFFont_GetDebugFont(), x, 370.0f, 15.0f, MFVector::one, MFStr("Mouse Delta: %g, %g", MFInput_Read(Mouse_XDelta, IDD_Mouse, a), MFInput_Read(Mouse_YDelta, IDD_Mouse, a)));
		MFFont_DrawText(MFFont_GetDebugFont(), x, 385.0f, 15.0f, MFVector::one, MFStr("Mouse Wheel: %g, %g", MFInput_Read(Mouse_Wheel, IDD_Mouse, a), MFInput_Read(Mouse_Wheel2, IDD_Mouse, a)));
		pText = "Mouse Buttons:";
		for(b=Mouse_MaxAxis; b<Mouse_Max; b++)
		{
			if(MFInput_Read(b, IDD_Mouse, a))
			{
				pText = MFStr("%s %s", pText, MFInput_EnumerateString(b, IDD_Mouse, a));
			}
		}
		MFFont_DrawText(MFFont_GetDebugFont(), x, 400.0f, 15.0f, MFVector::one, pText);

		x += 170.0f;
	}

	pText = "Keys:";
	for(a=0; a<Key_Max; a++)
	{
		if(MFInput_Read(a, IDD_Keyboard, 0))
		{
			pText = MFStr("%s %s", pText, MFInput_EnumerateString(a, IDD_Keyboard, 0));
		}
	}
	MFFont_DrawText(MFFont_GetDebugFont(), 80.0f, 430.0f, 15.0f, MFVector::one, pText);

	MFView_Pop();
}

void Game_Deinit()
{
	MFCALLSTACK;
}
