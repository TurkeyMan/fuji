#include "Common.h"
#include "Vector3.h"
#include "Input.h"
#include "Input_Internal.h"

// store device status for all devices
static DeviceStatus	deviceStatus[IDD_Max][Input_MaxInputID];

// store states for each input device..
static GamepadState	gamepadStates[Input_MaxInputID];
static GamepadState	prevGamepadStates[Input_MaxInputID];
static KeyState		keyStates[Input_MaxInputID];
static KeyState		prevKeyStates[Input_MaxInputID];
static MouseState	mouseStates[Input_MaxInputID];
static MouseState	prevMouseStates[Input_MaxInputID];

float gamepadDeadZone = 0.3f;
float mouseAccelleration = 1.0f;

int numGamepads = 0;
int numPointers = 0;
int numKeyboards = 0;

// DIK to ASCII mappings with shift, caps, and shift-caps tables
const char KEYtoASCII[256]			= {0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,'\t','q','w','e','r','t','y','u','i','o','p','[',']','\r',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'=',0,0,0,'@',':','_',0,0,0,0,0,0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,',',0,'/',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

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
void Input_InitModule()
{
	memset(gamepadStates, 0, sizeof(gamepadStates[0])*Input_MaxInputID);
	memset(keyStates, 0, sizeof(keyStates[0])*Input_MaxInputID);
	memset(mouseStates, 0, sizeof(mouseStates[0])*Input_MaxInputID);

	Input_InitModulePlatformSpecific();
}

void Input_DeinitModule()
{
	Input_DeinitModulePlatformSpecific();
}

void Input_Update()
{
	int a, b;

	// platform specific update
	Input_UpdatePlatformSpecific();

	// update state for each device
	for(a=0; a<IDD_Max; a++)
	{
		for(b=0; b<Input_MaxInputID; b++)
		{
			Input_GetDeviceStatusInternal(a, b, &deviceStatus[a][b]);
		}
	}

	// count number of gamepads
	int maxGamepad = -1;
	for(a=0; a<Input_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Gamepad][a].available)
			maxGamepad = a;
	}
	numGamepads = maxGamepad+1;

	// update number of keyboards
	int maxKB = -1;
	for(a=0; a<Input_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Keyboard][a].available)
			maxKB = a;
	}
	numKeyboards = maxKB+1;

	// update number of pointers
	int maxMouse = -1;
	for(a=0; a<Input_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Mouse][a].available)
			maxMouse = a;
	}
	numPointers = maxMouse+1;

	// copy current states into last states
	memcpy(prevGamepadStates, gamepadStates, sizeof(GamepadState) * Input_MaxInputID);
	memcpy(prevKeyStates, keyStates, sizeof(KeyState) * Input_MaxInputID);
	memcpy(prevMouseStates, mouseStates, sizeof(MouseState) * Input_MaxInputID);

	// update gamepad state
	for(a=0; a<numGamepads; a++)
	{
		if(Input_IsAvailable(IDD_Gamepad, a))
			Input_GetGamepadStateInternal(a, &gamepadStates[a]);
	}

	for(a=0; a<numKeyboards; a++)
	{
		if(Input_IsAvailable(IDD_Keyboard, a))
			Input_GetKeyStateInternal(a, &keyStates[a]);
	}

	for(a=0; a<numPointers; a++)
	{
		if(Input_IsAvailable(IDD_Mouse, a))
			Input_GetMouseStateInternal(a, &mouseStates[a]);
	}
}

bool Input_IsAvailable(int source, int sourceID)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	bool available = deviceStatus[source][sourceID].available || numKeyboards;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!available && source == IDD_Gamepad && sourceID == 0 && numKeyboards)
		available = true;
#endif

	return available;
}

bool Input_IsConnected(int source, int sourceID)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	bool connected = deviceStatus[source][sourceID].status != IDS_Disconnected;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!connected && source == IDD_Gamepad && sourceID == 0 && numKeyboards)
	{
		connected = Input_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return connected;
}

bool Input_IsReady(int source, int sourceID)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	bool ready = deviceStatus[source][sourceID].status == IDS_Ready;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!ready && source == IDD_Gamepad && sourceID == 0 && numKeyboards)
	{
		ready = Input_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return ready;
}

float Input_Read(int source, int sourceID, int type)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= -1 && source < Input_MaxInputID, "Invalid DeviceID");

	if(sourceID == -1)
	{
		float value = 0.0f;

		for(int a=0; a<Input_MaxInputID && !value; a++)
		{
			value = Input_Read(source, a, type);
		}

		return value;
	}

	switch(source)
	{
		case IDD_Gamepad:
		{
#if defined(_RETAIL)
			return gamepadStates[sourceID].values[type];
#else
			float value;

			if(sourceID == 0 && deviceStatus[IDD_Gamepad][0].status != IDS_Ready)
				value = InputInternal_GetGamepadKeyMapping(type, &keyStates[0]);
			else
				value = gamepadStates[sourceID].values[type];

			return value;
#endif
		}
		case IDD_Mouse:
			if(type < Mouse_MaxAxis)
			{
				return mouseStates[sourceID].values[type];
			}
			else if(type < Mouse_Max)
			{
				return mouseStates[sourceID].buttonState[type - Mouse_MaxAxis] ? 1.0f : 0.0f;
			}
			break;
		case IDD_Keyboard:
			return keyStates[sourceID].keys[type] ? 1.0f : 0.0f;
		default:
			break;
	}
	return 0.0f;
}

bool Input_WasPressed(int source, int sourceID, int type)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	if(sourceID == -1)
	{
		bool value = false;

		for(int a=0; a<Input_MaxInputID && !value; a++)
		{
			value = Input_WasPressed(source, a, type);
		}

		return value;
	}

	switch(source)
	{
		case IDD_Gamepad:
		{
#if defined(_RETAIL)
			return gamepadStates[sourceID].values[type] && !prevGamepadStates[sourceID].values[type];
#else
			bool value;

			if(sourceID == 0 && deviceStatus[IDD_Gamepad][0].status != IDS_Ready)
				value = InputInternal_GetGamepadKeyMapping(type, &keyStates[0]) && !InputInternal_GetGamepadKeyMapping(type, &prevKeyStates[0]);
			else
				value = gamepadStates[sourceID].values[type] && !prevGamepadStates[sourceID].values[type];

			return value;
#endif
		}
		case IDD_Mouse:
			if(type < Mouse_MaxAxis)
			{
				return mouseStates[sourceID].values[type] && !prevMouseStates[sourceID].values[type];
			}
			else if(type < Mouse_Max)
			{
				return mouseStates[sourceID].buttonState[type - Mouse_MaxAxis] && !prevMouseStates[sourceID].buttonState[type - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return keyStates[sourceID].keys[type] && !prevKeyStates[sourceID].keys[type];
		default:
			break;
	}
	return false;
}

bool Input_WasReleased(int source, int sourceID, int type)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	if(sourceID == -1)
	{
		bool value = false;

		for(int a=0; a<Input_MaxInputID && !value; a++)
		{
			value = Input_WasReleased(source, a, type);
		}

		return value;
	}

	switch(source)
	{
		case IDD_Gamepad:
		{
#if defined(_RETAIL)
			return !gamepadStates[sourceID].values[type] && prevGamepadStates[sourceID].values[type];
#else
			bool value;

			if(sourceID == 0 && deviceStatus[IDD_Gamepad][0].status != IDS_Ready)
				value = !InputInternal_GetGamepadKeyMapping(type, &keyStates[0]) && InputInternal_GetGamepadKeyMapping(type, &prevKeyStates[0]);
			else
				value = !gamepadStates[sourceID].values[type] && prevGamepadStates[sourceID].values[type];

			return value;
#endif
		}
		case IDD_Mouse:
			if(type < Mouse_MaxAxis)
			{
				return !mouseStates[sourceID].values[type] && prevMouseStates[sourceID].values[type];
			}
			else if(type < Mouse_Max)
			{
				return !mouseStates[sourceID].buttonState[type - Mouse_MaxAxis] && prevMouseStates[sourceID].buttonState[type - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return !keyStates[sourceID].keys[type] && prevKeyStates[sourceID].keys[type];
		default:
			break;
	}
	return false;
}

int Input_GetNumGamepads()
{
	return numGamepads;
}

int Input_GetNumKeyboards()
{
	return numKeyboards;
}

int Input_GetNumPointers()
{
	return numPointers;
}

void SetMouseMode(uint32 mouseMode)
{
	DBGASSERT(false, "SetMouseMode not written");
}

Vector3 Input_ReadMousePos(int mouseID)
{
	Vector3 pos = Vector3::zero;

	if(mouseID < 0)
	{
		for(int a=0; a<Input_MaxInputID; a++)
		{
			DBGASSERT(false, "Reading any mouse not written");
		}
	}
	else
	{
		pos.x = Input_Read(IDD_Mouse, mouseID, Mouse_XPos);
		pos.y = Input_Read(IDD_Mouse, mouseID, Mouse_YPos);
		pos.z = 0.0f;
	}

	return pos;
}

Vector3 Input_ReadMouseDelta(int mouseID)
{
	Vector3 delta = Vector3::zero;

	if(mouseID < 0)
	{
		for(int a=0; a<Input_MaxInputID; a++)
		{
			DBGASSERT(false, "Reading any mouse delta not written");
		}
	}
	else
	{
		delta.x = Input_Read(IDD_Mouse, mouseID, Mouse_XDelta);
		delta.y = Input_Read(IDD_Mouse, mouseID, Mouse_YDelta);
		delta.z = 0.0f;
	}

	return delta;
}

void Input_SetMouseMode(int mode)
{
	DBGASSERT(false, "SetMouseMode not written");
}

void Input_SetMouseAcceleration(float multiplier)
{
	DBGASSERT(false, "SetMouseAcceleration not written");
}

const char* Input_EnumerateString(int source, int sourceID, int type, bool includeDevice, bool includeDeviceID)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");

	switch(source)
	{
		case IDD_Gamepad:
			return STR("%s%s %s", includeDevice ? Input_GetDeviceName(source, sourceID) : "", includeDeviceID ? STR("(%d)", sourceID) : "", Input_GetGamepadButtonName(sourceID, type));
		case IDD_Mouse:
			if(type < Mouse_MaxAxis)
			{

			}
			else
			{
				return STR("%s%s Button %d", includeDevice ? Input_GetDeviceName(source, sourceID) : "", includeDeviceID ? STR("(%d)", sourceID) : "", type - Mouse_MaxAxis + 1);
			}
		case IDD_Keyboard:
			return STR("%s%s %s", includeDevice ? Input_GetDeviceName(source, sourceID) : "", includeDeviceID ? STR("(%d)", sourceID) : "", gKeyNames[type]);
	}

	return "";
}

void Input_SetDeadZone(float deadZone)
{
	gamepadDeadZone = deadZone;
}

float Input_GetDeadZone()
{
	return gamepadDeadZone;
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

float InputInternal_GetGamepadKeyMapping(int type, KeyState *pKeystate)
{
	if(!Input_GetKeyboardStatusState(KSS_ScrollLock, 0))
		return 0.0f;

	return pKeystate->keys[(int)gamepadMappingTable[type]] ? 1.0f : 0.0f;
}
