#include "Fuji.h"
#include "MFVector.h"
#include "MFInput_Internal.h"
#include "MFNetwork_Internal.h"

// store device status for all devices
static MFInputDeviceStatus	gDeviceStatus[IDD_Max][MFInput_MaxInputID];

// store states for each input device..
static MFGamepadState	gGamepadStates[MFInput_MaxInputID];
static MFGamepadState	gPrevGamepadStates[MFInput_MaxInputID];
static MFKeyState		gKeyStates[MFInput_MaxInputID];
static MFKeyState		gPrevKeyStates[MFInput_MaxInputID];
static MFMouseState		gMouseStates[MFInput_MaxInputID];
static MFMouseState		gPrevMouseStates[MFInput_MaxInputID];

static float gGamepadDeadZone = 0.3f;
static float gMouseAccelleration = 1.0f;

static int gNumGamepads = 0;
static int gNumPointers = 0;
static int gNumKeyboards = 0;

static int gNetGamepadStart = MFInput_MaxInputID;
static int gNetPointerStart = MFInput_MaxInputID;
static int gNetKeyboardStart = MFInput_MaxInputID;

// DIK to ASCII mappings with shift, caps, and shift-caps tables
static const char KEYtoASCII[256]			= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0,'\'',0,0,'*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',',',';','=','=',0,0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','[','\\',']',0,0,'`',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const char KEYtoASCIISHIFT[256]		= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0, '"',0,0,'*','+','<','+','>','?',')','!','@','#','$','%','^','&','*','(',',',':','=','+',0,0,0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','{', '|','}',0,0,'~',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const char KEYtoASCIICAPS[256]		= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0,'\'',0,0,'*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',',',';','=','=',0,0,0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','[','\\',']',0,0,'`',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const char KEYtoASCIICAPSSHIFT[256]	= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0, '"',0,0,'*','+','<','+','>','?',')','!','@','#','$','%','^','&','*','(',',',':','=','+',0,0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','{', '|','}',0,0,'~',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static const char * const gGamepadStrings[] =
{
	"Cross",
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
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Left Analog X-Axis",
	"Left Analog Y-Axis",
	"Right Analog X-Axis",
	"Right Analog Y-Axis"
};

static const char * const gKeyNames[] =
{
	"None",
	"Up",
	"Down",
	"Left",
	"Right",
	"Apps",			// on windows keyboards
	"LWin",			// on windows keyboards
	"RWin",			// on windows keyboards
	"Backspace",
	"Tab",
	"PrintScreen",
	"ScrollLock",
	"Break",
	"Return",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"OEM_102",		// on german keyboard
	"Escape",
	"MyComputer",	// on multimedia keyboards
	"Mail",			// on multimedia keyboards
	"Calculator",	// on multimedia keyboards
	"Stop",			// japanese keyboard
	"Space",
	"Insert",
	"Delete",
	"Home",
	"End",
	"PageUp",
	"PageDown",
	"Apostrophe",
	"ABNT_C1",		// on brazilian keyboard
	"ABNT_C2",		// on brazilian keyboard
	"Asterisk",
	"Plus",
	"Comma",
	"Hyphen",
	"Period",
	"Slash",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"NumPadComma",	// japanese keyboard
	"Semicolon",
	"NumPadEquals",	// japanese keyboard
	"Equals",
	"Sleep",		// on windows keyboards
	"Wake",			// on windows keyboards
	"Power",		// on windows keyboards
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Underline",	// japanese keyboard
	"LBracket",
	"Backslash",
	"RBracket",
	"F13",			// japanese keyboard
	"F14",			// japanese keyboard
	"Grave",
	"F15",			// japanese keyboard
	"Unlabeled",	// japanese keyboard
	"LControl",
	"LAlt",
	"LShift",
	"RControl",
	"RAlt",
	"RShift",
	"Capital",

	"NumLock",
	"Divide",
	"Subtract",
	"Decimal",

	"NumPad0",
	"NumPad1",
	"NumPad2",
	"NumPad3",
	"NumPad4",
	"NumPad5",
	"NumPad6",
	"NumPad7",
	"NumPad8",
	"NumPad9",
	"NumPadEnter",

	"PlayPause",	// on multimedia keyboards
	"MediaStop",	// on multimedia keyboards
	"MediaSelect",	// on multimedia keyboards
	"NextTrack",	// on multimedia keyboards
	"PrevTrack",	// on multimedia keyboards

	"VolumeDown",	// on multimedia keyboards
	"VolumeUp",		// on multimedia keyboards
	"Mute",			// on multimedia keyboards

	"WebBack",		// on multimedia keyboards
	"WebFavorites",	// on multimedia keyboards
	"WebForeward",	// on multimedia keyboards
	"WebHome",		// on multimedia keyboards
	"WebRefresh",	// on multimedia keyboards
	"WebSearch",	// on multimedia keyboards
	"WebStop",		// on multimedia keyboards

	"AT",			// japanese keyboard
	"AX",			// japanese keyboard
	"Colon",		// japanese keyboard
	"Convert",		// japanese keyboard
	"Kana",			// japanese keyboard
	"Kanji",		// japanese keyboard
	"NoConvert",	// japanese keyboard
	"Yen",			// japanese keyboard
};

// Functions
void MFInput_InitModule()
{
	MFZeroMemory(gGamepadStates, sizeof(gGamepadStates[0])*MFInput_MaxInputID);
	MFZeroMemory(gKeyStates, sizeof(gKeyStates[0])*MFInput_MaxInputID);
	MFZeroMemory(gMouseStates, sizeof(gMouseStates[0])*MFInput_MaxInputID);

	MFInput_InitModulePlatformSpecific();
}

void MFInput_DeinitModule()
{
	MFInput_DeinitModulePlatformSpecific();
}

void MFInputInternal_ApplySphericalDeadZone(float *pX, float *pY)
{
	float length = *pX**pX + *pY**pY;

	if(length)
	{
		length = MFSqrt(length);

		float scale = 1.0f / length * MFClamp(0.0f, (length - gGamepadDeadZone) / (1.0f - gGamepadDeadZone), 1.0f);

		*pX *= scale;
		*pY *= scale;
	}
}

void MFInput_Update()
{
	int a, b;

	// platform specific update
	MFInput_UpdatePlatformSpecific();

	// update state for each device
	for(a=0; a<IDD_Max; a++)
	{
		for(b=0; b<MFInput_MaxInputID; b++)
		{
			gDeviceStatus[a][b] = MFInput_GetDeviceStatusInternal(a, b);
		}
	}

	// count number of gamepads
	int maxGamepad = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_Gamepad, a))
			maxGamepad = a;
	}
	gNumGamepads = maxGamepad+1;

	// update number of keyboards
	int maxKB = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_Keyboard, a))
			maxKB = a;
	}
	gNumKeyboards = maxKB+1;

	// update number of pointers
	int maxMouse = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_Mouse, a))
			maxMouse = a;
	}
	gNumPointers = maxMouse+1;

	// copy current states into last states
	MFCopyMemory(gPrevGamepadStates, gGamepadStates, sizeof(gPrevGamepadStates));
	MFCopyMemory(gPrevKeyStates, gKeyStates, sizeof(gPrevKeyStates));
	MFCopyMemory(gPrevMouseStates, gMouseStates, sizeof(gPrevMouseStates));

	// update keyboard state
	for(a=0; a<gNumKeyboards; a++)
	{
		if(MFInput_IsAvailable(IDD_Keyboard, a))
			MFInput_GetKeyStateInternal(a, &gKeyStates[a]);
	}

	// update gamepad state
	for(a=0; a<gNumGamepads; a++)
	{
		if(MFInput_IsAvailable(IDD_Gamepad, a))
		{
			// update gamepad state
#if !defined(_RETAIL)
			if(gDeviceStatus[IDD_Gamepad][0] == IDS_Unavailable)
				MFInputInternal_GetGamepadStateFromKeyMap(&gGamepadStates[a], &gKeyStates[0]);
			else
#endif
			MFInput_GetGamepadStateInternal(a, &gGamepadStates[a]);

			// apply analog deadzone to axiis
			MFInputInternal_ApplySphericalDeadZone(&gGamepadStates[a].values[Axis_LX], &gGamepadStates[a].values[Axis_LY]);
			MFInputInternal_ApplySphericalDeadZone(&gGamepadStates[a].values[Axis_RX], &gGamepadStates[a].values[Axis_RY]);
		}
	}

	// update mouse state
	for(a=0; a<gNumPointers; a++)
	{
		if(MFInput_IsAvailable(IDD_Mouse, a))
			MFInput_GetMouseStateInternal(a, &gMouseStates[a]);
	}

	// add network devices
	if(MFNetwork_IsRemoteInputServerRunning())
	{
		gNetGamepadStart = gNumGamepads;
		gNetKeyboardStart = gNumKeyboards;
		gNetPointerStart = gNumPointers;

		MFNetwork_LockInputMutex();

		// add additional gamepads
		for(a=0; a<MFNetwork_MaxRemoteDevices(); a++)
		{
			MFInputDeviceStatus status = (MFInputDeviceStatus)MFNetwork_GetRemoteDeviceStatus(a);

			if(status > IDS_Unavailable)
			{
				gDeviceStatus[IDD_Gamepad][gNetGamepadStart + a] = status;

				MFNetwork_GetRemoteGamepadState(a, &gGamepadStates[gNetGamepadStart + a]);

				gNumGamepads = gNetGamepadStart + a + 1;
			}
		}

		MFNetwork_ReleaseInputMutex();
	}
}

bool MFInput_IsAvailable(int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= 0 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	bool available = gDeviceStatus[device][deviceID] != IDS_Unavailable;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!available && device == IDD_Gamepad && deviceID == 0 && gNumKeyboards)
		available = true;
#endif

	return available;
}

bool MFInput_IsConnected(int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= 0 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	bool connected = gDeviceStatus[device][deviceID] != IDS_Disconnected && gDeviceStatus[device][deviceID] != IDS_Unavailable;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!connected && device == IDD_Gamepad && deviceID == 0 && gNumKeyboards)
	{
		connected = MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return connected;
}

bool MFInput_IsReady(int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= 0 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	bool ready = gDeviceStatus[device][deviceID] == IDS_Ready;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!ready && device == IDD_Gamepad && deviceID == 0 && gNumKeyboards)
	{
		ready = MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return ready;
}

float MFInput_Read(int button, int device, int deviceID, float *pPrevState)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= -1 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	if(deviceID == -1)
	{
		float value = 0.0f;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_Read(button, device, a);
		}

		return value;
	}

	switch(device)
	{
		case IDD_Gamepad:
		{
			if(pPrevState)
				*pPrevState = gPrevGamepadStates[deviceID].values[button];

			return gGamepadStates[deviceID].values[button];
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				if(pPrevState)
					*pPrevState = gPrevMouseStates[deviceID].values[button];

				return gMouseStates[deviceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				if(pPrevState)
					*pPrevState = gPrevMouseStates[deviceID].buttonState[button - Mouse_MaxAxis] ? 1.0f : 0.0f;

				return gMouseStates[deviceID].buttonState[button - Mouse_MaxAxis] ? 1.0f : 0.0f;
			}
			break;
		case IDD_Keyboard:
			if(pPrevState)
				*pPrevState = gPrevKeyStates[deviceID].keys[button] ? 1.0f : 0.0f;

			return gKeyStates[deviceID].keys[button] ? 1.0f : 0.0f;
		default:
			break;
	}

	if(pPrevState)
		*pPrevState = 0.0f;

	return 0.0f;
}

bool MFInput_WasPressed(int button, int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= -1 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	if(deviceID == -1)
	{
		bool value = false;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_WasPressed(button, device, a);
		}

		return value;
	}

	switch(device)
	{
		case IDD_Gamepad:
		{
			return gGamepadStates[deviceID].values[button] && !gPrevGamepadStates[deviceID].values[button];
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				return gMouseStates[deviceID].values[button] && !gPrevMouseStates[deviceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				return gMouseStates[deviceID].buttonState[button - Mouse_MaxAxis] && !gPrevMouseStates[deviceID].buttonState[button - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return gKeyStates[deviceID].keys[button] && !gPrevKeyStates[deviceID].keys[button];
		default:
			break;
	}
	return false;
}

bool MFInput_WasReleased(int button, int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= -1 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	if(deviceID == -1)
	{
		bool value = false;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_WasReleased(button, device, a);
		}

		return value;
	}

	switch(device)
	{
		case IDD_Gamepad:
		{
			return !gGamepadStates[deviceID].values[button] && gPrevGamepadStates[deviceID].values[button];
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				return !gMouseStates[deviceID].values[button] && gPrevMouseStates[deviceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				return !gMouseStates[deviceID].buttonState[button - Mouse_MaxAxis] && gPrevMouseStates[deviceID].buttonState[button - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return !gKeyStates[deviceID].keys[button] && gPrevKeyStates[deviceID].keys[button];
		default:
			break;
	}
	return false;
}

int MFInput_GetNumGamepads()
{
	return gNumGamepads;
}

int MFInput_GetNumKeyboards()
{
	return gNumKeyboards;
}

int MFInput_GetNumPointers()
{
	return gNumPointers;
}

void MFSetMouseMode(uint32 mouseMode)
{
	MFDebug_Assert(false, "SetMouseMode not written");
}

MFVector MFInput_ReadMousePos(int mouseID)
{
	MFVector pos = MFVector::identity;

	if(mouseID < 0)
	{
		for(int a=0; a<MFInput_MaxInputID; a++)
		{
			MFDebug_Assert(false, "Reading any mouse not written");
		}
	}
	else
	{
		pos.x = MFInput_Read(Mouse_XPos, IDD_Mouse, mouseID);
		pos.y = MFInput_Read(Mouse_YPos, IDD_Mouse, mouseID);
		pos.z = 0.0f;
	}

	return pos;
}

MFVector MFInput_ReadMouseDelta(int mouseID)
{
	MFVector delta = MFVector::zero;

	if(mouseID < 0)
	{
		for(int a=0; a<MFInput_MaxInputID; a++)
		{
			MFDebug_Assert(false, "Reading any mouse delta not written");
		}
	}
	else
	{
		delta.x = MFInput_Read(Mouse_XDelta, IDD_Mouse, mouseID);
		delta.y = MFInput_Read(Mouse_YDelta, IDD_Mouse, mouseID);
		delta.z = 0.0f;
	}

	return delta;
}

void MFInput_SetMouseMode(int mode)
{
	MFDebug_Assert(false, "SetMouseMode not written");
}

void MFInput_SetMouseAcceleration(float multiplier)
{
	MFDebug_Assert(false, "SetMouseAcceleration not written");
}

const char* MFInput_GetDeviceName(int device, int deviceID)
{
	if(deviceID >= gNetGamepadStart && device == IDD_Gamepad)
	{
		return MFStr("Remote %s", MFNetwork_GetRemoteGamepadName(deviceID - gNetGamepadStart));
	}
	else
	{
#if !defined(_RETAIL)
		if(deviceID == IDD_Gamepad && gDeviceStatus[device][deviceID] == IDS_Unavailable)
			return "Keyboard Emulation";
#endif
		return MFInput_GetDeviceNameInternal(device, deviceID);
	}
}

const char* MFInput_GetGamepadButtonName(int button, int deviceID)
{
	if(deviceID >= gNetGamepadStart)
	{
		return MFNetwork_GetRemoteGamepadButtonName(deviceID - gNetGamepadStart, button);
	}
	else
	{
#if !defined(_RETAIL)
		if(deviceID == IDD_Gamepad && gDeviceStatus[IDD_Gamepad][0] == IDS_Unavailable)
			return gGamepadStrings[button];
#endif
		return MFInput_GetGamepadButtonNameInternal(button, deviceID);
	}
}

const char* MFInput_EnumerateString(int button, int device, int deviceID, bool includeDevice, bool includeDeviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");

	switch(device)
	{
		case IDD_Gamepad:
			return MFStr("%s%s %s", includeDevice ? MFInput_GetDeviceName(device, deviceID) : "", includeDeviceID ? MFStr("(%d)", deviceID) : "", MFInput_GetGamepadButtonName(button, deviceID));
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{

			}
			else
			{
				return MFStr("%s%s Button %d", includeDevice ? MFInput_GetDeviceName(device, deviceID) : "", includeDeviceID ? MFStr("(%d)", deviceID) : "", button - Mouse_MaxAxis + 1);
			}
		case IDD_Keyboard:
			return MFStr("%s%s %s", includeDevice ? MFInput_GetDeviceName(device, deviceID) : "", includeDeviceID ? MFStr("(%d)", deviceID) : "", gKeyNames[button]);
	}

	return "";
}

void MFInput_SetDeadZone(float deadZone)
{
	gGamepadDeadZone = deadZone;
}

float MFInput_GetDeadZone()
{
	return gGamepadDeadZone;
}


// gamepad mappings..
char gamepadMappingTable[] =
{
	Mapping_Cross, // Button_P2_Cross
	Mapping_Circle, // Button_P2_Circle
	Mapping_Box, // Button_P2_Box
	Mapping_Triangle, // Button_P2_Triangle

	Mapping_R1, // Button_P2_R1
	Mapping_L1, // Button_P2_L1
	Mapping_L2, // Button_P2_L2
	Mapping_R2, // Button_P2_R2

	Mapping_Start, // Button_P2_Start
	Mapping_Select, // Button_P2_Select

	Mapping_LThumb, // Button_P2_LThumb
	Mapping_RThumb, // Button_P2_RThumb

// general controller enums
	Mapping_DUp, // Button_DUp
	Mapping_DDown, // Button_DDown
	Mapping_DLeft, // Button_DLeft
	Mapping_DRight, // Button_DRight

	Mapping_LX,	// Axis_LX,
	Mapping_LY,	// Axis_LY,
	Mapping_RX,	// Axis_RX,
	Mapping_RY,	// Axis_RY,
};

void MFInputInternal_GetGamepadStateFromKeyMap(MFGamepadState *pGamepadState, MFKeyState *pKeyState)
{
	if(!MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0))
		return;

	for(int a=0; a<GamepadType_Max; a++)
	{
		pGamepadState->values[a] = pKeyState->keys[(int)gamepadMappingTable[a]] ? 1.0f : 0.0f;
	}
}
