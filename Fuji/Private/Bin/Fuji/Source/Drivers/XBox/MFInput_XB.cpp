#include "Fuji.h"

#if MF_INPUT == MF_DRIVER_XBOX

#include "MFVector.h"
#include "MFInput_Internal.h"

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

DEVICE_STATE dsDevices[] =
{
    { XDEVICE_TYPE_GAMEPAD, 0 },
    { XDEVICE_TYPE_MEMORY_UNIT, 0 },
    { XDEVICE_TYPE_VOICE_MICROPHONE, 0 },
    { XDEVICE_TYPE_VOICE_HEADPHONE, 0 },
    { XDEVICE_TYPE_DEBUG_KEYBOARD, 0 },
};
#define NUM_DEVICE_STATES   (sizeof(dsDevices)/sizeof(*dsDevices))

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

/*** Functions ***/

void MFInput_InitModulePlatformSpecific()
{
	MFCALLSTACK;

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

void MFInput_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

}

void MFInput_UpdatePlatformSpecific()
{
	MFCALLSTACKc;

	CheckDeviceChanges(dsDevices);
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	switch(device)
	{
		case IDD_Gamepad:
			if(id < 4)
			{
				if(dsDevices[DS_GAMEPAD].dwState & 1 << id && hPads[id])
					return IDS_Ready;
				else
					return IDS_Disconnected;
			}
			break;

		case IDD_Mouse:
			break;

		case IDD_Keyboard:
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

	if(MFInput_IsReady(IDD_Gamepad, id))
	{
		if(dsDevices[DS_GAMEPAD].dwState & 1 << id && hPads[id])
		{
			// Query latest state.
			XInputGetState(hPads[id], &inputState[id]);
		}

		for(int a=0; a<8; a++)
		{
			int button = inputState[id].Gamepad.bAnalogButtons[a];
			pGamepadState->values[a] = ((button > XINPUT_GAMEPAD_MAX_CROSSTALK) ? button : 0) * (1.0f/255.0f);
		}

		pGamepadState->values[Button_XB_Start] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 1.0f : 0.0f;
		pGamepadState->values[Button_XB_Back] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 1.0f : 0.0f;
		pGamepadState->values[Button_XB_LThumb] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 1.0f : 0.0f;
		pGamepadState->values[Button_XB_RThumb] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 1.0f : 0.0f;
		pGamepadState->values[Button_DUp] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1.0f : 0.0f;
		pGamepadState->values[Button_DDown] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1.0f : 0.0f;
		pGamepadState->values[Button_DLeft] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1.0f : 0.0f;
		pGamepadState->values[Button_DRight] = (inputState[id].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1.0f : 0.0f;

		pGamepadState->values[Axis_LX] = MFMin((float)inputState[id].Gamepad.sThumbLX / 32767.0f, 1.0f);
		pGamepadState->values[Axis_LY] = MFMin((float)inputState[id].Gamepad.sThumbLY / 32767.0f, 1.0f);
		pGamepadState->values[Axis_RX] = MFMin((float)inputState[id].Gamepad.sThumbRX / 32767.0f, 1.0f);
		pGamepadState->values[Axis_RY] = MFMin((float)inputState[id].Gamepad.sThumbRY / 32767.0f, 1.0f);
	}
	else
	{
		MFZeroMemory(pGamepadState, sizeof(*pGamepadState));
	}
}

uint32 MFInput_GetDeviceFlags(int device, int deviceID)
{
	return 0;
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFCALLSTACK;

	CheckKeyboard();
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFCALLSTACK;
}

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	const char *pText = NULL;

	switch(source)
	{
		case IDD_Gamepad:
			pText = "Gamepad";
			break;
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
	return XBoxButtons[button];
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	SHORT ks = 0;

	switch(keyboardState)
	{
		case KSS_NumLock:
			break;

		case KSS_CapsLock:
			break;

		case KSS_ScrollLock:
			break;

		case KSS_Insert:
			break;
	}

	return (ks & 1) != 0;
}

//
// Check for and handle any device changes.
//
void CheckDeviceChanges(DEVICE_STATE *pdsDevices)
{
	MFCALLSTACK;

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
	MFCALLSTACK;

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
	MFCALLSTACK;

	WCHAR szName[MAX_MUNAME + 1];
	ULARGE_INTEGER uliFreeAvail, uliFree, uliTotal;
	char szDrive[] = "X:\\";

	*szDrive = chDrive;
	GetDiskFreeSpaceEx( szDrive, &uliFreeAvail, &uliTotal, &uliFree );
	XMUNameFromDriveLetter( chDrive, szName, MAX_MUNAME + 1 );

	MFDebug_Message(MFStr("MU %c: (%S) free: %u total: %u.\r\n", chDrive, szName, uliFreeAvail.LowPart, uliTotal.LowPart)); 

	EnumSavedGames( *szDrive );
}

//
// Enumerate saved games on the specified device.
//
void EnumSavedGames(char chDrive)
{
	MFCALLSTACK;

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
	MFCALLSTACK;

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
	MFCALLSTACK;

	XINPUT_DEBUG_KEYSTROKE ks;
	CHAR szOut[2] = "X";

	if( ERROR_SUCCESS == XInputDebugGetKeystroke( &ks ) )
	{
		*szOut = ks.Ascii;
		OutputDebugString( szOut );
	}
}



#endif // MF_INPUT
