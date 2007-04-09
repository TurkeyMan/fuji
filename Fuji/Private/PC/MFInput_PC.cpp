#define DIRECTINPUT_VERSION 0x0800
#define _WIN32_WINNT 0x501   // This specifies WinXP or later - it is needed to access rawmouse from the user32.dll

#define SAMPLE_BUFFER_SIZE 128//50000

#define MFWHEEL_DELTA 120

#include "Fuji.h"
#include "MFVector.h"
#include "MFInput_Internal.h"
#include "MFHeap.h"
#include "MFIni.h"
#include "MFSystem.h"

#include "MFInputMappings_PC.h"

#include <dinput.h>
#include <dbt.h>

const GUID GUID_DEVINTERFACE_HID = { 0x4D1E55B2L, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

#if defined(ALLOW_RAW_INPUT)
	#define RAW_SYS_MOUSE 0      // The sys mouse combines all the other usb mice into one
	#define MAX_RAW_MOUSE_BUTTONS 5

	typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceList)(OUT PRAWINPUTDEVICELIST pRawInputDeviceList, IN OUT PINT puiNumDevices, IN UINT cbSize);
	typedef WINUSERAPI INT (WINAPI *pGetRawInputData)(IN HRAWINPUT hRawInput, IN UINT uiCommand, OUT LPVOID pData, IN OUT PINT pcbSize, IN UINT cbSizeHeader);
	typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceInfoA)(IN HANDLE hDevice, IN UINT uiCommand, OUT LPVOID pData, IN OUT PINT pcbSize);
	typedef WINUSERAPI BOOL (WINAPI *pRegisterRawInputDevices)(IN PCRAWINPUTDEVICE pRawInputDevices, IN UINT uiNumDevices, IN UINT cbSize);

	int InitRawMouse(bool _includeRDPMouse);
#endif

#if defined(SUPPORT_XINPUT)
	#include <XInput.h>
	#include <wbemidl.h>
	#include <stdio.h>

	#pragma comment(lib, "Xinput")

	HRESULT IsXInputDevice(const GUID* pGuidProductFromDirectInput);
#endif

void MFInputPC_LoadGamepadMappings();


/*** Structure definitions ***/

#if defined(ALLOW_RAW_INPUT)
struct MFRawMouse
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

struct MFGamepadPC
{
	// XInput
	int XInputID;

	// DirctInput
	IDirectInputDevice8	*pDevice;
	IDirectInputEffect *pForceFeedback;

	DIDEVCAPS caps;
	GUID deviceInstance;

	// Gamepad Info
	MFGamepadInfo *pGamepadInfo;

	int  forceFeedbackState;
	bool bHasPOV;
	bool wasRemoved;
};


/*** Globals ***/

static IDirectInput8 *pDirectInput = NULL;
static IDirectInputDevice8 *pKeyboard = NULL;
static IDirectInputDevice8 *pMouse = NULL;

static MFGamepadPC gPCJoysticks[MFInput_MaxInputID];

static int gGamepadCount = 0;
static int gKeyboardCount = 0;
static int gMouseCount = 0;

static bool gUseXInput = false;

extern HINSTANCE apphInstance;
extern HWND apphWnd;

static char gKeyState[256];

static bool gExclusiveMouse = false;
static float mouseMultiplier = 1.0f;

extern MFGamepadInfo *pGamepadMappingRegistry;

extern HWND apphWnd;
static HDEVNOTIFY hNotify;

static bool gUpdateDeviceList = false;

#if defined(ALLOW_RAW_INPUT)
static pGetRawInputDeviceList _GRIDL;
static pGetRawInputData _GRID;
static pGetRawInputDeviceInfoA _GRIDIA;
static pRegisterRawInputDevices _RRID;

static MFRawMouse *pRawMice = NULL;
static int rawMouseCount;

static BOOL includeRDPMouse;

bool gbAllowRawMouse = true;
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


/**** Platform Specific Functions ****/

BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

BOOL CALLBACK CheckConnectedCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	MFCALLSTACK;

#if defined(SUPPORT_XINPUT)
	// if device is an XInput device, we'll let XInput handle it
	if(gUseXInput && IsXInputDevice(&lpddi->guidProduct))
		return DIENUM_CONTINUE;
#endif

	int a;

	// scan for the gamepad in the current active list...
	for(a=0; a<gGamepadCount; a++)
	{
		if(gPCJoysticks[a].pDevice)
		{
			if(lpddi->guidInstance == gPCJoysticks[a].deviceInstance)
			{
				gPCJoysticks[a].wasRemoved = false;
				return DIENUM_CONTINUE;
			}
		}
	}

	// gamepad was not found in the list, it must have just been connected, so we'll add it in.
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(gPCJoysticks[a].XInputID == -1 && !gPCJoysticks[a].pDevice)
		{
			// we'll slot it in here..
			return EnumJoysticksCallback(lpddi, &a);
		}
	}

	return DIENUM_CONTINUE;
}

void DeviceChange(DEV_BROADCAST_DEVICEINTERFACE *pDevInf, bool connect)
{
	MFCALLSTACK;

	gUpdateDeviceList = true;
}

// DirectInput Enumeration Callback
BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	MFCALLSTACK;

	HRESULT hr;

#if defined(SUPPORT_XINPUT)
	// if device is an XInput device, we'll let XInput handle it
	if(gUseXInput && IsXInputDevice(&lpddi->guidProduct))
		return DIENUM_CONTINUE;
#endif

	int gamepadID = gGamepadCount;

	if(pvRef)
		gamepadID = *(int*)pvRef;

	// attempt to create device
	hr = pDirectInput->CreateDevice(lpddi->guidInstance, &gPCJoysticks[gamepadID].pDevice, NULL);

	if(FAILED(hr))
	{
		MFDebug_Warn(1, "Failed to create gamepad device.");
		return DIENUM_CONTINUE;
	}

	// we found a valid gamepad
	gPCJoysticks[gamepadID].XInputID = -1;
	gPCJoysticks[gamepadID].wasRemoved = false;

	gPCJoysticks[gamepadID].deviceInstance = lpddi->guidInstance;

	// get the device caps
	MFZeroMemory(&gPCJoysticks[gamepadID].caps, sizeof(DIDEVCAPS));
	gPCJoysticks[gamepadID].caps.dwSize = sizeof(DIDEVCAPS);

	gPCJoysticks[gamepadID].pDevice->GetCapabilities(&gPCJoysticks[gamepadID].caps);

	// find matching device descriptor
	MFGamepadInfo *pInfo = pGamepadMappingRegistry;
	for(; pInfo; pInfo = pInfo->pNext)
	{
		if(pInfo->vendorID && pInfo->productID)
		{
			if((lpddi->guidProduct.Data1 & 0xFFFF) == pInfo->vendorID && (lpddi->guidProduct.Data1 >> 16) == pInfo->productID)
				break;
		}
		else
		{
			if(!MFString_Compare(pInfo->pIdentifier, lpddi->tszProductName))
				break;
		}
	}

	if(!pInfo)
	{
		// use default descriptor
		gPCJoysticks[gamepadID].pGamepadInfo = pGamepadMappingRegistry;
		MFDebug_Warn(1, MFStr("Found an unknown gamepad '%s', using default mappings.", lpddi->tszProductName));

		// offer to send email detailing controller info..
//		MessageBox(NULL, "An unknown gamepad has been detected.\r\nWe strive to support every gamepad natively, please report your gamepad to Manu at turkeyman@gmail.com.\r\nI will contact you and request a few details about the gamepad so it can be added to the registry for the next release.", "Unknown gamepad detected...", MB_OK);
	}
	else
	{
		// use applicable descriptor
		gPCJoysticks[gamepadID].pGamepadInfo = pInfo;
		MFDebug_Log(2, MFStr("Found gamepad: %s '%s'.", pInfo->pName, pInfo->pIdentifier));
	}

	// test if device uses a POV for the digital directions
	if(gPCJoysticks[gamepadID].caps.dwPOVs)
		gPCJoysticks[gamepadID].bHasPOV = true;
	else
		gPCJoysticks[gamepadID].bHasPOV = false;

	// initialise the device
	hr = gPCJoysticks[gamepadID].pDevice->SetDataFormat(&c_dfDIJoystick2);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to set gamepad data format.");

	// set device cooperative level
	hr = gPCJoysticks[gamepadID].pDevice->SetCooperativeLevel(apphWnd, DISCL_FOREGROUND|DISCL_EXCLUSIVE);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to set gamepad cooperative level.");

	// check for force feedback availability
	if(gPCJoysticks[gamepadID].caps.dwFlags & DIDC_FORCEFEEDBACK)
	{
		// This application needs only one effect: Applying raw forces.
		DWORD           rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };
		LONG            rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf              = { 0 };

		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize                  = sizeof(DIEFFECT);
		eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.dwDuration              = INFINITE;
		eff.dwSamplePeriod          = 0;
		eff.dwGain                  = DI_FFNOMINALMAX;
		eff.dwTriggerButton         = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes                   = 2;
		eff.rgdwAxes                = rgdwAxes;
		eff.rglDirection            = rglDirection;
		eff.lpEnvelope              = 0;
		eff.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams   = &cf;
		eff.dwStartDelay            = 0;

		hr = gPCJoysticks[gamepadID].pDevice->CreateEffect(GUID_ConstantForce, &eff, &gPCJoysticks[gamepadID].pForceFeedback, NULL);

		if(FAILED(hr))
		{
			MFDebug_Warn(1, MFStr("Gamepad claims to support force feedback for device '%s', but DirectInput failed to create the effect.", gPCJoysticks[gamepadID].pGamepadInfo->pIdentifier));
		}
	}

	// attempt to acquire the device
	gPCJoysticks[gamepadID].pDevice->Acquire();

	if(gamepadID == gGamepadCount)
		gGamepadCount++;

	if(gGamepadCount >= MFInput_MaxInputID)
		return DIENUM_STOP;

	return DIENUM_CONTINUE;
}

void MFInput_InitModulePlatformSpecific()
{
	MFCALLSTACK;

	// initialise runtime data
	ZeroMemory(gKeyState,256);
	ZeroMemory(gPCJoysticks, sizeof(gPCJoysticks));

	for(int i=0; i<MFInput_MaxInputID; i++)
		gPCJoysticks[i].XInputID = -1;

	// load additional gamepad mappings...
	MFInputPC_LoadGamepadMappings();

	// create the direct inpur device
	if(FAILED(DirectInput8Create(apphInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDirectInput, NULL))) return;

	// for setting the sample buffer
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = SAMPLE_BUFFER_SIZE;

	// create keyboard device
	if(SUCCEEDED(pDirectInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL)))
	{
		pKeyboard->SetDataFormat(&c_dfDIKeyboard);
		pKeyboard->SetCooperativeLevel(apphWnd, DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE);
		pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
		pKeyboard->Acquire();
	}
	gKeyboardCount = 1;

	// create mouse device/s
#if defined(ALLOW_RAW_INPUT)
	// check we're running Windows XP

	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL r = GetVersionEx(&ver);
	gbAllowRawMouse = ver.dwMajorVersion >= 5 && ver.dwMinorVersion >= 1;

	if(gbAllowRawMouse && gDefaults.input.allowMultipleMice)
	{
		InitRawMouse(false);

		if(rawMouseCount > 1)
			gMouseCount = rawMouseCount;
	}
#endif
#if defined(ALLOW_DI_MOUSE)
	if(gMouseCount == 0)
	{
		if(SUCCEEDED(pDirectInput->CreateDevice(GUID_SysMouse, &pMouse, NULL)))
		{
			pMouse->SetDataFormat(&c_dfDIMouse2);
			pMouse->SetCooperativeLevel(apphWnd, DISCL_FOREGROUND | (gExclusiveMouse ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE));
			pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
			pMouse->Acquire();
		}
		gMouseCount = 1;
	}
#endif

#if defined(SUPPORT_XINPUT)
	// enumerate XInput devices
	XINPUT_STATE state;
	for(int a=0; a<4; a++)
	{
		if(XInputGetState(a, &state) == ERROR_SUCCESS)
		{
			// we have an xinput controller, reserve 4 gamepad slots for hotswapping
			gPCJoysticks[0].XInputID = 0;
			gPCJoysticks[1].XInputID = 1;
			gPCJoysticks[2].XInputID = 2;
			gPCJoysticks[3].XInputID = 3;

			gPCJoysticks[0].pGamepadInfo = &pGamepadMappingRegistry[1];
			gPCJoysticks[1].pGamepadInfo = &pGamepadMappingRegistry[1];
			gPCJoysticks[2].pGamepadInfo = &pGamepadMappingRegistry[1];
			gPCJoysticks[3].pGamepadInfo = &pGamepadMappingRegistry[1];

			gGamepadCount += 4;
			gUseXInput = true;

			break;
		}
	}
#endif

	// enumerate gamepads
	pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);

	DEV_BROADCAST_DEVICEINTERFACE dev;
	MFZeroMemory(&dev, sizeof(dev));
	dev.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	dev.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	dev.dbcc_classguid = GUID_DEVINTERFACE_HID;
	hNotify = RegisterDeviceNotification(apphWnd, &dev, DEVICE_NOTIFY_WINDOW_HANDLE);
}

void MFInput_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	int a;

	UnregisterDeviceNotification(hNotify);

	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(gPCJoysticks[a].pDevice)
		{
			gPCJoysticks[a].pDevice->Unacquire();
			gPCJoysticks[a].pDevice->Release();
			gPCJoysticks[a].pDevice = NULL;
		}
	}
	gGamepadCount = 0;

#if defined(ALLOW_DI_MOUSE)
	if(pMouse)
	{
		pMouse->Unacquire();
		pMouse->Release();
		pMouse = NULL;
	}
	gMouseCount = 0;
#endif

	if(pKeyboard)
	{
		pKeyboard->Unacquire();
		pKeyboard->Release();
		pKeyboard = NULL;
	}
	gKeyboardCount = 0;

	if(pDirectInput)
	{
		pDirectInput->Release();
		pDirectInput = NULL;
	}
}

void MFInput_UpdatePlatformSpecific()
{
	MFCALLSTACK;

	if(gUpdateDeviceList)
	{
		int a;

		// mark devices as removed
		for(a=0; a<gGamepadCount; a++)
		{
			if(gPCJoysticks[a].pDevice)
			{
				gPCJoysticks[a].wasRemoved = true;
			}
		}

		// enumerate all devices and scan for removals..
		pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, CheckConnectedCallback, NULL, DIEDFL_ATTACHEDONLY);

		// if any gamepads are still marked with wasRemoved==true, they must have been removed, so we'll clean them up.
		for(a=0; a<MFInput_MaxInputID; a++)
		{
			if(gPCJoysticks[a].pDevice && gPCJoysticks[a].wasRemoved)
			{
				// remove gamepad...
				if(gPCJoysticks[a].pForceFeedback)
				{
					gPCJoysticks[a].pForceFeedback->Release();
					gPCJoysticks[a].pForceFeedback = NULL;
				}

				gPCJoysticks[a].pDevice->Release();
				gPCJoysticks[a].pDevice = NULL;
			}
		}

		gUpdateDeviceList = false;
	}
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	MFCALLSTACK;

	switch(device)
	{
		case IDD_Gamepad:
			if(id < gGamepadCount)
			{
#if defined(SUPPORT_XINPUT)
				if(gPCJoysticks[id].XInputID > -1)
				{
					XINPUT_STATE state;
					DWORD attached = XInputGetState(gPCJoysticks[id].XInputID, &state);

					if(attached == ERROR_SUCCESS)
						return IDS_Ready;
					else if(attached == ERROR_DEVICE_NOT_CONNECTED)
						return IDS_Disconnected;
					else
						MFDebug_Assert(false, "Error reading XInput device state.");
				}
				else
#endif
				{
					if(gPCJoysticks[id].pDevice)
					{
						DIDEVCAPS caps;
						MFZeroMemory(&caps, sizeof(DIDEVCAPS));
						caps.dwSize = sizeof(DIDEVCAPS);

						gPCJoysticks[id].pDevice->GetCapabilities(&caps);

						if(caps.dwFlags & DIDC_ATTACHED)
							return IDS_Ready;
						else
							return IDS_Disconnected;
					}
					else
						return IDS_Disconnected;
				}
			}
			break;

		case IDD_Mouse:
			if(id < gMouseCount)
			{
				return IDS_Ready;
			}
			break;

		case IDD_Keyboard:
			if(id < gKeyboardCount)
			{
				return IDS_Ready;
			}
			break;

		default:
			MFDebug_Assert(false, "Invalid Input Device");
			break;
	}

	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	MFCALLSTACK;

	HRESULT hr;

	MFZeroMemory(pGamepadState, sizeof(*pGamepadState));

#if defined(SUPPORT_XINPUT)
	if(gPCJoysticks[id].XInputID > -1)
	{
		XINPUT_STATE state;
		DWORD attached = XInputGetState(gPCJoysticks[id].XInputID, &state);

		if(attached == ERROR_SUCCESS)
		{
			pGamepadState->values[Button_X3_A] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_B] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_X] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_Y] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_LB] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_RB] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_Start] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_Back] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_LThumb] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 1.0f : 0.0f;
			pGamepadState->values[Button_X3_RThumb] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 1.0f : 0.0f;

			pGamepadState->values[Button_DUp] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1.0f : 0.0f;
			pGamepadState->values[Button_DDown] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1.0f : 0.0f;
			pGamepadState->values[Button_DLeft] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1.0f : 0.0f;
			pGamepadState->values[Button_DRight] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1.0f : 0.0f;

			pGamepadState->values[Button_X3_LT] = (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? (float)state.Gamepad.bLeftTrigger * (1.0f / 255.0f) : 0.0f;
			pGamepadState->values[Button_X3_RT] = (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? (float)state.Gamepad.bRightTrigger * (1.0f / 255.0f) : 0.0f;

			pGamepadState->values[Axis_LX] = MFClamp(-1.0f, (float)state.Gamepad.sThumbLX * (1.0f / 32767.0f), 1.0f);
			pGamepadState->values[Axis_LY] = MFClamp(-1.0f, (float)state.Gamepad.sThumbLY * (1.0f / 32767.0f), 1.0f);
			pGamepadState->values[Axis_RX] = MFClamp(-1.0f, (float)state.Gamepad.sThumbRX * (1.0f / 32767.0f), 1.0f);
			pGamepadState->values[Axis_RY] = MFClamp(-1.0f, (float)state.Gamepad.sThumbRY * (1.0f / 32767.0f), 1.0f);
		}
	}
	else
#endif
	{
		if(gPCJoysticks[id].pDevice)
		{
			// poll the gamepad
			hr = gPCJoysticks[id].pDevice->Poll();

			if(FAILED(hr))
			{
				// attempt to recover the device
				hr = gPCJoysticks[id].pDevice->Acquire();

				if(SUCCEEDED(hr))
				{
					if(gPCJoysticks[id].pForceFeedback && gPCJoysticks[id].forceFeedbackState)
					{
						// restart the vibration effect
						gPCJoysticks[id].pForceFeedback->Start(1, 0);
					}
				}
			}

			// read gamepad
			if(SUCCEEDED(hr))
			{
				DIJOYSTATE2 joyState;

				// get device state
				hr = gPCJoysticks[id].pDevice->GetDeviceState(sizeof(DIJOYSTATE2), &joyState);

				if(SUCCEEDED(hr))
				{
					const int *pButtonMap = gPCJoysticks[id].pGamepadInfo->pButtonMap;
					LONG *pAxii = (LONG*)&joyState;

					// read pov
					DWORD pov = gPCJoysticks[id].bHasPOV ? joyState.rgdwPOV[0] : 0xFFFF;
					bool POVCentered = LOWORD(pov) == 0xFFFF;

					// convert input to float data
					for(int a=0; a<GamepadType_Max; a++)
					{
						if(pButtonMap[a] == -1)
							continue;

						int axisID = MFGETAXIS(pButtonMap[a]);
						bool readAnalog = false;

						// test if analog input is present
						if(pButtonMap[a] & AID_Analog)
						{
							DIDEVICEOBJECTINSTANCE axisInfo;
							axisInfo.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

							hr = gPCJoysticks[id].pDevice->GetObjectInfo(&axisInfo, axisID<<2, DIPH_BYOFFSET);

							if(SUCCEEDED(hr))
								readAnalog = true;
						}

						// if we are not reading the analog axis
						if(!readAnalog)
						{
							if(axisID >= 60 && gPCJoysticks[id].bHasPOV)
							{
								if(POVCentered)
								{
									// POV is centered
									pGamepadState->values[a] = 0.0f;
								}
								else
								{
									// read POV (or more appropriately titled, POS)
									if(axisID == 60)
										pGamepadState->values[Button_DUp] = ((pov >= 31500 && pov <= 36000) || (pov >= 0 && pov <= 4500)) ? 1.0f : 0.0f;
									else if(axisID == 61)
										pGamepadState->values[Button_DDown] = (pov >= 13500 && pov <= 22500) ? 1.0f : 0.0f;
									else if(axisID == 62)
										pGamepadState->values[Button_DLeft] = (pov >= 22500 && pov <= 31500) ? 1.0f : 0.0f;
									else if(axisID == 63)
										pGamepadState->values[Button_DRight] = (pov >= 4500 && pov <= 13500) ? 1.0f : 0.0f;
								}
							}
							else
							{
								// read digital button
								pGamepadState->values[a] = (joyState.rgbButtons[pButtonMap[a] & AID_ButtonMask] & 0x80) ? 1.0f : 0.0f;
							}
						}
						else
						{
							// read an analog axis
							pGamepadState->values[a] = MFMin(pAxii[axisID] * (1.0f/32767.0f) - 1.0f, 1.0f);
						}

						// invert any buttons with the AID_Negative flag
						if(pButtonMap[a] & AID_Negative)
							pGamepadState->values[a] = -pGamepadState->values[a];
						// clamp any butons with the AID_Clamp flag to the positive range
						if(pButtonMap[a] & AID_Clamp)
							pGamepadState->values[a] = MFMax(0.0f, pGamepadState->values[a]);
						// use the full range for any buttons with the AID_Full flag
						if(pButtonMap[a] & AID_Full)
							pGamepadState->values[a] = (pGamepadState->values[a] + 1.0f) * 0.5f;
					}
				}
			}
		}
	}
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFCALLSTACK;

	DIDEVICEOBJECTDATA inputBuffer[SAMPLE_BUFFER_SIZE];
	DWORD elements = SAMPLE_BUFFER_SIZE;

	HRESULT hr;

	hr = pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), inputBuffer, &elements, 0 );

	if(hr != DI_OK)
	{
		pKeyboard->Acquire();
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

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFCALLSTACK;

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

		hr = pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), inputBuffer, &elements, 0 );

		if(hr != DI_OK)
		{
			pMouse->Acquire();
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
		if(gbAllowRawMouse && gDefaults.input.allowMultipleMice)
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

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	const char *pText = NULL;

	switch(source)
	{
		case IDD_Gamepad:
		{
			pText = gPCJoysticks[sourceID].pGamepadInfo->pName;
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

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	return gPCJoysticks[sourceID].pGamepadInfo->ppButtonNameStrings[button];
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
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
void MFInputPC_GetDIJoyStateInternal(int id, DIJOYSTATE2 *pJoyState)
{
	gPCJoysticks[id].pDevice->GetDeviceState(sizeof(DIJOYSTATE2), pJoyState);
}

IDirectInputDevice8 *MFInputPC_GetDIGamepadDevice(int id)
{
	return gPCJoysticks[id].pDevice;
}

void MFInputPC_Acquire(bool acquire)
{
	MFCALLSTACK;

	int a;

	if(pKeyboard)
	{
		if(acquire) pKeyboard->Acquire();
		else pKeyboard->Unacquire();
	}

	if(pMouse)
	{
		if(acquire) pMouse->Acquire();
		else pMouse->Unacquire();
	}

	for(a=0; a<gGamepadCount; a++)
	{
		if(gPCJoysticks[a].pDevice)
		{
			if(acquire)
			{
				if(FAILED(gPCJoysticks[a].pDevice->Poll()))
				{
					gPCJoysticks[a].pDevice->Acquire();
				}
			}
			else gPCJoysticks[a].pDevice->Unacquire();
		}
	}
}

void MFInputPC_LoadGamepadMappings()
{
/*
	// load GamepadMappings.ini
	MFGamepadInfo *pGI = NULL;
	MFIni *pIni;

	// read GameMappings.ini file
	pIni = MFIni::Create("GamepadMappings.ini");

	if(pIni)
	{
		MFIniLine *pLine;
		pLine = pIni->GetFirstLine();

		while(pLine)
		{
			if(pLine->IsString(0, "Gamepad"))
			{
				const char *pName = pLine->GetString(1);
				pGI = (MFGamepadInfo*)MFHeap_Alloc(sizeof(MFGamepadInfo) + MFString_Length(pName) + 1);
				pGI->bUsePOV = true;
				pGI->pName = (char*)&pGI[1];
				pGI->ppButtonNameStrings = DefaultButtons;
				MFString_Copy(pGI->pName, pName);
				pGI->pNext = pGamepadMappingRegistry;
				pGamepadMappingRegistry = pGI;

				MFIniLine *pPadLine = pLine->Sub();
				while (pPadLine)
				{
					const char *pField = pPadLine->GetString(0);
					if (!MFString_CaseCmp(pField, "Axis_LX"))
					{
						pGI->axisMapping[0] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Axis_LY"))
					{
						pGI->axisMapping[1] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Axis_RX"))
					{
						pGI->axisMapping[2] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Axis_RY"))
					{
						pGI->axisMapping[3] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_Cross"))
					{
						pGI->buttonMapping[0] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_Circle"))
					{
						pGI->buttonMapping[1] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_Box"))
					{
						pGI->buttonMapping[2] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_Triangle"))
					{
						pGI->buttonMapping[3] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_R1"))
					{
						pGI->buttonMapping[4] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_L1"))
					{
						pGI->buttonMapping[5] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_L2"))
					{
						pGI->buttonMapping[6] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_R2"))
					{
						pGI->buttonMapping[7] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_Start"))
					{
						pGI->buttonMapping[8] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_Select"))
					{
						pGI->buttonMapping[9] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_LThumb"))
					{
						pGI->buttonMapping[10] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_P2_RThumb"))
					{
						pGI->buttonMapping[11] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_DUp"))
					{
						pGI->buttonMapping[12] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_DDown"))
					{
						pGI->buttonMapping[13] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_DLeft"))
					{
						pGI->buttonMapping[14] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "Button_DRight"))
					{
						pGI->buttonMapping[15] = pPadLine->GetInt(1);
					}
					else if(!MFString_CaseCmp(pField, "IgnorePOV"))
					{
						pGI->usePOV = false;
					}
					else if(!MFString_CaseCmp(pField, "ButtonStrings"))
					{
						const char *pStrings = pPadLine->GetString(1);
						if(!MFString_CaseCmp(pStrings, "XBox"))
						{
							pGI->ppButtonNameStrings = XBoxButtons;
						}
						else if(!MFString_CaseCmp(pStrings, "Playstation"))
						{
							pGI->ppButtonNameStrings = PS2Buttons;
						}
						else
						{
							MFDebug_Warn(2, MFStr("Gamepad button strings '%s' unavailable for gamepad type '%s'.", pStrings, pGI->pName));
						}
					}
					else
					{
						MFDebug_Warn(3, MFStr("Unknown controller property '%s' in gamepad '%s'.", pName, pGI->pName));
					}

					pPadLine = pPadLine->Next();
				}
			}

			pLine = pLine->Next();
		}

		MFIni::Destroy(pIni);
	}
*/
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
	MFDebug_Assert(_RRID(rid, sizeof(rid)/sizeof(rid[0]), sizeof(rid[0])), "Error: RegisterRawInputDevices() failed");

	return 0;
}

bool IsRDPMouse(const char *pName)
{
	return !MFString_CompareN(pName, "\\??\\Root#RDP_MOU#0000#", 22);
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
		MFDebug_Warn(1, "Cant open user32.dll");
		return 1;
	}

	_RRID = (pRegisterRawInputDevices)GetProcAddress(user32,"RegisterRawInputDevices");
	if (!_RRID)
	{
		MFDebug_Warn(1, "Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	_GRIDL = (pGetRawInputDeviceList)GetProcAddress(user32,"GetRawInputDeviceList");
	if (!_GRIDL)
	{
		MFDebug_Warn(1, "Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	_GRIDIA = (pGetRawInputDeviceInfoA)GetProcAddress(user32,"GetRawInputDeviceInfoA");
	if (!_GRIDIA)
	{
		MFDebug_Warn(1, "Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	_GRID = (pGetRawInputData)GetProcAddress(user32,"GetRawInputData");
	if (!_GRID)
	{
		MFDebug_Warn(1, "Failed to initialise RawInput. Addition mouse devices will not be available.");
		return 1;
	}

	rawMouseCount = 0;
	includeRDPMouse = _includeRDPMouse;

	// 1st call to GetRawInputDeviceList: Pass NULL to get the number of devices.
	if(_GRIDL(NULL, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
	{
		MFDebug_Assert(false, "Error: Unable to count raw input devices.");
		return 1;
	}

	// Allocate the array to hold the DeviceList
	pRawInputDeviceList = (RAWINPUTDEVICELIST*)malloc(sizeof(RAWINPUTDEVICELIST) * nInputDevices);

	// 2nd call to GetRawInputDeviceList: Pass the pointer to our DeviceList and GetRawInputDeviceList() will fill the array
	if(_GRIDL(pRawInputDeviceList, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) == -1)
	{
		MFDebug_Assert(false, "Error: Unable to get raw input device list.");
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
				MFDebug_Assert(false, "Error: Unable to get raw input device name.");
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
	pRawMice = (MFRawMouse*)malloc(sizeof(MFRawMouse) * rawMouseCount);
	ZeroMemory(pRawMice, sizeof(MFRawMouse) * rawMouseCount);

	// Loop through all devices and set the device handles and initialize the mouse values
	for(i = 0; i < nInputDevices; i++)
	{
		if(pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			nSize = 256;
			if((int)_GRIDIA(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, pName, &nSize) < 0)
			{
				MFDebug_Assert(false, "Error: Unable to get raw input device name.");
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
			MFString_Copy(pRawMice[i].buttonNames[j], MFStr("Button %i", j));
		}
	}

	// finally, register to recieve raw input WM_INPUT messages
	MFDebug_Assert(RegisterRawMouse() == 0, "Error: Unable to register raw input.");

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

	MFDebug_Assert(_GRID((HRAWINPUT)hDevice, RID_INPUT, pBuffer, &size, sizeof(RAWINPUTHEADER)) > -1, "Error: Couldn't read RawInput data. RawInput buffer overflow?");
	ReadRawInput((RAWINPUT*)pBuffer);

	return 0;
}

#endif

#if defined(SUPPORT_XINPUT)
//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput
//-----------------------------------------------------------------------------
HRESULT IsXInputDevice( const GUID* pGuidProductFromDirectInput )
{
    IWbemLocator*           pIWbemLocator  = NULL;
    IEnumWbemClassObject*   pEnumDevices   = NULL;
    IWbemClassObject*       pDevices[20]   = {0};
    IWbemServices*          pIWbemServices = NULL;
    BSTR                    bstrNamespace  = NULL;
    BSTR                    bstrDeviceID   = NULL;
    BSTR                    bstrClassName  = NULL;
    DWORD                   uReturned      = 0;
    bool                    bIsXinputDevice= false;
    UINT                    iDevice        = 0;
    VARIANT                 var;
    HRESULT                 hr;

    // CoInit if needed
    hr = CoInitialize(NULL);
    bool bCleanupCOM = SUCCEEDED(hr);

    // Create WMI
    hr = CoCreateInstance( __uuidof(WbemLocator),
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           __uuidof(IWbemLocator),
                           (LPVOID*) &pIWbemLocator);
    if( FAILED(hr) || pIWbemLocator == NULL )
        goto LCleanup;

    bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );if( bstrNamespace == NULL ) goto LCleanup;
    bstrClassName = SysAllocString( L"Win32_PNPEntity" );   if( bstrClassName == NULL ) goto LCleanup;
    bstrDeviceID  = SysAllocString( L"DeviceID" );          if( bstrDeviceID == NULL )  goto LCleanup;

    // Connect to WMI
    hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L,
                                       0L, NULL, NULL, &pIWbemServices );

	MFDebug_Assert(SUCCEEDED(hr) && pIWbemServices, "Failed to connect to WMI server.");
    if( FAILED(hr) || pIWbemServices == NULL )
        goto LCleanup;

    // Switch security level to IMPERSONATE.
    CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

    hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices );
    if( FAILED(hr) || pEnumDevices == NULL )
        goto LCleanup;

    // Loop over all devices
    for( ;; )
    {
        // Get 20 at a time
        hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
        if( FAILED(hr) )
            goto LCleanup;
        if( uReturned == 0 )
            break;

        for( iDevice=0; iDevice<uReturned; iDevice++ )
        {
            // For each device, get its device ID
            hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
            if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
            {
                // Check if the device ID contains "IG_".  If it does, then it's an XInput device
				    // This information can not be found from DirectInput
                if( wcsstr( var.bstrVal, L"IG_" ) )
                {
                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0;
                    WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
                    if( strVid && swscanf( strVid, L"VID_%4X", &dwVid ) != 1 )
                        dwVid = 0;
                    WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                    if( strPid && swscanf( strPid, L"PID_%4X", &dwPid ) != 1 )
                        dwPid = 0;

                    // Compare the VID/PID to the DInput device
                    DWORD dwVidPid = MAKELONG( dwVid, dwPid );
                    if( dwVidPid == pGuidProductFromDirectInput->Data1 )
                    {
                        bIsXinputDevice = true;
                        goto LCleanup;
                    }
                }
            }
			if(pDevices[iDevice])
			{
				pDevices[iDevice]->Release();
				pDevices[iDevice] = NULL;
			}
        }
    }

LCleanup:
    if(bstrNamespace)
        SysFreeString(bstrNamespace);
    if(bstrDeviceID)
        SysFreeString(bstrDeviceID);
    if(bstrClassName)
        SysFreeString(bstrClassName);
    for( iDevice=0; iDevice<20; iDevice++ )
	{
		if(pDevices[iDevice])
		{
			pDevices[iDevice]->Release();
			pDevices[iDevice] = NULL;
		}
	}
	if(pEnumDevices)
		pEnumDevices->Release();
	if(pIWbemLocator)
		pIWbemLocator->Release();
	if(pIWbemServices)
		pIWbemServices->Release();

    if( bCleanupCOM )
        CoUninitialize();

    return bIsXinputDevice;
}

#endif
