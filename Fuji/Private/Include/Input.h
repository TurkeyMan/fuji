#if !defined(_INPUT_H)
#define _INPUT_H

#define Input_MaxInputID	16

// interface functions
void	Input_InitModule();
void	Input_DeinitModule();
void	Input_Update();

bool	Input_IsAvailable(int source, int sourceID);
bool	Input_IsConnected(int source, int sourceID);
bool	Input_IsReady(int source, int sourceID);

float	Input_Read(int source, int sourceID, int type);
bool	Input_WasPressed(int source, int sourceID, int type);
bool	Input_WasReleased(int source, int sourceID, int type);

int		Input_GetNumGamepads();
int		Input_GetNumKeyboards();
int		Input_GetNumPointers();

Vector3 Input_ReadMousePos(int mouseID = -1);
Vector3 Input_ReadMouseDelta(int mouseID = -1);
void	Input_SetMouseMode(int mode);
void	Input_SetMouseAcceleration(float multiplier);

const char*	Input_EnumerateString(int source, int sourceID, int type);

void	Input_SetDeadZone(float deadZone);
float	Input_GetDeadZone();

// these are platform specific
const char*	Input_GetDeviceName(int source, int sourceID);
bool	Input_GetKeyboardStatusState(int keyboardState, int keyboardID = -1);	// get the state of the keyboard status flags

// input enums
enum InputDevice
{
	IDD_Gamepad,
	IDD_Mouse,
	IDD_Keyboard,

	IDD_Max,
	IDD_ForceInt = 0x7FFFFFFF
};

enum InputDeviceStatus
{
	IDS_Disconnected = 0,
	IDS_Ready = 1,
	IDS_Waiting,

	IDS_Max,
	IDS_ForceInt = 0x7FFFFFFF
};

enum GamepadType
{
// xbox controller enums
	Button_XB_A			= 0,
	Button_XB_B			= 1,
	Button_XB_X			= 2,
	Button_XB_Y			= 3,

	Button_XB_Black		= 4,
	Button_XB_White		= 5,

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

	Button_P2_R1		= 4,
	Button_P2_L1		= 5,
	Button_P2_L2		= 6,
	Button_P2_R2		= 7,

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
	Axis_RY,

	GamepadType_Max,
	GamepadType_ForceInt	= 0x7FFFFFFF
};

// mouse stuff
enum MouseMode
{
	MouseMode_Shared,		// this will continue to share the mouse with the system
	MouseMode_Exclusive,	// this will make the game take exclusive access to the mouse, (not applicable to consoles)

	MouseMode_Max,
	MouseMode_ForceInt	= 0x7FFFFFFF
};

enum MouseType
{
	Mouse_XPos,
	Mouse_YPos,

	Mouse_XDelta,
	Mouse_YDelta,

	Mouse_Wheel,
	Mouse_Wheel2,	// rare on mouses

	Mouse_MaxAxis,	// max axis enum

	Mouse_LeftButton = Mouse_MaxAxis,
	Mouse_RightButton,
	Mouse_MiddleButton,
	Mouse_Extra1,
	Mouse_Extra2,

	Mouse_Max,
	Mouse_ForceInt	= 0x7FFFFFFF
};

// keyboard stuff
enum KeyboardStatusState
{
	KSS_NumLock,
	KSS_CapsLock,
	KSS_ScrollLock,
	KSS_Insert
};

enum KeyboardType
{
	KEY_UNKNOWN = -1,

	KEY_NONE = 0,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_APPS,			// on windows keyboards
	KEY_LWIN,			// on windows keyboards
	KEY_RWIN,			// on windows keyboards
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_PRINTSCREEN,
	KEY_SCROLLLOCK,
	KEY_BREAK,
	KEY_RETURN,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_OEM_102,		// on german keyboard
	KEY_ESCAPE,
	KEY_MYCOMPUTER,		// on multimedia keyboards
	KEY_MAIL,			// on multimedia keyboards
	KEY_CALCULATOR,		// on multimedia keyboards
	KEY_STOP,			// japanese keyboard
	KEY_SPACE,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_APOSTROPHE,
	KEY_ABNT_C1,		// on brazilian keyboard
	KEY_ABNT_C2,		// on brazilian keyboard
	KEY_ASTERISK,
	KEY_PLUS,
	KEY_COMMA,
	KEY_HYPHEN,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_NUMPADCOMMA,	// japanese keyboard
	KEY_SEMICOLON,
	KEY_NUMPADEQUALS,	// japanese keyboard
	KEY_EQUALS,
	KEY_SLEEP,			// on windows keyboards
	KEY_WAKE,			// on windows keyboards
	KEY_POWER,			// on windows keyboards
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_UNDERLINE,		// japanese keyboard
	KEY_LBRACKET,
	KEY_BACKSLASH,
	KEY_RBRACKET,
	KEY_F13,			// japanese keyboard
	KEY_F14,			// japanese keyboard
	KEY_GRAVE,
	KEY_F15,			// japanese keyboard
	KEY_UNLABELED,		// japanese keyboard
	KEY_LCONTROL,
	KEY_LALT,
	KEY_LSHIFT,
	KEY_RCONTROL,
	KEY_RALT,
	KEY_RSHIFT,
	KEY_CAPITAL,

	KEY_NUMLOCK,
	KEY_DIVIDE,
	KEY_SUBTRACT,
	KEY_DECIMAL,

	KEY_NUMPAD0,
	KEY_NUMPAD1,
	KEY_NUMPAD2,
	KEY_NUMPAD3,
	KEY_NUMPAD4,
	KEY_NUMPAD5,
	KEY_NUMPAD6,
	KEY_NUMPAD7,
	KEY_NUMPAD8,
	KEY_NUMPAD9,
	KEY_NUMPADENTER,

	KEY_PLAYPAUSE,		// on multimedia keyboards
	KEY_MEDIASTOP,		// on multimedia keyboards
	KEY_MEDIASELECT,	// on multimedia keyboards
	KEY_NEXTTRACK,		// on multimedia keyboards
	KEY_PREVTRACK,		// on multimedia keyboards

	KEY_VOLUMEDOWN,		// on multimedia keyboards
	KEY_VOLUMEUP,		// on multimedia keyboards
	KEY_MUTE,			// on multimedia keyboards

	KEY_WEBBACK,		// on multimedia keyboards
	KEY_WEBFAVORITES,	// on multimedia keyboards
	KEY_WEBFORWARD,		// on multimedia keyboards
	KEY_WEBHOME,		// on multimedia keyboards
	KEY_WEBREFRESH,		// on multimedia keyboards
	KEY_WEBSEARCH,		// on multimedia keyboards
	KEY_WEBSTOP,		// on multimedia keyboards

	KEY_AT,				// japanese keyboard
	KEY_AX,				// japanese keyboard
	KEY_COLON,			// japanese keyboard
	KEY_CONVERT,		// japanese keyboard
	KEY_KANA,			// japanese keyboard
	KEY_KANJI,			// japanese keyboard
	KEY_NOCONVERT,		// japanese keyboard
	KEY_YEN,			// japanese keyboard

	KEY_ForceInt	= 0x7FFFFFFF
};

enum KeyGamepadMapping
{
	Mapping_Cross		= KEY_D,
	Mapping_Circle		= KEY_F,
	Mapping_Box			= KEY_S,
	Mapping_Triangle	= KEY_E,

	Mapping_R1			= KEY_G,
	Mapping_L1			= KEY_A,
	Mapping_L2			= KEY_Q,
	Mapping_R2			= KEY_T,

	Mapping_Start		= KEY_RETURN,
	Mapping_Select		= KEY_RSHIFT,

	Mapping_LThumb		= KEY_W,
	Mapping_RThumb		= KEY_R,

// general controller enums
	Mapping_DUp			= KEY_UP,
	Mapping_DDown		= KEY_DOWN,
	Mapping_DLeft		= KEY_LEFT,
	Mapping_DRight		= KEY_RIGHT,

//	Mapping_Cross		= KEY_UP,			// Axis_LX,
//	Mapping_Cross		= KEY_UP,			// Axis_LY,
//	Mapping_Cross		= KEY_UP,			// Axis_RX,
//	Mapping_Cross		= KEY_UP,			// Axis_RY,
};

#endif
