#if !defined(_INPUT_INTERNAL)
#define _INPUT_INTERNAL

struct DeviceStatus;

struct GamepadState;
struct KeyState;
struct MouseState;

// internal functions
void Input_InitModulePlatformSpecific();
void Input_DeinitModulePlatformSpecific();
void Input_UpdatePlatformSpecific();

void Input_GetDeviceStatusInternal(int device, int id, DeviceStatus *pDeviceStatus);

void Input_GetGamepadStateInternal(int id, GamepadState *pGamepadState);
void Input_GetKeyStateInternal(int id, KeyState *pGamepadState);
void Input_GetMouseStateInternal(int id, MouseState *pGamepadState);

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
