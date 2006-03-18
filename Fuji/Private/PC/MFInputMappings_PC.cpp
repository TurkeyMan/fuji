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
	12,  // Button_DUp
	13,  // Button_DDown
	14,  // Button_DLeft
	15,  // Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rx,               // Button_ThumbRX
	AID_Ry | AID_Negative // Button_ThumbRY
};

static const int gPS2ButtonID[GamepadType_Max] = 
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
	-1,  // Button_DUp
	-1,  // Button_DDown
	-1,  // Button_DLeft
	-1,  // Button_DRight
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
	-1,  // Button_DUp
	-1,  // Button_DDown
	-1,  // Button_DLeft
	-1,  // Button_DRight
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
	12,  // Button_DUp
	14,  // Button_DDown
	15,  // Button_DLeft
	13,  // Button_DRight
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
	-1,  // Button_DUp
	-1,  // Button_DDown
	-1,  // Button_DLeft
	-1,  // Button_DRight
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
	-1,  // Button_DUp
	-1,  // Button_DDown
	-1,  // Button_DLeft
	-1,  // Button_DRight
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
	"A",            // Button_A
	"B",            // Button_B
	"X",            // Button_X
	"Y",            // Button_Y
	"Z",			// Button_White
	"C",			// Button_Black
	"L",			// Button_LeftTrigger
	"R",			// Button_RightTrigger
	"Start",		// Button_Start
	"Red",          // Button_Back
	"Unavailable",  // Button_LeftThumb
	"Unavailable",  // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Unavailable",  // Button_ThumbLX
	"Unavailable",  // Button_ThumbLY
	"Unavailable",  // Button_ThumbRX
	"Unavailable"   // Button_ThumbRY
};

// Gamepad Info
static MFGamepadInfo gGamepadDescriptors[] =
{
	// default mappings
	{
		"Unknown Gamepad",
		"",
		gStandardButtonID,
		gStandardButtonNames,
		true,
		&gGamepadDescriptors[1]
	},

	// standard PS2 adapter
	{
		"PS2 Gamepad",
		"4 axis 16 button joystick",
		gPS2ButtonID,
		gPS2ButtonNames,
		false,
		&gGamepadDescriptors[2]
	},

	// different identities of the titanium adapters
	{
		"PS2 Gamepad",
		"SmartJoy PLUS USB Adapter",
		gTitaniumButtonID,
		gPS2ButtonNames,
		true,
		&gGamepadDescriptors[3]
	},

	{
		"PS2 Gamepad",
		"SmartJoy PLUS Adapter",
		gTitaniumButtonID,
		gPS2ButtonNames,
		true,
		&gGamepadDescriptors[4]
	},

	{
		"PS2 Gamepad",
		"USB Force Feedback Joypad (MP-8888)",
		gTitaniumButtonID,
		gPS2ButtonNames,
		true,
		&gGamepadDescriptors[5]
	},

	{
		"PS2 Gamepad",
		"MP-8888 USB Joypad",
		gTitaniumButtonID,
		gPS2ButtonNames,
		true,
		&gGamepadDescriptors[6]
	},

	{
		"PS2 Gamepad",
		"Dual USB Force Feedback Joypad (MP-8866)",
		gSuperDualBoxButtonID,
		gPS2ButtonNames,
		true,
		&gGamepadDescriptors[7]
	},

	// xbox gamepad
	{
		"XBox Gamepad",
		"XBCD XBox Gamepad",
		gXBCDButtonID,
		gXBoxButtonNames,
		true,
		&gGamepadDescriptors[8]
	},

	// logitech dual action
	{
		"Logitech Dual Action",
		"Logitech Dual Action",
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		true,
		&gGamepadDescriptors[9]
	},

	// xbox 360 wired gamepad
	{
		"XBox 360 Gamepad",
		"XBOX 360 For Windows (Controller)",
		gXBox360ButtonID,
		gXBox360ButtonNames,
		true,
		&gGamepadDescriptors[10]
	},

	// Xterminator Digital Gamepad
	{
		"Gravis Xterminator",
		"Xterminator Digital Gamepad",
		gXterminatorButtonID,
		gXterminatorButtonNames,
		false,
		&gGamepadDescriptors[11]
	},

	// Logitech Wingman Gamepad Extreme (Gameport)
	{
		"WingMan Gamepad Extreme",
		"Logitech WingMan Gamepad Extreme",
		gLogitechWingmanGamepadExtremeID,
		gLogitechWingmanGamepadExtremeNames,
		false,
		&gGamepadDescriptors[12]
	},

	// Logitech Wingman Gamepad Extreme (USB)
	{
		"WingMan Gamepad Extreme",
		"Logitech WingMan Gamepad Extreme (USB)",
		gLogitechWingmanGamepadExtremeID,
		gLogitechWingmanGamepadExtremeNames,
		false,
		NULL
	}
};

MFGamepadInfo *pGamepadMappingRegistry = gGamepadDescriptors;