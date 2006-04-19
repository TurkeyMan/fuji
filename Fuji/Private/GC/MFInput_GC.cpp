#include "Fuji.h"

#include "MFInput_Internal.h"
#include "MFHeap.h"
#include "MFIni.h"

#include <pad.h>

/*** Structure definitions ***/

/*** Globals ***/

int	gGamepadCount	= 4;
int	gKeyboardCount	= 0;
int	gMouseCount		= 0;

char gKeyState[256];

bool gExclusiveMouse = false;
float deadZone = 0.3f;

float mouseMultiplier = 1.0f;

PADStatus pads[4];

/**** Platform Specific Functions ****/

void MFInput_InitModulePlatformSpecific()
{
	MFCALLSTACK;

	PAD_Init();
}

void MFInput_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;
}

void MFInput_UpdatePlatformSpecific()
{
	PAD_Read(pads);
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	return IDS_Disconnected;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	MFCALLSTACK;
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFCALLSTACK;
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFCALLSTACK;
}

const char* MFInput_GetDeviceName(int source, int sourceID)
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

const char* MFInput_GetGamepadButtonName(int sourceID, int type)
{
	return "Button ?";
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
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
