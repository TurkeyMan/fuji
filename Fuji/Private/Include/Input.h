#if !defined(_INPUT_H)
#define _INPUT_H

enum InputType
{
// xbox controller enums
	Button_XB_A			= 0,
	Button_XB_B			= 1,
	Button_XB_X			= 2,
	Button_XB_Y			= 3,

	Button_XB_White		= 4,
	Button_XB_Black		= 5,

	Button_XB_LTrig		= 6,
	Button_XB_RTrig		= 7,

	Button_XB_Start		= 8,
	Button_XB_Back		= 9,

	Button_XB_LThumb	= 10,
	Button_XB_RThumb	= 11,

// PSX controller enums
	Button_P2_Cross		= 0,
	Button_P2_Circle	= 1,
	Button_P2_Box		= 2,
	Button_P2_Triangle	= 3,

	Button_P2_L1		= 4,
	Button_P2_R1		= 5,
	Button_P2_R2		= 6,
	Button_P2_L2		= 7,

	Button_P2_Start		= 8,
	Button_P2_Select	= 9,

	Button_P2_LThumb	= 10,
	Button_P2_RThumb	= 11,

// general controller enums
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

class ButtonMapping
{
public:
	int source;
	int sourceIndex;
	int control;
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
bool Input_ReadKeyboard(uint32 key, int keyboardID = -1);
bool Input_WasKeyPressed(uint32 key, int keyboardID = -1);

void SetKeyboardEventHandler(EventFunc pEventFunc);

enum MouseMode
{
	MM_Absolute,
	MM_Incremental
};

int Input_GetNumPointers();

bool Input_ReadMouseKey(uint32 key, int mouseID = -1);
bool Input_WasMousePressed(uint32 key, int mouseID = -1);

void SetMouseEventHandler(EventFunc pEventFunc);

void SetMouseMode(uint32 mouseMode = MM_Absolute);
Vector3 Input_ReadMousePos(int mouseID = -1);

char* Input_EnumerateString(ButtonMapping *pMap);

#endif
