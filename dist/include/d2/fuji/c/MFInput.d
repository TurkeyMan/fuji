module fuji.c.MFInput;

public import fuji.c.MFVector;
public import fuji.c.MFTypes;

nothrow:
@nogc:

enum MFInputEventType
{
	Unknown = -1,

	Connect = 0,
	Disconnect,
	Change,
}

extern (C) struct MFInputEvent
{
	ulong timestamp;
	MFInputEventType event;
	int input;
	float state, prevState;
}

extern (C) void MFInput_EnableBufferedInput(bool bEnable, int frequency = 200);

extern (C) size_t MFInput_GetEvents(int device, int deviceID, MFInputEvent* pEvents, size_t maxEvents, bool bPeek = false);

/**
* Tests is an input device is available.
* Tests is an input device is available.
* @param device The source device. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The source device index.
* @return True if the device is available.
* @see MFInput_IsConnected(), MFInput_IsReady()
*/
extern (C) bool MFInput_IsAvailable(int device, int deviceID);

/**
* Tests is an input device is connected.
* Tests is an input device is connected.
* @param device The source device. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The source device index.
* @return True if the device is connected.
* @see MFInput_IsAvailable(), MFInput_IsReady()
*/
extern (C) bool MFInput_IsConnected(int device, int deviceID);

/**
* Tests is an input device is ready.
* Tests is an input device is ready.
* @param device The source device. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The source device index.
* @return True if the device is ready.
* @see MFInput_IsAvailable(), MFInput_IsConnected()
*/
extern (C) bool MFInput_IsReady(int device, int deviceID);

/**
* Read input from a device.
* Read input from a device.
* @param button The button or control on the device to read. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKey enumerated types.
* @param device The source device. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The source device index.
* @param pPrevState Optional pointer to receive the previous button state. Pass NULL if the previous state is not required.
* @return A float representing the input state.
* @see MFInput_WasPressed(), MFInput_WasReleased()
*/
extern (C) float MFInput_Read(int button, int device, int deviceID = 0, float *pPrevState = null);

/**
* Test if a button was pressed this frame.
* Test if a button was pressed this frame.
* @param button The button or control on the device to test. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKey enumerated types.
* @param device The source device. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The source device index.
* @return True if the button was pressed this frame.
* @see MFInput_Read(), MFInput_WasReleased()
*/
extern (C) bool MFInput_WasPressed(int button, int device, int deviceID = 0);

/**
* Test if a button was released this frame.
* Test if a button was released this frame.
* @param button The button or control on the device to test. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKey enumerated types.
* @param device The source device. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The source device index.
* @return True if the button was released this frame.
* @see MFInput_Read(), MFInput_WasPressed()
*/
extern (C) bool MFInput_WasReleased(int button, int device, int deviceID = 0);

/**
* Get input device flags.
* Gets the device flags from an input device.
* @param device The source device. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The source device index.
* @return A bitfield containing the input device flags.
*/
extern (C) uint MFInput_GetDeviceFlags(int device, int deviceID = 0);

/**
* Get the number of available gamepads.
* Get the number of available gamepads.
* @return The number of gamepads available to the system.
* @see MFInput_GetNumKeyboards(), MFInput_GetNumPointers()
*/
extern (C) int MFInput_GetNumGamepads();

/**
* Get the number of available keyboards.
* Get the number of available keyboards.
* @return The number of keyboards available to the system.
* @see MFInput_GetNumGamepads(), MFInput_GetNumPointers()
*/
extern (C) int MFInput_GetNumKeyboards();

/**
* Get the number of available pointers.
* Get the number of available pointers.
* @return The number of pointers available to the system.
* @see MFInput_GetNumGamepads(), MFInput_GetNumKeyboards()
*/
extern (C) int MFInput_GetNumPointers();

/**
*
*/
extern (C) int MFInput_GetNumAccelerometers();

/**
*
*/
extern (C) int MFInput_GetNumTouchPanels();

/**
* Read the absolute position of the mouse pointer.
* Read the absolute position of the mouse pointer.
* @param mouseID Index of the mouse to read.
* @return A vector representing the absolute position of the mouse.
*/
extern (C) MFVector MFInput_ReadMousePos(int mouseID = -1);

/**
* Read the mouse movement delta for this frame.
* Read the mouse movement delta for this frame.
* @param mouseID Index of the mouse to read.
* @return A vector representing the mouse movement delta for this frame.
*/
extern (C) MFVector MFInput_ReadMouseDelta(int mouseID = -1);

/**
* Set the mouse access mode.
* Set the mouse access mode.
* @param mode Set the mouse access mode, this should be a value from the MFMouseMode enumerated type.
* @return None.
*/
extern (C) void MFInput_SetMouseMode(int mode);

/**
* Set the mouse movement multiplier.
* Set the mouse movement multiplier.
* @param multiplier The new mouse movement multiplier.
* @return None.
*/
extern (C) void MFInput_SetMouseAcceleration(float multiplier);

/**
* Sets a clipping rect for the mouse pointer.
* Restricts the mouse movement to a specified rect.
* @param mouseID Target mouse index.
* @param pRect A rect in screen space to restrict the mouse. NULL to reset the rect to the full screen.
* @return None.
*/
extern (C) void MFInput_SetMouseClippingRect(int mouseID, ref const(MFRect) rect);

/**
* Generate a string representing the input button/device configuration.
* Generate a string representing the input button/device configuration.
* @param button The button or control on the device. Must be a member of the #MFGamepadButton, #MFMouseButton or #MFKey enumerated types.
* @param device The device to read the name from. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The device index.
* @param includeDevice If true, the device name is included in the generated string.
* @param includeDeviceID If true, the device index is included in the generated string.
* @return A string representing the button name and optionally the device name and index.
*/
extern (C) const(char*) MFInput_EnumerateString(int button, int device, int deviceID = 0, bool includeDevice = false, bool includeDeviceID = false);

/**
* Set the analog dead zone.
* Set the analog dead zone.
* @param deadZone The new analog dead zone.
* @return None.
*/
extern (C) void MFInput_SetDeadZone(float deadZone);

/**
* Get the analog dead zone.
* Get the analog dead zone.
* @return The current analog dead zone.
*/
extern (C) float MFInput_GetDeadZone();

/**
* Get a device name.
* Get a device name.
* @param device The device to fetch the name of. Must be a member of the #MFInputDevice enumerated type.
* @param deviceID The device index.
* @return The name of the target device.
*/
extern (C) const(char*) MFInput_GetDeviceName(int device, int deviceID);

/**
* Get the button name from a gamepad.
* Get the button name from a gamepad.
* @param button Gamepad button ID. Must be a member of the #MFGamepadButton enumerated type.
* @param deviceID Gamepad index.
* @return The name of the specified button of the specified device.
*/
extern (C) const(char*) MFInput_GetGamepadButtonName(int button, int deviceID);

/**
* Get the state of the keyboard status flags.
* Get the state of the keyboard status flags.
* @param keyboardState Value from the #MFKeyboardStatusState enumerated type.
* @param keyboardID Keyboard index.
* @return The state of the keyboards status bit.
*/
extern (C) bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID = -1);

/**
*
*/
extern (C) char MFInput_KeyToAscii(int key, bool shift, bool caps);

/*** HACK ***/
struct MFTouchPanelState
{
	struct Contact
	{
		int x, y;
		int tapCount;
		int phase;
		int flags;		
	}

	Contact contacts[20];
	int numContacts;

	bool bDidShake;
}

/**
*
*/
extern (C) MFTouchPanelState* MFInput_GetContactInfo(int touchPanel);

/**
* Input devices.
* These are available input devices.
*/
enum MFInputDevice
{
	Gamepad,		/**< Gamepad device */
	Mouse,			/**< Mouse or pointer device */
	Keyboard,		/**< Keyboard device */
	Accelerometer,	/**< Accelerometer device */
	TouchPanel		/**< Touch panel device */
}

/**
* Input device status.
* These are possible device states.
*/
enum MFInputDeviceStatus
{
	Unavailable = 0,	/**< Device is not available */
	Ready = 1,			/**< Device is ready for reading */
	Disconnected,		/**< Device is disconnected */
	Waiting				/**< Device is waiting for initialisation */
}

/**
* Gamepad Flags.
* Various flags available to certain types of gamepads.
*/
enum MFGamepadFlags : uint
{
	// Controller type
	TypeMask					= 0xF,
	Type_Gamepad				= 0x0,
	Type_Mouse					= 0x1,
	Type_Keyboard				= 0x2,
	Type_TouchScreen			= 0x3,
	Type_Joystick				= 0x4,
	Type_ArcadeStick			= 0x5,
	Type_Guitar					= 0x6,
	Type_Drums					= 0x7,
	
	// Controller flags
	FlagsMask					= 0xFFFF0,
	IsProxy						= 0x10,	// is some sort of proxy driver (like xbox1 drivers)
	IsAdapter					= 0x20,	// controller is some sort of adapter
	HasAudio					= 0x40,	// controller has an audio device
	HasDisplay					= 0x80,	// controller has an integrated display
	
	// Gamepad flags
	Gamepad_Has6FaceButtons		= 0x100,
	Gamepad_HasTriggers			= 0x200,
	Gamepad_Has2Triggers		= 0x400,
	Gamepad_HasAnalogStick		= 0x800,
	Gamepad_Has2AnalogSticks	= 0x1000,
	Gamepad_HasDPad				= 0x2000,
	Gamepad_HasVibration		= 0x4000,
	
	// Arcade stick flags
	Arcade_Has6FaceButtons		= 0x100,
	Arcade_Has8FaceButtons		= 0x200,
	
	// Guitar specific flags
	Guitar_HasTilt				= 0x100,
	Guitar_HasSolo				= 0x200,
	Guitar_HasPickupSwitch		= 0x400,
	Guitar_HasSlider			= 0x800,
	
	// Drum specific flags
	Drums_Has5Drums				= 0x100,
	
	// Controllers for other systems
	SystemMask					= 0x3F00000,		// 6 bits for target system
	Unknown						= 0 << 20,
	PS2							= 1 << 20,
	PS3							= 2 << 20,
	PS4							= 3 << 20,
	XBox						= 4 << 20,
	X360						= 5 << 20,
	XBOne						= 6 << 20,
	Nes							= 7 << 20,
	Snes						= 8 << 20,
	N64							= 9 << 20,
	GC							= 10 << 20,
	Wii							= 11 << 20,
	WiiU						= 12 << 20,
	Genesis						= 13 << 20,
	Saturn						= 14 << 20,
	DC							= 15 << 20,
	
	// System flags
	SystemFlagsMask				= 0xFC000000,
	DontUseSphericalDeadzone	= 0x4000000
}

/**
* Enums to access gamepad buttons.
* These represent the buttons on a gamepad.
*/
enum MFGamepadButton
{
	// PSX controller enums
	P2_Cross	= 0,	/**< Cross button on a Playstation or PS2 gamepad */
	P2_Circle	= 1,	/**< Circle button on a Playstation or PS2 gamepad */
	P2_Box		= 2,	/**< Box button on a Playstation or PS2 gamepad */
	P2_Triangle	= 3,	/**< Triangle button on a Playstation or PS2 gamepad */

	P2_L1		= 4,	/**< L1 trigger on a Playstation or PS2 gamepad */
	P2_R1		= 5,	/**< R1 trigger on a Playstation or PS2 gamepad */
	P2_L2		= 6,	/**< L2 trigger on a Playstation or PS2 gamepad */
	P2_R2		= 7,	/**< R2 trigger on a Playstation or PS2 gamepad */

	P2_Start	= 8,	/**< Start button on a Playstation or PS2 gamepad */
	P2_Select	= 9,	/**< Select button on a Playstation or PS2 gamepad */

	P2_L3		= 10,	/**< Left Thumbstick button on a Playstation or PS2 gamepad */
	P2_R3		= 11,	/**< Right Thumbstick button on a Playstation or PS2 gamepad */

	// PSP controller enums
	PP_Cross	= 0,	/**< Cross button on the PSP gamepad */
	PP_Circle	= 1,	/**< Circle button on the PSP gamepad */
	PP_Box		= 2,	/**< Box button on the PSP gamepad */
	PP_Triangle	= 3,	/**< Triangle button on the PSP gamepad */

	PP_L		= 6,	/**< L trigger on the PSP gamepad */
	PP_R		= 7,	/**< R trigger on the PSP gamepad */

	PP_Start	= 8,	/**< Start button on the PSP gamepad */
	PP_Select	= 9,	/**< Select button on the PSP gamepad */

	// XBox controller enums
	XB_A		= 0,	/**< A button on an XBox gamepad */
	XB_B		= 1,	/**< B button on an XBox gamepad */
	XB_X		= 2,	/**< X button on an XBox gamepad */
	XB_Y		= 3,	/**< Y button on an XBox gamepad */

	XB_White	= 4,	/**< White button on an XBox gamepad */
	XB_Black	= 5,	/**< Black button on an XBox gamepad */

	XB_LTrig	= 6,	/**< Left trigger on an XBox gamepad */
	XB_RTrig	= 7,	/**< Right trigger on an XBox gamepad */

	XB_Start	= 8,	/**< Start button on an XBox gamepad */
	XB_Back		= 9,	/**< Back button on an XBox gamepad */

	XB_LThumb	= 10,	/**< Left Thumbstick button on an XBox gamepad */
	XB_RThumb	= 11,	/**< Right Thumbstick button on an XBox gamepad */

	// XBox 360 controller enums
	X3_A		= 0,	/**< A button on an XBox 360 gamepad */
	X3_B		= 1,	/**< B button on an XBox 360 gamepad */
	X3_X		= 2,	/**< X button on an XBox 360 gamepad */
	X3_Y		= 3,	/**< Y button on an XBox 360 gamepad */

	X3_LB		= 4,	/**< White button on an XBox 360 gamepad */
	X3_RB		= 5,	/**< Black button on an XBox 360 gamepad */
	X3_LT		= 6,	/**< Left trigger on an XBox 360 gamepad */
	X3_RT		= 7,	/**< Right trigger on an XBox 360 gamepad */

	X3_Start	= 8,	/**< Start button on an XBox 360 gamepad */
	X3_Back		= 9,	/**< Back button on an XBox 360 gamepad */

	X3_LThumb	= 10,	/**< Left Thumbstick button on an XBox 360 gamepad */
	X3_RThumb	= 11,	/**< Right Thumbstick button on an XBox 360 gamepad */

	// GC controller enums
	GC_A			= 0,	/**< A button on a Gamecube gamepad */
	GC_X			= 1,	/**< X button on a Gamecube gamepad */
	GC_B			= 2,	/**< B button on a Gamecube gamepad */
	GC_Y			= 3,	/**< Y button on a Gamecube gamepad */

	GC_Z			= 5,	/**< Z button on a Gamecube gamepad */
	GC_L			= 6,	/**< Left Trigger button on a Gamecube gamepad */
	GC_R			= 7,	/**< Right Trigger button on a Gamecube gamepad */

	GC_Start		= 8,	/**< Start button on a Gamecube gamepad */

	// Dreamcast controller enums
	DC_A			= 0,	/**< A button on a Dreamcast gamepad */
	DC_B			= 1,	/**< B button on a Dreamcast gamepad */
	DC_X			= 2,	/**< X button on a Dreamcast gamepad */
	DC_Y			= 3,	/**< Y button on a Dreamcast gamepad */

	DC_L			= 6,	/**< Left trigger on a Dreamcast gamepad */
	DC_R			= 7,	/**< Right trigger on a Dreamcast gamepad */

	DC_Start		= 8,	/**< Start button on a Dreamcast gamepad */

	// General controller enums
	DUp = 12,				/**< Up on the digital pad */
	DDown,					/**< Down on the digital pad */
	DLeft,					/**< Left on the digital pad */
	DRight,					/**< Right on the digital pad */

	// Analog Axiis
	Axis_LX = 16,			/**< X-Axis on the Left analog stick */
	Axis_LY,				/**< Y-Axis on the Left analog stick */
	Axis_RX,				/**< X-Axis on the Right analog stick */
	Axis_RY,				/**< Y-Axis on the Right analog stick */

	// Extra buttons
	Button_Home = 20,

	Max,					/**< Maximum gamepad button id */

	// Guitar controller enums
	GH_Green		= P2_R2,		/**< Green key on Guitar controller */
	GH_Red			= P2_Circle,	/**< Red key on Guitar controller */
	GH_Yellow		= P2_Triangle,	/**< Yellow key on Guitar controller */
	GH_Blue			= P2_Cross,		/**< Blue key on Guitar controller */
	GH_Orange		= P2_Box,		/**< Orange key on Guitar controller */

	GH_Start		= P2_Start,		/**< Start button on Guitar controller */
	GH_Select		= P2_Select,	/**< Select button on Guitar controller */

	GH_Solo			= P2_L1,		/**< Solo buttons on Guitar controller */
	GH_TiltTrigger	= P2_L2,		/**< Vertical tilt trigger on Guitar controller */

	GH_StrumDown	= DDown,		/**< Strum down on Guitar controller */
	GH_StrumUp		= DUp,			/**< Strum up on Guitar controller */

	GH_Whammy		= Axis_LX,		/**< Whammy bar depression on Guitar controller */
	GH_Tilt			= Axis_RY,		/**< Analog vertical tilt sensor on Guitar controller */

	GH_Roll			= Axis_LY,		/**< Jump/Roll sensor on Guitar controller */
	GH_PickupSwitch	= Axis_RX,		/**< Pickup Switch on Guitar controller */
	GH_Slider		= Axis_RX,		/**< Slider on Guitar controller */

	// Rock Band Drums enums
	Drum_Red	= P2_Circle,	/**< Red drum on Drum controller */
	Drum_Yellow	= P2_Triangle,	/**< Red drum on Drum controller */
	Drum_Blue	= P2_Box,		/**< Yellow drum on Drum controller */
	Drum_Green	= P2_Cross,		/**< Blue drum on Drum controller */
	Drum_Kick	= P2_L1,		/**< Kick pedal on Drum controller */

	// Guitar Hero: World Tour enums
	Drum_Hat	= Drum_Yellow,	/**< Hi-Hat drum on Drum controller */
	Drum_Snare	= Drum_Red,		/**< Snare on Drum controller */
	Drum_Tom1	= Drum_Blue,	/**< Tom-1 on Drum controller */
	Drum_Tom2	= Drum_Green,	/**< Tom-2 on Drum controller */
	Drum_Cymbal	= P2_R1,		/**< Cymbal on Drum controller */

	Drum_Start	= P2_Start,		/**< Start button on Drum controller */
	Drum_Select	= P2_Select		/**< Select button on Drum controller */
}

/**
* Mouse access modes.
* These are mouse access modes.
*/
enum MFMouseMode
{
	Shared,		/**< Continue to share the mouse with the system */
	Exclusive	/**< The game take exclusive access to the mouse, (not applicable to consoles) */
}

/**
* Enums to access mouse buttons.
* These represent the buttons on a mouse.
*/
enum MFMouseButton
{
	XPos,		/**< The mouses absolute position on the X-Axis */
	YPos,		/**< The mouses absolute position on the Y-Axis */

	XDelta,		/**< The mouses movement delta on the X-Axis */
	YDelta,		/**< The mouses movement delta on the Y-Axis */

	Wheel,		/**< The mouses wheel delta */
	Wheel2,		/**< The mouses second wheel delta (rare on mouses) */

	LeftButton,					/**< Left mouse button */
	RightButton,				/**< Right mouse button */
	MiddleButton,				/**< Middle mouse button */
	Extra1,						/**< Extra mouse button 1 (often maps to 'back') */
	Extra2,						/**< Extra mouse button 2 (often maps to 'forward') */
	Extra3,						/**< Extra mouse button 3 */
	Extra4,						/**< Extra mouse button 4 */
	Extra5,						/**< Extra mouse button 5 */

	Max,							/**< Maximum mouse input */
	MaxAxis = LeftButton,			/**< Max axis enum */
	MaxButtons = Max - LeftButton	/**< Number of mouse buttons */
}

/**
* Keyboard status flags.
* These represent the various state keys on the keyboard.
*/
enum MFKeyboardStatusState
{
	NumLock,		/**< Numlock state */
	CapsLock,		/**< Capslock state */
	ScrollLock,		/**< Scroll lock state */
	Insert			/**< Insert state */
}

/**
* Keyboard key enums.
* These represent all the keys on the keyboard.
*/
enum MFKey
{
	Unknown = -1,	/**< Unknown key */

	None = 0,		/**< Represents no key */
	Up,				/**< Up arrow key */
	Down,			/**< Down arrow key */
	Left,			/**< Left arrow key */
	Right,			/**< Right arrow key */
	Apps,			// on windows keyboards
	LWin,			// on windows keyboards
	RWin,			// on windows keyboards
	Backspace,
	Tab,
	PrintScreen,
	ScrollLock,
	Break,
	Return,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	OEM_102,		// on german keyboard
	Escape,
	MyComputer,		// on multimedia keyboards
	Mail,			// on multimedia keyboards
	Calculator,		// on multimedia keyboards
	Stop,			// japanese keyboard
	Space,
	Insert,
	Delete,
	Home,
	End,
	PageUp,
	PageDown,
	Apostrophe,
	ABNT_C1,		// on brazilian keyboard
	ABNT_C2,		// on brazilian keyboard
	Asterisk,
	Plus,
	Comma,
	Hyphen,
	Period,
	Slash,
	_0,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	_9,
	NumPadComma,	// japanese keyboard
	Semicolon,
	NumPadEquals,	// japanese keyboard
	Equals,
	Sleep,			// on windows keyboards
	Wake,			// on windows keyboards
	Power,			// on windows keyboards
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Underline,		// japanese keyboard
	LBracket,
	Backslash,
	RBracket,
	F13,			// japanese keyboard
	F14,			// japanese keyboard
	Grave,
	F15,			// japanese keyboard
	Unlabeled,		// japanese keyboard
	LControl,
	LAlt,
	LShift,
	RControl,
	RAlt,
	RShift,
	CapsLock,

	NumLock,
	Divide,
	Subtract,
	Decimal,

	NumPad0,
	NumPad1,
	NumPad2,
	NumPad3,
	NumPad4,
	NumPad5,
	NumPad6,
	NumPad7,
	NumPad8,
	NumPad9,
	NumPadEnter,

	PlayPause,		// on multimedia keyboards
	MediaStop,		// on multimedia keyboards
	MediaSelect,	// on multimedia keyboards
	NextTrack,		// on multimedia keyboards
	PrevTrack,		// on multimedia keyboards

	VolumeDown,		// on multimedia keyboards
	VolumeUp,		// on multimedia keyboards
	Mute,			// on multimedia keyboards

	WebBack,		// on multimedia keyboards
	WebFavorites,	// on multimedia keyboards
	WebForward,		// on multimedia keyboards
	WebHome,		// on multimedia keyboards
	WebRefresh,		// on multimedia keyboards
	WebSearch,		// on multimedia keyboards
	WebStop,		// on multimedia keyboards

	AT,				// japanese keyboard
	AX,				// japanese keyboard
	Colon,			// japanese keyboard
	Convert,		// japanese keyboard
	Kana,			// japanese keyboard
	Kanji,			// japanese keyboard
	NoConvert,		// japanese keyboard
	Yen,			// japanese keyboard

	Max
}

/**
* Accelerometer input.
* These represent available accelerometer readings.
*/
enum MFAccelerometerInput
{
	XAxis,
	YAxis,
	ZAxis,
	Acceleration,

	XDelta,
	YDelta,
	ZDelta,
	AccelerationDelta
}

/**
* Touch panel input.
* These represent available touch panel input readings.
*/
enum MFTouchPanelInput
{
	Contact0_XPos,
	Contact0_YPos,
	Contact0_XDelta,
	Contact0_YDelta,
	Contact1_XPos,
	Contact1_YPos,
	Contact1_XDelta,
	Contact1_YDelta,
	Contact2_XPos,
	Contact2_YPos,
	Contact2_XDelta,
	Contact2_YDelta,
	Contact3_XPos,
	Contact3_YPos,
	Contact3_XDelta,
	Contact3_YDelta,
	Contact4_XPos,
	Contact4_YPos,
	Contact4_XDelta,
	Contact4_YDelta,
	Contact5_XPos,
	Contact5_YPos,
	Contact5_XDelta,
	Contact5_YDelta,
	Shake
}

enum MFTouch_XPos(alias contact) = MFTouchPanelInput.Contact0_XPos + (MFTouchPanelInput.Contact1_XPos - MFTouchPanelInput.Contact0_XPos)*contact;
enum MFTouch_YPos(alias contact) = MFTouchPanelInput.Contact0_YPos + (MFTouchPanelInput.Contact1_YPos - MFTouchPanelInput.Contact0_YPos)*contact;
enum MFTouch_XDelta(alias contact) = MFTouchPanelInput.Contact0_XDelta + (MFTouchPanelInput.Contact1_XDelta - MFTouchPanelInput.Contact0_XDelta)*contact;
enum MFTouch_YDelta(alias contact) = MFTouchPanelInput.Contact0_YDelta + (MFTouchPanelInput.Contact1_YDelta - MFTouchPanelInput.Contact0_YDelta)*contact;

MFTouchPanelInput MFTouch_XPos(int contact) { return cast(MFTouchPanelInput)(MFTouchPanelInput.Contact0_XPos + (MFTouchPanelInput.Contact1_XPos - MFTouchPanelInput.Contact0_XPos)*contact); }
MFTouchPanelInput MFTouch_YPos(int contact) { return cast(MFTouchPanelInput)(MFTouchPanelInput.Contact0_YPos + (MFTouchPanelInput.Contact1_YPos - MFTouchPanelInput.Contact0_YPos)*contact); }
MFTouchPanelInput MFTouch_XDelta(int contact) { return cast(MFTouchPanelInput)(MFTouchPanelInput.Contact0_XDelta + (MFTouchPanelInput.Contact1_XDelta - MFTouchPanelInput.Contact0_XDelta)*contact); }
MFTouchPanelInput MFTouch_YDelta(int contact) { return cast(MFTouchPanelInput)(MFTouchPanelInput.Contact0_YDelta + (MFTouchPanelInput.Contact1_YDelta - MFTouchPanelInput.Contact0_YDelta)*contact); }

