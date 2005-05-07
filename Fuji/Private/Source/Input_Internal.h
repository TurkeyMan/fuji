#if !defined(_INPUT_INTERNAL_H)
#define _INPUT_INTERNAL_H

#include "Input.h"

#define Input_MaxInputID	16

// foreward declarations
struct DeviceStatus;

struct GamepadState;
struct KeyState;
struct MouseState;

// internal functions
void Input_InitModule();
void Input_DeinitModule();
void Input_Update();

void Input_InitModulePlatformSpecific();
void Input_DeinitModulePlatformSpecific();
void Input_UpdatePlatformSpecific();

void Input_GetDeviceStatusInternal(int device, int id, DeviceStatus *pDeviceStatus);

void Input_GetGamepadStateInternal(int id, GamepadState *pGamepadState);
void Input_GetKeyStateInternal(int id, KeyState *pGamepadState);
void Input_GetMouseStateInternal(int id, MouseState *pGamepadState);

float InputInternal_GetGamepadKeyMapping(int type, KeyState *pKeystate);

// gamepad emulation key mapping
enum KeyGamepadMapping
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
struct DeviceStatus
{
	bool available;
	int status;
};

// this may be expanded in the future
struct GamepadState
{
	float values[GamepadType_Max];
};

// state of a keyboard
struct KeyState
{
	uint8 keys[256];
};

// state of a mouse
struct MouseState
{
	float values[Mouse_MaxAxis];
	uint8 buttonState[8];
};

#endif
