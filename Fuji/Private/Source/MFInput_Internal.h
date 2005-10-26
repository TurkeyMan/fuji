#if !defined(_INPUT_INTERNAL_H)
#define _INPUT_INTERNAL_H

#include "MFInput.h"

#define MFInput_MaxInputID	16

// foreward declarations
struct MFDeviceStatus;

struct MFGamepadState;
struct MFKeyState;
struct MFMouseState;

// internal functions
void MFInput_InitModule();
void MFInput_DeinitModule();
void MFInput_Update();

void MFInput_InitModulePlatformSpecific();
void MFInput_DeinitModulePlatformSpecific();
void MFInput_UpdatePlatformSpecific();

void MFInput_GetDeviceStatusInternal(int device, int id, MFDeviceStatus *pDeviceStatus);

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState);
void MFInput_GetKeyStateInternal(int id, MFKeyState *pGamepadState);
void MFInput_GetMouseStateInternal(int id, MFMouseState *pGamepadState);

float MFInputInternal_GetGamepadKeyMapping(int type, MFKeyState *pKeystate);

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

//	Mapping_Cross		= Key_Up,			// Axis_LX,
//	Mapping_Cross		= Key_Up,			// Axis_LY,
//	Mapping_Cross		= Key_Up,			// Axis_RX,
//	Mapping_Cross		= Key_Up,			// Axis_RY,
};

// input device state
struct MFDeviceStatus
{
	bool available;
	int status;
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

#endif
