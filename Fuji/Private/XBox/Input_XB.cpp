#include "Common.h"
#include "Input.h"
#include "Vector3.h"

/*** Structures ***/

struct DEVICE_STATE
{
    XPP_DEVICE_TYPE *pxdt;
    DWORD dwState;
};

/*** Function Declarations ***/

void CheckDeviceChanges(DEVICE_STATE *pdsDevices);
void HandleDeviceChanges(XPP_DEVICE_TYPE *pxdt, DWORD dwInsert, DWORD dwRemove);
void ShowMUInfo(char chDrive);
void EnumSavedGames(char chDrive);
void ReadGamepad();
HANDLE GetFirstController();
void CheckKeyboard();

/*** Global Variables ***/

// The index of the gamepad info in the device states array.
#define DS_GAMEPAD  0

char	chMUDrives[4][2];
HANDLE	hPads[4];
XINPUT_STATE inputState[4];
XINPUT_STATE prevState[4];

DEVICE_STATE dsDevices[] =
{
    { XDEVICE_TYPE_GAMEPAD, 0 },
    { XDEVICE_TYPE_MEMORY_UNIT, 0 },
    { XDEVICE_TYPE_VOICE_MICROPHONE, 0 },
    { XDEVICE_TYPE_VOICE_HEADPHONE, 0 },
    { XDEVICE_TYPE_DEBUG_KEYBOARD, 0 },
};
#define NUM_DEVICE_STATES   (sizeof(dsDevices)/sizeof(*dsDevices))

float deadZone = 0.3f;

/*** Functions ***/

void Input_InitModule()
{
	CALLSTACK;

    XDEVICE_PREALLOC_TYPE xdpt[] =
	{
        {XDEVICE_TYPE_GAMEPAD, 4},
        {XDEVICE_TYPE_MEMORY_UNIT, 8},
        {XDEVICE_TYPE_DEBUG_KEYBOARD, 1}
	};

    XINPUT_DEBUG_KEYQUEUE_PARAMETERS xdkp =
	{
        XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN    |
		XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT  |
        XINPUT_DEBUG_KEYQUEUE_FLAG_ASCII_ONLY,
        50,
        500,
        30
	};

    int i;


    // Initialize the peripherals.
    XInitDevices( sizeof(xdpt) / sizeof(XDEVICE_PREALLOC_TYPE), xdpt );

    // Create the keyboard queue.
    XInputDebugInitKeyboardQueue( &xdkp );

    // Set device handles to invalid.
    ZeroMemory( hPads, sizeof(hPads) );

    // Set drive letters to invalid.
    ZeroMemory( chMUDrives, sizeof(chMUDrives) );

    // Get initial state of all connected devices.
    for( i = 0; i < NUM_DEVICE_STATES; i++ )
    {
        dsDevices[i].dwState = XGetDevices( dsDevices[i].pxdt );
        HandleDeviceChanges( dsDevices[i].pxdt, dsDevices[i].dwState, 0 );
    }
}

void Input_DeinitModule()
{
	CALLSTACK;

}

void Input_Update()
{
	CALLSTACKc;

	CheckDeviceChanges(dsDevices);
	CheckKeyboard();

	for(int a=0; a<4; a++)
	{
		if( dsDevices[DS_GAMEPAD].dwState & 1 << a && hPads[a] )
		{
			// Query latest state.
			memcpy(&prevState[a], &inputState[a], sizeof(inputState[a]));
			XInputGetState(hPads[a], &inputState[a]);
		}
	}
}

float Input_ReadGamepad(int controlID, uint32 type)
{
	CALLSTACKc;

	static DWORD dwStartLast = 0;
	float inputValue;

	if(controlID>3)
	{
		OutputDebugString("Error: Invalid Control ID\n\n");
		return 0.0f;
	}

	if(controlID > -1)
	{
		if(!(dsDevices[DS_GAMEPAD].dwState & 1 << controlID && hPads[controlID])) return 0.0f;
	}
	else return 0.0f;

	switch(type)
	{
		case Button_A:
		case Button_B:
		case Button_X:
		case Button_Y:
		case Button_Black:
		case Button_White:
		case Button_LTrig:
		case Button_RTrig:
			return ((inputState[controlID].Gamepad.bAnalogButtons[type]>XINPUT_GAMEPAD_MAX_CROSSTALK) ? (float)inputState[controlID].Gamepad.bAnalogButtons[type] : 0.0f) / 255.0f;

		case Button_Start:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 1.0f : 0.0f;

		case Button_Back:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 1.0f : 0.0f;

		case Button_LThumb:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 1.0f : 0.0f;

		case Button_RThumb:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 1.0f : 0.0f;

		case Button_DUp:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1.0f : 0.0f;

		case Button_DDown:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1.0f : 0.0f;

		case Button_DLeft:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1.0f : 0.0f;

		case Button_DRight:
			return (inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1.0f : 0.0f;

		case Axis_LX:
			inputValue = ((float)inputState[controlID].Gamepad.sThumbLX) / 32767.0f;
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		case Axis_LY:
			inputValue = ((float)inputState[controlID].Gamepad.sThumbLY) / 32767.0f;
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		case Axis_RX:
			inputValue = ((float)inputState[controlID].Gamepad.sThumbRX) / 32767.0f;
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		case Axis_RY:
			inputValue = ((float)inputState[controlID].Gamepad.sThumbRY) / 32767.0f;
			return (abs(inputValue) > deadZone) ? inputValue : 0.0f;

		default:
			OutputDebugString("Error: Undefined Control Pad Input Type\n\n");
	}

	return 0.0f;
}

bool Input_WasPressed(int controlID, uint32 type)
{
	CALLSTACKc;

	static DWORD dwStartLast = 0;

	if(controlID>3)
	{
		OutputDebugString("Error: Invalid Control ID\n\n");
		return false;
	}

	if(controlID > -1)
	{
		if(!(dsDevices[DS_GAMEPAD].dwState & 1 << controlID && hPads[controlID])) return false;
	}
	else return false;

	switch(type)
	{
		case Button_A:
		case Button_B:
		case Button_X:
		case Button_Y:
		case Button_Black:
		case Button_White:
		case Button_LTrig:
		case Button_RTrig:
			return inputState[controlID].Gamepad.bAnalogButtons[type]>XINPUT_GAMEPAD_MAX_CROSSTALK && !(prevState[controlID].Gamepad.bAnalogButtons[type]>XINPUT_GAMEPAD_MAX_CROSSTALK);

		case Button_Start:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_START && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_START);

		case Button_Back:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_BACK && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_BACK);

		case Button_LThumb:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);

		case Button_RThumb:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);

		case Button_DUp:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);

		case Button_DDown:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);

		case Button_DLeft:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);

		case Button_DRight:
			return inputState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT && !(prevState[controlID].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

		default:
			OutputDebugString("Error: Undefined Control Pad Input Type\n\n");
	}

	return false;
}

// "Is Pad Connected" Function?
bool Input_IsConnected(int controlID)
{
	CALLSTACK;

	return dsDevices[DS_GAMEPAD].dwState & 1 << controlID && hPads[controlID];
}

void SetGamepadEventHandler(EventFunc pEventFunc)
{
	
}

int Input_GetNumKeyboards()
{
	return 0;
}

bool Input_ReadKeyboard(int keyboardID, uint32 key)
{
	return false;
}

bool Input_WasKeyPressed(int keyboardID, uint32 key)
{
	return false;
}

void SetKeyboardEventHandler(EventFunc pEventFunc)
{

}

int Input_GetNumPointers()
{
	return 0;
}

bool Input_ReadMouseKey(int mouseID, uint32 key)
{
	return false;
}

bool Input_WasMousePressed(int mouseID, uint32 key)
{
	return false;
}

void SetMouseEventHandler(EventFunc pEventFunc)
{

}

void SetMouseMode(uint32 mouseMode)
{

}

Vector3 Input_ReadMousePos(int mouseID)
{
	return Vector(0.0f, 0.0f, 0.0f);
}

//
// Check for and handle any device changes.
//
void CheckDeviceChanges(DEVICE_STATE *pdsDevices)
{
	CALLSTACK;

	DWORD dwInsert, dwRemove;
	int i;

	// Check each device type to see if any changes have occurred.
	for( i = 0; i < NUM_DEVICE_STATES; i++ )
	{
		if( XGetDeviceChanges( pdsDevices[i].pxdt, &dwInsert, &dwRemove ) )
		{
			// Call handler to service the insertion and/or removal.
			HandleDeviceChanges( pdsDevices[i].pxdt, dwInsert, dwRemove );

			// Update new device state.
			pdsDevices[i].dwState &= ~dwRemove;
			pdsDevices[i].dwState |= dwInsert;
		}
	}
}

void HandleDeviceChanges(XPP_DEVICE_TYPE *pxdt, DWORD dwInsert, DWORD dwRemove)
{
	CALLSTACK;

	DWORD iPort, iSlot, dwMask;
	//DWORD dwErr;
	char szDrive[] = "X:\\";

	if( XDEVICE_TYPE_GAMEPAD == pxdt || XDEVICE_TYPE_DEBUG_KEYBOARD == pxdt )
	{
		for( iPort = 0; iPort < 4; iPort++ )
		{
			// Close removals.
			if( (1 << iPort & dwRemove) && hPads[iPort] )
			{
				XInputClose( hPads[iPort] );
				hPads[iPort] = 0;
			}

			// Open insertions.
			if( 1 << iPort & dwInsert )
			{
				hPads[iPort] = XInputOpen( pxdt, iPort, XDEVICE_NO_SLOT, NULL );
			}
		}
	}

    else if( XDEVICE_TYPE_MEMORY_UNIT == pxdt )
    {
        for( iPort = 0; iPort < 4; iPort++ )
        {
            for( iSlot = 0; iSlot < 2; iSlot++ )
            {

                // Get mask from port and slot.
                dwMask = iPort + (iSlot ? 16 : 0);
                dwMask = 1 << dwMask;

                // Unmount removals.
                if( (dwMask & dwRemove) && chMUDrives[iPort][iSlot] )
                {
                    XUnmountMU( iPort, iSlot );
                    chMUDrives[iPort][iSlot] = 0;
                }

                // Mount insertions.
                if( dwMask & dwInsert )
                {
                    XMountMU( iPort, iSlot, chMUDrives[iPort] + iSlot );
                    ShowMUInfo( chMUDrives[iPort][iSlot] );
                }
            }
        }
    }

    return;
}

//
// Display free space, name, and saved games on a mounted MU.
//
void ShowMUInfo(char chDrive)
{
	CALLSTACK;

	WCHAR szName[MAX_MUNAME + 1];
	ULARGE_INTEGER uliFreeAvail, uliFree, uliTotal;
	char szDrive[] = "X:\\";


	*szDrive = chDrive;
	GetDiskFreeSpaceEx( szDrive, &uliFreeAvail, &uliTotal, &uliFree );
	XMUNameFromDriveLetter( chDrive, szName, MAX_MUNAME + 1 );

	dprintf( "MU %c: (%S) free: %u total: %u.\r\n",
				chDrive, szName,
				uliFreeAvail.LowPart, uliTotal.LowPart ); 

	EnumSavedGames( *szDrive );
}

//
// Enumerate saved games on the specified device.
//
void EnumSavedGames(char chDrive)
{
	CALLSTACK;

	HANDLE hFind;
	XGAME_FIND_DATA xgfd;
	char szDrive[] = "X:\\";

	*szDrive = chDrive;
	hFind = XFindFirstSaveGame( szDrive, &xgfd );

	if( INVALID_HANDLE_VALUE != hFind )
	{
		do
		{
			OutputDebugStringW( xgfd.szSaveGameName );
			OutputDebugString( "\r\n" );
		} while( XFindNextSaveGame( hFind, &xgfd ) );

		XFindClose( hFind );
	}

	return;
}

//
// Find the first connected controller.
// This could be cached and then checked only when a device change
// is detected.
//
HANDLE GetFirstController()
{
	CALLSTACK;

	HANDLE hPad = 0;
	int i;

	// Check the global gamepad state for a connected device.
	for( i = 0; i < 4; i++ )
	{
		if( dsDevices[DS_GAMEPAD].dwState & 1 << i && hPads[i] )
		{
			hPad = hPads[i];
			break;
		}
	}

	return hPad;
}

void CheckKeyboard()
{
	CALLSTACK;

	XINPUT_DEBUG_KEYSTROKE ks;
	CHAR szOut[2] = "X";

	if( ERROR_SUCCESS == XInputDebugGetKeystroke( &ks ) )
	{
		*szOut = ks.Ascii;
		OutputDebugString( szOut );
	}
}


