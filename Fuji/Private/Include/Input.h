#if !defined(_INPUT_H)
#define _INPUT_H

enum InputType
{
	Button_A		= 0,
	Button_Cross	= 0,
	Button_B		= 1,
	Button_Circle	= 1,
	Button_X		= 2,
	Button_Box		= 2,
	Button_Y		= 3,
	Button_Triangle	= 3,

	Button_Black	= 4,
	Button_R2		= 4,
	Button_White	= 5,
	Button_L2		= 5,

	Button_LTrig	= 6,
	Button_L1		= 6,
	Button_RTrig	= 7,
	Button_R1		= 7,

	Button_Start	= 8,

	Button_Back		= 9,
	Button_Select	= 9,

	Button_LThumb	= 10,
	Button_RThumb	= 11,

	Button_DUp,
	Button_DDown,
	Button_DLeft,
	Button_DRight,

	Axis_LX,
	Axis_LY,
	Axis_RX,
	Axis_RY
};

enum InputEvent
{
	IE_Unknown = -1,
	IE_None = 0,

	IE_KeyDown,
	IE_KeyUp,

	IE_MouseKeyDown,
	IE_MouseKeyUp,

	IE_JoyButtonDown,
	IE_JoyButtonUp,
};

typedef void (*EventFunc)(uint32 event, uint32 data, uint32 timestamp);

void Input_InitModule();
void Input_DeinitModule();
void Input_Update();

float Input_ReadGamepad(int controlID, uint32 type);
bool Input_WasPressed(int controlID, uint32 type);

bool Input_IsConnected(int controlID);

void SetGamepadEventHandler(EventFunc pEventFunc);

int Input_GetNumKeyboards();
bool Input_ReadKeyboard(int keyboardID, uint32 key);
bool Input_WasKeyPressed(int keyboardID, uint32 type);

void SetKeyboardEventHandler(EventFunc pEventFunc);

enum MouseMode
{
	MM_Absolute,
	MM_Incremental
};

int Input_GetNumPointers();

bool Input_ReadMouseKey(int mouseID, uint32 key);
bool Input_WasMousePressed(int mouseID, uint32 type);

void SetMouseEventHandler(EventFunc pEventFunc);

void SetMouseMode(uint32 mouseMode = MM_Absolute);
Vector3 Input_ReadMousePos(int mouseID, uint32 key);

#endif