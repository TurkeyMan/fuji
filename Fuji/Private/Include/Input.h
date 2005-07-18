#if !defined(_INPUT_H)
#define _INPUT_H

#include "View.h"

// interface functions
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
void	Input_SetMouseClippingRect(int mouseID, MFRect *pRect);

const char*	Input_EnumerateString(int source, int sourceID, int type, bool includeDevice = false, bool includeDeviceID = false);

void	Input_SetDeadZone(float deadZone);
float	Input_GetDeadZone();

// these are platform specific
const char*	Input_GetDeviceName(int source, int sourceID);
const char* Input_GetGamepadButtonName(int sourceID, int type);
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

// PSP controller enums
	Button_PP_Cross		= 0,
	Button_PP_Circle	= 1,
	Button_PP_Box		= 2,
	Button_PP_Triangle	= 3,

	Button_PP_L			= 6,
	Button_PP_R			= 7,

	Button_PP_Start		= 8,
	Button_PP_Select	= 9,

// general controller enums
	Button_DUp = 12,
	Button_DDown,
	Button_DLeft,
	Button_DRight,

	Axis_LX = 16,
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
	Mouse_Extra3,
	Mouse_Extra4,
	Mouse_Extra5,

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
	Key_Unknown = -1,

	Key_None = 0,
	Key_Up,
	Key_Down,
	Key_Left,
	Key_Right,
	Key_Apps,			// on windows keyboards
	Key_LWin,			// on windows keyboards
	Key_RWin,			// on windows keyboards
	Key_Backspace,
	Key_Tab,
	Key_PrintScreen,
	Key_ScrollLock,
	Key_Break,
	Key_Return,
	Key_F1,
	Key_F2,
	Key_F3,
	Key_F4,
	Key_F5,
	Key_F6,
	Key_F7,
	Key_F8,
	Key_F9,
	Key_F10,
	Key_F11,
	Key_F12,
	Key_OEM_102,		// on german keyboard
	Key_Escape,
	Key_MyComputer,		// on multimedia keyboards
	Key_Mail,			// on multimedia keyboards
	Key_Calculator,		// on multimedia keyboards
	Key_Stop,			// japanese keyboard
	Key_Space,
	Key_Insert,
	Key_Delete,
	Key_Home,
	Key_End,
	Key_PageUp,
	Key_PageDown,
	Key_Apostrophe,
	Key_ABNT_C1,		// on brazilian keyboard
	Key_ABNT_C2,		// on brazilian keyboard
	Key_Asterisk,
	Key_Plus,
	Key_Comma,
	Key_Hyphen,
	Key_Period,
	Key_Slash,
	Key_0,
	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_8,
	Key_9,
	Key_NumPadComma,	// japanese keyboard
	Key_Semicolon,
	Key_NumPadEquals,	// japanese keyboard
	Key_Equals,
	Key_Sleep,			// on windows keyboards
	Key_Wake,			// on windows keyboards
	Key_Power,			// on windows keyboards
	Key_A,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,
	Key_Underline,		// japanese keyboard
	Key_LBracket,
	Key_Backslash,
	Key_RBracket,
	Key_F13,			// japanese keyboard
	Key_F14,			// japanese keyboard
	Key_Grave,
	Key_F15,			// japanese keyboard
	Key_Unlabeled,		// japanese keyboard
	Key_LControl,
	Key_LAlt,
	Key_LShift,
	Key_RControl,
	Key_RAlt,
	Key_RShift,
	Key_CapsLock,

	Key_NumLock,
	Key_Divide,
	Key_Subtract,
	Key_Decimal,

	Key_NumPad0,
	Key_NumPad1,
	Key_NumPad2,
	Key_NumPad3,
	Key_NumPad4,
	Key_NumPad5,
	Key_NumPad6,
	Key_NumPad7,
	Key_NumPad8,
	Key_NumPad9,
	Key_NumPadEnter,

	Key_PlayPause,		// on multimedia keyboards
	Key_MediaStop,		// on multimedia keyboards
	Key_MediaSelect,	// on multimedia keyboards
	Key_NextTrack,		// on multimedia keyboards
	Key_PrevTrack,		// on multimedia keyboards

	Key_VolumeDown,		// on multimedia keyboards
	Key_VolumeUp,		// on multimedia keyboards
	Key_Mute,			// on multimedia keyboards

	Key_WebBack,		// on multimedia keyboards
	Key_WebFavorites,	// on multimedia keyboards
	Key_WebForeward,		// on multimedia keyboards
	Key_WebHome,		// on multimedia keyboards
	Key_WebRefresh,		// on multimedia keyboards
	Key_WebSearch,		// on multimedia keyboards
	Key_WebStop,		// on multimedia keyboards

	Key_AT,				// japanese keyboard
	Key_AX,				// japanese keyboard
	Key_Colon,			// japanese keyboard
	Key_Convert,		// japanese keyboard
	Key_Kana,			// japanese keyboard
	Key_Kanji,			// japanese keyboard
	Key_NoConvert,		// japanese keyboard
	Key_Yen,			// japanese keyboard

	Key_Max,
	Key_ForceInt	= 0x7FFFFFFF
};

#endif
