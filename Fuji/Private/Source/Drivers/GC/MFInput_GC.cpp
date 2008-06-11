#include "Fuji.h"

#if MF_INPUT == MF_DRIVER_GC

#include "MFInput_Internal.h"
#include "MFHeap.h"
#include "MFIni.h"

#include <ogc/pad.h>

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

const char * const gGCButtonNames[] =
{
// Gamecube controller enums
	"A",
	"X",
	"B",
	"Y",
	"N/A",
	"Z",
	"L",
	"R",
	"Start",
	"N/A",
	"N/A",
	"N/A",

// general controller enums
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Analog X-Axis",
	"Analog Y-Axis",
	"C Analog X-Axis",
	"C Analog Y-Axis"
};

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

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	switch(source)
	{
		case IDD_Gamepad:
			return "Gamecube Gamepad";
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
	MFDebug_Assert(sourceID < 4, "Invalid source ID...");

	return gGCButtonNames[button];
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}

#endif
