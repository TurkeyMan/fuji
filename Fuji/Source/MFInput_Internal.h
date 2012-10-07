#if !defined(_INPUT_INTERNAL_H)
#define _INPUT_INTERNAL_H

#include "MFInput.h"

#define MFInput_MaxInputID	16

// forward declarations
struct MFDeviceStatus;

struct MFGamepadState;
struct MFKeyState;
struct MFMouseState;
struct MFAccelerometerState;
struct MFTouchPanelState;

// internal functions
MFInitStatus MFInput_InitModule();
void MFInput_DeinitModule();
void MFInput_Update();

void MFInput_InitModulePlatformSpecific();
void MFInput_DeinitModulePlatformSpecific();
void MFInput_UpdatePlatformSpecific();

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id);

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState);
void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState);
void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState);
void MFInput_GetAccelerometerStateInternal(int id, MFAccelerometerState *pAccelerometerState);
void MFInput_GetTouchPanelStateInternal(int id, MFTouchPanelState *pTouchPanelState);

const char* MFInput_GetDeviceNameInternal(int device, int deviceID);
const char* MFInput_GetGamepadButtonNameInternal(int button, int deviceID);

void MFInputInternal_GetGamepadStateFromKeyMap(MFGamepadState *pGamepadState, MFKeyState *pKeyState);

// gamepad emulation key mapping
enum MFKeyGamepadMapping
{
	Mapping_Cross		= Key_D,
	Mapping_Circle		= Key_F,
	Mapping_Box			= Key_S,
	Mapping_Triangle	= Key_E,

	Mapping_R1			= Key_G,
	Mapping_L1			= Key_A,
	Mapping_L2			= Key_Q,
	Mapping_R2			= Key_T,

	Mapping_Start		= Key_Return,
	Mapping_Select		= Key_RShift,

	Mapping_LThumb		= Key_W,
	Mapping_RThumb		= Key_R,

// general controller enums
	Mapping_DUp			= Key_Up,
	Mapping_DDown		= Key_Down,
	Mapping_DLeft		= Key_Left,
	Mapping_DRight		= Key_Right,

	Mapping_LX			= Key_Y,
	Mapping_LY			= Key_H,
	Mapping_RX			= Key_U,
	Mapping_RY			= Key_J,
};

// this may be expanded in the future
struct MFGamepadState
{
	float values[GamepadType_Max];
};

// state of a keyboard
struct MFKeyState
{
	uint8 keys[256];
};

// state of a mouse
struct MFMouseState
{
	float values[Mouse_MaxAxis];
	uint8 buttonState[8];
};

// state of an accelerometer
struct MFAccelerometerState
{
	float values[Acc_XDelta];
};

// state of a touch panel
/*
struct MFTouchPanelState
{
	struct Contact
	{
		int x, y;
		int tapCount;
		int phase;
		int flags;		
	};

	Contact contacts[20];
	int numContacts;

	bool bDidShake;
};
*/

#endif
