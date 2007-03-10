#include "Fuji.h"
#include "MFInput.h"

#include "MFInputMappings_PC.h"

// Button Mappings
static const int gStandardButtonID[GamepadType_Max] =
{
	0,   // Button_A
	1,   // Button_B
	2,   // Button_X
	3,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	6,   // Button_LeftTrigger
	7,   // Button_RightTrigger
	8,   // Button_Start
	9,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	12 | POV_Up,	// Button_DUp
	13 | POV_Down,	// Button_DDown
	14 | POV_Left,	// Button_DLeft
	15 | POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rx,               // Button_ThumbRX
	AID_Ry | AID_Negative // Button_ThumbRY
};

static const int gEMSButtonID[GamepadType_Max] =
{
	2,   // Button_A
	1,   // Button_B
	3,   // Button_X
	0,   // Button_Y
	6,   // Button_White
	7,   // Button_Black
	4,   // Button_LeftTrigger
	5,   // Button_RightTrigger
	9,   // Button_Start
	8,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	12,  // Button_DUp
	14,  // Button_DDown
	15,  // Button_DLeft
	13,  // Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rz,               // Button_ThumbRX
	AID_Z | AID_Negative  // Button_ThumbRY
};

static const int gTitaniumButtonID[GamepadType_Max] =
{
	2,   // Button_A
	1,   // Button_B
	3,   // Button_X
	0,   // Button_Y
	6,   // Button_White
	7,   // Button_Black
	4,   // Button_LeftTrigger
	5,   // Button_RightTrigger
	8,   // Button_Start
	9,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	12 | POV_Up,	// Button_DUp
	14 | POV_Down,	// Button_DDown
	15 | POV_Left,	// Button_DLeft
	13 | POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Z,                // Button_ThumbRX
	AID_Rz | AID_Negative // Button_ThumbRY
};

static const int gSuperDualBoxButtonID[GamepadType_Max] =
{
	2,   // Button_A
	1,   // Button_B
	3,   // Button_X
	0,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	6,   // Button_LeftTrigger
	7,   // Button_RightTrigger
	8,   // Button_Start
	9,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rz,               // Button_ThumbRX
	AID_Z | AID_Negative  // Button_ThumbRY
};

static const int gXBCDButtonID[GamepadType_Max] =
{
	0,   // Button_A
	1,   // Button_B
	2,   // Button_X
	3,   // Button_Y
	5,   // Button_White
	4,   // Button_Black
	10 | AID_Z,  // Button_LeftTrigger  // if Z axis is present, the L-Trigger is in analog mode, if not, use button 10
	11 | AID_Rz, // Button_RightTrigger // if Rz axis is present the R-Trigger is in analog mode, if not, use button 11
	6,   // Button_Start
	7,   // Button_Back
	8,   // Button_LeftThumb
	9,   // Button_RightThumb
	12 | POV_Up,	// Button_DUp
	14 | POV_Down,	// Button_DDown
	15 | POV_Left,	// Button_DLeft
	13 | POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rx,               // Button_ThumbRX
	AID_Ry | AID_Negative // Button_ThumbRY
};

static const int gLogitechDualActionButtonID[GamepadType_Max] =
{
	1,   // Button_A
	2,   // Button_B
	0,   // Button_X
	3,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	6,   // Button_LeftTrigger  // if Z axis is present, the L-Trigger is in analog mode, if not, use button 10
	7,   // Button_RightTrigger // if Rz axis is present the R-Trigger is in analog mode, if not, use button 11
	9,   // Button_Start
	8,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Z,                // Button_ThumbRX
	AID_Rz | AID_Negative // Button_ThumbRY
};

static const int gXBox360ButtonID[GamepadType_Max] =
{
	0,   // Button_A
	1,   // Button_B
	2,   // Button_X
	3,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	AID_Z | AID_Clamp,				// Button_LeftTrigger  // use negative range of clamped z axis
	AID_Z | AID_Negative | AID_Clamp,	// Button_RightTrigger // use positive range of clamped z axis
	7,   // Button_Start
	6,   // Button_Back
	8,   // Button_LeftThumb
	9,   // Button_RightThumb
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rx,               // Button_ThumbRX
	AID_Ry | AID_Negative // Button_ThumbRY
};

static const int gXterminatorButtonID[GamepadType_Max] =
{
	0,   // Button_A
	1,   // Button_B
	3,   // Button_X
	4,   // Button_Y
	6,   // Button_White
	7,   // Button_Black
	AID_Rz | AID_Negative | AID_Clamp,				// Button_LeftTrigger  // use negative range of clamped z axis
	AID_Rz | AID_Clamp,	// Button_RightTrigger // use positive range of clamped z axis
	9,   // Button_Start
	8,   // Button_Back
	5,  // Button_LeftThumb
	2,  // Button_RightThumb
	AID_Rx | AID_Negative | AID_Clamp,  // Button_DUp
	AID_Rx | AID_Clamp,  // Button_DDown
	AID_Slider1 | AID_Negative | AID_Clamp,  // Button_DLeft
	AID_Slider1 | AID_Clamp,  // Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	-1, // Button_ThumbRX
	-1  // Button_ThumbRY
};

static const int gLogitechWingmanGamepadExtremeID[GamepadType_Max] =
{
	0,   // Button_A
	1,   // Button_B
	3,   // Button_X
	4,   // Button_Y
	5,   // Button_White
	2,   // Button_Black
	6,	 // Button_LeftTrigger
	7,	 // Button_RightTrigger
	8,   // Button_Start
	9,   // Button_Back
	-1,  // Button_LeftThumb
	-1,  // Button_RightThumb
	AID_Y | AID_Negative | AID_Clamp,  // Button_DUp
	AID_Y | AID_Clamp,  // Button_DDown
	AID_X | AID_Negative | AID_Clamp,  // Button_DLeft
	AID_X | AID_Clamp,  // Button_DRight
	-1, // Button_ThumbLX
	-1, // Button_ThumbLY
	-1, // Button_ThumbRX
	-1  // Button_ThumbRY
};

static const int gPowerWaveButtonID[GamepadType_Max] =
{
	0,   // Button_A
	1,   // Button_B
	2,   // Button_X
	3,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	6,   // Button_LeftTrigger  // if Z axis is present, the L-Trigger is in analog mode, if not, use button 10
	7,   // Button_RightTrigger // if Rz axis is present the R-Trigger is in analog mode, if not, use button 11
	9,   // Button_Start
	8,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Z,                // Button_ThumbRX
	AID_Rz | AID_Negative // Button_ThumbRY
};

static const int gGreenAsiaButtonID[GamepadType_Max] =
{
	2,   // Button_A
	3,   // Button_B
	0,   // Button_X
	1,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	6,   // Button_LeftTrigger  // if Z axis is present, the L-Trigger is in analog mode, if not, use button 10
	7,   // Button_RightTrigger // if Rz axis is present the R-Trigger is in analog mode, if not, use button 11
	9,   // Button_Start
	8,   // Button_Back
	-1,  // Button_LeftThumb
	-1,  // Button_RightThumb
	AID_Y | AID_Negative | AID_Clamp,  // Button_DUp
	AID_Y | AID_Clamp,  // Button_DDown
	AID_X | AID_Negative | AID_Clamp,  // Button_DLeft
	AID_X | AID_Clamp,  // Button_DRight
	-1,	// Button_ThumbLX
	-1,	// Button_ThumbLY
	-1,	// Button_ThumbRX
	-1	// Button_ThumbRY
};

static const int gRadioshackButtonID[GamepadType_Max] =
{
	2,   // Button_A
	1,   // Button_B
	3,   // Button_X
	0,   // Button_Y
	6,   // Button_White
	7,   // Button_Black
	4,   // Button_LeftTrigger
	5,   // Button_RightTrigger
	9,   // Button_Start
	8,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rz,               // Button_ThumbRX
	AID_Z | AID_Negative  // Button_ThumbRY
};

static const int gGenericPS2ButtonID[GamepadType_Max] =
{
	2,	// Button_A
	1,	// Button_B
	3,	// Button_X
	0,	// Button_Y
	6,	// Button_White
	7,	// Button_Black
	4,	// Button_LeftTrigger
	5,	// Button_RightTrigger
	11,	// Button_Start
	8,	// Button_Back
	9,	// Button_LeftThumb
	10,	// Button_RightThumb
	12,	// Button_DUp
	14,	// Button_DDown
	15,	// Button_DLeft
	13,	// Button_DRight
	AID_X,					// Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Z,					// Button_ThumbRX
	AID_Rz | AID_Negative,	// Button_ThumbRY
};

// Button Names
static const char * gStandardButtonNames[GamepadType_Max] =
{
	"Button 0",     // Button_A
	"Button 1",     // Button_B
	"Button 2",     // Button_X
	"Button 3",     // Button_Y
	"Button 4",     // Button_White
	"Button 5",     // Button_Black
	"Button 6",     // Button_LeftTrigger
	"Button 7",     // Button_RightTrigger
	"Button 8",     // Button_Start
	"Button 9",     // Button_Back
	"Button 10",    // Button_LeftThumb
	"Button 11",    // Button_RightThumb
	"POV Up",       // Button_DUp
	"POV Down",     // Button_DDown
	"POV Left",     // Button_DLeft
	"POV Right",    // Button_DRight
	"Analog X-Axis",  // Button_ThumbLX
	"Analog Y-Axis",  // Button_ThumbLY
	"Analog RX-Axis", // Button_ThumbRX
	"Analog RY-Axis"  // Button_ThumbRY
};

static const char * gPS2ButtonNames[GamepadType_Max] =
{
	"Cross",        // Button_A
	"Circle",       // Button_B
	"Box",          // Button_X
	"Triangle",     // Button_Y
	"L1",           // Button_White
	"R1",           // Button_Black
	"L2",           // Button_LeftTrigger
	"R2",           // Button_RightTrigger
	"Start",        // Button_Start
	"Select",       // Button_Back
	"L3",           // Button_LeftThumb
	"R3",           // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis"  // Button_ThumbRY
};

static const char * gXBoxButtonNames[GamepadType_Max] =
{
	"A",            // Button_A
	"B",            // Button_B
	"X",            // Button_X
	"Y",            // Button_Y
	"White",        // Button_White
	"Black",        // Button_Black
	"L-Trigger",    // Button_LeftTrigger
	"R-Trigger",    // Button_RightTrigger
	"Start",        // Button_Start
	"Back",         // Button_Back
	"L-Thumb",      // Button_LeftThumb
	"R-Thumb",      // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis"  // Button_ThumbRY
};

static const char * gLogitechDualActionButtonNames[GamepadType_Max] =
{
	"2",            // Button_A
	"3",            // Button_B
	"1",            // Button_X
	"4",            // Button_Y
	"5",            // Button_White
	"6",            // Button_Black
	"7",            // Button_LeftTrigger
	"8",            // Button_RightTrigger
	"10",           // Button_Start
	"9",            // Button_Back
	"L-Thumb",      // Button_LeftThumb
	"R-Thumb",      // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis"  // Button_ThumbRY
};

static const char * gXBox360ButtonNames[GamepadType_Max] =
{
	"A",            // Button_A
	"B",            // Button_B
	"X",            // Button_X
	"Y",            // Button_Y
	"LB",           // Button_White
	"RB",           // Button_Black
	"LT",           // Button_LeftTrigger
	"RT",           // Button_RightTrigger
	"Start",        // Button_Start
	"Back",         // Button_Back
	"L-Thumb",      // Button_LeftThumb
	"R-Thumb",      // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis"  // Button_ThumbRY
};

static const char * gXterminatorButtonNames[GamepadType_Max] =
{
	"1",            // Button_A
	"2",            // Button_B
	"4",            // Button_X
	"5",            // Button_Y
	"L-Grip",       // Button_White
	"R-Grip",       // Button_Black
	"L-Trigger",    // Button_LeftTrigger
	"R-Trigger",    // Button_RightTrigger
	"S",            // Button_Start
	"Top",          // Button_Back
	"6",            // Button_LeftThumb
	"3",            // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis"  // Button_ThumbRY
};

static const char * gLogitechWingmanGamepadExtremeNames[GamepadType_Max] =
{
	"A",			// Button_A
	"B",			// Button_B
	"X",			// Button_X
	"Y",			// Button_Y
	"Z",			// Button_White
	"C",			// Button_Black
	"L",			// Button_LeftTrigger
	"R",			// Button_RightTrigger
	"Start",		// Button_Start
	"Red",			// Button_Back
	"Unavailable",  // Button_LeftThumb
	"Unavailable",  // Button_RightThumb
	"Up",			// Button_DUp
	"Down",			// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Unavailable",	// Button_ThumbLX
	"Unavailable",	// Button_ThumbLY
	"Unavailable",	// Button_ThumbRX
	"Unavailable"	// Button_ThumbRY
};

static const char * gPowerWaveButtonNames[GamepadType_Max] =
{
	"1",            // Button_A
	"2",            // Button_B
	"3",            // Button_X
	"4",            // Button_Y
	"L1",           // Button_White
	"R1",           // Button_Black
	"L2",           // Button_LeftTrigger
	"R2",           // Button_RightTrigger
	"Start",        // Button_Start
	"Select",       // Button_Back
	"L-Thumb",      // Button_LeftThumb
	"R-Thumb",      // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis"  // Button_ThumbRY
};

static const char * gGreenAsiaButtonNames[GamepadType_Max] =
{
	"3",            // Button_A
	"4",            // Button_B
	"1",            // Button_X
	"2",            // Button_Y
	"5",			// Button_White
	"6",			// Button_Black
	"7",			// Button_LeftTrigger
	"8",			// Button_RightTrigger
	"10",			// Button_Start
	"9",			// Button_Back
	"Unavailable",	// Button_LeftThumb
	"Unavailable",	// Button_RightThumb
	"Up",			// Button_DUp
	"Down",			// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Unavailable",	// Button_ThumbLX
	"Unavailable",	// Button_ThumbLY
	"Unavailable",	// Button_ThumbRX
	"Unavailable"	// Button_ThumbRY
};

// Gamepad Info
static MFGamepadInfo gGamepadDescriptors[] =
{
	// Default mappings
	{
		"Unknown Gamepad",
		"",
		0, 0,
		gStandardButtonID,
		gStandardButtonNames,
		&gGamepadDescriptors[1]
	},

	// XBox 360 wired gamepad
	{
		"XBox 360 Gamepad",
		"XBOX 360 For Windows (Controller)",
		0x045E, 0x028E,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		&gGamepadDescriptors[2]
	},

	// XBox 360 wireless gamepad
	{
		"XBox 360 Gamepad",
		"Controller (Xbox 360 Wireless Receiver for Windows)",
		0x045E, 0x0719,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		&gGamepadDescriptors[3]
	},

	// XBox 360 DOA Arcade Stick
	{
		"DOA Arcade Stick",
		"Arcade Stick (Dead or Alive4Stick)",
		0x0F0D, 0x000A,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		&gGamepadDescriptors[4]
	},

	// EMS USB2 adapter
	{
		"PS2 Gamepad",
		"EMS USB2",
		0x0B43, 0x0003,
		gEMSButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[5]
	},

	// Different identities of the titanium adapters
	{
		"PS2 Gamepad",
		"SmartJoy PLUS USB Adapter",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[6]
	},

	{
		"PS2 Gamepad",
		"SmartJoy PLUS Adapter",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[7]
	},

	{
		"PS2 Gamepad",
		"USB Force Feedback Joypad (MP-8888)",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[8]
	},

	{
		"PS2 Gamepad",
		"MP-8888 USB Joypad",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[9]
	},

	{
		"PS2 Gamepad",
		"Dual USB Force Feedback Joypad (MP-8866)",
		0, 0,
		gSuperDualBoxButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[10]
	},

	// XBox gamepad
	{
		"XBox Gamepad",
		"XBCD XBox Gamepad",
		0, 0,
		gXBCDButtonID,
		gXBoxButtonNames,
		&gGamepadDescriptors[11]
	},

	// Logitech dual action
	{
		"Logitech Dual Action",
		"Logitech Dual Action",
		0, 0,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		&gGamepadDescriptors[12]
	},

	// Logitech dual action
	{
		"Logitech Dual Action",
		"Logitech Dual Action USB",
		0, 0,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		&gGamepadDescriptors[13]
	},

	// Xterminator Digital Gamepad
	{
		"Gravis Xterminator",
		"Xterminator Digital Gamepad",
		0, 0,
		gXterminatorButtonID,
		gXterminatorButtonNames,
		&gGamepadDescriptors[14]
	},

	// Logitech Wingman Gamepad Extreme (Gameport)
	{
		"WingMan Gamepad Extreme",
		"Logitech WingMan Gamepad Extreme",
		0, 0,
		gLogitechWingmanGamepadExtremeID,
		gLogitechWingmanGamepadExtremeNames,
		&gGamepadDescriptors[15]
	},

	// Logitech Wingman Gamepad Extreme (USB)
	{
		"WingMan Gamepad Extreme",
		"Logitech WingMan Gamepad Extreme (USB)",
		0, 0,
		gLogitechWingmanGamepadExtremeID,
		gLogitechWingmanGamepadExtremeNames,
		&gGamepadDescriptors[16]
	},

	// Logitech RumblePad 2
	{
		"Logitech RumblePad 2",
		"Logitech RumblePad 2 USB",
		0, 0,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		&gGamepadDescriptors[17]
	},

	// 'USB Joystick' basic PS2 style USB gamepad
	{
		"USB Gamepad",
		"'GreenAsia' USB Joystick",
		0x0E8F, 0x0002,
		gGreenAsiaButtonID,
		gGreenAsiaButtonNames,
		&gGamepadDescriptors[18]
	},

	// Radioshack 'USB Dual Vibration Joystick' another PS2 gamepad adapter,
	{
		"PS2 Gamepad",
		"USB Dual Vibration Joystick",
		0x0E8F, 0x0003,
		gRadioshackButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[19]
	},

	// POWER))WAVE PS2 style gamepad
	{
		"PowerWave PS2 Style Gamepad",
		"USB Joystick",
		0, 0,
		gPowerWaveButtonID,
		gPowerWaveButtonNames,
		&gGamepadDescriptors[20]
	},

	// Some more PS2 adapters
	{
		"PS2 Gamepad",
		"4 axis 16 button joystick",
		0x6666, 0x0667,
		gGenericPS2ButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[21]
	},

	{
		"PS2 Gamepad",
		"GIC USB Joystick",
		0x3427, 0x1190,
		gGenericPS2ButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[22]
	},

	{
		"PS2 Gamepad",
		"SmartJoy Dual PLUS Adapter",
		0x6677, 0x8802,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[23]
	},

	// Logitech Cordless RumblePad 2
	{
		"RumblePad 2",
		"Logitech Cordless RumblePad 2",
		0x046d, 0xc219,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		NULL
	}
};

MFGamepadInfo *pGamepadMappingRegistry = gGamepadDescriptors;
