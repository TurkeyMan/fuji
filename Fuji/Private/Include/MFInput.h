/**
 * @file MFInput.h
 * @brief Provide access to the systems user input devices.
 * @author Manu Evans
 * @defgroup MFInput Input Functions
 * @{
 */

#if !defined(_MFINPUT_H)
#define _MFINPUT_H

#include "MFVector.h"

/**
 * Tests is an input device is available.
 * Tests is an input device is available.
 * @param device The source device. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The source device index.
 * @return Returns true if the device is available.
 * @see MFInput_IsConnected(), MFInput_IsReady()
 */
bool MFInput_IsAvailable(int device, int deviceID);

/**
 * Tests is an input device is connected.
 * Tests is an input device is connected.
 * @param device The source device. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The source device index.
 * @return Returns true if the device is connected.
 * @see MFInput_IsAvailable(), MFInput_IsReady()
 */
bool MFInput_IsConnected(int device, int deviceID);

/**
 * Tests is an input device is ready.
 * Tests is an input device is ready.
 * @param device The source device. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The source device index.
 * @return Returns true if the device is ready.
 * @see MFInput_IsAvailable(), MFInput_IsConnected()
 */
bool MFInput_IsReady(int device, int deviceID);

/**
 * Read input from a device.
 * Read input from a device.
 * @param button The button or control on the device to read. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKeyboardButton enumerated types.
 * @param device The source device. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The source device index.
 * @param pPrevState Optional pointer to receive the previous button state. Pass NULL if the previous state is not required.
 * @return Returns a float representing the input state.
 * @see MFInput_WasPressed(), MFInput_WasReleased()
 */
float MFInput_Read(int button, int device, int deviceID = 0, float *pPrevState = NULL);

/**
 * Test if a button was pressed this frame.
 * Test if a button was pressed this frame.
 * @param button The button or control on the device to test. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKeyboardButton enumerated types.
 * @param device The source device. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The source device index.
 * @return Returns true if the button was pressed this frame.
 * @see MFInput_Read(), MFInput_WasReleased()
 */
bool MFInput_WasPressed(int button, int device, int deviceID = 0);

/**
 * Test if a button was released this frame.
 * Test if a button was released this frame.
 * @param button The button or control on the device to test. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKeyboardButton enumerated types.
 * @param device The source device. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The source device index.
 * @return Returns true if the button was released this frame.
 * @see MFInput_Read(), MFInput_WasPressed()
 */
bool MFInput_WasReleased(int button, int device, int deviceID = 0);

/**
 * Get input device flags.
 * Gets the device flags from an input device.
 * @param device The source device. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The source device index.
 * @return Returns a bitfield containing the input device flags.
 */
uint32 MFInput_GetDeviceFlags(int device, int deviceID = 0);

/**
 * Get the number of available gamepads.
 * Get the number of available gamepads.
 * @return Returns the number of gamepads available to the system.
 * @see MFInput_GetNumKeyboards(), MFInput_GetNumPointers()
 */
int MFInput_GetNumGamepads();

/**
 * Get the number of available keyboards.
 * Get the number of available keyboards.
 * @return Returns the number of keyboards available to the system.
 * @see MFInput_GetNumGamepads(), MFInput_GetNumPointers()
 */
int MFInput_GetNumKeyboards();

/**
 * Get the number of available pointers.
 * Get the number of available pointers.
 * @return Returns the number of pointers available to the system.
 * @see MFInput_GetNumGamepads(), MFInput_GetNumKeyboards()
 */
int MFInput_GetNumPointers();

/**
 * Read the absolute position of the mouse pointer.
 * Read the absolute position of the mouse pointer.
 * @param mouseID Index of the mouse to read.
 * @return Returns a vector representing the absolute position of the mouse.
 */
MFVector MFInput_ReadMousePos(int mouseID = -1);

/**
 * Read the mouse movement delta for this frame.
 * Read the mouse movement delta for this frame.
 * @param mouseID Index of the mouse to read.
 * @return Returns a vector representing the mouse movement delta for this frame.
 */
MFVector MFInput_ReadMouseDelta(int mouseID = -1);

/**
 * Set the mouse access mode.
 * Set the mouse access mode.
 * @param mode Set the mouse access mode, this should be a value from the MFMouseMode enumerated type.
 * @return None.
 */
void MFInput_SetMouseMode(int mode);

/**
 * Set the mouse movement multiplier.
 * Set the mouse movement multiplier.
 * @param multiplier The new mouse movement multiplier.
 * @return None.
 */
void MFInput_SetMouseAcceleration(float multiplier);

/**
 * Sets a clipping rect for the mouse pointer.
 * Restricts the mouse movement to a specified rect.
 * @param mouseID Target mouse index.
 * @param pRect A rect in screen space to restrict the mouse. NULL to reset the rect to the full screen.
 * @return None.
 */
void MFInput_SetMouseClippingRect(int mouseID, MFRect *pRect);

/**
 * Generate a string representing the input button/device configuration.
 * Generate a string representing the input button/device configuration.
 * @param button The button or control on the device. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKeyboardButton enumerated types.
 * @param device The device to read the name from. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The device index.
 * @param includeDevice If true, the device name is included in the generated string.
 * @param includeDeviceID If true, the device index is included in the generated string.
 * @return Returns a string representing the button name and optionally the device name and index.
 */
const char* MFInput_EnumerateString(int button, int device, int deviceID = 0, bool includeDevice = false, bool includeDeviceID = false);

/**
 * Set the analog dead zone.
 * Set the analog dead zone.
 * @param deadZone The new analog dead zone.
 * @return None.
 */
void MFInput_SetDeadZone(float deadZone);

/**
 * Get the analog dead zone.
 * Get the analog dead zone.
 * @return Returns the current analog dead zone.
 */
float MFInput_GetDeadZone();

/**
 * Get a device name.
 * Get a device name.
 * @param device The device to fetch the name of. Must be a member of the #MFInputDevice enumerated type.
 * @param deviceID The device index.
 * @return Returns the name of the target device.
 */
const char*	MFInput_GetDeviceName(int device, int deviceID);

/**
 * Get the button name from a gamepad.
 * Get the button name from a gamepad.
 * @param button Gamepad button ID. Must be a member of the #MFGamepadButton enumerated type.
 * @param deviceID Gamepad index.
 * @return Returns the name of the specified button of the specified device.
 */
const char* MFInput_GetGamepadButtonName(int button, int deviceID);

/**
 * Get the state of the keyboard status flags.
 * Get the state of the keyboard status flags.
 * @param keyboardState Value from the #MFKeyboardStatusState enumerated type.
 * @param keyboardID Keyboard index.
 * @return Returns the state of the keyboards status bit.
 */
bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID = -1);

/**
 *
 */
char MFInput_KeyToAscii(int key, bool shift, bool caps);

/**
 * Input devices.
 * These are available input devices.
 */
enum MFInputDevice
{
	IDD_Gamepad,	/**< Gamepad device */
	IDD_Mouse,		/**< Mouse or pointer device */
	IDD_Keyboard,	/**< Keyboard device */

	IDD_Max,		/**< Maximum device */
	IDD_ForceInt = 0x7FFFFFFF	/**< Force device to an int type */
};

/**
 * Input device status.
 * These are possible device states.
 */
enum MFInputDeviceStatus
{
	IDS_Unavailable = 0,	/**< Device is not available */
	IDS_Ready = 1,			/**< Device is ready for reading */
	IDS_Disconnected,		/**< Device is disconnected */
	IDS_Waiting,			/**< Device is waiting for initialisation */

	IDS_Max,				/**< Maximum device state */
	IDS_ForceInt = 0x7FFFFFFF	/**< Force device state to an int type */
};

/**
 * Gamepad Flags.
 * Various flags available to certain types of gamepads.
 */
enum MFGamepadFlags
{
	MFGF_UserFlags					= 0xFFFF,
	MFGF_IsPS2Adapter				= 0x1,
	MFGF_IsGuitar					= 0x2,
	MFGF_IsDrums					= 0x4,

	// Guitar specific flags
	MFGF_Guitar_HasTilt				= 0x100,
	MFGF_Guitar_HasSolo				= 0x200,
	MFGF_Guitar_HasPickupSwitch		= 0x400,
	MFGF_Guitar_HasSlider			= 0x800,

	// Drum specific flags
	MFGF_Drums_Has5Drums			= 0x100,

	MFGF_SystemFlags				= 0xFFFF0000,
	MFGF_DontUseSphericalDeadzone	= 0x10000,
};

/**
 * Enums to access gamepad buttons.
 * These represent the buttons on a gamepad.
 */
enum MFGamepadButton
{
// PSX controller enums
	Button_P2_Cross		= 0,	/**< Cross button on a Playstation or PS2 gamepad */
	Button_P2_Circle	= 1,	/**< Circle button on a Playstation or PS2 gamepad */
	Button_P2_Box		= 2,	/**< Box button on a Playstation or PS2 gamepad */
	Button_P2_Triangle	= 3,	/**< Triangle button on a Playstation or PS2 gamepad */

	Button_P2_L1		= 4,	/**< L1 trigger on a Playstation or PS2 gamepad */
	Button_P2_R1		= 5,	/**< R1 trigger on a Playstation or PS2 gamepad */
	Button_P2_L2		= 6,	/**< L2 trigger on a Playstation or PS2 gamepad */
	Button_P2_R2		= 7,	/**< R2 trigger on a Playstation or PS2 gamepad */

	Button_P2_Start		= 8,	/**< Start button on a Playstation or PS2 gamepad */
	Button_P2_Select	= 9,	/**< Select button on a Playstation or PS2 gamepad */

	Button_P2_L3		= 10,	/**< Left Thumbstick button on a Playstation or PS2 gamepad */
	Button_P2_R3		= 11,	/**< Right Thumbstick button on a Playstation or PS2 gamepad */

// PSP controller enums
	Button_PP_Cross		= 0,	/**< Cross button on the PSP gamepad */
	Button_PP_Circle	= 1,	/**< Circle button on the PSP gamepad */
	Button_PP_Box		= 2,	/**< Box button on the PSP gamepad */
	Button_PP_Triangle	= 3,	/**< Triangle button on the PSP gamepad */

	Button_PP_L			= 6,	/**< L trigger on the PSP gamepad */
	Button_PP_R			= 7,	/**< R trigger on the PSP gamepad */

	Button_PP_Start		= 8,	/**< Start button on the PSP gamepad */
	Button_PP_Select	= 9,	/**< Select button on the PSP gamepad */

// XBox controller enums
	Button_XB_A			= 0,	/**< A button on an XBox gamepad */
	Button_XB_B			= 1,	/**< B button on an XBox gamepad */
	Button_XB_X			= 2,	/**< X button on an XBox gamepad */
	Button_XB_Y			= 3,	/**< Y button on an XBox gamepad */

	Button_XB_White		= 4,	/**< White button on an XBox gamepad */
	Button_XB_Black		= 5,	/**< Black button on an XBox gamepad */

	Button_XB_LTrig		= 6,	/**< Left trigger on an XBox gamepad */
	Button_XB_RTrig		= 7,	/**< Right trigger on an XBox gamepad */

	Button_XB_Start		= 8,	/**< Start button on an XBox gamepad */
	Button_XB_Back		= 9,	/**< Back button on an XBox gamepad */

	Button_XB_LThumb	= 10,	/**< Left Thumbstick button on an XBox gamepad */
	Button_XB_RThumb	= 11,	/**< Right Thumbstick button on an XBox gamepad */

// XBox 360 controller enums
	Button_X3_A			= 0,	/**< A button on an XBox 360 gamepad */
	Button_X3_B			= 1,	/**< B button on an XBox 360 gamepad */
	Button_X3_X			= 2,	/**< X button on an XBox 360 gamepad */
	Button_X3_Y			= 3,	/**< Y button on an XBox 360 gamepad */

	Button_X3_LB		= 4,	/**< White button on an XBox 360 gamepad */
	Button_X3_RB		= 5,	/**< Black button on an XBox 360 gamepad */
	Button_X3_LT		= 6,	/**< Left trigger on an XBox 360 gamepad */
	Button_X3_RT		= 7,	/**< Right trigger on an XBox 360 gamepad */

	Button_X3_Start		= 8,	/**< Start button on an XBox 360 gamepad */
	Button_X3_Back		= 9,	/**< Back button on an XBox 360 gamepad */

	Button_X3_LThumb	= 10,	/**< Left Thumbstick button on an XBox 360 gamepad */
	Button_X3_RThumb	= 11,	/**< Right Thumbstick button on an XBox 360 gamepad */

// GC controller enums
	Button_GC_A			= 0,	/**< A button on a Gamecube gamepad */
	Button_GC_X			= 1,	/**< X button on a Gamecube gamepad */
	Button_GC_B			= 2,	/**< B button on a Gamecube gamepad */
	Button_GC_Y			= 3,	/**< Y button on a Gamecube gamepad */

	Button_GC_Z			= 5,	/**< Z button on a Gamecube gamepad */
	Button_GC_L			= 6,	/**< Left Trigger button on a Gamecube gamepad */
	Button_GC_R			= 7,	/**< Right Trigger button on a Gamecube gamepad */

	Button_GC_Start		= 8,	/**< Start button on a Gamecube gamepad */

// Dreamcast controller enums
	Button_DC_A			= 0,	/**< A button on a Dreamcast gamepad */
	Button_DC_B			= 1,	/**< B button on a Dreamcast gamepad */
	Button_DC_X			= 2,	/**< X button on a Dreamcast gamepad */
	Button_DC_Y			= 3,	/**< Y button on a Dreamcast gamepad */

	Button_DC_L			= 6,	/**< Left trigger on a Dreamcast gamepad */
	Button_DC_R			= 7,	/**< Right trigger on a Dreamcast gamepad */

	Button_DC_Start		= 8,	/**< Start button on a Dreamcast gamepad */

// General controller enums
	Button_DUp = 12,			/**< Up on the digital pad */
	Button_DDown,				/**< Down on the digital pad */
	Button_DLeft,				/**< Left on the digital pad */
	Button_DRight,				/**< Right on the digital pad */

// Analog Axiis
	Axis_LX = 16,				/**< X-Axis on the Left analog stick */
	Axis_LY,					/**< Y-Axis on the Left analog stick */
	Axis_RX,					/**< X-Axis on the Right analog stick */
	Axis_RY,					/**< Y-Axis on the Right analog stick */

// Extra buttons
	Button_Home = 20,

	GamepadType_Max,			/**< Maximum gamepad button id */
	GamepadType_ForceInt	= 0x7FFFFFFF,	/**< Force button ID to an int type */

// Guitar controller enums
	Button_GH_Green			= Button_P2_R2,			/**< Green key on Guitar controller */
	Button_GH_Red			= Button_P2_Circle,		/**< Red key on Guitar controller */
	Button_GH_Yellow		= Button_P2_Triangle,	/**< Yellow key on Guitar controller */
	Button_GH_Blue			= Button_P2_Cross,		/**< Blue key on Guitar controller */
	Button_GH_Orange		= Button_P2_Box,		/**< Orange key on Guitar controller */

	Button_GH_Start			= Button_P2_Start,		/**< Start button on Guitar controller */
	Button_GH_Select		= Button_P2_Select,		/**< Select button on Guitar controller */

	Button_GH_Solo			= Button_P2_L1,			/**< Solo buttons on Guitar controller */
	Button_GH_TiltTrigger	= Button_P2_L2,			/**< Vertical tilt trigger on Guitar controller */

	Button_GH_StrumDown		= Button_DDown,			/**< Strum down on Guitar controller */
	Button_GH_StrumUp		= Button_DUp,			/**< Strum up on Guitar controller */

	Button_GH_Whammy		= Axis_LX,				/**< Whammy bar depression on Guitar controller */
	Button_GH_Tilt			= Axis_RY,				/**< Analog vertical tilt sensor on Guitar controller */

	Button_GH_Roll			= Axis_LY,				/**< Jump/Roll sensor on Guitar controller */
	Button_GH_PickupSwitch	= Axis_RX,				/**< Pickup Switch on Guitar controller */
	Button_GH_Slider		= Axis_RX,				/**< Slider on Guitar controller */

// Rock Band Drums enums
	Button_Drum_Red		= Button_P2_Circle,		/**< Red drum on Drum controller */
	Button_Drum_Yellow	= Button_P2_Triangle,	/**< Red drum on Drum controller */
	Button_Drum_Blue	= Button_P2_Box,		/**< Yellow drum on Drum controller */
	Button_Drum_Green	= Button_P2_Cross,		/**< Blue drum on Drum controller */
	Button_Drum_Kick	= Button_P2_L1,			/**< Kick pedal on Drum controller */

// Guitar Hero: World Tour enums
	Button_Drum_Hat		= Button_Drum_Yellow,	/**< Hi-Hat drum on Drum controller */
	Button_Drum_Snare	= Button_Drum_Red,		/**< Snare on Drum controller */
	Button_Drum_Tom1	= Button_Drum_Blue,		/**< Tom-1 on Drum controller */
	Button_Drum_Tom2	= Button_Drum_Green,	/**< Tom-2 on Drum controller */
	Button_Drum_Cymbal	= Button_P2_R1,			/**< Cymbal on Drum controller */

	Button_Drum_Start	= Button_P2_Start,		/**< Start button on Drum controller */
	Button_Drum_Select	= Button_P2_Select		/**< Select button on Drum controller */
};

/**
 * Mouse access modes.
 * These are mouse access modes.
 */
enum MFMouseMode
{
	MouseMode_Shared,		/**< Continue to share the mouse with the system */
	MouseMode_Exclusive,	/**< The game take exclusive access to the mouse, (not applicable to consoles) */

	MouseMode_Max,			/**< Maximum mouse mode */
	MouseMode_ForceInt	= 0x7FFFFFFF	/**< Force mouse mode to an int type */
};

/**
 * Enums to access mouse buttons.
 * These represent the buttons on a mouse.
 */
enum MFMouseButton
{
	Mouse_XPos,		/**< The mouses absolute position on the X-Axis */
	Mouse_YPos,		/**< The mouses absolute position on the Y-Axis */

	Mouse_XDelta,	/**< The mouses movement delta on the X-Axis */
	Mouse_YDelta,	/**< The mouses movement delta on the Y-Axis */

	Mouse_Wheel,	/**< The mouses wheel delta */
	Mouse_Wheel2,	/**< The mouses second wheel delta (rare on mouses) */

	Mouse_MaxAxis,	/**< Max axis enum */

	Mouse_LeftButton = Mouse_MaxAxis,	/**< Left mouse button */
	Mouse_RightButton,					/**< Right mouse button */
	Mouse_MiddleButton,					/**< Middle mouse button */
	Mouse_Extra1,						/**< Extra mouse button 1 (often maps to 'back') */
	Mouse_Extra2,						/**< Extra mouse button 2 (often maps to 'forward') */
	Mouse_Extra3,						/**< Extra mouse button 3 */
	Mouse_Extra4,						/**< Extra mouse button 4 */
	Mouse_Extra5,						/**< Extra mouse button 5 */

	Mouse_Max,											/**< Maximum mouse input */
	Mouse_MaxButtons = Mouse_Max - Mouse_LeftButton,	/**< Number of mouse buttons */
	Mouse_ForceInt	= 0x7FFFFFFF						/**< Force mouse button to an int type */
};

/**
 * Keyboard status flags.
 * These represent the various state keys on the keyboard.
 */
enum MFKeyboardStatusState
{
	KSS_NumLock,		/**< Numlock state */
	KSS_CapsLock,		/**< Capslock state */
	KSS_ScrollLock,		/**< Scroll lock state */
	KSS_Insert,			/**< Insert state */

	KSS_Max,			/**< Maximum keyboard state */
	KSS_ForceInt	= 0x7FFFFFFF	/**< Force keyboard state to an int type */
};

/**
 * Keyboard key enums.
 * These represent all the keys on the keyboard.
 */
enum MFKeyboardButton
{
	Key_Unknown = -1,	/**< Unknown key */

	Key_None = 0,		/**< Represents no key */
	Key_Up,				/**< Up arrow key */
	Key_Down,			/**< Down arrow key */
	Key_Left,			/**< Left arrow key */
	Key_Right,			/**< Right arrow key */
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
	Key_WebForward,		// on multimedia keyboards
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

	Key_Max,			/**< Max MFKeyboardButton enumeration */
	Key_ForceInt	= 0x7FFFFFFF	/**< Force MFKeyboardButton to an int type */
};

#endif

/** @} */
