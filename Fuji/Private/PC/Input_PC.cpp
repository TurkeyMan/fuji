#define SAMPLE_BUFFER_SIZE 128//50000

#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>

#include "Common.h"
#include "Vector3.h"

#include "Input.h"
#include "../Source/Input_Internal.h"

#include "FileSystem.h"
#include "IniFile.h"
#include "Heap.h"

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdpDirectInputstance, VOID* pContext);

void Input_SetCooperativeLevels();

void Input_UpdateKeyboard();
void Input_UpdateMouse();
void Input_UpdateJoystick();

/*** Structure definitions ***/

struct GamepadInfo
{
	char *pName;

	int axisMapping[4];
	int buttonMapping[16];

	GamepadInfo *pNext;
};

GamepadInfo *GetGamepadInfo(const char *pGamepad);
void LoadGamepadMappings();

/*** Globals ***/

IDirectInput8		*pDirectInput					= NULL;
IDirectInputDevice8	*pDIKeyboard					= NULL;
IDirectInputDevice8	*pDIMouse						= NULL;
IDirectInputDevice8	*pDIJoystick[Input_MaxInputID]	= {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

GamepadInfo *pGamepadMappings[Input_MaxInputID]		= {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

int	gGamepadCount	= 0;
int	gKeyboardCount	= 0;
int	gMouseCount		= 0;

extern HINSTANCE apphInstance;
extern HWND apphWnd;

char gKeyState[256];

bool gExclusiveMouse = false;
float deadZone = 0.3f;

float mouseMultiplier = 1.0f;

// windows mouse (WM_MOUSE messages)
float screenMouseX = 0.0f, screenMouseY = 0.0f;
float screenMouseRangeX = 639.0f, screenMouseRangeY = 479.0f;
float screenMouseWheel = 0.0f;
char screenMouseKey[5] = { 0, 0, 0, 0, 0 };

const long joyAxii[24] = {0,1,2,3,4,5,44,45,46,47,48,49,52,53,54,55,56,57,60,61,62,63,64,65};
const long joySliders[4] = {6,50,58,66};

GamepadInfo *pGamepadMappingRegistry = NULL;


// KEY to DIK mapping table
uint8 KEYtoDIK[256] =
{
	0, // KEY_NONE = 0,
	DIK_UP, // KEY_UP,
	DIK_DOWN, // KEY_DOWN,
	DIK_LEFT, // KEY_LEFT,
	DIK_RIGHT, // KEY_RIGHT,
	DIK_APPS, // KEY_APPS,			// on windows keyboards
	DIK_LWIN, // KEY_LWIN,			// on windows keyboards
	DIK_RWIN, // KEY_RWIN,			// on windows keyboards
	DIK_BACK, // KEY_BACKSPACE,
	DIK_TAB, // KEY_TAB,
	DIK_SYSRQ, // KEY_PRINTSCREEN,
	DIK_SCROLL, // KEY_SCROLLLOCK,
	DIK_PAUSE, // KEY_BREAK,
	DIK_RETURN, // KEY_RETURN,
	DIK_F1, // KEY_F1,
	DIK_F2, // KEY_F2,
	DIK_F3, // KEY_F3,
	DIK_F4, // KEY_F4,
	DIK_F5, // KEY_F5,
	DIK_F6, // KEY_F6,
	DIK_F7, // KEY_F7,
	DIK_F8, // KEY_F8,
	DIK_F9, // KEY_F9,
	DIK_F10, // KEY_F10,
	DIK_F11, // KEY_F11,
	DIK_F12, // KEY_F12,
	DIK_OEM_102, // KEY_OEM_102,		// on german keyboard
	DIK_ESCAPE, // KEY_ESCAPE,
	DIK_MYCOMPUTER, // KEY_MYCOMPUTER,		// on multimedia keyboards
	DIK_MAIL, // KEY_MAIL,			// on multimedia keyboards
	DIK_CALCULATOR, // KEY_CALCULATOR,		// on multimedia keyboards
	DIK_STOP, // KEY_STOP,			// japanese keyboard
	DIK_SPACE, // KEY_SPACE,
	DIK_INSERT, // KEY_INSERT,
	DIK_DELETE, // KEY_DELETE,
	DIK_HOME, // KEY_HOME,
	DIK_END, // KEY_END,
	DIK_PRIOR, // KEY_PAGEUP,
	DIK_NEXT, // KEY_PAGEDOWN,
	DIK_APOSTROPHE, // KEY_APOSTROPHE,
	DIK_ABNT_C1, // KEY_ABNT_C1,		// on brazilian keyboard
	DIK_ABNT_C2, // KEY_ABNT_C2,		// on brazilian keyboard
	DIK_MULTIPLY, // KEY_ASTERISK,
	DIK_ADD, // KEY_PLUS,
	DIK_COMMA, // KEY_COMMA,
	DIK_MINUS, // KEY_HYPHEN,
	DIK_PERIOD, // KEY_PERIOD,
	DIK_SLASH, // KEY_SLASH,
	DIK_0, // KEY_0,
	DIK_1, // KEY_1,
	DIK_2, // KEY_2,
	DIK_3, // KEY_3,
	DIK_4, // KEY_4,
	DIK_5, // KEY_5,
	DIK_6, // KEY_6,
	DIK_7, // KEY_7,
	DIK_8, // KEY_8,
	DIK_9, // KEY_9,
	DIK_NUMPADCOMMA, // KEY_NUMPADCOMMA,	// japanese keyboard
	DIK_SEMICOLON, // KEY_SEMICOLON,
	DIK_NUMPADEQUALS, // KEY_NUMPADEQUALS,	// japanese keyboard
	DIK_EQUALS, // KEY_EQUALS,
	DIK_SLEEP, // KEY_SLEEP,			// on windows keyboards
	DIK_WAKE, // KEY_WAKE,			// on windows keyboards
	DIK_POWER, // KEY_POWER,			// on windows keyboards
	DIK_A, // KEY_A,
	DIK_B, // KEY_B,
	DIK_C, // KEY_C,
	DIK_D, // KEY_D,
	DIK_E, // KEY_E,
	DIK_F, // KEY_F,
	DIK_G, // KEY_G,
	DIK_H, // KEY_H,
	DIK_I, // KEY_I,
	DIK_J, // KEY_J,
	DIK_K, // KEY_K,
	DIK_L, // KEY_L,
	DIK_M, // KEY_M,
	DIK_N, // KEY_N,
	DIK_O, // KEY_O,
	DIK_P, // KEY_P,
	DIK_Q, // KEY_Q,
	DIK_R, // KEY_R,
	DIK_S, // KEY_S,
	DIK_T, // KEY_T,
	DIK_U, // KEY_U,
	DIK_V, // KEY_V,
	DIK_W, // KEY_W,
	DIK_X, // KEY_X,
	DIK_Y, // KEY_Y,
	DIK_Z, // KEY_Z,
	DIK_UNDERLINE, // KEY_UNDERLINE,		// japanese keyboard
	DIK_LBRACKET, // KEY_LBRACKET,
	DIK_BACKSLASH, // KEY_BACKSLASH,
	DIK_RBRACKET, // KEY_RBRACKET,
	DIK_F13, // KEY_F13,			// japanese keyboard
	DIK_F14, // KEY_F14,			// japanese keyboard
	DIK_GRAVE, // KEY_GRAVE,
	DIK_F15, // KEY_F15,			// japanese keyboard
	DIK_UNLABELED, // KEY_UNLABELED,		// japanese keyboard
	DIK_LCONTROL, // KEY_LCONTROL,
	DIK_LMENU, // KEY_LALT,
	DIK_LSHIFT, // KEY_LSHIFT,
	DIK_RCONTROL, // KEY_RCONTROL,
	DIK_RMENU, // KEY_RALT,
	DIK_RSHIFT, // KEY_RSHIFT,
	DIK_CAPITAL, // KEY_CAPITAL,

	DIK_NUMLOCK, // KEY_NUMLOCK,
	DIK_DIVIDE, // KEY_DIVIDE,
	DIK_SUBTRACT, // KEY_SUBTRACT,
	DIK_DECIMAL, // KEY_DECIMAL
	DIK_NUMPAD0, // KEY_NUMPAD0,
	DIK_NUMPAD1, // KEY_NUMPAD1,
	DIK_NUMPAD2, // KEY_NUMPAD2,
	DIK_NUMPAD3, // KEY_NUMPAD3,
	DIK_NUMPAD4, // KEY_NUMPAD4,
	DIK_NUMPAD5, // KEY_NUMPAD5,
	DIK_NUMPAD6, // KEY_NUMPAD6,
	DIK_NUMPAD7, // KEY_NUMPAD7,
	DIK_NUMPAD8, // KEY_NUMPAD8,
	DIK_NUMPAD9, // KEY_NUMPAD9,
	DIK_NUMPADENTER, // KEY_NUMPADENTER,

	DIK_PLAYPAUSE, // KEY_PLAYPAUSE,		// on multimedia keyboards
	DIK_MEDIASTOP, // KEY_MEDIASTOP,		// on multimedia keyboards
	DIK_MEDIASELECT, // KEY_MEDIASELECT,	// on multimedia keyboards
	DIK_NEXTTRACK, // KEY_NEXTTRACK,		// on multimedia keyboards
	DIK_PREVTRACK, // KEY_PREVTRACK,		// on multimedia keyboards

	DIK_VOLUMEDOWN, // KEY_VOLUMEDOWN,		// on multimedia keyboards
	DIK_VOLUMEUP, // KEY_VOLUMEUP,		// on multimedia keyboards
	DIK_MUTE, // KEY_MUTE,			// on multimedia keyboards

	DIK_WEBBACK, // KEY_WEBBACK,		// on multimedia keyboards
	DIK_WEBFAVORITES, // KEY_WEBFAVORITES,	// on multimedia keyboards
	DIK_WEBFORWARD, // KEY_WEBFORWARD,		// on multimedia keyboards
	DIK_WEBHOME, // KEY_WEBHOME,		// on multimedia keyboards
	DIK_WEBREFRESH, // KEY_WEBREFRESH,		// on multimedia keyboards
	DIK_WEBSEARCH, // KEY_WEBSEARCH,		// on multimedia keyboards
	DIK_WEBSTOP, // KEY_WEBSTOP,		// on multimedia keyboards

	DIK_AT, // KEY_AT,				// japanese keyboard
	DIK_AX, // KEY_AX,				// japanese keyboard
	DIK_COLON, // KEY_COLON,			// japanese keyboard
	DIK_CONVERT, // KEY_CONVERT,		// japanese keyboard
	DIK_KANA, // KEY_KANA,			// japanese keyboard
	DIK_KANJI, // KEY_KANJI,			// japanese keyboard
	DIK_NOCONVERT, // KEY_NOCONVERT,		// japanese keyboard
	DIK_YEN  // KEY_YEN,			// japanese keyboard
};


void Input_InitModulePlatformSpecific()
{
	CALLSTACK;

	int a;

	ZeroMemory(gKeyState,256);
	LoadGamepadMappings();

	if(FAILED(DirectInput8Create(apphInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDirectInput, NULL))) return;

	if(SUCCEEDED(pDirectInput->CreateDevice(GUID_SysKeyboard, &pDIKeyboard, NULL)))
	{
		pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	}
	gKeyboardCount = 1;

	if(SUCCEEDED(pDirectInput->CreateDevice(GUID_SysMouse, &pDIMouse, NULL)))
	{
		pDIMouse->SetDataFormat(&c_dfDIMouse2);
	}
	gMouseCount = 1;

	pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);

	for(a=0; a<gGamepadCount; a++)
	{
		if(pDIJoystick[a])
		{
			pDIJoystick[a]->SetDataFormat(&c_dfDIJoystick2);
		}
	}

	Input_SetCooperativeLevels();

/*
	RAWINPUTDEVICE rid[] =
	{
		{0x1, 0x2, RIDEV_CAPTUREMOUSE, apphWnd},
		{0x1, 0x6, NULL, apphWnd},
	};

	RegisterRawInputDevices(rid, sizeof(rid) / sizeof(rid[0]), sizeof(rid[0]));
*/
}

void Input_DeinitModulePlatformSpecific()
{
	CALLSTACK;

	int a;

	for(a=0; a<16; a++)
	{
		if(pDIJoystick[a])
		{
			pDIJoystick[a]->Unacquire();
			pDIJoystick[a]->Release();
			pDIJoystick[a] = NULL;
		}
	}
	gGamepadCount=0;

	if(pDIMouse)
	{
		pDIMouse->Unacquire();
		pDIMouse->Release();
		pDIMouse = NULL;
	}
	gMouseCount=0;

	if(pDIKeyboard)
	{
		pDIKeyboard->Unacquire();
		pDIKeyboard->Release();
		pDIKeyboard = NULL;
	}
	gKeyboardCount=0;

	if(pDirectInput)
	{
		pDirectInput->Release();
		pDirectInput = NULL;
	}
}

void Input_UpdatePlatformSpecific()
{

}

void Input_GetDeviceStatusInternal(int device, int id, DeviceStatus *pDeviceStatus)
{
	pDeviceStatus->available = false;
	pDeviceStatus->status = IDS_Disconnected;

	switch(device)
	{
		case IDD_Gamepad:
			if(id < gGamepadCount)
			{
				pDeviceStatus->available = true;
				pDeviceStatus->status = IDS_Ready;
			}
			break;

		case IDD_Mouse:
			if(id < gMouseCount)
			{
				pDeviceStatus->available = true;
				pDeviceStatus->status = IDS_Ready;
			}
			break;

		case IDD_Keyboard:
			if(id < gKeyboardCount)
			{
				pDeviceStatus->available = true;
				pDeviceStatus->status = IDS_Ready;
			}
			break;

		default:
			DBGASSERT(false, "Invalid Input Device");
			break;
	}
}

void Input_GetGamepadStateInternal(int id, GamepadState *pGamepadState)
{
	CALLSTACK;

	DIJOYSTATE2 joyState;
	int a;

	memset(pGamepadState, 0, sizeof(*pGamepadState));

	if(pDIJoystick[id])
	{
		if(FAILED(pDIJoystick[id]->Poll()))
		{
			pDIJoystick[id]->Acquire();
			return;
		}

		pDIJoystick[id]->GetDeviceState(sizeof(DIJOYSTATE2), &joyState);
	}

	for(a=0; a<16; a++)
	{
		if(pGamepadMappings[id]->buttonMapping[a] > -1)
			pGamepadState->values[a] = joyState.rgbButtons[pGamepadMappings[id]->buttonMapping[a]] ? 1.0f : 0.0f;
	}

	if(pGamepadMappings[id]->buttonMapping[Button_DUp] == -1)
	{
		// read from POV
		DWORD pov = joyState.rgdwPOV[0];

		bool POVCentered = (LOWORD(pov) == 0xFFFF);

		if(!POVCentered)
		{
			if((pov >= 0 && pov <= 4500) || (pov >= 31500 && pov <= 36000))
				pGamepadState->values[Button_DUp] = 1.0f;
			if((pov >= 4500 && pov <= 13500))
				pGamepadState->values[Button_DRight] = 1.0f;
			if((pov >= 13500 && pov <= 22500))
				pGamepadState->values[Button_DDown] = 1.0f;
			if((pov >= 22500 && pov <= 31500))
				pGamepadState->values[Button_DLeft] = 1.0f;
		}
	}

	float *pGamepadAxis = &pGamepadState->values[Axis_LX];
	LONG *pSourceAxis = (LONG*)&joyState;

	float deadZone = Input_GetDeadZone();

	for(a=0; a<4; a++)
	{
		if(pGamepadMappings[id]->axisMapping[a] > -1)
		{
			float inputValue = ((float)(pSourceAxis[pGamepadMappings[id]->axisMapping[a]] - 32767.0f)) * (1.0f/32767.0f);
			pGamepadAxis[a] = (abs(inputValue) > deadZone) ? inputValue : 0.0f;

			if(a&1) // y axis's need to be inverted.. direct input seems to like to report them upside down ;)
				pGamepadAxis[a] = -pGamepadAxis[a];
		}
	}
}

void Input_GetKeyStateInternal(int id, KeyState *pKeyState)
{
	CALLSTACK;

	DIDEVICEOBJECTDATA inputBuffer[SAMPLE_BUFFER_SIZE];
	DWORD elements = SAMPLE_BUFFER_SIZE;

	HRESULT hr;

	hr = pDIKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), inputBuffer, &elements, 0 );

	if(hr != DI_OK)
	{
		pDIKeyboard->Acquire();
		return;
	}
	else
	{
		for(DWORD a=0; a<elements; a++) 
		{
			gKeyState[inputBuffer[a].dwOfs]=(char)inputBuffer[a].dwData;
			if(inputBuffer[a].dwOfs==DIK_PAUSE)
			{
				// why am i doing this again?
				gKeyState[DIK_PAUSE]=(char)0x80;
			}
		}
	}

	uint8 *pKeys = pKeyState->keys;

#pragma message("This part is redundant, needs to DIK->KEY table")
	for(int a=0; a<256; a++)
	{
		pKeys[0] = gKeyState[KEYtoDIK[a]] ? -1 : 0;
	}
}

void Input_GetMouseStateInternal(int id, MouseState *pMouseState)
{
	CALLSTACK;

	DIDEVICEOBJECTDATA inputBuffer[SAMPLE_BUFFER_SIZE];
	DWORD elements = SAMPLE_BUFFER_SIZE;

	HRESULT hr;

	hr = pDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), inputBuffer, &elements, 0 );

	if(hr != DI_OK)
	{
		pDIMouse->Acquire();
		return;
	}
	else
	{
		for(DWORD a=0; a<elements; a++) 
		{
			switch(inputBuffer[a].dwOfs)
			{
				case DIMOFS_BUTTON0:
					pMouseState->buttonState[0] = inputBuffer[a].dwData ? -1 : 0;
					break;
				case DIMOFS_BUTTON1:
					pMouseState->buttonState[1] = inputBuffer[a].dwData ? -1 : 0;
					break;
				case DIMOFS_BUTTON2:
					pMouseState->buttonState[2] = inputBuffer[a].dwData ? -1 : 0;
					break;
				case DIMOFS_BUTTON3:
					pMouseState->buttonState[3] = inputBuffer[a].dwData ? -1 : 0;
					break;
				case DIMOFS_BUTTON4:
					pMouseState->buttonState[4] = inputBuffer[a].dwData ? -1 : 0;
					break;
				case DIMOFS_BUTTON5:
					pMouseState->buttonState[5] = inputBuffer[a].dwData ? -1 : 0;
					break;
				case DIMOFS_BUTTON6:
					pMouseState->buttonState[6] = inputBuffer[a].dwData ? -1 : 0;
					break;
				case DIMOFS_BUTTON7:
					pMouseState->buttonState[7] = inputBuffer[a].dwData ? -1 : 0;
					break;

				case DIMOFS_X:
					pMouseState->values[Mouse_XPos]  += (float)inputBuffer[a].dwData;
					pMouseState->values[Mouse_XDelta] = (float)inputBuffer[a].dwData;
					break;
				case DIMOFS_Y:
					pMouseState->values[Mouse_YPos]  += (float)inputBuffer[a].dwData;
					pMouseState->values[Mouse_YDelta] = (float)inputBuffer[a].dwData;
					break;

				case DIMOFS_Z:
					pMouseState->values[Mouse_Wheel]  = (float)inputBuffer[a].dwData;
					break;

				default:
					break;
			}
		}
	}
}

char* Input_GetDeviceName(int source, int sourceID)
{
	char *pText = NULL;

	switch(source)
	{
		case IDD_Gamepad:
			break;
		case IDD_Mouse:
			break;
		case IDD_Keyboard:
			break;
		default:
			break;
	}

	return pText;
}

bool Input_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	SHORT ks = 0;

	switch(keyboardState)
	{
		case KSS_NumLock:
			ks = GetKeyState(VK_NUMLOCK);
			break;

		case KSS_CapsLock:
			ks = GetKeyState(VK_CAPITAL);
			break;

		case KSS_ScrollLock:
			ks = GetKeyState(VK_SCROLL);
			break;

		case KSS_Insert:
			ks = GetKeyState(VK_INSERT);
			break;
	}

	return (ks & 1) != 0;
}

// internal functions
void Input_SetCooperativeLevels()
{
	CALLSTACK;

	int a;

	if(pDIKeyboard)
	{
		if(FAILED(pDIKeyboard->SetCooperativeLevel(apphWnd, DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE)))
		{
			DBGASSERT(false, "Failed to set Keyboard cooperative level");
		}
	}

	if(pDIMouse)
	{
		if(FAILED(pDIMouse->SetCooperativeLevel(apphWnd, DISCL_FOREGROUND | (gExclusiveMouse ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE))))
		{
			DBGASSERT(false, "Failed to set Mouse cooperative level");
		}
	}

	for(a=0; a<gGamepadCount; a++)
	{
		if(pDIJoystick[a])
		{
			if(FAILED(pDIJoystick[a]->SetCooperativeLevel(apphWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
			{
				DBGASSERT(false, STR("Failed to set Gamepad %d cooperative level", a));
			}
		}
	}

	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = SAMPLE_BUFFER_SIZE;

	if(pDIKeyboard) pDIKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if(pDIMouse) pDIMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
}

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	CALLSTACK;

	if(gGamepadCount<16)
	{
		if(FAILED(pDirectInput->CreateDevice(pdidInstance->guidInstance, &pDIJoystick[gGamepadCount], NULL)))
			return DIENUM_CONTINUE;

		pGamepadMappings[gGamepadCount] = GetGamepadInfo(pdidInstance->tszProductName);
		gGamepadCount++;

		return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}

void Input_Acquire(bool acquire)
{
	CALLSTACK;

	int a;

	if(pDIKeyboard)
	{
		if(acquire) pDIKeyboard->Acquire();
		else pDIKeyboard->Unacquire();
	}

	if(pDIMouse)
	{
		if(acquire) pDIMouse->Acquire();
		else pDIMouse->Unacquire();
	}

	for(a=0; a<gGamepadCount; a++)
	{
		if(pDIJoystick[a])
		{
			if(acquire)
			{
				if(FAILED(pDIJoystick[a]->Poll()))
				{
					pDIJoystick[a]->Acquire();
				}
			}
			else pDIJoystick[a]->Unacquire();
		}
	}
}

GamepadInfo *GetGamepadInfo(const char *pGamepad)
{
	for(GamepadInfo *pT = pGamepadMappingRegistry; pT; pT = pT->pNext)
	{
		if(!strcmp(pT->pName, pGamepad))
			return pT;
	}

	const char *pDefault = "default";

	if(pDefault != pGamepad)
		return GetGamepadInfo(pDefault);

	return NULL;
}

void LoadGamepadMappings()
{
	// load GamepadMappings.ini
	GamepadInfo *pGI = NULL;
	IniFile ini;

	// create default
	pGI = (GamepadInfo*)Heap_Alloc(sizeof(GamepadInfo) + strlen("default") + 1);
	pGI->pName = (char*)&pGI[1];
	strcpy(pGI->pName, "default");

	pGI->pNext = pGamepadMappingRegistry;
	pGamepadMappingRegistry = pGI;

	for(int a=0; a<16; a++)
	{
		pGI->buttonMapping[a] = a;
	}

	for(int a=0; a<4; a++)
	{
		pGI->axisMapping[a] = a;
	}

	// read GameMappings.ini file
	if(!ini.Create(File_SystemPath("GamepadMappings.ini")))
	{
		ini.GetFirstLine();

		while(!ini.EndOfFile())
		{
			char *pName = ini.GetName();

			if(ini.IsSection())
			{
				pGI = (GamepadInfo*)Heap_Alloc(sizeof(GamepadInfo) + strlen(pName) + 1);
				pGI->pName = (char*)&pGI[1];
				strcpy(pGI->pName, pName);

				pGI->pNext = pGamepadMappingRegistry;
				pGamepadMappingRegistry = pGI;
			}
			else
			{
				if(!stricmp(pName, "Axis_LX"))
				{
					pGI->axisMapping[0] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Axis_LY"))
				{
					pGI->axisMapping[1] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Axis_RX"))
				{
					pGI->axisMapping[2] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Axis_RY"))
				{
					pGI->axisMapping[3] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_Cross"))
				{
					pGI->buttonMapping[0] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_Circle"))
				{
					pGI->buttonMapping[1] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_Box"))
				{
					pGI->buttonMapping[2] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_Triangle"))
				{
					pGI->buttonMapping[3] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_R1"))
				{
					pGI->buttonMapping[4] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_L1"))
				{
					pGI->buttonMapping[5] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_L2"))
				{
					pGI->buttonMapping[6] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_R2"))
				{
					pGI->buttonMapping[7] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_Start"))
				{
					pGI->buttonMapping[8] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_Select"))
				{
					pGI->buttonMapping[9] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_LThumb"))
				{
					pGI->buttonMapping[10] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_P2_RThumb"))
				{
					pGI->buttonMapping[11] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_DUp"))
				{
					pGI->buttonMapping[12] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_DDown"))
				{
					pGI->buttonMapping[13] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_DLeft"))
				{
					pGI->buttonMapping[14] = ini.AsInt(0);
				}
				else if(!stricmp(pName, "Button_DRight"))
				{
					pGI->buttonMapping[15] = ini.AsInt(0);
				}
			}

			ini.GetNextLine();
		}

		ini.Release();
	}
}
