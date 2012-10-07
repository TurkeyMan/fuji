#include "Fuji.h"

#if MF_INPUT == MF_DRIVER_DC

#include "MFVector.h"
#include "Input_Internal.h"
#include "Heap.h"
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

/**** Platform Specific Functions ****/

void Input_InitModulePlatformSpecific()
{
	CALLSTACK;
}

void Input_DeinitModulePlatformSpecific()
{
	CALLSTACK;
}

void Input_UpdatePlatformSpecific()
{

}

void Input_GetDeviceStatusInternal(int device, int id, DeviceStatus *pDeviceStatus)
{
	pDeviceStatus->available = false;
	pDeviceStatus->status = IDS_Disconnected;
}

void Input_GetGamepadStateInternal(int id, GamepadState *pGamepadState)
{
	CALLSTACK;
}

void Input_GetKeyStateInternal(int id, KeyState *pKeyState)
{
	CALLSTACK;
}

void Input_GetMouseStateInternal(int id, MouseState *pMouseState)
{
	CALLSTACK;
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
	return "Button ?";
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

#endif
