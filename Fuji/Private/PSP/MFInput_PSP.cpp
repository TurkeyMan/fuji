#include "Common.h"

#include "MFVector.h"
#include "Input_Internal.h"
#include "Heap.h"
#include "MFIni.h"

#include <pspctrl.h>

/*** Structure definitions ***/

/*** Globals ***/

int	gGamepadCount	= 1;
int	gKeyboardCount	= 0;
int	gMouseCount		= 0;

char gKeyState[256];

float deadZone = 0.2f;

SceCtrlData padData;

const char * const PSPButtons[] =
{
// PSP controller enums
	"X",
	"Circle",
	"Box",
	"Triangle",
	"N/A",
	"N/A",
	"Left Trigger",
	"Right Trigger",
	"Start",
	"Select",
	"N/A",
	"N/A",

// general controller enums
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Left Analog X-Axis",
	"Left Analog Y-Axis",
	"N/A",
	"N/A"
};

/**** Platform Specific Functions ****/

void Input_InitModulePlatformSpecific()
{
	CALLSTACK;

	memset(gKeyState, 0, 256);

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

void Input_DeinitModulePlatformSpecific()
{
	CALLSTACK;
}

void Input_UpdatePlatformSpecific()
{
	sceCtrlReadBufferPositive(&padData, 1);
}

void Input_GetDeviceStatusInternal(int device, int id, DeviceStatus *pDeviceStatus)
{
	if(device == IDD_Gamepad && id == 0)
	{
		pDeviceStatus->available = true;
		pDeviceStatus->status = IDS_Ready;
	}
	else
	{
		pDeviceStatus->available = false;
		pDeviceStatus->status = IDS_Disconnected;
	}
}

void Input_GetGamepadStateInternal(int id, GamepadState *pGamepadState)
{
	CALLSTACK;

	memset(pGamepadState, 0, sizeof(GamepadState));

	pGamepadState->values[Button_P2_Cross] = (padData.Buttons & PSP_CTRL_CROSS) ? 1.0f : 0.0f;
	pGamepadState->values[Button_P2_Circle] = (padData.Buttons & PSP_CTRL_CIRCLE) ? 1.0f : 0.0f;
	pGamepadState->values[Button_P2_Box] = (padData.Buttons & PSP_CTRL_SQUARE) ? 1.0f : 0.0f;
	pGamepadState->values[Button_P2_Triangle] = (padData.Buttons & PSP_CTRL_TRIANGLE) ? 1.0f : 0.0f;

	pGamepadState->values[Button_P2_R1] = 0.0f;
	pGamepadState->values[Button_P2_L1] = 0.0f;
	pGamepadState->values[Button_P2_L2] = (padData.Buttons & PSP_CTRL_LTRIGGER) ? 1.0f : 0.0f;
	pGamepadState->values[Button_P2_R2] = (padData.Buttons & PSP_CTRL_RTRIGGER) ? 1.0f : 0.0f;

	pGamepadState->values[Button_P2_Start] = (padData.Buttons & PSP_CTRL_START) ? 1.0f : 0.0f;
	pGamepadState->values[Button_P2_Select] = (padData.Buttons & PSP_CTRL_SELECT) ? 1.0f : 0.0f;

	pGamepadState->values[Button_P2_LThumb] = 0.0f;
	pGamepadState->values[Button_P2_RThumb] = 0.0f;

	pGamepadState->values[Button_DUp] = (padData.Buttons & PSP_CTRL_UP) ? 1.0f : 0.0f;
	pGamepadState->values[Button_DDown] = (padData.Buttons & PSP_CTRL_DOWN) ? 1.0f : 0.0f;
	pGamepadState->values[Button_DLeft] = (padData.Buttons & PSP_CTRL_LEFT) ? 1.0f : 0.0f;
	pGamepadState->values[Button_DRight] = (padData.Buttons & PSP_CTRL_RIGHT) ? 1.0f : 0.0f;

	pGamepadState->values[Axis_LX] = (float)padData.Lx/127.5f - 1.0f;
	pGamepadState->values[Axis_LY] = -((float)padData.Ly/127.5f - 1.0f);
	pGamepadState->values[Axis_RX] = 0.0f;
	pGamepadState->values[Axis_RY] = 0.0f;

	if(fabsf(pGamepadState->values[Axis_LX]) < deadZone) pGamepadState->values[Axis_LX] = 0.0f;
	if(fabsf(pGamepadState->values[Axis_LY]) < deadZone) pGamepadState->values[Axis_LY] = 0.0f;
}

void Input_GetKeyStateInternal(int id, KeyState *pKeyState)
{
	CALLSTACK;

	memset(pKeyState, 0, sizeof(KeyState));
}

void Input_GetMouseStateInternal(int id, MouseState *pMouseState)
{
	CALLSTACK;

	memset(pMouseState, 0, sizeof(MouseState));
}

const char* Input_GetDeviceName(int source, int sourceID)
{
	const char *pText = NULL;

	switch(source)
	{
		case IDD_Gamepad:
		{
			pText = "Gamepad";
			break;
		}
		case IDD_Mouse:
			pText = "Mouse";
			break;
		case IDD_Keyboard:
			pText = "Keyboard";
			break;
		default:
			break;
	}

	return pText;
}

const char* Input_GetGamepadButtonName(int sourceID, int type)
{
	DBGASSERT(sourceID < 1, "Only one gamepad available on PSP...");

	return PSPButtons[type];
}

bool Input_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	switch(keyboardState)
	{
		case KSS_NumLock:
			break;

		case KSS_CapsLock:
			break;

		case KSS_ScrollLock:
			break;

		case KSS_Insert:
			break;
	}

	return 0;
}
