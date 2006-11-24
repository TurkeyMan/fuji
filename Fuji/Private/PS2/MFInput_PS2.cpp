#include "Fuji.h"

#include "MFVector.h"
#include "MFInput_Internal.h"
#include "MFHeap.h"
#include "MFIni.h"

/*** Structure definitions ***/

/*** Globals ***/

int	gGamepadCount	= 0;
int	gKeyboardCount	= 0;
int	gMouseCount		= 0;

char gKeyState[256];

bool gExclusiveMouse = false;
float deadZone = 0.3f;

float mouseMultiplier = 1.0f;

static const char * const gPS2Buttons[] =
{
// PS2 controller enums
	"X",
	"Circle",
	"Box",
	"Triangle",
	"L1",
	"R1",
	"L2",
	"R2",
	"Start",
	"Select",
	"L3",
	"R3",

// general controller enums
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Left X-Axis",
	"Left Y-Axis",
	"Right X-Axis",
	"Right Y-Axis"
};

/**** Platform Specific Functions ****/

void MFInput_InitModulePlatformSpecific()
{
	MFCALLSTACK;
}

void MFInput_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;
}

void MFInput_UpdatePlatformSpecific()
{
	MFCALLSTACK;
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	if(device == IDD_Gamepad && id < 2)
		return IDS_Disconnected;

	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	MFCALLSTACK;

	MFZeroMemory(pGamepadState, sizeof(MFGamepadState));

	// ... read gamepad
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFCALLSTACK;

	MFZeroMemory(pKeyState, sizeof(MFKeyState));
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFCALLSTACK;

	MFZeroMemory(pMouseState, sizeof(MFMouseState));
}

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	switch(source)
	{
		case IDD_Gamepad:
			return "DualShock 2";
		case IDD_Mouse:
			return "Mouse";
		case IDD_Keyboard:
			return "Keyboard";
		default:
			break;
	}

	return NULL;
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	MFDebug_Assert(sourceID < 2, "Only two gamepads available on PS2..."); // multitap??

	return gPS2Buttons[button];
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
