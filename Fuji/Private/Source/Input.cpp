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

// DIK to ASCII mappings with shift, caps, and shift-caps tables
char KEYtoASCII[256]			= {0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,'\t','q','w','e','r','t','y','u','i','o','p','[',']','\r',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'=',0,0,0,'@',':','_',0,0,0,0,0,0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,',',0,'/',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

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
	Input_InitModulePlatformSpecific();
}

void Input_Update()
{
	int a, b;

	for(a=0; a<IDD_Max; a++)
	{
		for(b=0; b<Input_MaxInputID; b++)
		{
			Input_GetDeviceStatusInternal(a, b, &deviceStatus[a][b]);
		}
	}

	memcpy(prevGamepadStates, gamepadStates, sizeof(GamepadState) * Input_MaxInputID);
	memcpy(prevKeyStates, keyStates, sizeof(KeyState) * Input_MaxInputID);
	memcpy(prevMouseStates, mouseStates, sizeof(MouseState) * Input_MaxInputID);

	for(a=0; a<Input_MaxInputID; a++)
	{
		if(Input_IsAvailable(IDD_Gamepad, a))
			Input_GetGamepadStateInternal(a, &gamepadStates[a]);
	}

	for(a=0; a<Input_MaxInputID; a++)
	{
		if(Input_IsAvailable(IDD_Keyboard, a))
			Input_GetKeyStateInternal(a, &keyStates[a]);
	}

	for(a=0; a<Input_MaxInputID; a++)
	{
		if(Input_IsAvailable(IDD_Mouse, a))
			Input_GetMouseStateInternal(a, &mouseStates[a]);
	}
}

bool Input_IsAvailable(int source, int sourceID)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	return deviceStatus[source][sourceID].available ? true : false;
}

bool Input_IsConnected(int source, int sourceID)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	return deviceStatus[source][sourceID].status == IDS_Disconnected ? true : false;
}
bool Input_IsReady(int source, int sourceID)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	return deviceStatus[source][sourceID].status == IDS_Ready ? true : false;
}

float Input_Read(int source, int sourceID, int type)
{
	DBGASSERT(source >= 0 && source < IDD_Max, "Invalid Input Device");
	DBGASSERT(sourceID >= 0 && source < Input_MaxInputID, "Invalid DeviceID");

	DBGASSERT(deviceStatus[source][sourceID].status == IDS_Ready, STR("Device not ready for reading input: Device %d, ID %d", source, sourceID))

	switch(source)
	{
		case IDD_Gamepad:
			return gamepadStates[sourceID].values[type];
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

	DBGASSERT(deviceStatus[source][sourceID].status == IDS_Ready, STR("Device not ready for reading input: Device %d, ID %d", source, sourceID))

	switch(source)
	{
		case IDD_Gamepad:
			return gamepadStates[sourceID].values[type] && !prevGamepadStates[sourceID].values[type];
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

	DBGASSERT(deviceStatus[source][sourceID].status == IDS_Ready, STR("Device not ready for reading input: Device %d, ID %d", source, sourceID))

	switch(source)
	{
		case IDD_Gamepad:
			return !gamepadStates[sourceID].values[type] && prevGamepadStates[sourceID].values[type];
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
	int maxGamepad = -1;

	for(int a=0; a<Input_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Gamepad][a].available)
			maxGamepad = a;
	}

	return maxGamepad+1;
}

int Input_GetNumKeyboards()
{
	int maxKB = -1;

	for(int a=0; a<Input_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Keyboard][a].available)
			maxKB = a;
	}

	return maxKB+1;
}

int Input_GetNumPointers()
{
	int maxMouse = -1;

	for(int a=0; a<Input_MaxInputID; a++)
	{
		if(deviceStatus[IDD_Mouse][a].available)
			maxMouse = a;
	}

	return maxMouse+1;
}

void SetMouseMode(uint32 mouseMode)
{
#pragma message("Set Mouse Mode")
}

Vector3 Input_ReadMousePos(int mouseID)
{
	Vector3 pos;

	if(mouseID < 0)
	{
		for(int a=0; a<Input_MaxInputID; a++)
		{
#pragma message("ReadANYMouse")
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
	Vector3 delta;

	if(mouseID < 0)
	{
		for(int a=0; a<Input_MaxInputID; a++)
		{
#pragma message("ReadANYMouse")
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
#pragma message("SetMouseMode")
}

void Input_SetMouseAcceleration(float multiplier)
{
#pragma message("SetMouseAcceleration")
}

char* Input_EnumerateString(int source, int sourceID, int type)
{
	return "Unknown";
}

void Input_SetDeadZone(float deadZone)
{
	gamepadDeadZone = deadZone;
}

float Input_GetDeadZone()
{
	return gamepadDeadZone;
}

