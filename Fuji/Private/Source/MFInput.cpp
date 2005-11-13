#include "Fuji.h"
#include "MFVector.h"
#include "MFInput.h"
#include "MFInput_Internal.h"

// store device status for all devices
static MFDeviceStatus	deviceStatus[IDD_Max][MFInput_MaxInputID];

// store states for each input device..
static MFGamepadState	gGamepadStates[MFInput_MaxInputID];
static MFGamepadState	gPrevGamepadStates[MFInput_MaxInputID];
static MFKeyState		gKeyStates[MFInput_MaxInputID];
static MFKeyState		gPrevKeyStates[MFInput_MaxInputID];
static MFMouseState		gMouseStates[MFInput_MaxInputID];
static MFMouseState		gPrevMouseStates[MFInput_MaxInputID];

float gGamepadDeadZone = 0.3f;
float gMouseAccelleration = 1.0f;

int gNumGamepads = 0;
int gNumPointers = 0;
int gNumKeyboards = 0;

// DIK to ASCII mappings with shift, caps, and shift-caps tables
const char KEYtoASCII[256]			= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0,'\'',0,0,'*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',',',';','=','=',0,0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','[','\\',']',0,0,'`',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const char KEYtoASCIISHIFT[256]		= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0, '"',0,0,'*','+','<','+','>','?',')','!','@','#','$','%','^','&','*','(',',',':','=','+',0,0,0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','{', '|','}',0,0,'~',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const char KEYtoASCIICAPS[256]		= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0,'\'',0,0,'*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',',',';','=','=',0,0,0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','[','\\',']',0,0,'`',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const char KEYtoASCIICAPSSHIFT[256]	= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0, '"',0,0,'*','+','<','+','>','?',')','!','@','#','$','%','^','&','*','(',',',':','=','+',0,0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','{', '|','}',0,0,'~',0,0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,'¥',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const char * const gKeyNames[] =
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
	"MyComputer",		// on multimedia keyboards
	"Mail",			// on multimedia keyboards
	"Calculator",		// on multimedia keyboards
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
	"Sleep",			// on windows keyboards
	"Wake",			// on windows keyboards
	"Power",			// on windows keyboards
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
	"Underline",		// japanese keyboard
	"LBracket",
	"Backslash",
	"RBracket",
	"F13",			// japanese keyboard
	"F14",			// japanese keyboard
	"Grave",
	"F15",			// japanese keyboard
	"Unlabeled",		// japanese keyboard
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

	"PlayPause",		// on multimedia keyboards
	"MediaStop",		// on multimedia keyboards
	"MediaSelect",	// on multimedia keyboards
	"NextTrack",		// on multimedia keyboards
	"PrevTrack",		// on multimedia keyboards

	"VolumeDown",		// on multimedia keyboards
	"VolumeUp",		// on multimedia keyboards
	"Mute",			// on multimedia keyboards

	"WebBack",		// on multimedia keyboards
	"WebFavorites",	// on multimedia keyboards
	"WebForeward",		// on multimedia keyboards
	"WebHome",		// on multimedia keyboards
	"WebRefresh",		// on multimedia keyboards
	"WebSearch",		// on multimedia keyboards
	"WebStop",		// on multimedia keyboards

	"AT",				// japanese keyboard
	"AX",				// japanese keyboard
	"Colon",			// japanese keyboard
	"Convert",		// japanese keyboard
	"Kana",			// japanese keyboard
	"Kanji",			// japanese keyboard
	"NoConvert",		// japanese keyboard
	"Yen",			// japanese keyboard
};

// Functions
void MFInput_InitModule()
{
	memset(gGamepadStates, 0, sizeof(gGamepadStates[0])*MFInput_MaxInputID);
	memset(gKeyStates, 0, sizeof(gKeyStates[0])*MFInput_MaxInputID);
	memset(gMouseStates, 0, sizeof(gMouseStates[0])*MFInput_MaxInputID);

	MFInput_InitModulePlatformSpecific();
}

void MFInput_DeinitModule()
{
	MFInput_DeinitModulePlatformSpecific();
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
			MFInput_GetDeviceStatusInternal(a, b, &deviceStatus[a][b]);
		}
	}

	// count number of gamepads
	int maxGamepad = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Gamepad][a].available)
			maxGamepad = a;
	}
	gNumGamepads = maxGamepad+1;

	// update number of keyboards
	int maxKB = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Keyboard][a].available)
			maxKB = a;
	}
	gNumKeyboards = maxKB+1;

	// update number of pointers
	int maxMouse = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Mouse][a].available)
			maxMouse = a;
	}
	gNumPointers = maxMouse+1;

	// copy current states into last states
	memcpy(gPrevGamepadStates, gGamepadStates, sizeof(MFGamepadState) * MFInput_MaxInputID);
	memcpy(gPrevKeyStates, gKeyStates, sizeof(MFKeyState) * MFInput_MaxInputID);
	memcpy(gPrevMouseStates, gMouseStates, sizeof(MFMouseState) * MFInput_MaxInputID);

	// update gamepad state
	for(a=0; a<gNumGamepads; a++)
	{
		if(MFInput_IsAvailable(IDD_Gamepad, a))
			MFInput_GetGamepadStateInternal(a, &gGamepadStates[a]);
	}

	for(a=0; a<gNumKeyboards; a++)
	{
		if(MFInput_IsAvailable(IDD_Keyboard, a))
			MFInput_GetKeyStateInternal(a, &gKeyStates[a]);
	}

	for(a=0; a<gNumPointers; a++)
	{
		if(MFInput_IsAvailable(IDD_Mouse, a))
			MFInput_GetMouseStateInternal(a, &gMouseStates[a]);
	}
}

bool MFInput_IsAvailable(int source, int sourceID)
{
	MFDebug_Assert(source >= 0 && source < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(sourceID >= 0 && source < MFInput_MaxInputID, "Invalid DeviceID");

	bool available = deviceStatus[source][sourceID].available || gNumKeyboards;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!available && source == IDD_Gamepad && sourceID == 0 && gNumKeyboards)
		available = true;
#endif

	return available;
}

bool MFInput_IsConnected(int source, int sourceID)
{
	MFDebug_Assert(source >= 0 && source < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(sourceID >= 0 && source < MFInput_MaxInputID, "Invalid DeviceID");

	bool connected = deviceStatus[source][sourceID].status != IDS_Disconnected;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!connected && source == IDD_Gamepad && sourceID == 0 && gNumKeyboards)
	{
		connected = MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return connected;
}

bool MFInput_IsReady(int source, int sourceID)
{
	MFDebug_Assert(source >= 0 && source < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(sourceID >= 0 && source < MFInput_MaxInputID, "Invalid DeviceID");

	bool ready = deviceStatus[source][sourceID].status == IDS_Ready;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!ready && source == IDD_Gamepad && sourceID == 0 && gNumKeyboards)
	{
		ready = MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return ready;
}

float MFInput_Read(int button, int source, int sourceID)
{
	MFDebug_Assert(source >= 0 && source < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(sourceID >= -1 && source < MFInput_MaxInputID, "Invalid DeviceID");

	if(sourceID == -1)
	{
		float value = 0.0f;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_Read(button, source, a);
		}

		return value;
	}

	switch(source)
	{
		case IDD_Gamepad:
		{
#if defined(_RETAIL)
			return gGamepadStates[sourceID].values[button];
#else
			float value;

			if(sourceID == 0 && deviceStatus[IDD_Gamepad][0].status != IDS_Ready)
				value = MFInputInternal_GetGamepadKeyMapping(button, &gKeyStates[0]);
			else
				value = gGamepadStates[sourceID].values[button];

			return value;
#endif
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				return gMouseStates[sourceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				return gMouseStates[sourceID].buttonState[button - Mouse_MaxAxis] ? 1.0f : 0.0f;
			}
			break;
		case IDD_Keyboard:
			return gKeyStates[sourceID].keys[button] ? 1.0f : 0.0f;
		default:
			break;
	}
	return 0.0f;
}

bool MFInput_WasPressed(int button, int source, int sourceID)
{
	MFDebug_Assert(source >= 0 && source < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(sourceID >= 0 && source < MFInput_MaxInputID, "Invalid DeviceID");

	if(sourceID == -1)
	{
		bool value = false;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_WasPressed(button, source, a);
		}

		return value;
	}

	switch(source)
	{
		case IDD_Gamepad:
		{
#if defined(_RETAIL)
			return gGamepadStates[sourceID].values[button] && !gPrevGamepadStates[sourceID].values[button];
#else
			bool value;

			if(sourceID == 0 && deviceStatus[IDD_Gamepad][0].status != IDS_Ready)
				value = MFInputInternal_GetGamepadKeyMapping(button, &gKeyStates[0]) && !MFInputInternal_GetGamepadKeyMapping(button, &gPrevKeyStates[0]);
			else
				value = gGamepadStates[sourceID].values[button] && !gPrevGamepadStates[sourceID].values[button];

			return value;
#endif
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				return gMouseStates[sourceID].values[button] && !gPrevMouseStates[sourceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				return gMouseStates[sourceID].buttonState[button - Mouse_MaxAxis] && !gPrevMouseStates[sourceID].buttonState[button - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return gKeyStates[sourceID].keys[button] && !gPrevKeyStates[sourceID].keys[button];
		default:
			break;
	}
	return false;
}

bool MFInput_WasReleased(int button, int source, int sourceID)
{
	MFDebug_Assert(source >= 0 && source < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(sourceID >= 0 && source < MFInput_MaxInputID, "Invalid DeviceID");

	if(sourceID == -1)
	{
		bool value = false;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_WasReleased(button, source, a);
		}

		return value;
	}

	switch(source)
	{
		case IDD_Gamepad:
		{
#if defined(_RETAIL)
			return !gGamepadStates[sourceID].values[button] && gPrevGamepadStates[sourceID].values[button];
#else
			bool value;

			if(sourceID == 0 && deviceStatus[IDD_Gamepad][0].status != IDS_Ready)
				value = !MFInputInternal_GetGamepadKeyMapping(button, &gKeyStates[0]) && MFInputInternal_GetGamepadKeyMapping(button, &gPrevKeyStates[0]);
			else
				value = !gGamepadStates[sourceID].values[button] && gPrevGamepadStates[sourceID].values[button];

			return value;
#endif
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				return !gMouseStates[sourceID].values[button] && gPrevMouseStates[sourceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				return !gMouseStates[sourceID].buttonState[button - Mouse_MaxAxis] && gPrevMouseStates[sourceID].buttonState[button - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return !gKeyStates[sourceID].keys[button] && gPrevKeyStates[sourceID].keys[button];
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

const char* MFInput_EnumerateString(int button, int source, int sourceID, bool includeDevice, bool includeDeviceID)
{
	MFDebug_Assert(source >= 0 && source < IDD_Max, "Invalid Input Device");

	switch(source)
	{
		case IDD_Gamepad:
			return MFStr("%s%s %s", includeDevice ? MFInput_GetDeviceName(source, sourceID) : "", includeDeviceID ? MFStr("(%d)", sourceID) : "", MFInput_GetGamepadButtonName(button, sourceID));
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{

			}
			else
			{
				return MFStr("%s%s Button %d", includeDevice ? MFInput_GetDeviceName(source, sourceID) : "", includeDeviceID ? MFStr("(%d)", sourceID) : "", button - Mouse_MaxAxis + 1);
			}
		case IDD_Keyboard:
			return MFStr("%s%s %s", includeDevice ? MFInput_GetDeviceName(source, sourceID) : "", includeDeviceID ? MFStr("(%d)", sourceID) : "", gKeyNames[button]);
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

//	KEY_UP,	// Axis_LX,
//	KEY_UP,	// Axis_LY,
//	KEY_UP,	// Axis_RX,
//	KEY_UP,	// Axis_RY,
};

float MFInputInternal_GetGamepadKeyMapping(int button, MFKeyState *pKeystate)
{
	if(!MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0))
		return 0.0f;

	return pKeystate->keys[(int)gamepadMappingTable[button]] ? 1.0f : 0.0f;
}
