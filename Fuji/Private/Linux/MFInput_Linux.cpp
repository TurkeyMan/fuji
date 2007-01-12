#include "Fuji.h"
#include "MFInput_Internal.h"
#include "X11_linux.h"
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

	int axis[32];
	char button[128];
	int numButtons;
	int numAxiis;

	int joyFD;

	void *pMapping;
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


/**** Platform Specific Functions ****/

void MFInputLinux_InitGamepad(int fd, LinuxGamepad *pGamepad)
{
	MFCALLSTACK;

	pGamepad->joyFD = fd;

	ioctl(fd, JSIOCGNAME(80), pGamepad->identifier);
	ioctl(fd, JSIOCGAXES, &pGamepad->numAxiis);
	ioctl(fd, JSIOCGBUTTONS, &pGamepad->numButtons);
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
		if(gGamepads[a].joyFD)
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
				// some error occurred....
			}
		}
	}
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	if(device == IDD_Gamepad && id < gMaxGamepad)
		return gGamepads[id].joyFD ? IDS_Ready : IDS_Disconnected;

	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
    MFCALLSTACK;

	MFZeroMemory(pGamepadState, sizeof(MFGamepadState));

	if(gGamepads[id].joyFD)
	{
		pGamepadState->values[Button_X3_A] = gGamepads[id].button[0] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_B] = gGamepads[id].button[1] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_X] = gGamepads[id].button[2] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_Y] = gGamepads[id].button[3] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_LB] = gGamepads[id].button[6] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_RB] = gGamepads[id].button[7] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_Start] = gGamepads[id].button[8] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_Back] = gGamepads[id].button[9] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_LThumb] = gGamepads[id].button[10] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_RThumb] = gGamepads[id].button[11] ? 1.0f : 0.0f;

		pGamepadState->values[Button_DUp] = MFMax(-((float)gGamepads[id].axis[5] * (1.0f / 32767.0f)), 0.0f);
		pGamepadState->values[Button_DDown] = MFMax((float)gGamepads[id].axis[5] * (1.0f / 32767.0f), 0.0f);
		pGamepadState->values[Button_DLeft] = MFMax(-((float)gGamepads[id].axis[4] * (1.0f / 32767.0f)), 0.0f);
		pGamepadState->values[Button_DRight] = MFMax((float)gGamepads[id].axis[4] * (1.0f / 32767.0f), 0.0f);

		pGamepadState->values[Button_X3_LT] = gGamepads[id].button[4] ? 1.0f : 0.0f;
		pGamepadState->values[Button_X3_RT] = gGamepads[id].button[5] ? 1.0f : 0.0f;

		pGamepadState->values[Axis_LX] = (float)gGamepads[id].axis[0] * (1.0f / 32767.0f);
		pGamepadState->values[Axis_LY] = -((float)gGamepads[id].axis[1] * (1.0f / 32767.0f));
		pGamepadState->values[Axis_RX] = (float)gGamepads[id].axis[2] * (1.0f / 32767.0f);
		pGamepadState->values[Axis_RY] = -((float)gGamepads[id].axis[3] * (1.0f / 32767.0f));
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
			return gGamepads[deviceID].identifier;
		case IDD_Mouse:
			return "Mouse";
		case IDD_Keyboard:
			return "Keyboard";
	}
	return NULL;
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	return "Null Input Device";
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}
