#define SAMPLE_BUFFER_SIZE 128//50000

#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>

#include "Common.h"
#include "Input.h"
#include "Vector3.h"
#include "Input_PC.h"

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdpDirectInputstance, VOID* pContext);

void Input_SetCooperativeLevels();

void Input_UpdateKeyboard();
void Input_UpdateMouse();
void Input_UpdateJoystick();

/*** Globals ***/

IDirectInput8		*pDirectInput	 = NULL;
IDirectInputDevice8	*pDIKeyboard	 = NULL;
IDirectInputDevice8	*pDIMouse		 = NULL;
IDirectInputDevice8	*pDIJoystick[16] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

int	gKeyboardCount	= 0;
int	gJoystickCount	= 0;
int	gMouseCount		= 0;

extern HINSTANCE apphInstance;
extern HWND apphWnd;

char			gKeyState[16][256];
char			gPrevKeyState[16][256];
DIMOUSESTATE2	gMouseState[16];
DIMOUSESTATE2	gPrevMouseState[16];
DIJOYSTATE2		gJoyState[16];
DIJOYSTATE2		gPrevJoyState[16];

EventFunc pKeyEventFunc		= NULL;
EventFunc pMouseEventFunc	= NULL;
EventFunc pJoyEventFunc		= NULL;

bool gExclusiveMouse = false;
float deadZone = 0.3f;

float meScreenMouseX = 0.0f, meScreenMouseY = 0.0f;
float meScreenMouseRangeX = 639.0f, meScreenMouseRangeY = 479.0f;
float meScreenMouseWheel = 0.0f;
char meScreenMouseKey[5] = { 0, 0, 0, 0, 0 };

const long joyaxii[24] = {0,1,2,3,4,5,44,45,46,47,48,49,52,53,54,55,56,57,60,61,62,63,64,65};
const long joysliders[4] = {6,50,58,66};

char DIKtoCHAR[256] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'=',0,0,0,'@',':','_',0,0,0,0,0,0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,',',0,'/',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void Input_InitModule()
{
	CALLSTACK;

	int a;

	if(FAILED(DirectInput8Create(apphInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDirectInput, NULL))) return;

	ZeroMemory(gKeyState,256*16);
	ZeroMemory(gPrevKeyState,256*16);
	ZeroMemory(&gMouseState,sizeof(DIMOUSESTATE2)*16);
	ZeroMemory(&gPrevMouseState,sizeof(DIMOUSESTATE2)*16);
	ZeroMemory(&gJoyState,sizeof(DIJOYSTATE2)*16);
	ZeroMemory(&gPrevJoyState,sizeof(DIJOYSTATE2)*16);

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

	for(a=0; a<gJoystickCount; a++)
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

void Input_DeinitModule()
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
	gJoystickCount=0;

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

void Input_Update()
{
	CALLSTACK;

	Input_UpdateKeyboard();
	Input_UpdateMouse();
	Input_UpdateJoystick();
/*
	if(gMouseState[0].lX)
	{
		meScreenMouseX+=gMouseState[0].lX*gMouseStateSensitivity;
		if(meScreenMouseX<0.0f) meScreenMouseX=0.0f;
		if(meScreenMouseX>meScreenMouseRangeX) meScreenMouseX=meScreenMouseRangeX;
	}

	if(gMouseState[0].lY)
	{
		meScreenMouseY+=gMouseState[0].lY*gMouseStateSensitivity;
		if(meScreenMouseY<0.0f) meScreenMouseY=0.0f;
		if(meScreenMouseY>meScreenMouseRangeY) meScreenMouseY=meScreenMouseRangeY;
	}

	meScreenMouseWheel=(float)gMouseState[0].lZ;

	for(int a=0; a<5; a++) meScreenMouseKey[a]=gMouseState[0].rgbButtons[a];
*/
}

float Input_ReadGamepad(int controlID, uint32 type)
{
	CALLSTACK;

	float inputValue;

	if(!Input_IsConnected(controlID)) return 0.0f;

	switch(type)
	{
		case Button_P2_Cross:
			inputValue = (gJoyState[controlID].rgbButtons[2]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_Circle:
			inputValue = (gJoyState[controlID].rgbButtons[1]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_Box:
			inputValue = (gJoyState[controlID].rgbButtons[3]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_Triangle:
			inputValue = (gJoyState[controlID].rgbButtons[0]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_R1:
			inputValue = (gJoyState[controlID].rgbButtons[7]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_L1:
			inputValue = (gJoyState[controlID].rgbButtons[6]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_L2:
			inputValue = (gJoyState[controlID].rgbButtons[4]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_R2:
			inputValue = (gJoyState[controlID].rgbButtons[5]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_Start:
			inputValue = (gJoyState[controlID].rgbButtons[9]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_Select:
			inputValue = (gJoyState[controlID].rgbButtons[8]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_LThumb:
			inputValue = (gJoyState[controlID].rgbButtons[10]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_P2_RThumb:
			inputValue = (gJoyState[controlID].rgbButtons[11]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_DUp:
			inputValue = (gJoyState[controlID].rgbButtons[12]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_DDown:
			inputValue = (gJoyState[controlID].rgbButtons[14]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_DLeft:
			inputValue = (gJoyState[controlID].rgbButtons[15]&0x80) ? 1.0f : 0.0f;
			break;
		case Button_DRight:
			inputValue = (gJoyState[controlID].rgbButtons[13]&0x80) ? 1.0f : 0.0f;
			break;

		case Axis_LX:
			inputValue = ((float)(gJoyState[controlID].lX - 32767)) / 32767.0f;
			inputValue = (abs(inputValue) > deadZone) ? inputValue : 0.0f;
			break;

		case Axis_LY:
			inputValue = -(((float)(gJoyState[controlID].lY - 32767)) / 32767.0f);
			inputValue = (abs(inputValue) > deadZone) ? inputValue : 0.0f;
			break;

		case Axis_RX:
			inputValue = ((float)(gJoyState[controlID].lRz - 32767)) / 32767.0f;
			inputValue = (abs(inputValue) > deadZone) ? inputValue : 0.0f;
			break;

		case Axis_RY:
			inputValue = -(((float)(gJoyState[controlID].lZ - 32767)) / 32767.0f);
			inputValue = (abs(inputValue) > deadZone) ? inputValue : 0.0f;
			break;

		default:
			dprintf("Error: Undefined Control Pad Input Type\n\n");
	}

	if(controlID <= 0 && inputValue == 0.0f && Input_GetKeyboardStatusState(KSS_ScrollLock))
	{
		switch(type)
		{
			case Button_P2_Cross:
				inputValue = Input_ReadKeyboard(DIK_H);
				break;
			case Button_P2_Circle:
				inputValue = Input_ReadKeyboard(DIK_J);
				break;
			case Button_P2_Box:
				inputValue = Input_ReadKeyboard(DIK_G);
				break;
			case Button_P2_Triangle:
				inputValue = Input_ReadKeyboard(DIK_Y);
				break;
			case Button_P2_R1:
				inputValue = Input_ReadKeyboard(DIK_K);
				break;
			case Button_P2_L1:
				inputValue = Input_ReadKeyboard(DIK_F);
				break;
			case Button_P2_L2:
				inputValue = Input_ReadKeyboard(DIK_D);
				break;
			case Button_P2_R2:
				inputValue = Input_ReadKeyboard(DIK_L);
				break;
			case Button_P2_Start:
				inputValue = Input_ReadKeyboard(DIK_SPACE);
				break;
			case Button_P2_Select:
				inputValue = Input_ReadKeyboard(DIK_RALT);
				break;
			case Button_P2_LThumb:
				inputValue = Input_ReadKeyboard(DIK_T);
				break;
			case Button_P2_RThumb:
				inputValue = Input_ReadKeyboard(DIK_U);
				break;
			case Button_DUp:
				inputValue = Input_ReadKeyboard(DIK_UP) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;
			case Button_DDown:
				inputValue = Input_ReadKeyboard(DIK_DOWN) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;
			case Button_DLeft:
				inputValue = Input_ReadKeyboard(DIK_LEFT) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;
			case Button_DRight:
				inputValue = Input_ReadKeyboard(DIK_RIGHT) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;

			case Axis_LX:
				inputValue = Input_GetKeyboardStatusState(KSS_CapsLock) ? 
					(Input_ReadKeyboard(DIK_LEFT) ? -1.0f : 0.0f) +
					(Input_ReadKeyboard(DIK_RIGHT) ? 1.0f : 0.0f)
					: 0.0f;
				break;

			case Axis_LY:
				inputValue = Input_GetKeyboardStatusState(KSS_CapsLock) ? 
					(Input_ReadKeyboard(DIK_DOWN) ? -1.0f : 0.0f) +
					(Input_ReadKeyboard(DIK_UP) ? 1.0f : 0.0f)
					: 0.0f;
				break;

			case Axis_RX:
				break;

			case Axis_RY:
				break;
		}
	}

	return inputValue;
}

bool Input_WasPressed(int controlID, uint32 type)
{
	CALLSTACK;

	if(!Input_IsConnected(controlID)) return 0.0f;

	bool inputValue = false;

	switch(type)
	{
		case Button_P2_Cross:
			inputValue = gJoyState[controlID].rgbButtons[2]&0x80 && !(gPrevJoyState[controlID].rgbButtons[2]&0x80);
			break;
		case Button_P2_Circle:
			inputValue = gJoyState[controlID].rgbButtons[1]&0x80 && !(gPrevJoyState[controlID].rgbButtons[1]&0x80);
			break;
		case Button_P2_Box:
			inputValue = gJoyState[controlID].rgbButtons[3]&0x80 && !(gPrevJoyState[controlID].rgbButtons[3]&0x80);
			break;
		case Button_P2_Triangle:
			inputValue = gJoyState[controlID].rgbButtons[0]&0x80 && !(gPrevJoyState[controlID].rgbButtons[0]&0x80);
			break;
		case Button_P2_R1:
			inputValue = gJoyState[controlID].rgbButtons[7]&0x80 && !(gPrevJoyState[controlID].rgbButtons[7]&0x80);
			break;
		case Button_P2_L1:
			inputValue = gJoyState[controlID].rgbButtons[6]&0x80 && !(gPrevJoyState[controlID].rgbButtons[6]&0x80);
			break;
		case Button_P2_L2:
			inputValue = gJoyState[controlID].rgbButtons[4]&0x80 && !(gPrevJoyState[controlID].rgbButtons[4]&0x80);
			break;
		case Button_P2_R2:
			inputValue = gJoyState[controlID].rgbButtons[5]&0x80 && !(gPrevJoyState[controlID].rgbButtons[5]&0x80);
			break;
		case Button_P2_Start:
			inputValue = gJoyState[controlID].rgbButtons[9]&0x80 && !(gPrevJoyState[controlID].rgbButtons[9]&0x80);
			break;
		case Button_P2_Select:
			inputValue = gJoyState[controlID].rgbButtons[8]&0x80 && !(gPrevJoyState[controlID].rgbButtons[8]&0x80);
			break;
		case Button_P2_LThumb:
			inputValue = gJoyState[controlID].rgbButtons[10]&0x80 && !(gPrevJoyState[controlID].rgbButtons[10]&0x80);
			break;
		case Button_P2_RThumb:
			inputValue = gJoyState[controlID].rgbButtons[11]&0x80 && !(gPrevJoyState[controlID].rgbButtons[11]&0x80);
			break;
		case Button_DUp:
			inputValue = gJoyState[controlID].rgbButtons[12]&0x80 && !(gPrevJoyState[controlID].rgbButtons[12]&0x80);
			break;
		case Button_DDown:
			inputValue = gJoyState[controlID].rgbButtons[14]&0x80 && !(gPrevJoyState[controlID].rgbButtons[14]&0x80);
			break;
		case Button_DLeft:
			inputValue = gJoyState[controlID].rgbButtons[15]&0x80 && !(gPrevJoyState[controlID].rgbButtons[15]&0x80);
			break;
		case Button_DRight:
			inputValue = gJoyState[controlID].rgbButtons[13]&0x80 && !(gPrevJoyState[controlID].rgbButtons[13]&0x80);
			break;
	}

	if(controlID <= 0 && !inputValue && Input_GetKeyboardStatusState(KSS_ScrollLock))
	{
		switch(type)
		{
			case Button_P2_Cross:
				inputValue = Input_WasKeyPressed(DIK_H);
				break;
			case Button_P2_Circle:
				inputValue = Input_WasKeyPressed(DIK_J);
				break;
			case Button_P2_Box:
				inputValue = Input_WasKeyPressed(DIK_G);
				break;
			case Button_P2_Triangle:
				inputValue = Input_WasKeyPressed(DIK_Y);
				break;
			case Button_P2_R1:
				inputValue = Input_WasKeyPressed(DIK_K);
				break;
			case Button_P2_L1:
				inputValue = Input_WasKeyPressed(DIK_F);
				break;
			case Button_P2_L2:
				inputValue = Input_WasKeyPressed(DIK_D);
				break;
			case Button_P2_R2:
				inputValue = Input_WasKeyPressed(DIK_L);
				break;
			case Button_P2_Start:
				inputValue = Input_WasKeyPressed(DIK_SPACE);
				break;
			case Button_P2_Select:
				inputValue = Input_WasKeyPressed(DIK_RALT);
				break;
			case Button_P2_LThumb:
				inputValue = Input_WasKeyPressed(DIK_T);
				break;
			case Button_P2_RThumb:
				inputValue = Input_WasKeyPressed(DIK_U);
				break;
			case Button_DUp:
				inputValue = Input_WasKeyPressed(DIK_UP) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;
			case Button_DDown:
				inputValue = Input_WasKeyPressed(DIK_DOWN) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;
			case Button_DLeft:
				inputValue = Input_WasKeyPressed(DIK_LEFT) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;
			case Button_DRight:
				inputValue = Input_WasKeyPressed(DIK_RIGHT) && !Input_GetKeyboardStatusState(KSS_CapsLock);
				break;
		}
	}

	return inputValue;
}

// "Is Pad Connected" Function?
bool Input_IsConnected(int controlID)
{
	CALLSTACK;

	return controlID < gJoystickCount;
}

void SetGamepadEventHandler(EventFunc pEventFunc)
{
	pJoyEventFunc = pEventFunc;
}

int Input_GetNumKeyboards()
{
	return gKeyboardCount;
}

bool Input_ReadKeyboard(uint32 key, int keyboardID)
{
	DBGASSERT(keyboardID >= -1 && keyboardID < gKeyboardCount, STR("Keyboard %d unavailable", keyboardID));

	return gKeyState[keyboardID+1][key] != 0;
}

bool Input_WasKeyPressed(uint32 key, int keyboardID)
{
	DBGASSERT(keyboardID >= -1 && keyboardID < gKeyboardCount, STR("Keyboard %d unavailable", keyboardID));

	return gKeyState[keyboardID+1][key] && !gPrevKeyState[keyboardID+1][key];
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

void SetKeyboardEventHandler(EventFunc pEventFunc)
{
	pKeyEventFunc = pEventFunc;
}

int Input_GetNumPointers()
{
	return gMouseCount;
}

bool Input_ReadMouseKey(uint32 key, int mouseID)
{
	DBGASSERT(mouseID >= -1 && mouseID < gMouseCount, STR("Mouse %d unavailable", mouseID));

	return false;
}

bool Input_WasMousePressed(uint32 key, int mouseID)
{
	DBGASSERT(mouseID >= -1 && mouseID < gMouseCount, STR("Mouse %d unavailable", mouseID));

	return false;
}

void SetMouseEventHandler(EventFunc pEventFunc)
{
	pMouseEventFunc = pEventFunc;
}

void SetMouseMode(uint32 mouseMode)
{

}

Vector3 Input_ReadMousePos(int mouseID)
{
	DBGASSERT(mouseID >= -1 && mouseID < gMouseCount, STR("Mouse %d unavailable", mouseID));

	return Vector(0.0f, 0.0f, 0.0f);
}

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

	for(a=0; a<gJoystickCount; a++)
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

	if(gJoystickCount<16)
	{
		if(FAILED(pDirectInput->CreateDevice(pdidInstance->guidInstance, &pDIJoystick[gJoystickCount], NULL)))
			return DIENUM_CONTINUE;

		gJoystickCount++;

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

	for(a=0; a<gJoystickCount; a++)
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

void Input_UpdateKeyboard()
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
		memcpy(gPrevKeyState, gKeyState, 256*16);

		for(DWORD a=0; a<elements; a++) 
		{
			gKeyState[0][inputBuffer[a].dwOfs]=(char)inputBuffer[a].dwData;
			if(inputBuffer[a].dwOfs==DIK_PAUSE) gKeyState[0][DIK_PAUSE]=(char)0x80;

			if(pKeyEventFunc)
				pKeyEventFunc(inputBuffer[a].dwData ? IE_KeyDown : IE_KeyUp, inputBuffer[a].dwOfs, inputBuffer[a].dwTimeStamp);
		}
	}
}

void Input_UpdateMouse()
{
	CALLSTACK;

	DIDEVICEOBJECTDATA inputBuffer[SAMPLE_BUFFER_SIZE];
	DWORD elements = SAMPLE_BUFFER_SIZE;

	HRESULT hr;

	gMouseState[0].lX=0;
	gMouseState[0].lY=0;
	gMouseState[0].lZ=0;

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
					gMouseState[0].rgbButtons[0]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_BUTTON1:
					gMouseState[0].rgbButtons[1]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_BUTTON2:
					gMouseState[0].rgbButtons[2]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_BUTTON3:
					gMouseState[0].rgbButtons[3]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_BUTTON4:
					gMouseState[0].rgbButtons[4]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_BUTTON5:
					gMouseState[0].rgbButtons[5]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_BUTTON6:
					gMouseState[0].rgbButtons[6]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_BUTTON7:
					gMouseState[0].rgbButtons[7]=(char)inputBuffer[a].dwData;
					break;

				case DIMOFS_X:
					gMouseState[0].lX+=inputBuffer[a].dwData;
					break;

				case DIMOFS_Y:
					gMouseState[0].lY+=inputBuffer[a].dwData;
					break;

				case DIMOFS_Z:
					gMouseState[0].lZ+=inputBuffer[a].dwData;
					break;

				default:
					break;
			}
		}
	}
}

void Input_UpdateJoystick()
{
	CALLSTACK;

	for(int a=0; a<gJoystickCount; a++)
	{
		if(pDIJoystick[a])
		{
			if(FAILED(pDIJoystick[a]->Poll()))
			{
				pDIJoystick[a]->Acquire();
				return;
			}

			gPrevJoyState[a] = gJoyState[a];

			pDIJoystick[a]->GetDeviceState(sizeof(DIJOYSTATE2), &gJoyState[a]);
		}
	}
}

char* Input_EnumerateString(ButtonMapping *pMap)
{
	char *pString;

	if(pMap->source==CTRL_JOYSTICK)
	{
		pString = STR("Joy %d ", pMap->sourceIndex);

		switch(pMap->control)
		{
			case CTRL_JOYSTICK_X:
				pString = STR("%s %sX-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_Y:
				pString = STR("%s %sY-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_Z:
				pString = STR("%s %sZ-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_RX:
				pString = STR("%s %sX-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_RY:
				pString = STR("%s %sY-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_RZ:
				pString = STR("%s %sZ-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_SLIDER:
				pString = STR("%s %sSlider", pString, "");
				break;
			case CTRL_JOYSTICK_BUTTON:
				pString = STR("%s Button", pString);
				break;
			case CTRL_JOYSTICK_VX:
				pString = STR("%s %sVX-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_VY:
				pString = STR("%s %sVY-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_VZ:
				pString = STR("%s %sVZ-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_VRX:
				pString = STR("%s %sVX-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_VRY:
				pString = STR("%s %sVY-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_VRZ:
				pString = STR("%s %sVZ-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_VSLIDER:
				pString = STR("%s %sVSlider", pString, "");
				break;
			case CTRL_JOYSTICK_AX:
				pString = STR("%s %sAX-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_AY:
				pString = STR("%s %sAY-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_AZ:
				pString = STR("%s %sAZ-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_ARX:
				pString = STR("%s %sAX-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_ARY:
				pString = STR("%s %sAY-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_ARZ:
				pString = STR("%s %sAZ-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_ASLIDER:
				pString = STR("%s %sASlider", pString, "");
				break;
			case CTRL_JOYSTICK_FX:
				pString = STR("%s %sFX-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_FY:
				pString = STR("%s %sFY-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_FZ:
				pString = STR("%s %sFZ-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_FRX:
				pString = STR("%s %sFX-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_FRY:
				pString = STR("%s %sFY-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_FRZ:
				pString = STR("%s %sFZ-Rotation-Axis", pString, "");
				break;
			case CTRL_JOYSTICK_FSLIDER:
				pString = STR("%s %sFSlider", pString, "");
				break;
			default:
				pString = STR("%s Unknown", pString);
				break;
		}
	}
	else if(pMap->source==CTRL_MOUSE)
	{
		pString = STR("Mouse %d ", pMap->sourceIndex);

		switch(pMap->control)
		{
			case CTRL_MOUSE_X:
				pString = STR("%s %sX-Axis", pString, "");
				break;
			case CTRL_MOUSE_Y:
				pString = STR("%s %sY-Axis", pString, "");
				break;
			case CTRL_MOUSE_Z:
				pString = STR("%s Wheel %s", pString, "");
				break;
			case CTRL_MOUSE_BUTTON:
				pString = STR("%s Button", pString);
				break;
			default:
				pString = STR("%s Unknown", pString);
				break;
		}
	}
	else if(pMap->source==CTRL_KEY)
	{
//		pString = STR("Keyboard %d ", pMap->sourceIndex);

		switch(pMap->control)
		{
			case DIK_0:
				pString = STR("0");
				break;
			case DIK_1:
				pString = STR("1");
				break;
			case DIK_2:
				pString = STR("2");
				break;
			case DIK_3:
				pString = STR("3");
				break;
			case DIK_4:
				pString = STR("4");
				break;
			case DIK_5:
				pString = STR("5");
				break;
			case DIK_6:
				pString = STR("6");
				break;
			case DIK_7:
				pString = STR("7");
				break;
			case DIK_8:
				pString = STR("8");
				break;
			case DIK_9:
				pString = STR("9");
				break;
			case DIK_A:
				pString = STR("A");
				break;
			case DIK_ABNT_C1:
				pString = STR("ABTN C1");
				break;
			case DIK_ABNT_C2:
				pString = STR("ABTN C2");
				break;
			case DIK_ADD:
				pString = STR("Numpad Plus");
				break;
			case DIK_APOSTROPHE:
				pString = STR("Apostrophe");
				break;
			case DIK_APPS:
				pString = STR("Apps");
				break;
			case DIK_AT:
				pString = STR("AT");
				break;
			case DIK_AX:
				pString = STR("AX");
				break;
			case DIK_B:
				pString = STR("B");
				break;
			case DIK_BACK:
				pString = STR("Backspace");
				break;
			case DIK_BACKSLASH:
				pString = STR("Backslash");
				break;
			case DIK_C:
				pString = STR("C");
				break;
			case DIK_CALCULATOR:
				pString = STR("Calculator");
				break;
			case DIK_CAPITAL:
				pString = STR("CapsLock");
				break;
			case DIK_COLON:
				pString = STR("Colon");
				break;
			case DIK_COMMA:
				pString = STR("Comma");
				break;
			case DIK_CONVERT:
				pString = STR("Convert");
				break;
			case DIK_D:
				pString = STR("D");
				break;
			case DIK_DECIMAL:
				pString = STR("Numpad Decimal");
				break;
			case DIK_DELETE:
				pString = STR("Del");
				break;
			case DIK_DIVIDE:
				pString = STR("Numpad Slash");
				break;
			case DIK_DOWN:
				pString = STR("Down");
				break;
			case DIK_E:
				pString = STR("E");
				break;
			case DIK_END:
				pString = STR("End");
				break;
			case DIK_EQUALS:
				pString = STR("Equals");
				break;
			case DIK_ESCAPE:
				pString = STR("Escape");
				break;
			case DIK_F:
				pString = STR("F");
				break;
			case DIK_F1:
				pString = STR("F1");
				break;
			case DIK_F2:
				pString = STR("F2");
				break;
			case DIK_F3:
				pString = STR("F3");
				break;
			case DIK_F4:
				pString = STR("F4");
				break;
			case DIK_F5:
				pString = STR("F5");
				break;
			case DIK_F6:
				pString = STR("F6");
				break;
			case DIK_F7:
				pString = STR("F7");
				break;
			case DIK_F8:
				pString = STR("F8");
				break;
			case DIK_F9:
				pString = STR("F9");
				break;
			case DIK_F10:
				pString = STR("F10");
				break;
			case DIK_F11:
				pString = STR("F11");
				break;
			case DIK_F12:
				pString = STR("F12");
				break;
			case DIK_F13:
				pString = STR("F13");
				break;
			case DIK_F14:
				pString = STR("F14");
				break;
			case DIK_F15:
				pString = STR("F15");
				break;
			case DIK_G:
				pString = STR("G");
				break;
			case DIK_GRAVE:
				pString = STR("Grave");
				break;
			case DIK_H:
				pString = STR("H");
				break;
			case DIK_HOME:
				pString = STR("Home");
				break;
			case DIK_I:
				pString = STR("I");
				break;
			case DIK_INSERT:
				pString = STR("Insert");
				break;
			case DIK_J:
				pString = STR("J");
				break;
			case DIK_K:
				pString = STR("K");
				break;
			case DIK_KANA:
				pString = STR("Kana");
				break;
			case DIK_KANJI:
				pString = STR("Kanji");
				break;
			case DIK_L:
				pString = STR("L");
				break;
			case DIK_LBRACKET:
				pString = STR("LSqrBracket");
				break;
			case DIK_LCONTROL:
				pString = STR("LCtrl");
				break;
			case DIK_LEFT:
				pString = STR("Left");
				break;
			case DIK_LMENU:
				pString = STR("LAtl");
				break;
			case DIK_LSHIFT:
				pString = STR("LShift");
				break;
			case DIK_LWIN:
				pString = STR("LWin");
				break;
			case DIK_M:
				pString = STR("M");
				break;
			case DIK_MAIL:
				pString = STR("Mail");
				break;
			case DIK_MEDIASELECT:
				pString = STR("Media Select");
				break;
			case DIK_MEDIASTOP:
				pString = STR("Media Stop");
				break;
			case DIK_MINUS:
				pString = STR("Minus");
				break;
			case DIK_MULTIPLY:
				pString = STR("Numpad Multiply");
				break;
			case DIK_MUTE:
				pString = STR("Mute");
				break;
			case DIK_MYCOMPUTER:
				pString = STR("My Computer");
				break;
			case DIK_N:
				pString = STR("N");
				break;
			case DIK_NEXT:
				pString = STR("PgDn");
				break;
			case DIK_NEXTTRACK:
				pString = STR("Next Track");
				break;
			case DIK_NOCONVERT:
				pString = STR("No Convert");
				break;
			case DIK_NUMLOCK:
				pString = STR("NumLock");
				break;
			case DIK_NUMPAD0:
				pString = STR("Numpad 0");
				break;
			case DIK_NUMPAD1:
				pString = STR("Numpad 1");
				break;
			case DIK_NUMPAD2:
				pString = STR("Numpad 2");
				break;
			case DIK_NUMPAD3:
				pString = STR("Numpad 3");
				break;
			case DIK_NUMPAD4:
				pString = STR("Numpad 4");
				break;
			case DIK_NUMPAD5:
				pString = STR("Numpad 5");
				break;
			case DIK_NUMPAD6:
				pString = STR("Numpad 6");
				break;
			case DIK_NUMPAD7:
				pString = STR("Numpad 7");
				break;
			case DIK_NUMPAD8:
				pString = STR("Numpad 8");
				break;
			case DIK_NUMPAD9:
				pString = STR("Numpad 9");
				break;
			case DIK_NUMPADCOMMA:
				pString = STR("Numpad Comma");
				break;
			case DIK_NUMPADENTER:
				pString = STR("Numpad Enter");
				break;
			case DIK_NUMPADEQUALS:
				pString = STR("Numpad Equals");
				break;
			case DIK_O:
				pString = STR("O");
				break;
			case DIK_OEM_102:
				pString = STR("OEM 102");
				break;
			case DIK_P:
				pString = STR("P");
				break;
			case DIK_PAUSE:
				pString = STR("Pause");
				break;
			case DIK_PERIOD:
				pString = STR("Period");
				break;
			case DIK_PLAYPAUSE:
				pString = STR("PlayPause");
				break;
			case DIK_POWER:
				pString = STR("Power");
				break;
			case DIK_PREVTRACK:
				pString = STR("Prev Track");
				break;
			case DIK_PRIOR:
				pString = STR("PgUp");
				break;
			case DIK_Q:
				pString = STR("Q");
				break;
			case DIK_R:
				pString = STR("R");
				break;
			case DIK_RBRACKET:
				pString = STR("RSqrBracket");
				break;
			case DIK_RCONTROL:
				pString = STR("RCtrl");
				break;
			case DIK_RETURN:
				pString = STR("Enter");
				break;
			case DIK_RIGHT:
				pString = STR("Right");
				break;
			case DIK_RMENU:
				pString = STR("RAlt");
				break;
			case DIK_RSHIFT:
				pString = STR("RShift");
				break;
			case DIK_RWIN:
				pString = STR("RWin");
				break;
			case DIK_S:
				pString = STR("S");
				break;
			case DIK_SCROLL:
				pString = STR("ScrollLock");
				break;
			case DIK_SEMICOLON:
				pString = STR("Semicolon");
				break;
			case DIK_SLASH:
				pString = STR("Slash");
				break;
			case DIK_SLEEP:
				pString = STR("Sleep");
				break;
			case DIK_SPACE:
				pString = STR("Space");
				break;
			case DIK_STOP:
				pString = STR("Stop");
				break;
			case DIK_SUBTRACT:
				pString = STR("Numpad Minus");
				break;
			case DIK_SYSRQ:
				pString = STR("SysRq");
				break;
			case DIK_T:
				pString = STR("T");
				break;
			case DIK_TAB:
				pString = STR("Tab");
				break;
			case DIK_U:
				pString = STR("U");
				break;
			case DIK_UNDERLINE:
				pString = STR("Underscore");
				break;
			case DIK_UNLABELED:
				pString = STR("Unlabeled");
				break;
			case DIK_UP:
				pString = STR("Up");
				break;
			case DIK_V:
				pString = STR("V");
				break;
			case DIK_VOLUMEDOWN:
				pString = STR("Volume Down");
				break;
			case DIK_VOLUMEUP:
				pString = STR("Volume Up");
				break;
			case DIK_W:
				pString = STR("W");
				break;
			case DIK_WAKE:
				pString = STR("Wake");
				break;
			case DIK_WEBBACK:
				pString = STR("Web Back");
				break;
			case DIK_WEBFAVORITES:
				pString = STR("Web Favorites");
				break;
			case DIK_WEBFORWARD:
				pString = STR("Web Foreward");
				break;
			case DIK_WEBHOME:
				pString = STR("Web Home");
				break;
			case DIK_WEBREFRESH:
				pString = STR("Web Refresh");
				break;
			case DIK_WEBSEARCH:
				pString = STR("Web Search");
				break;
			case DIK_WEBSTOP:
				pString = STR("Web Stop");
				break;
			case DIK_X:
				pString = STR("X");
				break;
			case DIK_Y:
				pString = STR("Y");
				break;
			case DIK_YEN:
				pString = STR("Yen");
				break;
			case DIK_Z:
				pString = STR("Z");
				break;
			default:
				pString = STR("Unknown");
				break;
		}
	}

	return pString;
}
