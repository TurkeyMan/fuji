#include "Fuji.h"
#include "MFInput_Internal.h"
#include "X11_linux.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

/*** Structure definitions ***/

/*** Globals ***/
int num_of_axis=0, num_of_buttons=0;
char name_of_joystick[80];
int *axis = NULL;
char *button = NULL;
struct js_event js;
int joy_fd;

/**** Platform Specific Functions ****/

void MFInput_InitModulePlatformSpecific()
{
  joy_fd = open("/dev/input/js0", O_RDONLY); // This is horrible - need to something more robust - will work for testing
  ioctl (joy_fd, JSIOCGAXES, &num_of_axis);
  ioctl (joy_fd, JSIOCGBUTTONS, &num_of_buttons);
  ioctl (joy_fd, JSIOCGNAME(80), &name_of_joystick );

  axis = (int *) calloc (num_of_axis, sizeof(int));
  button = (char *) calloc (num_of_buttons, sizeof(char));

  MFCALLSTACK;
}

void MFInput_DeinitModulePlatformSpecific()
{
  free(axis);
  free(button);
  close (joy_fd);
  MFCALLSTACK;
}

void MFInput_UpdatePlatformSpecific()
{
  read (joy_fd, &js, sizeof(struct js_event));
    switch (js.type & ~JS_EVENT_INIT)
    {
       case JS_EVENT_AXIS:
         axis [js.number] = js.value;
         break;
       case JS_EVENT_BUTTON:
         button [js.number] = js.value;
         break;
    }
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	MFCALLSTACK;

	MFZeroMemory(pGamepadState, sizeof(MFGamepadState));
//         switch(device)
// 	{
//             case IDD_Gamepad: break;
//             case IDD_Mouse: break;
//             case IDD_Keyboard:
// break;
//             default:
// 		MFDebug_Assert(false, "Invalid Input Device");
// 		break;
//         }
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFZeroMemory(pKeyState, sizeof(MFKeyState));
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFZeroMemory(pMouseState, sizeof(MFMouseState));
}

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	return "Null Input Device";
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	return "Null Input Device";
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}
