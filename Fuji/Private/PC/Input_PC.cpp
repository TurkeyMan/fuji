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
		case Button_A:
			return (gJoyState[controlID].rgbButtons[2]&0x80) ? 1.0f : 0.0f;
		case Button_B:
			return (gJoyState[controlID].rgbButtons[1]&0x80) ? 1.0f : 0.0f;
		case Button_X:
			return (gJoyState[controlID].rgbButtons[3]&0x80) ? 1.0f : 0.0f;
		case Button_Y:
			return (gJoyState[controlID].rgbButtons[0]&0x80) ? 1.0f : 0.0f;
		case Button_Black:
			return (gJoyState[controlID].rgbButtons[7]&0x80) ? 1.0f : 0.0f;
		case Button_White:
			return (gJoyState[controlID].rgbButtons[6]&0x80) ? 1.0f : 0.0f;
		case Button_LTrig:
			return (gJoyState[controlID].rgbButtons[4]&0x80) ? 1.0f : 0.0f;
		case Button_RTrig:
			return (gJoyState[controlID].rgbButtons[5]&0x80) ? 1.0f : 0.0f;
		case Button_Start:
			return (gJoyState[controlID].rgbButtons[9]&0x80) ? 1.0f : 0.0f;
		case Button_Back:
			return (gJoyState[controlID].rgbButtons[8]&0x80) ? 1.0f : 0.0f;
		case Button_LThumb:
			return (gJoyState[controlID].rgbButtons[10]&0x80) ? 1.0f : 0.0f;
		case Button_RThumb:
			return (gJoyState[controlID].rgbButtons[11]&0x80) ? 1.0f : 0.0f;
		case Button_DUp:
			return (gJoyState[controlID].rgbButtons[12]&0x80) ? 1.0f : 0.0f;
		case Button_DDown:
			return (gJoyState[controlID].rgbButtons[14]&0x80) ? 1.0f : 0.0f;
		case Button_DLeft:
			return (gJoyState[controlID].rgbButtons[15]&0x80) ? 1.0f : 0.0f;
		case Button_DRight:
			return (gJoyState[controlID].rgbButtons[13]&0x80) ? 1.0f : 0.0f;

		case Axis_LX:
			inputValue = ((float)(gJoyState[controlID].lX - 32767)) / 32767.0f;
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		case Axis_LY:
			inputValue = -(((float)(gJoyState[controlID].lY - 32767)) / 32767.0f);
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		case Axis_RX:
			inputValue = ((float)(gJoyState[controlID].lRz - 32767)) / 32767.0f;
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		case Axis_RY:
			inputValue = -(((float)(gJoyState[controlID].lZ - 32767)) / 32767.0f);
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		default:
			OutputDebugString("Error: Undefined Control Pad Input Type\n\n");
	}

	return 0.0f;
}

bool Input_WasPressed(int controlID, uint32 type)
{
	CALLSTACK;

	if(!Input_IsConnected(controlID)) return 0.0f;

	switch(type)
	{
		case Button_A:
			return gJoyState[controlID].rgbButtons[2]&0x80 && !(gPrevJoyState[controlID].rgbButtons[2]&0x80);
		case Button_B:
			return gJoyState[controlID].rgbButtons[1]&0x80 && !(gPrevJoyState[controlID].rgbButtons[1]&0x80);
		case Button_X:
			return gJoyState[controlID].rgbButtons[3]&0x80 && !(gPrevJoyState[controlID].rgbButtons[3]&0x80);
		case Button_Y:
			return gJoyState[controlID].rgbButtons[0]&0x80 && !(gPrevJoyState[controlID].rgbButtons[0]&0x80);
		case Button_Black:
			return gJoyState[controlID].rgbButtons[7]&0x80 && !(gPrevJoyState[controlID].rgbButtons[7]&0x80);
		case Button_White:
			return gJoyState[controlID].rgbButtons[6]&0x80 && !(gPrevJoyState[controlID].rgbButtons[6]&0x80);
		case Button_LTrig:
			return gJoyState[controlID].rgbButtons[4]&0x80 && !(gPrevJoyState[controlID].rgbButtons[4]&0x80);
		case Button_RTrig:
			return gJoyState[controlID].rgbButtons[5]&0x80 && !(gPrevJoyState[controlID].rgbButtons[5]&0x80);
		case Button_Start:
			return gJoyState[controlID].rgbButtons[9]&0x80 && !(gPrevJoyState[controlID].rgbButtons[9]&0x80);
		case Button_Back:
			return gJoyState[controlID].rgbButtons[8]&0x80 && !(gPrevJoyState[controlID].rgbButtons[8]&0x80);
		case Button_LThumb:
			return gJoyState[controlID].rgbButtons[10]&0x80 && !(gPrevJoyState[controlID].rgbButtons[10]&0x80);
		case Button_RThumb:
			return gJoyState[controlID].rgbButtons[11]&0x80 && !(gPrevJoyState[controlID].rgbButtons[11]&0x80);
		case Button_DUp:
			return gJoyState[controlID].rgbButtons[12]&0x80 && !(gPrevJoyState[controlID].rgbButtons[12]&0x80);
		case Button_DDown:
			return gJoyState[controlID].rgbButtons[14]&0x80 && !(gPrevJoyState[controlID].rgbButtons[14]&0x80);
		case Button_DLeft:
			return gJoyState[controlID].rgbButtons[15]&0x80 && !(gPrevJoyState[controlID].rgbButtons[15]&0x80);
		case Button_DRight:
			return gJoyState[controlID].rgbButtons[13]&0x80 && !(gPrevJoyState[controlID].rgbButtons[13]&0x80);
	}

	return false;
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

/*
void meGetInput(ME_CONTROL *ctrl, void (*ProcessFunc)())
{
	char			testKeys[16][256];
	DIJOYSTATE2		testJoystick[16];
	DIMOUSESTATE2	testMouse[16];

	int complete=0;

	long  *tiaxis=NULL;
	long  *taxis=NULL;
	//char t[100];

	int a,b,c,d;

	meProcessInput();

	memcpy(testKeys, meKeys, (sizeof(char)*256)*gKeyboardCount);
	memcpy(testJoystick, gJoyState, sizeof(DIJOYSTATE2)*gJoystickCount);
	memcpy(&testMouse, &gMouseState, sizeof(DIMOUSESTATE2)*gMouseCount);

	do
	{
		meProcessInput();

		if(ProcessFunc) ProcessFunc();

		for(d=0; d<gKeyboardCount&&!complete; d++)
		{
			for(a=0; a<256&&!complete; a++)
			{
				if(testKeys[d][a] && !meKeys[d][a]) testKeys[d][a]=NULL;
				if(!testKeys[d][a] && meKeys[d][a])
				{
					ctrl->Type = ME_CTRLTYPE_KEY;
					ctrl->Data = &meKeys[d][a];
					ctrl->Sign = NULL;

					ctrl->Source = CTRL_KEY;
					ctrl->Index = d;

					ctrl->Object = a;
					ctrl->Property = NULL;

					meEnumerateString(ctrl, ctrl->Desc);

					complete++;
				}
			}
		}

		for(d=0; d<gMouseCount&&!complete; d++)
		{
			for(a=0; a<3; a++)
			{
				tiaxis=&(gMouseState[d].lX)+a;
				taxis=&(testMouse[d].lX)+a;

				if(*tiaxis<-3)
				{
					ctrl->Type = ME_CTRLTYPE_AXIS;
					ctrl->Data = tiaxis;
					ctrl->Sign = -1;

					ctrl->Source = CTRL_MOUSE;
					ctrl->Index = d;

					ctrl->Object = a;
					ctrl->Property = NULL;

					meEnumerateString(ctrl, ctrl->Desc);

					complete++;
				}

				if(*tiaxis>3)
				{
					ctrl->Type = ME_CTRLTYPE_AXIS;
					ctrl->Data = tiaxis;
					ctrl->Sign = 1;

					ctrl->Source = CTRL_MOUSE;
					ctrl->Index = d;

					ctrl->Object = a;
					ctrl->Property = NULL;

					meEnumerateString(ctrl, ctrl->Desc);

					complete++;
				}
			}

			for(b=0; b<8&&!complete; b++)
			{
				if(testMouse[d].rgbButtons[b] && !gMouseState[d].rgbButtons[b]) testMouse[d].rgbButtons[b]=NULL;
				if(!testMouse[d].rgbButtons[b] && gMouseState[d].rgbButtons[b])
				{
					ctrl->Type = ME_CTRLTYPE_BUTTON;
					ctrl->Data = &gMouseState[d].rgbButtons[b];
					ctrl->Sign = NULL;

					ctrl->Source = CTRL_MOUSE;
					ctrl->Index = d;

					ctrl->Object = CTRL_MOUSE_BUTTON;
					ctrl->Property = b;

					meEnumerateString(ctrl, ctrl->Desc);

					complete++;
				}
			}
		}

		for(a=0; a<gJoystickCount&&!complete; a++)
		{
			for(b=0; b<24&&!complete; b++)
			{
				tiaxis=&(gJoyState[a].lX)+joyaxii[b];
				taxis=&(testJoystick[a].lX)+joyaxii[b];

				if(*tiaxis<(*taxis*0.5))
				{
					ctrl->Type = ME_CTRLTYPE_AXIS;
					ctrl->Data = tiaxis;
					ctrl->Sign = -1;

					ctrl->Source = CTRL_JOYSTICK;
					ctrl->Index = a;

					ctrl->Object = joyaxii[b];
					ctrl->Property = NULL;

					meEnumerateString(ctrl, ctrl->Desc);

					complete++;
				}

				if(*tiaxis>(*taxis*1.5))
				{
					ctrl->Type = ME_CTRLTYPE_AXIS;
					ctrl->Data = tiaxis;
					ctrl->Sign = 1;

					ctrl->Source = CTRL_JOYSTICK;
					ctrl->Index = a;

					ctrl->Object = joyaxii[b];
					ctrl->Property = NULL;

					meEnumerateString(ctrl, ctrl->Desc);

					complete++;
				}
			}

			for(b=0; b<4&&!complete; b++)
			{
				for(c=0; c<2&&!complete; c++)
				{
					tiaxis=&(gJoyState[a].lX)+joysliders[b]+c;
					taxis=&(testJoystick[a].lX)+joysliders[b]+c;

					if(*tiaxis<(*taxis*0.5))
					{
						ctrl->Type = ME_CTRLTYPE_AXIS;
						ctrl->Data = tiaxis;
						ctrl->Sign = -1;

						ctrl->Source = CTRL_JOYSTICK;
						ctrl->Index = a;

						ctrl->Object = joysliders[b];
						ctrl->Property = c;

						meEnumerateString(ctrl, ctrl->Desc);

						complete++;
					}

					if(*tiaxis>(*taxis*1.5))
					{
						ctrl->Type = ME_CTRLTYPE_AXIS;
						ctrl->Data = tiaxis;
						ctrl->Sign = 1;

						ctrl->Source = CTRL_JOYSTICK;
						ctrl->Index = a;

						ctrl->Object = joysliders[b];
						ctrl->Property = c;

						meEnumerateString(ctrl, ctrl->Desc);

						complete++;
					}
				}
			}

			for(b=0; b<128&&!complete; b++)
			{
				if(testJoystick[a].rgbButtons[b] && !gJoyState[a].rgbButtons[b]) testJoystick[a].rgbButtons[b]=NULL;
				if(!testJoystick[a].rgbButtons[b] && gJoyState[a].rgbButtons[b])
				{
					ctrl->Type = ME_CTRLTYPE_BUTTON;
					ctrl->Data = &gJoyState[a].rgbButtons[b];
					ctrl->Sign = NULL;

					ctrl->Source = CTRL_JOYSTICK;
					ctrl->Index = a;

					ctrl->Object = CTRL_JOYSTICK_BUTTON;
					ctrl->Property = b;

					meEnumerateString(ctrl, ctrl->Desc);

					complete++;
				}
			}
		}
	} while(!complete);
}
*/
/*
int meDigitalReading(ME_CONTROL *ctrl)
{
	if(!ctrl->Data) return 0;

	long t=NULL;
	int x=0;

	switch(ctrl->Type)
	{
		case ME_CTRLTYPE_KEY:
		case ME_CTRLTYPE_BUTTON:
			if(*(char*)(ctrl->Data)) x=1;
			break;
		case ME_CTRLTYPE_AXIS:
			if(ctrl->Source==CTRL_JOYSTICK)
			{
				t=*(long*)(ctrl->Data);
				t-=32768;
				t*=ctrl->Sign;
				if(t>=(32768*meDigitalRatio)) x=1;
			}
			else if(ctrl->Source==CTRL_MOUSE)
			{
				if(ctrl->Sign>0 && *(long*)(ctrl->Data)>0) x=1;
				else if(ctrl->Sign<0 && *(long*)(ctrl->Data)<0) x=1;
			}
			break;
		default:
			x=0;
			break;
	}

	return x;
}

int meAnalogReading(ME_CONTROL *ctrl)
{
	if(!ctrl->Data) return 0;

	float t=NULL;
	int x=meAnalogMin;

	switch(ctrl->Type)
	{
		case ME_CTRLTYPE_KEY:
		case ME_CTRLTYPE_BUTTON:
			if(*(char*)(ctrl->Data)) x=meAnalogMin+meAnalogRange;
			break;
		case ME_CTRLTYPE_AXIS:
			if(ctrl->Source==CTRL_JOYSTICK)
			{
				t=(float)(*(long*)(ctrl->Data));
				t-=32768;
				t*=ctrl->Sign;
				if(t<0) t=0;
				else
				{
					t/=(32768*meAnalogDeadZone)/((float)meAnalogRange);
					if(t>(float)meAnalogRange) t=(float)meAnalogRange;
				}
				t+=meAnalogMin;
				x=(int)t;
			}
			else if(ctrl->Source==CTRL_MOUSE)
			{
				if(ctrl->Sign>0 && *(long*)(ctrl->Data)>0) x=*(long*)(ctrl->Data);
				else if(ctrl->Sign<0 && *(long*)(ctrl->Data)<0) x=-*(long*)(ctrl->Data);
			}
			break;
		default:
			x=meAnalogMin;
			break;
	}

	return x;
}
*/

void meEnumerateString(ME_CONTROL *ctrl, char *String)
{
	if(ctrl->Source==CTRL_JOYSTICK)
	{
		sprintf(String, "Joy %d ", ctrl->Index);

		switch(ctrl->Object)
		{
			case CTRL_JOYSTICK_X:
				sprintf(String, "%s %sX-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_Y:
				sprintf(String, "%s %sY-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_Z:
				sprintf(String, "%s %sZ-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_RX:
				sprintf(String, "%s %sX-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_RY:
				sprintf(String, "%s %sY-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_RZ:
				sprintf(String, "%s %sZ-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_SLIDER:
				sprintf(String, "%s %sSlider %d", String, ctrl->Sign==1?"+":"-", ctrl->Property);
				break;
			case CTRL_JOYSTICK_BUTTON:
				sprintf(String, "%s Button %d", String, ctrl->Property);
				break;
			case CTRL_JOYSTICK_VX:
				sprintf(String, "%s %sVX-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_VY:
				sprintf(String, "%s %sVY-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_VZ:
				sprintf(String, "%s %sVZ-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_VRX:
				sprintf(String, "%s %sVX-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_VRY:
				sprintf(String, "%s %sVY-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_VRZ:
				sprintf(String, "%s %sVZ-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_VSLIDER:
				sprintf(String, "%s %sVSlider %d", String, ctrl->Sign==1?"+":"-", ctrl->Property);
				break;
			case CTRL_JOYSTICK_AX:
				sprintf(String, "%s %sAX-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_AY:
				sprintf(String, "%s %sAY-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_AZ:
				sprintf(String, "%s %sAZ-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_ARX:
				sprintf(String, "%s %sAX-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_ARY:
				sprintf(String, "%s %sAY-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_ARZ:
				sprintf(String, "%s %sAZ-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_ASLIDER:
				sprintf(String, "%s %sASlider %d", String, ctrl->Sign==1?"+":"-", ctrl->Property);
				break;
			case CTRL_JOYSTICK_FX:
				sprintf(String, "%s %sFX-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_FY:
				sprintf(String, "%s %sFY-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_FZ:
				sprintf(String, "%s %sFZ-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_FRX:
				sprintf(String, "%s %sFX-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_FRY:
				sprintf(String, "%s %sFY-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_FRZ:
				sprintf(String, "%s %sFZ-Rotation-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_JOYSTICK_FSLIDER:
				sprintf(String, "%s %sFSlider %d", String, ctrl->Sign==1?"+":"-", ctrl->Property);
				break;
			default:
				strcat(String, "Unknown");
				break;
		}
	}
	else if(ctrl->Source==CTRL_MOUSE)
	{
		sprintf(String, "Mouse %d ", ctrl->Index);

		switch(ctrl->Object)
		{
			case CTRL_MOUSE_X:
				sprintf(String, "%s %sX-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_MOUSE_Y:
				sprintf(String, "%s %sY-Axis", String, ctrl->Sign==1?"+":"-");
				break;
			case CTRL_MOUSE_Z:
				sprintf(String, "%s Wheel %s", String, ctrl->Sign==1?"Up":"Down");
				break;
			case CTRL_MOUSE_BUTTON:
				sprintf(String, "%s Button %d", String, ctrl->Property);
				break;
			default:
				strcat(String, "Unknown");
				break;
		}
	}
	else if(ctrl->Source==CTRL_KEY)
	{
		sprintf(String, "Keyboard %d ", ctrl->Index);

		switch(ctrl->Object)
		{
			case DIK_0:
				strcat(String, "0");
				break;
			case DIK_1:
				strcat(String, "1");
				break;
			case DIK_2:
				strcat(String, "2");
				break;
			case DIK_3:
				strcat(String, "3");
				break;
			case DIK_4:
				strcat(String, "4");
				break;
			case DIK_5:
				strcat(String, "5");
				break;
			case DIK_6:
				strcat(String, "6");
				break;
			case DIK_7:
				strcat(String, "7");
				break;
			case DIK_8:
				strcat(String, "8");
				break;
			case DIK_9:
				strcat(String, "9");
				break;
			case DIK_A:
				strcat(String, "A");
				break;
			case DIK_ABNT_C1:
				strcat(String, "ABTN C1");
				break;
			case DIK_ABNT_C2:
				strcat(String, "ABTN C2");
				break;
			case DIK_ADD:
				strcat(String, "Numpad Plus");
				break;
			case DIK_APOSTROPHE:
				strcat(String, "Apostrophe");
				break;
			case DIK_APPS:
				strcat(String, "Apps");
				break;
			case DIK_AT:
				strcat(String, "AT");
				break;
			case DIK_AX:
				strcat(String, "AX");
				break;
			case DIK_B:
				strcat(String, "B");
				break;
			case DIK_BACK:
				strcat(String, "Backspace");
				break;
			case DIK_BACKSLASH:
				strcat(String, "Backslash");
				break;
			case DIK_C:
				strcat(String, "C");
				break;
			case DIK_CALCULATOR:
				strcat(String, "Calculator");
				break;
			case DIK_CAPITAL:
				strcat(String, "CapsLock");
				break;
			case DIK_COLON:
				strcat(String, "Colon");
				break;
			case DIK_COMMA:
				strcat(String, "Comma");
				break;
			case DIK_CONVERT:
				strcat(String, "Convert");
				break;
			case DIK_D:
				strcat(String, "D");
				break;
			case DIK_DECIMAL:
				strcat(String, "Numpad Decimal");
				break;
			case DIK_DELETE:
				strcat(String, "Del");
				break;
			case DIK_DIVIDE:
				strcat(String, "Numpad Slash");
				break;
			case DIK_DOWN:
				strcat(String, "Down");
				break;
			case DIK_E:
				strcat(String, "E");
				break;
			case DIK_END:
				strcat(String, "End");
				break;
			case DIK_EQUALS:
				strcat(String, "Equals");
				break;
			case DIK_ESCAPE:
				strcat(String, "Escape");
				break;
			case DIK_F:
				strcat(String, "F");
				break;
			case DIK_F1:
				strcat(String, "F1");
				break;
			case DIK_F2:
				strcat(String, "F2");
				break;
			case DIK_F3:
				strcat(String, "F3");
				break;
			case DIK_F4:
				strcat(String, "F4");
				break;
			case DIK_F5:
				strcat(String, "F5");
				break;
			case DIK_F6:
				strcat(String, "F6");
				break;
			case DIK_F7:
				strcat(String, "F7");
				break;
			case DIK_F8:
				strcat(String, "F8");
				break;
			case DIK_F9:
				strcat(String, "F9");
				break;
			case DIK_F10:
				strcat(String, "F10");
				break;
			case DIK_F11:
				strcat(String, "F11");
				break;
			case DIK_F12:
				strcat(String, "F12");
				break;
			case DIK_F13:
				strcat(String, "F13");
				break;
			case DIK_F14:
				strcat(String, "F14");
				break;
			case DIK_F15:
				strcat(String, "F15");
				break;
			case DIK_G:
				strcat(String, "G");
				break;
			case DIK_GRAVE:
				strcat(String, "Grave");
				break;
			case DIK_H:
				strcat(String, "H");
				break;
			case DIK_HOME:
				strcat(String, "Home");
				break;
			case DIK_I:
				strcat(String, "I");
				break;
			case DIK_INSERT:
				strcat(String, "Insert");
				break;
			case DIK_J:
				strcat(String, "J");
				break;
			case DIK_K:
				strcat(String, "K");
				break;
			case DIK_KANA:
				strcat(String, "Kana");
				break;
			case DIK_KANJI:
				strcat(String, "Kanji");
				break;
			case DIK_L:
				strcat(String, "L");
				break;
			case DIK_LBRACKET:
				strcat(String, "LSqrBracket");
				break;
			case DIK_LCONTROL:
				strcat(String, "LCtrl");
				break;
			case DIK_LEFT:
				strcat(String, "Left");
				break;
			case DIK_LMENU:
				strcat(String, "LAtl");
				break;
			case DIK_LSHIFT:
				strcat(String, "LShift");
				break;
			case DIK_LWIN:
				strcat(String, "LWin");
				break;
			case DIK_M:
				strcat(String, "M");
				break;
			case DIK_MAIL:
				strcat(String, "Mail");
				break;
			case DIK_MEDIASELECT:
				strcat(String, "Media Select");
				break;
			case DIK_MEDIASTOP:
				strcat(String, "Media Stop");
				break;
			case DIK_MINUS:
				strcat(String, "Minus");
				break;
			case DIK_MULTIPLY:
				strcat(String, "Numpad Multiply");
				break;
			case DIK_MUTE:
				strcat(String, "Mute");
				break;
			case DIK_MYCOMPUTER:
				strcat(String, "My Computer");
				break;
			case DIK_N:
				strcat(String, "N");
				break;
			case DIK_NEXT:
				strcat(String, "PgDn");
				break;
			case DIK_NEXTTRACK:
				strcat(String, "Next Track");
				break;
			case DIK_NOCONVERT:
				strcat(String, "No Convert");
				break;
			case DIK_NUMLOCK:
				strcat(String, "NumLock");
				break;
			case DIK_NUMPAD0:
				strcat(String, "Numpad 0");
				break;
			case DIK_NUMPAD1:
				strcat(String, "Numpad 1");
				break;
			case DIK_NUMPAD2:
				strcat(String, "Numpad 2");
				break;
			case DIK_NUMPAD3:
				strcat(String, "Numpad 3");
				break;
			case DIK_NUMPAD4:
				strcat(String, "Numpad 4");
				break;
			case DIK_NUMPAD5:
				strcat(String, "Numpad 5");
				break;
			case DIK_NUMPAD6:
				strcat(String, "Numpad 6");
				break;
			case DIK_NUMPAD7:
				strcat(String, "Numpad 7");
				break;
			case DIK_NUMPAD8:
				strcat(String, "Numpad 8");
				break;
			case DIK_NUMPAD9:
				strcat(String, "Numpad 9");
				break;
			case DIK_NUMPADCOMMA:
				strcat(String, "Numpad Comma");
				break;
			case DIK_NUMPADENTER:
				strcat(String, "Numpad Enter");
				break;
			case DIK_NUMPADEQUALS:
				strcat(String, "Numpad Equals");
				break;
			case DIK_O:
				strcat(String, "O");
				break;
			case DIK_OEM_102:
				strcat(String, "OEM 102");
				break;
			case DIK_P:
				strcat(String, "P");
				break;
			case DIK_PAUSE:
				strcat(String, "Pause");
				break;
			case DIK_PERIOD:
				strcat(String, "Period");
				break;
			case DIK_PLAYPAUSE:
				strcat(String, "PlayPause");
				break;
			case DIK_POWER:
				strcat(String, "Power");
				break;
			case DIK_PREVTRACK:
				strcat(String, "Prev Track");
				break;
			case DIK_PRIOR:
				strcat(String, "PgUp");
				break;
			case DIK_Q:
				strcat(String, "Q");
				break;
			case DIK_R:
				strcat(String, "R");
				break;
			case DIK_RBRACKET:
				strcat(String, "RSqrBracket");
				break;
			case DIK_RCONTROL:
				strcat(String, "RCtrl");
				break;
			case DIK_RETURN:
				strcat(String, "Enter");
				break;
			case DIK_RIGHT:
				strcat(String, "Right");
				break;
			case DIK_RMENU:
				strcat(String, "RAlt");
				break;
			case DIK_RSHIFT:
				strcat(String, "RShift");
				break;
			case DIK_RWIN:
				strcat(String, "RWin");
				break;
			case DIK_S:
				strcat(String, "S");
				break;
			case DIK_SCROLL:
				strcat(String, "ScrollLock");
				break;
			case DIK_SEMICOLON:
				strcat(String, "Semicolon");
				break;
			case DIK_SLASH:
				strcat(String, "Slash");
				break;
			case DIK_SLEEP:
				strcat(String, "Sleep");
				break;
			case DIK_SPACE:
				strcat(String, "Space");
				break;
			case DIK_STOP:
				strcat(String, "Stop");
				break;
			case DIK_SUBTRACT:
				strcat(String, "Numpad Minus");
				break;
			case DIK_SYSRQ:
				strcat(String, "SysRq");
				break;
			case DIK_T:
				strcat(String, "T");
				break;
			case DIK_TAB:
				strcat(String, "Tab");
				break;
			case DIK_U:
				strcat(String, "U");
				break;
			case DIK_UNDERLINE:
				strcat(String, "Underscore");
				break;
			case DIK_UNLABELED:
				strcat(String, "Unlabeled");
				break;
			case DIK_UP:
				strcat(String, "Up");
				break;
			case DIK_V:
				strcat(String, "V");
				break;
			case DIK_VOLUMEDOWN:
				strcat(String, "Volume Down");
				break;
			case DIK_VOLUMEUP:
				strcat(String, "Volume Up");
				break;
			case DIK_W:
				strcat(String, "W");
				break;
			case DIK_WAKE:
				strcat(String, "Wake");
				break;
			case DIK_WEBBACK:
				strcat(String, "Web Back");
				break;
			case DIK_WEBFAVORITES:
				strcat(String, "Web Favorites");
				break;
			case DIK_WEBFORWARD:
				strcat(String, "Web Foreward");
				break;
			case DIK_WEBHOME:
				strcat(String, "Web Home");
				break;
			case DIK_WEBREFRESH:
				strcat(String, "Web Refresh");
				break;
			case DIK_WEBSEARCH:
				strcat(String, "Web Search");
				break;
			case DIK_WEBSTOP:
				strcat(String, "Web Stop");
				break;
			case DIK_X:
				strcat(String, "X");
				break;
			case DIK_Y:
				strcat(String, "Y");
				break;
			case DIK_YEN:
				strcat(String, "Yen");
				break;
			case DIK_Z:
				strcat(String, "Z");
				break;
			default:
				strcat(String, "Unknown");
				break;
		}
	}
}

ME_CONTROL::ME_CONTROL()
{
	Type=ME_CTRLTYPE_UNDEFINED;
	Data=NULL;
	Sign=0;

	Source=CTRL_UNDEFINED;
	Index=0;

	Object=NULL;
	Property=0;

	Desc[0]=NULL;
}

ME_CONTROL::~ME_CONTROL()
{
	Type=ME_CTRLTYPE_UNDEFINED;
	Data=NULL;
	Sign=0;

	Source=CTRL_UNDEFINED;
	Index=0;

	Object=NULL;
	Property=0;

	Desc[0]=NULL;
}

void meAcquireDataPointer(ME_CONTROL *ctrl)
{
	switch(ctrl->Source)
	{
		case CTRL_KEY:
			ctrl->Type=ME_CTRLTYPE_KEY;
			ctrl->Data=&(gKeyState[ctrl->Index][ctrl->Object]);
			break;

		case CTRL_JOYSTICK:
			switch(ctrl->Object)
			{
				case CTRL_JOYSTICK_BUTTON:
				ctrl->Type = ME_CTRLTYPE_BUTTON;
				ctrl->Data = &gJoyState[min(ctrl->Index, gJoystickCount-1)].rgbButtons[ctrl->Property];
				return;

				default:
				ctrl->Type = ME_CTRLTYPE_AXIS;
				ctrl->Data = &gJoyState[min(ctrl->Index, gJoystickCount-1)];
				float *tf = (float*)ctrl->Data;
				tf += (ctrl->Object+(ctrl->Property));
				ctrl->Data = tf;
				return;
			}
			break;

		case CTRL_MOUSE:
			switch(ctrl->Object)
			{
				case CTRL_MOUSE_BUTTON:
				ctrl->Type = ME_CTRLTYPE_BUTTON;
				ctrl->Data = &gMouseState[min(ctrl->Index, gMouseCount-1)].rgbButtons[ctrl->Property];
				return;

				default:
				ctrl->Type = ME_CTRLTYPE_AXIS;
				ctrl->Data = &gMouseState[min(ctrl->Index, gMouseCount-1)];
				float *tf = (float*)ctrl->Data;
				tf += (ctrl->Object+(ctrl->Property));
				ctrl->Data = tf;
				return;
			}
			break;

		default:
			ctrl->Type=ME_CTRLTYPE_UNDEFINED;
			ctrl->Data=NULL;
			break;
	}

	meEnumerateString(ctrl, ctrl->Desc);
}
