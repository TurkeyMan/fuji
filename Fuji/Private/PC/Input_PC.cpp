#define DIRECTINPUT_VERSION 0x0800
#define _WIN32_WINNT 0x501   // This specifies WinXP or later - it is needed to access rawmouse from the user32.dll

#define SAMPLE_BUFFER_SIZE 128//50000

#include "Fuji.h"
#include <dinput.h>

#include "MFVector.h"
#include "Input_Internal.h"
#include "Heap.h"
#include "MFIni.h"

#if defined(ALLOW_RAW_INPUT)
	#define RAW_SYS_MOUSE 0      // The sys mouse combines all the other usb mice into one
	#define MAX_RAW_MOUSE_BUTTONS 5

	typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceList)(OUT PRAWINPUTDEVICELIST pRawInputDeviceList, IN OUT PINT puiNumDevices, IN UINT cbSize);
	typedef WINUSERAPI INT (WINAPI *pGetRawInputData)(IN HRAWINPUT hRawInput, IN UINT uiCommand, OUT LPVOID pData, IN OUT PINT pcbSize, IN UINT cbSizeHeader);
	typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceInfoA)(IN HANDLE hDevice, IN UINT uiCommand, OUT LPVOID pData, IN OUT PINT pcbSize);
	typedef WINUSERAPI BOOL (WINAPI *pRegisterRawInputDevices)(IN PCRAWINPUTDEVICE pRawInputDevices, IN UINT uiNumDevices, IN UINT cbSize);

	int InitRawMouse(bool _includeRDPMouse);
#endif

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdpDirectInputstance, VOID* pContext);

void Input_SetCooperativeLevels();

void Input_UpdateKeyboard();
void Input_UpdateMouse();
void Input_UpdateJoystick();

/*** Structure definitions ***/

#if defined(ALLOW_RAW_INPUT)
struct RawMouse
{
	HANDLE deviceHandle;

	int x, xDelta;
	int y, yDelta;
	int zDelta;

	bool isAbsolute;
	bool isVirtualDesktop;

	int buttonPressed[MAX_RAW_MOUSE_BUTTONS];

	char buttonNames[MAX_RAW_MOUSE_BUTTONS][64];
};
#endif

struct GamepadInfo
{
	char * pName;
	const char * const * ppButtonNameStrings;

	int axisMapping[4];
	int buttonMapping[16];
	bool usePOV;

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

const long joyAxii[24] = {0,1,2,3,4,5,44,45,46,47,48,49,52,53,54,55,56,57,60,61,62,63,64,65};
const long joySliders[4] = {6,50,58,66};

GamepadInfo *pGamepadMappingRegistry = NULL;

#if defined(ALLOW_RAW_INPUT)
pGetRawInputDeviceList _GRIDL;
pGetRawInputData _GRID;
pGetRawInputDeviceInfoA _GRIDIA;
pRegisterRawInputDevices _RRID;

RawMouse *pRawMice = NULL;
int rawMouseCount;

BOOL includeRDPMouse;
#endif

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

const char * const DefaultButtons[] =
{
// Default controller enums
	"Button 1",
	"Button 2",
	"Button 3",
	"Button 4",
	"Button 5",
	"Button 6",
	"Button 7",
	"Button 8",
	"Button 9",
	"Button 10",
	"Button 11",
	"Button 12",

// general controller enums
	"Digital Up",
	"Digital Down",
	"Digital Left",
	"Digital Right",
	"Analog X-Axis",
	"Analog Y-Axis",
	"Analog RX-Axis",
	"Analog RY-Axis"
};

const char * const XBoxButtons[] =
{
// xbox controller enums
	"A",
	"B",
	"X",
	"Y",
	"Black",
	"White",
	"Left Trigger",
	"Right Trigger",
	"Start",
	"Back",
	"Left Thumb",
	"Right Thumb",

// general controller enums
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Left Analog X-Axis",
	"Left Analog Y-Axis",
	"Right Analog X-Axis",
	"Right Analog Y-Axis"
};

const char * const PS2Buttons[] =
{
// PSX controller enums
	"Cross",
	"Circle",
	"Box",
	"Triangle",
	"R1",
	"L1",
	"L2",
	"R2",
	"Start",
	"Select",
	"Left Thumb",
	"Right Thumb",

// general controller enums
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Left Analog X-Axis",
	"Left Analog Y-Axis",
	"Right Analog X-Axis",
	"Right Analog Y-Axis"
};

/**** Platform Specific Functions ****/

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

#if defined(ALLOW_RAW_INPUT)
	if(gDefaults.input.allowMultipleMice)
	{
		InitRawMouse(false);

		if(rawMouseCount > 1)
			gMouseCount = rawMouseCount;
	}
#endif
#if defined(ALLOW_DI_MOUSE)
	if(gMouseCount == 0)
	{
		if(SUCCEEDED(pDirectInput->CreateDevice(GUID_SysMouse, &pDIMouse, NULL)))
		{
			pDIMouse->SetDataFormat(&c_dfDIMouse2);
		}
		gMouseCount = 1;
	}
#endif

	pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);

	for(a=0; a<gGamepadCount; a++)
	{
		if(pDIJoystick[a])
		{
			pDIJoystick[a]->SetDataFormat(&c_dfDIJoystick2);
		}
	}

	Input_SetCooperativeLevels();
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
	gGamepadCount = 0;

#if defined(ALLOW_DI_MOUSE)
	if(pDIMouse)
	{
		pDIMouse->Unacquire();
		pDIMouse->Release();
		pDIMouse = NULL;
	}
	gMouseCount = 0;
#endif

	if(pDIKeyboard)
	{
		pDIKeyboard->Unacquire();
		pDIKeyboard->Release();
		pDIKeyboard = NULL;
	}
	gKeyboardCount = 0;

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

				DIDEVCAPS caps;
				memset(&caps, 0, sizeof(DIDEVCAPS));
				caps.dwSize = sizeof(DIDEVCAPS);

				pDIJoystick[id]->GetCapabilities(&caps);

				if(caps.dwFlags & DIDC_ATTACHED)
				{
					pDeviceStatus->status = IDS_Ready;
				}
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

		for(a=0; a<16; a++)
		{
			if(pGamepadMappings[id]->buttonMapping[a] > -1)
				pGamepadState->values[a] = joyState.rgbButtons[pGamepadMappings[id]->buttonMapping[a]] ? 1.0f : 0.0f;
		}

		if(pGamepadMappings[id]->usePOV)
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
		}
	}

	uint8 *pKeys = pKeyState->keys;

#pragma message("This part is redundant, needs to DIK->KEY table")
	for(int a=0; a<256; a++)
	{
		pKeys[a] = gKeyState[KEYtoDIK[a]] ? -1 : 0;
	}
}

void GetWindowMousePos(float *pX, float *pY)
{
	POINT mouse;
	RECT client;
	GetCursorPos(&mouse);
	ScreenToClient(apphWnd, &mouse);
	GetClientRect(apphWnd, &client);

	mouse.x = MFMax(mouse.x, client.left);
	mouse.y = MFMax(mouse.y, client.top);
	mouse.x = MFMin(mouse.x, client.right-1);
	mouse.y = MFMin(mouse.y, client.bottom-1);

	*pX = (float)mouse.x;
	*pY = (float)mouse.y;
}

void Input_GetMouseStateInternal(int id, MouseState *pMouseState)
{
	CALLSTACK;

#if defined(ALLOW_DI_MOUSE)
	if(id == 0 && gMouseCount == 1)
	{
		DIDEVICEOBJECTDATA inputBuffer[SAMPLE_BUFFER_SIZE];
		DWORD elements = SAMPLE_BUFFER_SIZE;

		HRESULT hr;

		pMouseState->values[Mouse_XDelta] = 0.0f;
		pMouseState->values[Mouse_YDelta] = 0.0f;
		pMouseState->values[Mouse_Wheel] = 0.0f;
		pMouseState->values[Mouse_Wheel2] = 0.0f;

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
						pMouseState->values[Mouse_XPos]  += (float)((int)inputBuffer[a].dwData);
						pMouseState->values[Mouse_XDelta] = (float)((int)inputBuffer[a].dwData);
						break;
					case DIMOFS_Y:
						pMouseState->values[Mouse_YPos]  += (float)((int)inputBuffer[a].dwData);
						pMouseState->values[Mouse_YDelta] = (float)((int)inputBuffer[a].dwData);
						break;
					case DIMOFS_Z:
						pMouseState->values[Mouse_Wheel]  = (float)((int)inputBuffer[a].dwData) / 120.0f;
						break;
					default:
						break;
				}
			}
		}
	}
	else
#endif
	{
#if defined(ALLOW_RAW_INPUT)
		if(gDefaults.input.allowMultipleMice)
		{
			// read from the raw mouse
			for(int a=0; a<MAX_RAW_MOUSE_BUTTONS; a++)
			{
				pMouseState->buttonState[a] = pRawMice[id].buttonPressed[a] ? -1 : 0;
			}

			if(id != 0 || !gDefaults.input.mouseZeroIsSystemMouse || !gDefaults.input.systemMouseUseWindowsCursor)
			{
				pMouseState->values[Mouse_XPos] = (float)pRawMice[id].x;
				pMouseState->values[Mouse_YPos] = (float)pRawMice[id].y;
				pMouseState->values[Mouse_XDelta] = (float)pRawMice[id].xDelta;
				pMouseState->values[Mouse_YDelta] = (float)pRawMice[id].yDelta;
			}
			pMouseState->values[Mouse_Wheel] = (float)pRawMice[id].zDelta;

			// reset the deltas
			pRawMice[id].xDelta = 0;
			pRawMice[id].yDelta = 0;
			pRawMice[id].zDelta = 0;
		}
#endif
	}

#if defined(USE_WINDOWS_MOUSE_COORDS)
	if(id == 0 && gDefaults.input.mouseZeroIsSystemMouse && gDefaults.input.systemMouseUseWindowsCursor)
	{
		// get the mouse position and delta's from the absolute screen position on PC
		float x, y;
		GetWindowMousePos(&x, &y);

		pMouseState->values[Mouse_XDelta] = x - pMouseState->values[Mouse_XPos];
		pMouseState->values[Mouse_YDelta] = y - pMouseState->values[Mouse_YPos];
		pMouseState->values[Mouse_XPos] = x;
		pMouseState->values[Mouse_YPos] = y;
	}
#endif

	// clamp absolute mouse coords to mouse rect
	//....
}

const char* Input_GetDeviceName(int source, int sourceID)
{
	const char *pText = NULL;

	switch(source)
	{
		case IDD_Gamepad:
		{
			if(strcmp(pGamepadMappings[sourceID]->pName, "default"))
				pText = pGamepadMappings[sourceID]->pName;
			else
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
	return pGamepadMappings[sourceID]->ppButtonNameStrings[type];
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

		if(pGamepadMappings[gGamepadCount]->usePOV)
		{
			DIDEVCAPS caps;
			memset(&caps, 0, sizeof(DIDEVCAPS));
			caps.dwSize = sizeof(DIDEVCAPS);

			pDIJoystick[gGamepadCount]->GetCapabilities(&caps);

			if(caps.dwPOVs < 1)
				pGamepadMappings[gGamepadCount]->usePOV = false;
		}

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
	MFIni *pIni;

	// create default
	pGI = (GamepadInfo*)Heap_Alloc(sizeof(GamepadInfo) + strlen("default") + 1);
	pGI->usePOV = true;
	pGI->pName = (char*)&pGI[1];
	pGI->ppButtonNameStrings = DefaultButtons;
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
	pIni = MFIni::Create("GamepadMappings.ini");

	if(pIni)
	{
		MFIniLine *pLine;
		pLine = pIni->GetFirstLine();

		while(pLine)
		{
			if (pLine->IsString(0, "Gamepad"))
			{
				const char *pName = pLine->GetString(1);
				pGI = (GamepadInfo*)Heap_Alloc(sizeof(GamepadInfo) + strlen(pName) + 1);
				pGI->usePOV = true;
				pGI->pName = (char*)&pGI[1];
				pGI->ppButtonNameStrings = DefaultButtons;
				strcpy(pGI->pName, pName);
				pGI->pNext = pGamepadMappingRegistry;
				pGamepadMappingRegistry = pGI;

				MFIniLine *pPadLine = pLine->Sub();
				while (pPadLine)
				{
					const char *pField = pPadLine->GetString(0);
					if (!stricmp(pField, "Axis_LX"))
					{
						pGI->axisMapping[0] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Axis_LY"))
					{
						pGI->axisMapping[1] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Axis_RX"))
					{
						pGI->axisMapping[2] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Axis_RY"))
					{
						pGI->axisMapping[3] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_Cross"))
					{
						pGI->buttonMapping[0] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_Circle"))
					{
						pGI->buttonMapping[1] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_Box"))
					{
						pGI->buttonMapping[2] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_Triangle"))
					{
						pGI->buttonMapping[3] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_R1"))
					{
						pGI->buttonMapping[4] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_L1"))
					{
						pGI->buttonMapping[5] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_L2"))
					{
						pGI->buttonMapping[6] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_R2"))
					{
						pGI->buttonMapping[7] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_Start"))
					{
						pGI->buttonMapping[8] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_Select"))
					{
						pGI->buttonMapping[9] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_LThumb"))
					{
						pGI->buttonMapping[10] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_P2_RThumb"))
					{
						pGI->buttonMapping[11] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_DUp"))
					{
						pGI->buttonMapping[12] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_DDown"))
					{
						pGI->buttonMapping[13] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_DLeft"))
					{
						pGI->buttonMapping[14] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "Button_DRight"))
					{
						pGI->buttonMapping[15] = pPadLine->GetInt(1);
					}
					else if(!stricmp(pField, "IgnorePOV"))
					{
						pGI->usePOV = false;
					}
					else if(!stricmp(pField, "ButtonStrings"))
					{
						const char *pStrings = pPadLine->GetString(1);
						if(!stricmp(pStrings, "XBox"))
						{
							pGI->ppButtonNameStrings = XBoxButtons;
						}
						else if(!stricmp(pStrings, "Playstation"))
						{
							pGI->ppButtonNameStrings = PS2Buttons;
						}
						else
						{
							LOGD(STR("Error: Gamepad button strings '%s' unavailable for gamepad type '%s'.", pStrings, pGI->pName));
						}
					}
					else
					{
						LOGD(STR("Error: Unknown controller property '%s' in gamepad '%s'.", pName, pGI->pName));
					}

					pPadLine = pPadLine->Next();
				}
			}

			pLine = pLine->Next();
		}

		MFIni::Destroy(pIni);
	}
}


/**** Raw Mouse Functions ****/
#if defined(ALLOW_RAW_INPUT)

int RegisterRawMouse(void)
{
	// This function registers to receive the WM_INPUT messages
	RAWINPUTDEVICE rid[1]; // Register only for mouse messages from wm_input.  

	//register to get wm_input messages
	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;
//	rid[0].dwFlags = RIDEV_CAPTUREMOUSE; // this denies the mouse activating other windows
	rid[0].dwFlags = 0;// RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages

#if 1
	rid[0].dwFlags |= RIDEV_INPUTSINK;
	rid[0].hwndTarget = apphWnd;
#else
	rid[0].hwndTarget = NULL;
#endif

	// Register to receive the WM_INPUT message for any change in mouse (buttons, wheel, and movement will all generate the same message)
	DBGASSERT(_RRID(rid, sizeof(rid)/sizeof(rid[0]), sizeof(rid[0])), "Error: RegisterRawInputDevices() failed");

	return 0;
}

bool IsRDPMouse(const char *pName)
{
	return !strncmp(pName, "\\??\\Root#RDP_MOU#0000#", 22);
}

int InitRawMouse(bool _includeRDPMouse)
{
	// "0" to exclude, "1" to include

	int nInputDevices, i, j;
	RAWINPUTDEVICELIST *pRawInputDeviceList;

	int currentmouse = 0;
	int nSize;
	char pName[256];

	// Return 0 if rawinput is not available
	HMODULE user32 = LoadLibrary("user32.dll");
	if (!user32)
	{
		LOGD("Cant open user32.dll");
		return 1;
	}

	_RRID = (pRegisterRawInputDevices)GetProcAddress(user32,"RegisterRawInputDevices");
	if (!_RRID)
	{
		LOGD("Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	_GRIDL = (pGetRawInputDeviceList)GetProcAddress(user32,"GetRawInputDeviceList");
	if (!_GRIDL)
	{
		LOGD("Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	_GRIDIA = (pGetRawInputDeviceInfoA)GetProcAddress(user32,"GetRawInputDeviceInfoA");
	if (!_GRIDIA)
	{
		LOGD("Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	_GRID = (pGetRawInputData)GetProcAddress(user32,"GetRawInputData");
	if (!_GRID)
	{
		LOGD("Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	rawMouseCount = 0;
	includeRDPMouse = _includeRDPMouse;

	// 1st call to GetRawInputDeviceList: Pass NULL to get the number of devices.
	if(_GRIDL(NULL, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
	{
		DBGASSERT(false, "Error: Unable to count raw input devices.");
		return 1;
	}

	// Allocate the array to hold the DeviceList
	pRawInputDeviceList = (RAWINPUTDEVICELIST*)malloc(sizeof(RAWINPUTDEVICELIST) * nInputDevices);

	// 2nd call to GetRawInputDeviceList: Pass the pointer to our DeviceList and GetRawInputDeviceList() will fill the array
	if(_GRIDL(pRawInputDeviceList, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) == -1)
	{
		DBGASSERT(false, "Error: Unable to get raw input device list.");
		return 1;
	}

	// Loop through all devices and count the mice
	for(i = 0; i < nInputDevices; i++)
	{
		if(pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			nSize = 256;
			if((int)_GRIDIA(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, pName, &nSize) < 0)
			{
				DBGASSERT(false, "Error: Unable to get raw input device name.");
				return 1;
			} 

			// Count this mouse for allocation if it's not an RDP mouse or if we want to include the rdp mouse
			if(includeRDPMouse || !IsRDPMouse(pName))
			{
				rawMouseCount++;
			}
		}
	}

	if(rawMouseCount < 2)
	{
		// we only found 1 mouse, so we will use DirectInput instead (Its probably more stable) ;)
		free(pRawInputDeviceList);
		return 0;
	}

	// add system mouse is requested
	if(gDefaults.input.mouseZeroIsSystemMouse)
	{
		++rawMouseCount;
		++currentmouse;
	}

	// Allocate the array for the raw mice
	pRawMice = (RawMouse*)malloc(sizeof(RawMouse) * rawMouseCount);
	ZeroMemory(pRawMice, sizeof(RawMouse) * rawMouseCount);

	// Loop through all devices and set the device handles and initialize the mouse values
	for(i = 0; i < nInputDevices; i++)
	{
		if(pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			nSize = 256;
			if((int)_GRIDIA(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, pName, &nSize) < 0)
			{
				DBGASSERT(false, "Error: Unable to get raw input device name.");
				return 1;
			} 

			// Add this mouse to the array if it's not an RDPMouse or if we wish to include the RDP mouse
			if(includeRDPMouse || !IsRDPMouse(pName))
			{
				pRawMice[currentmouse].deviceHandle = pRawInputDeviceList[i].hDevice;
				currentmouse++;
			}
		}
	}

	// free the RAWINPUTDEVICELIST
	free(pRawInputDeviceList);

	for(i = 0; i < rawMouseCount; i++)
	{
		for(j = 0; j < MAX_RAW_MOUSE_BUTTONS; j++)
		{
			// Create the name for this button
			strcpy(pRawMice[i].buttonNames[j], STR("Button %i", j));
		}
	}

	// finally, register to recieve raw input WM_INPUT messages
	DBGASSERT(RegisterRawMouse() == 0, "Error: Unable to register raw input.");

	return 0;  
}

int ReadRawInput(RAWINPUT *pRaw)
{
	// should be static when I get around to it
	int i;

	for(i = 0 ; i < rawMouseCount; i++)
	{
		if(pRawMice[i].deviceHandle == pRaw->header.hDevice)
		{
			if(pRaw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)			pRawMice[i].isAbsolute = 1;
			else														pRawMice[i].isAbsolute = 0;
//			else if (pRaw->data.mouse.usFlags & MOUSE_MOVE_RELATIVE)	pRawMice[i].isAbsolute = 0;
			if(pRaw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)		pRawMice[i].isVirtualDesktop = 1;
			else														pRawMice[i].isVirtualDesktop = 0;

			// Update the values for the specified mouse
			if(pRawMice[i].isAbsolute)
			{
				pRawMice[i].xDelta = pRaw->data.mouse.lLastX - pRawMice[i].x;
				pRawMice[i].yDelta = pRaw->data.mouse.lLastY - pRawMice[i].y;
				pRawMice[i].x = pRaw->data.mouse.lLastX;
				pRawMice[i].y = pRaw->data.mouse.lLastY;
			}
			else
			{
				pRawMice[i].xDelta = pRaw->data.mouse.lLastX;
				pRawMice[i].yDelta = pRaw->data.mouse.lLastY;
				pRawMice[i].x += pRaw->data.mouse.lLastX;
				pRawMice[i].y += pRaw->data.mouse.lLastY;
			}

			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) pRawMice[i].buttonPressed[0] = 1;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)   pRawMice[i].buttonPressed[0] = 0;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) pRawMice[i].buttonPressed[1] = 1;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)   pRawMice[i].buttonPressed[1] = 0;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) pRawMice[i].buttonPressed[2] = 1;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP)   pRawMice[i].buttonPressed[2] = 0;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) pRawMice[i].buttonPressed[3] = 1;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)   pRawMice[i].buttonPressed[3] = 0;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) pRawMice[i].buttonPressed[4] = 1;
			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)   pRawMice[i].buttonPressed[4] = 0;

			if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
			{
				// If the current message has a mouse_wheel message
				if ((SHORT)pRaw->data.mouse.usButtonData > 0)
				{
					pRawMice[i].zDelta++;
				}
				if ((SHORT)pRaw->data.mouse.usButtonData < 0)
				{
					pRawMice[i].zDelta--;
				}
			}

			// Feed the values for every mouse into the system mouse
			if(gDefaults.input.mouseZeroIsSystemMouse)
			{
				if(pRawMice[i].isAbsolute)
				{
					pRawMice[RAW_SYS_MOUSE].xDelta = pRaw->data.mouse.lLastX - pRawMice[i].x;
					pRawMice[RAW_SYS_MOUSE].yDelta = pRaw->data.mouse.lLastY - pRawMice[i].y;
					pRawMice[RAW_SYS_MOUSE].x = pRaw->data.mouse.lLastX;
					pRawMice[RAW_SYS_MOUSE].y = pRaw->data.mouse.lLastY;
				}
				else
				{ // relative
					pRawMice[RAW_SYS_MOUSE].xDelta = pRaw->data.mouse.lLastX;
					pRawMice[RAW_SYS_MOUSE].yDelta = pRaw->data.mouse.lLastY;
					pRawMice[RAW_SYS_MOUSE].x += pRaw->data.mouse.lLastX;
					pRawMice[RAW_SYS_MOUSE].y += pRaw->data.mouse.lLastY;
				}

				// This is innacurate:  If 2 mice have their buttons down and I lift up on one, this will register the
				//   system mouse as being "up".  I checked out on my windows desktop, and Microsoft was just as
				//   lazy as I'm going to be.  Drag an icon with the 2 left mouse buttons held down & let go of one.

				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) pRawMice[0].buttonPressed[0] = 1;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) pRawMice[0].buttonPressed[0] = 0;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) pRawMice[0].buttonPressed[1] = 1;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) pRawMice[0].buttonPressed[1] = 0;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) pRawMice[0].buttonPressed[2] = 1;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) pRawMice[0].buttonPressed[2] = 0;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) pRawMice[0].buttonPressed[3] = 1;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) pRawMice[0].buttonPressed[3] = 0;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) pRawMice[0].buttonPressed[4] = 1;
				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) pRawMice[0].buttonPressed[4] = 0;

				// If an absolute mouse is triggered, sys mouse will be considered absolute till the end of time.
				if(pRaw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)		pRawMice[RAW_SYS_MOUSE].isAbsolute = 1;
				// Same goes for virtual desktop
				if(pRaw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)	pRawMice[RAW_SYS_MOUSE].isVirtualDesktop = 1;

				if(pRaw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
				{      // If the current message has a mouse_wheel message
					if((SHORT)pRaw->data.mouse.usButtonData > 0)
					{
						pRawMice[RAW_SYS_MOUSE].zDelta++;
					}
					if((SHORT)pRaw->data.mouse.usButtonData < 0)
					{
						pRawMice[RAW_SYS_MOUSE].zDelta--;
					}
				}
			}
		}
	}

	return 0;
}

int HandleRawMouseMessage(HANDLE hDevice)
{
	char pBuffer[sizeof(RAWINPUT)];
	int size = sizeof(RAWINPUT);

	if(!pRawMice)
		return 1;

	DBGASSERT(_GRID((HRAWINPUT)hDevice, RID_INPUT, pBuffer, &size, sizeof(RAWINPUTHEADER)) > -1, "Error: Couldn't read RawInput data. RawInput buffer overflow?");
	ReadRawInput((RAWINPUT*)pBuffer);

	return 0;
}

#endif