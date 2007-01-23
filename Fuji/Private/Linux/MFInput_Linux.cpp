#include "Fuji.h"
#include "MFInput_Internal.h"
#include "MFMath.h"
#include "../PC/MFInputMappings_PC.h"

#include "X11_linux.h"

#include <X11/extensions/XInput.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <errno.h>

/*** Structure definitions ***/

#define MAX_LINUX_GAMEPADS 16

struct LinuxGamepad
{
	char name[128];
	char identifier[128];
	char deviceName[64];

//	uint8 axisMap[64];
	int axis[32];
	char button[128];
	int numButtons;
	int numAxiis;

	int joyFD;
	bool bVirtualDevice;

	MFGamepadInfo *pGamepadInfo;
};


/*** Globals ***/

LinuxGamepad gGamepads[MAX_LINUX_GAMEPADS];
int gMaxGamepad = 0;

const char * gDeviceNames[] =
{
	"/dev/input/js%d",
	"/dev/input/djs%d",
	"/dev/js%d",
	"/dev/djs%d",
	NULL
};

extern MFGamepadInfo *pGamepadMappingRegistry;


// HACK: mapping for the second EMS2 gamepad
static const int gEMS2ButtonID[GamepadType_Max] =
{
	18,   // Button_A
	17,   // Button_B
	19,   // Button_X
	16,   // Button_Y
	22,   // Button_White
	23,   // Button_Black
	20,   // Button_LeftTrigger
	21,   // Button_RightTrigger
	25,   // Button_Start
	24,   // Button_Back
	26,  // Button_LeftThumb
	27,  // Button_RightThumb
	28,  // Button_DUp
	30,  // Button_DDown
	31,  // Button_DLeft
	29,  // Button_DRight
	AID_Rx,						// Button_ThumbLX
	AID_Ry | AID_Negative,		// Button_ThumbLY
	AID_Slider1,				// Button_ThumbRX
	AID_Slider2 | AID_Negative	// Button_ThumbRY
};

MFGamepadInfo gEMS2Info =
{
	"PS2 Gamepad",
	"EMS USB2",
	0x0B43, 0x0003,
	gEMS2ButtonID,
	NULL,
	false,
	NULL
};


/**** Platform Specific Functions ****/

void MFInputLinux_InitGamepad(int fd, LinuxGamepad *pGamepad)
{
	MFCALLSTACK;

	pGamepad->joyFD = fd;

	// get the pad details
	ioctl(fd, JSIOCGNAME(80), pGamepad->identifier);
	ioctl(fd, JSIOCGAXES, &pGamepad->numAxiis);
	ioctl(fd, JSIOCGBUTTONS, &pGamepad->numButtons);

	MFGamepadInfo *pGI = pGamepadMappingRegistry;

	// we need to find a better way to get the VID/PID from the gamepad...
	if(!MFString_CompareN(pGamepad->identifier, "HID", 3))
	{
		// the device was unnamed, but it is a HID device, so we'll try and match the VID/PID
		char *pBase = pGamepad->identifier + 3, *pEnd;

		// get the VID string
		while(*pBase && !MFIsHex(*pBase))
			++pBase;

		pEnd = pBase + 1;
		while(MFIsHex(*pEnd))
			++pEnd;

		uint32 vid = MFHexToInt(MFStrN(pBase, pEnd - pBase));

		// get the PID string
		pBase = pEnd;
		while(*pBase && !MFIsHex(*pBase))
			++pBase;

		pEnd = pBase + 1;
		while(MFIsHex(*pEnd))
			++pEnd;

		uint32 pid = MFHexToInt(MFStrN(pBase, pEnd - pBase));

		// find a matching descriptor
		for(; pGI; pGI = pGI->pNext)
		{
			if(pGI->vendorID == vid && pGI->productID == pid)
				break;
		}
	}
	else
	{
		// the device is named, so we'll compare the name against our list and hope its the same as windows..
		// since we dont have the VID/PID though, we cant verify its not a device with an aliased name.
		pGI = pGI->pNext; // skip the first one
		for(; pGI; pGI = pGI->pNext)
		{
			// since linux appends the manufacturer name, we'll just check for a match on the end of the string
			int len1 = MFString_Length(pGamepad->identifier);
			int len2 = MFString_Length(pGI->pIdentifier);
			if(!MFString_Compare(pGamepad->identifier + (len1 - len2), pGI->pIdentifier))
				break;
		}
	}

	if(!pGI)
	{
		// use default descriptor
		pGamepad->pGamepadInfo = pGamepadMappingRegistry;
		MFDebug_Warn(1, MFStr("Found an unknown gamepad '%s', using default mappings.", pGamepad->identifier));

		// offer to send email detailing controller info..
//		MessageBox(NULL, "An unknown gamepad has been detected.\r\nWe strive to support every gamepad natively, please report your gamepad to Manu at turkeyman@gmail.com.\r\nI will contact you and request a few details about the gamepad so it can be added to the registry for the next release.", "Unknown gamepad detected...", MB_OK);
	}
	else
	{
		// use applicable descriptor
		pGamepad->pGamepadInfo = pGI;
		MFDebug_Log(2, MFStr("Found gamepad: %s '%s'.", pGI->pName, pGI->pIdentifier));
	}
/*
	// fix up the linux mapping table
	const int *pButtonMap = pGamepad->pGamepadInfo->pButtonMap;
	int numAxiis = 0;

	for(int a=0; a<60; a++)
	{
		for(int b=0; b<GamepadType_Max; ++b)
		{
			if((pButtonMap[b] & AID_Analog) && MFGETAXIS(pButtonMap[b]) == a)
			{
				pGamepad->axisMap[a] = numAxiis;
				printf("%d = %d\n", a, numAxiis);
				++numAxiis;
			}
		}
	}
*/
}

void MFInput_InitModulePlatformSpecific()
{
	MFCALLSTACK;

	MFZeroMemory(gGamepads, sizeof(gGamepads));

	int fd;
	const char **ppDevNames = gDeviceNames;

	// search for joystick devices....
	while(*ppDevNames)
	{
		for(int a=0; a<16; a++)
		{
			const char *pDevice = MFStr(*ppDevNames, a);
			fd = open(pDevice, O_RDONLY|O_NONBLOCK);

			if(fd > 0)
			{
				MFInputLinux_InitGamepad(fd, &gGamepads[gMaxGamepad]);
				MFString_Copy(gGamepads[gMaxGamepad].deviceName, pDevice);
				++gMaxGamepad;

				// HACK: unsavory hack to make the second EMS2 gamepad work...
				if(gGamepads[gMaxGamepad-1].pGamepadInfo->vendorID == gEMS2Info.vendorID && gGamepads[gMaxGamepad-1].pGamepadInfo->productID == gEMS2Info.productID)
				{
					// set the second EMS gamepad as a virtual device of the first one
					gGamepads[gMaxGamepad].bVirtualDevice = true;
					gGamepads[gMaxGamepad].pGamepadInfo = &gEMS2Info;
					gEMS2Info.ppButtonNameStrings = gGamepads[gMaxGamepad-1].pGamepadInfo->ppButtonNameStrings;
					++gMaxGamepad;
				}
			}
		}

		++ppDevNames;
	}
}

void MFInput_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	for(int a=0; a<gMaxGamepad; a++)
	{
		if(gGamepads[a].joyFD)
			close(gGamepads[a].joyFD);
	}
}

void MFInput_UpdatePlatformSpecific()
{
	struct js_event js;
	int error;

	for(int a=0; a<gMaxGamepad; a++)
	{
		if(gGamepads[a].joyFD && !gGamepads[a].bVirtualDevice)
		{
			while((error = read(gGamepads[a].joyFD, &js, sizeof(struct js_event))) > 0)
			{
				switch (js.type & ~JS_EVENT_INIT)
				{
					case JS_EVENT_AXIS:
						gGamepads[a].axis[js.number] = js.value;
						break;
					case JS_EVENT_BUTTON:
						gGamepads[a].button[js.number] = js.value;
						break;
				}
			}

			if(error == -1 && errno != EAGAIN)
			{
				MFDebug_Assert(false, "Some error occurred....");
				// some error occurred....
			}
		}
	}
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	if(device == IDD_Gamepad && id < gMaxGamepad)
	{
		while(gGamepads[id].bVirtualDevice)
			--id;

		return gGamepads[id].joyFD ? IDS_Ready : IDS_Disconnected;
	}

	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
    MFCALLSTACK;

	MFZeroMemory(pGamepadState, sizeof(MFGamepadState));

	LinuxGamepad *pPad = &gGamepads[id];
	const int *pButtonMap = pPad->pGamepadInfo->pButtonMap;

	// if this is a virtual device, find the source device
	while(pPad->bVirtualDevice)
		--pPad;

	if(pPad->joyFD)
	{
		// convert input to float data
		for(int a=0; a<GamepadType_Max; a++)
		{
			if(pButtonMap[a] == -1)
				continue;

			// if we are not reading the analog axis
			if(!(pButtonMap[a] & AID_Analog))
			{
				// read digital button
				pGamepadState->values[a] = pPad->button[pButtonMap[a] & AID_ButtonMask] ? 1.0f : 0.0f;
			}
			else
			{
				// read an analog axis
//				pGamepadState->values[a] = MFMin(pad.axis[pad.axisMap[MFGETAXIS(pButtonMap[a])]] * (1.0f/32767.0f), 1.0f);
				pGamepadState->values[a] = MFMin(pPad->axis[MFGETAXIS(pButtonMap[a])] * (1.0f/32767.0f), 1.0f);
			}

			// invert any buttons with the AID_Negative flag
			pGamepadState->values[a] = (pButtonMap[a] & AID_Negative) ? -pGamepadState->values[a] : pGamepadState->values[a];
			// clamp any butons with the AID_Clamp flag to the positive range
			pGamepadState->values[a] = (pButtonMap[a] & AID_Clamp) ? MFMax(0.0f, pGamepadState->values[a]) : pGamepadState->values[a];
		}

/*
		// if device has a pov, and we want to read from it
		if(gPCJoysticks[id].bUsePOV)
		{
			// read POV
			DWORD pov = joyState.rgdwPOV[0];
			bool POVCentered = (LOWORD(pov) == 0xFFFF);

			if(POVCentered)
			{
				// POV is centered
				pGamepadState->values[Button_DUp] = 0.0f;
				pGamepadState->values[Button_DDown] = 0.0f;
				pGamepadState->values[Button_DLeft] = 0.0f;
				pGamepadState->values[Button_DRight] = 0.0f;
			}
			else
			{
				// read POV (or more appropriately titled, POS)
				pGamepadState->values[Button_DUp] = ((pov >= 31500 && pov <= 36000) || (pov >= 0 && pov <= 4500)) ? 1.0f : 0.0f;
				pGamepadState->values[Button_DDown] = (pov >= 13500 && pov <= 22500) ? 1.0f : 0.0f;
				pGamepadState->values[Button_DLeft] = (pov >= 22500 && pov <= 31500) ? 1.0f : 0.0f;
				pGamepadState->values[Button_DRight] = (pov >= 4500 && pov <= 13500) ? 1.0f : 0.0f;
			}
		}
*/
	}
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFZeroMemory(pKeyState, sizeof(MFKeyState));
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFZeroMemory(pMouseState, sizeof(MFMouseState));
}

const char* MFInput_GetDeviceNameInternal(int device, int deviceID)
{
	switch(device)
	{
		case IDD_Gamepad:
			return gGamepads[deviceID].pGamepadInfo->pName;
		case IDD_Mouse:
			return "Mouse";
		case IDD_Keyboard:
			return "Keyboard";
	}
	return NULL;
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	return gGamepads[sourceID].pGamepadInfo->ppButtonNameStrings[button];
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}
