#include "Fuji_Internal.h"

#if MF_INPUT == MF_DRIVER_PC || MF_INPUT == MF_DRIVER_LINUX

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
	AID_X,                 // Button_ThumbLX
	AID_Y | AID_Negative,  // Button_ThumbLY
	AID_Rx,                // Button_ThumbRX
	AID_Ry | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
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
	AID_Z | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
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
	AID_X,                 // Button_ThumbLX
	AID_Y | AID_Negative,  // Button_ThumbLY
	AID_Z,                 // Button_ThumbRX
	AID_Rz | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
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
	AID_Z | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
};

static const int gGGE909ButtonID[GamepadType_Max] =
{
	2,   // Button_A
	1,   // Button_B
	3,   // Button_X
	0,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	6,   // Button_LeftTrigger
	7,   // Button_RightTrigger
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
	AID_Z | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
};

static const int gGGE909VariantButtonID[GamepadType_Max] =
{
	2,   // Button_A
	1,   // Button_B
	3,   // Button_X
	0,   // Button_Y
	4,   // Button_White
	5,   // Button_Black
	6,   // Button_LeftTrigger
	7,   // Button_RightTrigger
	9,   // Button_Start
	8,   // Button_Back
	10,  // Button_LeftThumb
	11,  // Button_RightThumb
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,                 // Button_ThumbLX
	AID_Y | AID_Negative,  // Button_ThumbLY
	AID_Z,                 // Button_ThumbRX
	AID_Rz | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
};

static const int gXBCDButtonID[GamepadType_Max] =
{
	0,   // Button_A
	1,   // Button_B
	2,   // Button_X
	3,   // Button_Y
	5,   // Button_White
	4,   // Button_Black
	10,// | AID_Z,  // Button_LeftTrigger  // if Z axis is present, the L-Trigger is in analog mode, if not, use button 10
	11,// | AID_Rz, // Button_RightTrigger // if Rz axis is present the R-Trigger is in analog mode, if not, use button 11
	6,   // Button_Start
	7,   // Button_Back
	8,   // Button_LeftThumb
	9,   // Button_RightThumb
	12 | POV_Up,	// Button_DUp
	14 | POV_Down,	// Button_DDown
	15 | POV_Left,	// Button_DLeft
	13 | POV_Right,	// Button_DRight
	AID_X,                 // Button_ThumbLX
	AID_Y | AID_Negative,  // Button_ThumbLY
	AID_Rx,                // Button_ThumbRX
	AID_Ry | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
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
	AID_X,                 // Button_ThumbLX
	AID_Y | AID_Negative,  // Button_ThumbLY
	AID_Z,                 // Button_ThumbRX
	AID_Rz | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
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
	AID_X,                 // Button_ThumbLX
	AID_Y | AID_Negative,  // Button_ThumbLY
	AID_Rx,                // Button_ThumbRX
	AID_Ry | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
};

static const int gXGamepadButtonID[GamepadType_Max] =
{
	12, // Button_A
	13, // Button_B
	14, // Button_X
	15, // Button_Y
	8,  // Button_White
	9,  // Button_Black
	AID_Z,	// Button_LeftTrigger
	AID_Rz,	// Button_RightTrigger
	4,  // Button_Start
	5,  // Button_Back
	6,  // Button_LeftThumb
	7,  // Button_RightThumb
	0,	// Button_DUp
	1,	// Button_DDown
	2,	// Button_DLeft
	3,	// Button_DRight
	AID_X,	// Button_ThumbLX
	AID_Y,	// Button_ThumbLY
	AID_Rx,	// Button_ThumbRX
	AID_Ry, // Button_ThumbRY
	-1  // Button_Home
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
	5,   // Button_LeftThumb
	2,   // Button_RightThumb
	AID_Rx | AID_Negative | AID_Clamp,  // Button_DUp
	AID_Rx | AID_Clamp,  // Button_DDown
	AID_Slider1 | AID_Negative | AID_Clamp,  // Button_DLeft
	AID_Slider1 | AID_Clamp,  // Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	-1,   // Button_ThumbRX
	-1,   // Button_ThumbRY
	-1    // Button_Home
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
	-1, // Button_ThumbRY
	-1  // Button_Home
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
	AID_X,                 // Button_ThumbLX
	AID_Y | AID_Negative,  // Button_ThumbLY
	AID_Z,                 // Button_ThumbRX
	AID_Rz | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
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
	-1,	// Button_ThumbRY
	-1	// Button_Home
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
	AID_Z | AID_Negative, // Button_ThumbRY
	-1   // Button_Home
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
	-1   // Button_Home
};

static const int gJoyBox3ButtonID[GamepadType_Max] =
{
	2,	// Button_A
	1,	// Button_B
	3,	// Button_X
	0,	// Button_Y
	6,	// Button_White
	7,	// Button_Black
	4,	// Button_LeftTrigger
	5,	// Button_RightTrigger
	8,	// Button_Start
	9,	// Button_Back
	10,	// Button_LeftThumb
	11,	// Button_RightThumb
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,					// Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Z,					// Button_ThumbRX
	AID_Rz | AID_Negative,	// Button_ThumbRY
	-1   // Button_Home
};

static const int gSixaxisButtonID[GamepadType_Max] =
{
	14,	// Button_A
	13,	// Button_B
	15,	// Button_X
	12,	// Button_Y
	10,	// Button_White
	11,	// Button_Black
	8,	// Button_LeftTrigger
	9,	// Button_RightTrigger
	3,	// Button_Start
	0,	// Button_Back
	1,	// Button_LeftThumb
	2,	// Button_RightThumb
	4,	// Button_DUp
	6,	// Button_DDown
	7,	// Button_DLeft
	5,	// Button_DRight
	AID_X,					// Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	-1, // AID_Z,					// Button_ThumbRX // sixaxis driver doesnt seem to like the right stick...
	-1, // AID_Rz | AID_Negative,	// Button_ThumbRY // sixaxis driver doesnt seem to like the right stick...
	-1  // Button_Home
};

static const int gUSBGamepadButtonID[GamepadType_Max] =
{
	2,	// Button_A
	1,	// Button_B
	3,	// Button_X
	0,	// Button_Y
	6,	// Button_White
	7,	// Button_Black
	4,	// Button_LeftTrigger
	5,	// Button_RightTrigger
	9,	// Button_Start
	8,	// Button_Back
	-1,	// Button_LeftThumb
	-1,	// Button_RightThumb
	12,	// Button_DUp
	14,	// Button_DDown
	15,	// Button_DLeft
	13,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Rz, // Button_ThumbRX
	AID_Z | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gTwinShockButtonID[GamepadType_Max] =
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
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Z, // Button_ThumbRX
	AID_Rz | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gPSVibrationFeedbackButtonID[GamepadType_Max] =
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
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Rz, // Button_ThumbRX
	AID_Z | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gUNIVERSALRumbleWheelButtonID[GamepadType_Max] =
{
	2,	// Button_A
	1,	// Button_B
	3,	// Button_X
	0,	// Button_Y
	6,	// Button_White
	7,	// Button_Black
	4,	// Button_LeftTrigger
	5,	// Button_RightTrigger
	9,	// Button_Start
	8,	// Button_Back
	10,	// Button_LeftThumb
	11,	// Button_RightThumb
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Slider1, // Button_ThumbRX
	AID_Rz | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gSaitek990ButtonID[GamepadType_Max] =
{
	1,	// Button_A
	2,	// Button_B
	0,	// Button_X
	3,	// Button_Y
	4,	// Button_White
	5,	// Button_Black
	6,	// Button_LeftTrigger
	7,	// Button_RightTrigger
	9,	// Button_Start
	8,	// Button_Back
	10,	// Button_LeftThumb
	11,	// Button_RightThumb
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Rz, // Button_ThumbRX
	AID_Z | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gSaitek880ButtonID[GamepadType_Max] =
{
	2,	// Button_A
	3,	// Button_B
	0,	// Button_X
	1,	// Button_Y
	4,	// Button_White
	5,	// Button_Black
	6,	// Button_LeftTrigger
	7,	// Button_RightTrigger
	9,	// Button_Start
	8,	// Button_Back
	10,	// Button_LeftThumb
	11,	// Button_RightThumb
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Rz, // Button_ThumbRX
	AID_Z | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gUnusualPS2AdapterButtonID[GamepadType_Max] =
{
	2,	// Button_A
	3,	// Button_B
	1,	// Button_X
	0,	// Button_Y
	6,	// Button_White
	4,	// Button_Black
	7,	// Button_LeftTrigger
	5,	// Button_RightTrigger
	8,	// Button_Start
	9,	// Button_Back
	11,	// Button_LeftThumb
	10,	// Button_RightThumb
	AID_Ry | AID_Negative | AID_Clamp,	// Button_DUp
	AID_Ry | AID_Clamp,	// Button_DDown
	AID_Rx | AID_Negative | AID_Clamp,	// Button_DLeft
	AID_Rx | AID_Clamp,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Z, // Button_ThumbRX
	AID_Rz | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gGamePadProButtonID[GamepadType_Max] =
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
	-1,  // Button_LeftThumb
	-1,  // Button_RightThumb
	AID_Y | AID_Negative | AID_Clamp,  // Button_DUp
	AID_Y | AID_Clamp,  // Button_DDown
	AID_X | AID_Negative | AID_Clamp,  // Button_DLeft
	AID_X | AID_Clamp,  // Button_DRight
	-1,	// Button_ThumbLX
	-1,	// Button_ThumbLY
	-1,	// Button_ThumbRX
	-1,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gAirFloButtonID[GamepadType_Max] =
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
	11,  // Button_LeftThumb
	12,  // Button_RightThumb
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Rz,               // Button_ThumbRX
	AID_Z | AID_Negative, // Button_ThumbRY
	-1  // Button_Home
};

static const int gRandomAdapterButtonID[GamepadType_Max] =
{
	2,	// Button_A
	1,	// Button_B
	3,	// Button_X
	0,	// Button_Y
	4,	// Button_White
	5,	// Button_Black
	6,	// Button_LeftTrigger
	7,	// Button_RightTrigger
	11,	// Button_Start
	8,	// Button_Back
	9,	// Button_LeftThumb
	10,	// Button_RightThumb
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Rz, // Button_ThumbRX
	AID_Z | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
};

static const int gXPlorerButtonID[GamepadType_Max] =
{
	2,	// Button_GH_Blue
	1,	// Button_GH_Red
	4,	// Button_GH_Orange
	3,	// Button_GH_Yellow
	-1,	// Button_GH_Solo
	-1,	// Unused
	AID_Ry | AID_Clamp | AID_Negative,	// Button_GH_TiltTrigger
	0,	// Button_GH_Green
	7,	// Button_GH_Start
	6,	// Button_GH_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		// Button_GH_StrumUp
	POV_Down,	// Button_GH_StrumDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_Rx | AID_Full,		// Button_GH_Whammy
	AID_Z,					// Jump/Roll sensor
	-1,						// Button_GH_PickupSwitch
	AID_Ry | AID_Negative,	// Button_GH_Tilt
	-1  // Button_Home
};

static const int g360RBGuitarButtonID[GamepadType_Max] =
{
	2,	// Button_GH_Blue
	1,	// Button_GH_Red
	4,	// Button_GH_Orange
	3,	// Button_GH_Yellow
	8,	// Button_GH_Solo
	-1,	// Unused
	AID_Ry | AID_Clamp | AID_Negative,	// Button_GH_TiltTrigger
	0,	// Button_GH_Green
	7,	// Button_GH_Start
	6,	// Button_GH_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		// Button_GH_StrumUp
	POV_Down,	// Button_GH_StrumDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_Rx | AID_Full,	// Button_GH_Whammy
	-1,					// Jump/Roll sensor
	AID_Z,				// Button_GH_PickupSwitch
	-1,					// Button_GH_Tilt
	-1  // Button_Home
};

static const int gPS3RBGuitarButtonID[GamepadType_Max] =
{
	0,	// Button_GH_Blue
	2,	// Button_GH_Red
	4,	// Button_GH_Orange
	3,	// Button_GH_Yellow
	6,	// Button_GH_Solo
	-1,	// Unused
	5,	// Button_GH_TiltTrigger
	1,	// Button_GH_Green
	9,	// Button_GH_Start
	8,	// Button_GH_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		  // Button_GH_StrumUp
	POV_Down,	  // Button_GH_StrumDown
	POV_Left,	  // Button_DLeft
	POV_Right,	// Button_DRight
	AID_Z | AID_Negative,	// Button_GH_Whammy
	-1,       // Jump/Roll sensor
	-1,       // Button_GH_PickupSwitch
	-1,       // Button_GH_Tilt
	12  // Button_Home
};

static const int gGH3PS3GuitarButtonID[GamepadType_Max] =
{
	3,	// Button_GH_Blue
	2,	// Button_GH_Red
	4,	// Button_GH_Orange
	0,	// Button_GH_Yellow
	-1,	// Button_GH_Solo
	-1,	// Unused
	-1,	// Button_GH_TiltTrigger
	1,	// Button_GH_Green
	9,	// Button_GH_Start
	8,	// Button_GH_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		  // Button_GH_StrumUp
	POV_Down,	  // Button_GH_StrumDown
	POV_Left,	  // Button_DLeft
	POV_Right,	// Button_DRight
	AID_Z | AID_Full | AID_Negative,		// Button_GH_Whammy
	-1,					// Jump/Roll sensor
	-1,         // Button_GH_PickupSwitch
	-1,	        // Button_GH_Tilt
	12  // Button_Home
};

static const int g360GH5GuitarButtonID[GamepadType_Max] =
{
	2,	// Button_GH_Blue
	1,	// Button_GH_Red
	4,	// Button_GH_Orange
	3,	// Button_GH_Yellow
	-1,	// Button_GH_Solo
	-1,	// Unused
	AID_Ry | AID_Clamp | AID_Negative,	// Button_GH_TiltTrigger
	0,	// Button_GH_Green
	7,	// Button_GH_Start
	6,	// Button_GH_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		// Button_GH_StrumUp
	POV_Down,	// Button_GH_StrumDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	-1,						// Button_GH_Whammy
	-1,						// Jump/Roll sensor
	-1,						// Button_GH_PickupSwitch
	AID_Ry | AID_Negative,	// Button_GH_Tilt
	-1  // Button_Home
};

static const int gPS3GH4GuitarButtonID[GamepadType_Max] =
{
	3,	// Button_GH_Blue
	2,	// Button_GH_Red
	4,	// Button_GH_Orange
	0,	// Button_GH_Yellow
	6,	// Button_GH_Solo
	-1,	// Unused
	5,	// Button_GH_TiltTrigger
	1,	// Button_GH_Green
	9,	// Button_GH_Start
	8,	// Button_GH_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		  // Button_GH_StrumUp
	POV_Down,	  // Button_GH_StrumDown
	POV_Left,	  // Button_DLeft
	POV_Right,	// Button_DRight
	AID_Z,	// Button_GH_Whammy
	-1,       // Jump/Roll sensor
	AID_Rz | AID_Full, // Button_GH_Slider
	-1,       // Button_GH_Tilt
	12  // Button_Home
};

static const int g360RBDrumsButtonID[GamepadType_Max] =
{
	0,	// Button_Drum_Green
	1,	// Button_Drum_Red
	2,	// Button_Drum_Blue
	3,	// Button_Drum_Yellow
	4,	// Button_Drum_Kick
	-1,	// Unused
	-1,	// Unused
	-1,	// Unused
	7,	// Button_Start
	6,	// Button_Back
	-1,	// Unused
	-1,	// Unused
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	-1, // Unused
	-1,	// Unused
	-1, // Unused
	-1,	// Unused
	-1  // Button_Home
};

static const int gPS3GH4DrumsButtonID[GamepadType_Max] =
{
	1,	// Button_Drum_Tom2
	2,	// Button_Drum_Snare
	0,	// Button_Drum_Tom1
	3,	// Button_Drum_Hat
	4,	// Button_Drum_Kick
	5,	// Button_Drum_Cymbal
	-1,	// Unused
	-1,	// Unused
	9,	// Button_Start
	8,	// Button_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	-1, // Unused
	-1,	// Unused
	-1, // Unused
	-1,	// Unused
	12  // Button_Home
};

static const int g360GH4DrumsButtonID[GamepadType_Max] =
{
	0,	// Button_Drum_Tom2
	1,	// Button_Drum_Snare
	2,	// Button_Drum_Tom1
	3,	// Button_Drum_Hat
	4,	// Button_Drum_Kick
	5,	// Button_Drum_Cymbal
	-1,	// Unused
	-1,	// Unused
	7,	// Button_Start
	6,	// Button_Select
	-1,	// Unused
	-1,	// Unused
	POV_Up,		// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	-1, // Unused
	-1,	// Unused
	-1, // Unused
	-1,	// Unused
	-1  // Unused
};

static const int gXGuitarButtonID[GamepadType_Max] =
{
	14,		// Button_GH_Blue
	13,		// Button_GH_Red
	8,		// Button_GH_Orange
	15,		// Button_GH_Yellow
	9,		// Button_GH_Solo
	-1,		// Unused
	AID_Ry | AID_Clamp,	// Button_GH_TiltTrigger
	12,		// Button_GH_Green
	4,		// Button_GH_Start
	5,		// Button_GH_Select
	6,		// Unused
	7,		// Unused
	0,		// Button_GH_StrumUp
	1,		// Button_GH_StrumDown
	2,		// Button_DLeft
	3,		// Button_DRight
	AID_Rx | AID_Full,	// Button_GH_Whammy
	AID_Z,	// Jump/Roll sensor
	AID_Rz,	// Button_GH_PickupSwitch
	AID_Ry,	// Button_GH_Tilt
	-1		// Button_Home
};

static const int gXDrumsButtonID[GamepadType_Max] =
{
	12,		// Button_Drum_Green
	13,		// Button_Drum_Red
	14,		// Button_Drum_Blue
	15,		// Button_Drum_Yellow
	8,		// Button_Drum_Kick
	9,		// Unused
	AID_Z,	// Unused
	AID_Rz,	// Unused
	4,		// Button_Start
	5,		// Button_Back
	6,		// Unused
	7,		// Unused
	0,		// Button_DUp
	1,		// Button_DDown
	2,		// Button_DLeft
	3,		// Button_DRight
	AID_X,	// Unused
	AID_Y,	// Unused
	AID_Rx,	// Unused
	AID_Ry,	// Unused
	-1		// Button_Home
};

static const int gOUYAButtonID[GamepadType_Max] =
{
	0,	// Button_A
	3,	// Button_B
	1,	// Button_X
	2,	// Button_Y
	4,	// Button_White
	5,	// Button_Black
	AID_Z | AID_Full,	// Button_LeftTrigger
	AID_Rz | AID_Full,	// Button_RightTrigger
	-1,	// Button_Start
	-1,	// Button_Back
	6,	// Button_LeftThumb
	7,	// Button_RightThumb
	8,	// Button_DUp
	9,	// Button_DDown
	10,	// Button_DLeft
	11,	// Button_DRight
	AID_X, // Button_ThumbLX
	AID_Y | AID_Negative,	// Button_ThumbLY
	AID_Rx, // Button_ThumbRX
	AID_Ry | AID_Negative,	// Button_ThumbRY
	-1  // Button_Home
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
	"Analog RY-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Unavailable",	// Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
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
	"Unavailable",	// Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gGGE909ButtonNames[GamepadType_Max] =
{
	"3",			// Button_A
	"2",			// Button_B
	"4",			// Button_X
	"1",			// Button_Y
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gTwinShockButtonNames[GamepadType_Max] =
{
	"3",			// Button_A
	"2",			// Button_B
	"4",			// Button_X
	"1",			// Button_Y
	"7",            // Button_White
	"8",            // Button_Black
	"5",            // Button_LeftTrigger
	"6",            // Button_RightTrigger
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gPS2LookAlikeButtonNames[GamepadType_Max] =
{
	"3",			// Button_A
	"2",			// Button_B
	"4",			// Button_X
	"1",			// Button_Y
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gSaitek990ButtonNames[GamepadType_Max] =
{
	"2",			// Button_A
	"3",			// Button_B
	"1",			// Button_X
	"4",			// Button_Y
	"L1",           // Button_White
	"R1",           // Button_Black
	"L2",           // Button_LeftTrigger
	"R2",           // Button_RightTrigger
	"Grey",         // Button_Start
	"Black",        // Button_Back
	"L3",           // Button_LeftThumb
	"R3",           // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gSaitek880ButtonNames[GamepadType_Max] =
{
	"3",			// Button_A
	"4",			// Button_B
	"1",			// Button_X
	"2",			// Button_Y
	"5",            // Button_White
	"6",            // Button_Black
	"L",            // Button_LeftTrigger
	"R",            // Button_RightTrigger
	"10",           // Button_Start
	"9",            // Button_Back
	"L3",           // Button_LeftThumb
	"R3",           // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gXPlorerButtonNames[GamepadType_Max] =
{
	"Blue",			// Button_A
	"Red",			// Button_B
	"Orange",		// Button_X
	"Yellow",		// Button_Y
	"Unavailable",	// Button_White
	"Unavailable",	// Button_Black
	"Tilt Trigger",	// Button_LeftTrigger
	"Green",		// Button_RightTrigger
	"Start",		// Button_Start
	"Select",		// Button_Back
	"Unavailable",	// Button_LeftThumb
	"Unavailable",	// Button_RightThumb
	"Up Strum",		// Button_DUp
	"Down Strum",	// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Whammy",		// Button_ThumbLX
	"Motion",		// Button_ThumbLY
	"Unavailable",	// Button_ThumbRX
	"Tilt",			// Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gPS3RBGuitarButtonNames[GamepadType_Max] =
{
	"Blue",			// Button_A
	"Red",			// Button_B
	"Orange",		// Button_X
	"Yellow",		// Button_Y
	"Solo",			// Button_White
	"Unavailable",	// Button_Black
	"Tilt Trigger",	// Button_LeftTrigger
	"Green",		// Button_RightTrigger
	"Start",		// Button_Start
	"Select",		// Button_Back
	"Unavailable",	// Button_LeftThumb
	"Unavailable",	// Button_RightThumb
	"Up Strum",		// Button_DUp
	"Down Strum",	// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Whammy",		// Button_ThumbLX
	"Unavailable",	// Button_ThumbLY
	"Pickup Switch",// Button_ThumbRX
	"Unavailable",	// Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gPS3GH4GuitarButtonNames[GamepadType_Max] =
{
	"Blue",			// Button_A
	"Red",			// Button_B
	"Orange",		// Button_X
	"Yellow",		// Button_Y
	"Unavailable",	// Button_White
	"Unavailable",	// Button_Black
	"Unavailable",	// Button_LeftTrigger
	"Green",		// Button_RightTrigger
	"Start",		// Button_Start
	"Select",		// Button_Back
	"Unavailable",	// Button_LeftThumb
	"Unavailable",	// Button_RightThumb
	"Up Strum",		// Button_DUp
	"Down Strum",	// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Whammy",		// Button_ThumbLX
	"Unavailable",	// Button_ThumbLY
	"Slider",		// Button_ThumbRX
	"Unavailable",	// Button_ThumbRY
	"PS Home"		// Button_Home
};

static const char * g360RBGuitarButtonNames[GamepadType_Max] =
{
	"Blue",			// Button_A
	"Red",			// Button_B
	"Orange",		// Button_X
	"Yellow",		// Button_Y
	"Solo",			// Button_White
	"Unavailable",	// Button_Black
	"Tilt Trigger",	// Button_LeftTrigger
	"Green",		// Button_RightTrigger
	"Start",		// Button_Start
	"Back",			// Button_Back
	"Unavailable",	// Button_LeftThumb
	"Unavailable",	// Button_RightThumb
	"Up Strum",		// Button_DUp
	"Down Strum",	// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Whammy",		// Button_ThumbLX
	"Unavailable",	// Button_ThumbLY
	"Pickup Switch",// Button_ThumbRX
	"Unavailable",	// Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gXGuitarButtonNames[GamepadType_Max] =
{
	"Blue",			// Button_A
	"Red",			// Button_B
	"Orange",		// Button_X
	"Yellow",		// Button_Y
	"Solo",			// Button_White
	"Unavailable",	// Button_Black
	"Tilt Trigger",	// Button_LeftTrigger
	"Green",		// Button_RightTrigger
	"Start",		// Button_Start
	"Back",			// Button_Back
	"Unavailable",	// Button_LeftThumb
	"Unavailable",	// Button_RightThumb
	"Up Strum",		// Button_DUp
	"Down Strum",	// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Whammy",		// Button_ThumbLX
	"Motion",		// Button_ThumbLY
	"Pickup Switch",// Button_ThumbRX
	"Tilt",			// Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gGamePadProButtonNames[GamepadType_Max] =
{
	"Yellow",		// Button_A
	"Green",		// Button_B
	"Red",			// Button_X
	"Blue",			// Button_Y
	"L1",           // Button_White
	"R1",           // Button_Black
	"L2",           // Button_LeftTrigger
	"R2",           // Button_RightTrigger
	"Start",        // Button_Start
	"Select",       // Button_Back
	"Unavailable",  // Button_LeftThumb
	"Unavailable",  // Button_RightThumb
	"Up",      // Button_DUp
	"Down",    // Button_DDown
	"Left",    // Button_DLeft
	"Right",   // Button_DRight
	"Unavailable",  // Button_ThumbLX
	"Unavailable",  // Button_ThumbLY
	"Unavailable",  // Button_ThumbRX
	"Unavailable",  // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gAirFlo[GamepadType_Max] =
{
	"1",            // Button_A
	"2",            // Button_B
	"3",            // Button_X
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
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gOUYAButtonNames[GamepadType_Max] =
{
	"O",            // Button_A
	"A",            // Button_B
	"U",            // Button_X
	"Y",            // Button_Y
	"L1",           // Button_White
	"R2",           // Button_Black
	"L2",           // Button_LeftTrigger
	"R2",           // Button_RightTrigger
	"Unavailable",  // Button_Start
	"Unavailable",  // Button_Back
	"L3",           // Button_LeftThumb
	"R3",           // Button_RightThumb
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Left X-Axis",  // Button_ThumbLX
	"Left Y-Axis",  // Button_ThumbLY
	"Right X-Axis", // Button_ThumbRX
	"Right Y-Axis", // Button_ThumbRY
	"Unavailable"	// Button_Home
};

static const char * gPS3RBDrumsButtonNames[GamepadType_Max] =
{
	"Green",        // Button_Drum_Green / Button_DM_Cymbal
	"Red",          // Button_Drum_Red / Button_DM_Snare
	"Blue",         // Button_Drum_Blue / Button_DM_Tom1
	"Yellow",       // Button_Drum_Yellow / Button_DM_Hat
	"Kick",         // Button_Drum_Kick / Button_DM_Kick
	"Unavailable",  // Button_DM_Tom2
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Start",        // Button_Drum_Start
	"Select",       // Button_Drum_Select
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable"	// Button_Home
};

static const char * gPS3GH4DrumsButtonNames[GamepadType_Max] =
{
	"Green",        // Button_Drum_Green / Button_Drum_Tom2
	"Red",          // Button_Drum_Red / Button_Drum_Snare
	"Blue",         // Button_Drum_Blue / Button_Drum_Tom1
	"Yellow",       // Button_Drum_Yellow / Button_Drum_Hat
	"Kick",         // Button_Drum_Kick
	"Orange",		// Button_Drum_Cymbal
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Start",        // Button_Drum_Start
	"Select",       // Button_Drum_Select
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"PS Home"	// Button_Home
};

static const char * g360RBDrumsButtonNames[GamepadType_Max] =
{
	"Green",        // Button_Drum_Green / Button_DM_Cymbal
	"Red",          // Button_Drum_Red / Button_DM_Snare
	"Blue",         // Button_Drum_Blue / Button_DM_Tom1
	"Yellow",       // Button_Drum_Yellow / Button_DM_Hat
	"Kick",         // Button_Drum_Kick / Button_DM_Kick
	"Unavailable",  // Button_DM_Tom2
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Start",        // Button_Drum_Start
	"Back",         // Button_Drum_Select
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable"	// Button_Home
};

static const char * g360GH4DrumsButtonNames[GamepadType_Max] =
{
	"Green",        // Button_Drum_Green / Button_Drum_Tom2
	"Red",          // Button_Drum_Red / Button_Drum_Snare
	"Blue",         // Button_Drum_Blue / Button_Drum_Tom1
	"Yellow",       // Button_Drum_Yellow / Button_Drum_Hat
	"Kick",         // Button_Drum_Kick
	"Orange",		// Button_Drum_Cymbal
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Start",        // Button_Drum_Start
	"Back",         // Button_Drum_Back
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"DPad Up",      // Button_DUp
	"DPad Down",    // Button_DDown
	"DPad Left",    // Button_DLeft
	"DPad Right",   // Button_DRight
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable",  // Unused
	"Unavailable"	// Button_Home
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
		0
	},

	// XBox 360 wired gamepad
	{
		"XBox 360 Gamepad",
		"XBOX 360 For Windows (Controller)",
		0x045E, 0x028E,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		0
	},

	// XBox 360 wireless gamepad
	{
		"XBox 360 Gamepad",
		"Controller (Xbox 360 Wireless Receiver for Windows)",
		0x045E, 0x02A1,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		0
	},

	// XBox 360 wireless gamepad
	{
		"XBox 360 Gamepad",
		"Controller (Xbox 360 Wireless Receiver for Windows)",
		0x045E, 0x0719,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		0
	},

	// XBox 360 DOA Arcade Stick
	{
		"DOA Arcade Stick",
		"Arcade Stick (Dead or Alive4Stick)",
		0x0F0D, 0x000A,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		0
	},

	// EMS USB2 adapter
	{
		"PS2 Gamepad",
		"EMS USB2",
		0x0B43, 0x0003,
		gEMSButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// Different identities of the titanium adapters
	{
		"PS2 Gamepad",
		"SmartJoy PLUS USB Adapter",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"SmartJoy PLUS Adapter",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"USB Force Feedback Joypad (MP-8888)",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"MP-8888 USB Joypad",
		0, 0,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"Dual USB Force Feedback Joypad (MP-8866)",
		0, 0,
		gSuperDualBoxButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// XBox gamepad
	{
		"XBox Gamepad",
		"XBCD XBox Gamepad",
		0, 0, // 0x0738, 0x4516, // i think the S-Pad and 3rd party pads are different..
		gXBCDButtonID,
		gXBoxButtonNames,
		0
	},

	// Logitech dual action
	{
		"Logitech Dual Action",
		"Logitech Dual Action",
		0, 0,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		0
	},

	// Logitech dual action
	{
		"Logitech Dual Action",
		"Logitech Dual Action USB",
		0, 0,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		0
	},

	// Xterminator Digital Gamepad
	{
		"Gravis Xterminator",
		"Xterminator Digital Gamepad",
		0, 0,
		gXterminatorButtonID,
		gXterminatorButtonNames,
		0
	},

	// Logitech Wingman Gamepad Extreme (Gameport)
	{
		"WingMan Gamepad Extreme",
		"Logitech WingMan Gamepad Extreme",
		0, 0,
		gLogitechWingmanGamepadExtremeID,
		gLogitechWingmanGamepadExtremeNames,
		0
	},

	// Logitech Wingman Gamepad Extreme (USB)
	{
		"WingMan Gamepad Extreme",
		"Logitech WingMan Gamepad Extreme (USB)",
		0, 0,
		gLogitechWingmanGamepadExtremeID,
		gLogitechWingmanGamepadExtremeNames,
		0
	},

	// Logitech RumblePad 2
	{
		"Logitech RumblePad 2",
		"Logitech RumblePad 2 USB",
		0, 0,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		0
	},

	// 'USB Joystick' basic PS2 style USB gamepad
	{
		"USB Gamepad",
		"'GreenAsia' USB Joystick",
		0x0E8F, 0x0002,
		gGreenAsiaButtonID,
		gGreenAsiaButtonNames,
		0
	},

	// Radioshack 'USB Dual Vibration Joystick' another PS2 gamepad adapter,
	{
		"PS2 Gamepad",
		"USB Dual Vibration Joystick",
		0x0E8F, 0x0003,
		gRadioshackButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// POWER))WAVE PS2 style gamepad
	{
		"PowerWave PS2 Style Gamepad",
		"USB Joystick",
		0, 0,
		gPowerWaveButtonID,
		gPowerWaveButtonNames,
		0
	},

	// Some more PS2 adapters
	{
		"PS2 Gamepad",
		"4 axis 16 button joystick",
		0x6666, 0x0667,
		gGenericPS2ButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"GIC USB Joystick",
		0x3427, 0x1190,
		gGenericPS2ButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"SmartJoy Dual PLUS Adapter",
		0x6677, 0x8802,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"TWIN PS TO PC CONVERTER",
		0x0810, 0x0001,
		gRadioshackButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"Unknown PS2 gamepad adapter",
		0x0F30, 0x010A,
		gRadioshackButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"MP-8866 Dual USB Joypad",
		0x0925, 0x8866,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// Logitech Cordless RumblePad 2
	{
		"RumblePad 2",
		"Logitech Cordless RumblePad 2",
		0x046d, 0xc219,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		0
	},

	// Guitar Hero X-Plorer
	{
		"X-Plorer Guitar",
		"Controller (Guitar Hero X-plorer  )",
		0x1430, 0x4748,
		gXPlorerButtonID,
		gXPlorerButtonNames,
		MFGF_IsGuitar | MFGF_Guitar_HasTilt | MFGF_DontUseSphericalDeadzone
	},

	// PS TO PC CONVERTOR
	{
		"PS2 Gamepad",
		"PS TO PC CONVERTOR",
		0x19FA, 0x8D01,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// GGE90x PC Recoil Pad
	{
		"GGE908 Recoil Pad",
		"GGE908 PC Recoil Pad",
		0x0F30, 0x0107,
		gGGE909ButtonID,
		gGGE909ButtonNames,
		0
	},

	{
		"GGE909 Recoil Pad",
		"GGE909 PC Recoil Pad",
		0x0F30, 0x010B,
		gGGE909ButtonID,
		gGGE909ButtonNames,
		0
	},

	// Super Joy Box 3 Pro
	{
		"PS2 Gamepad",
		"Super Joy Box 3 Pro",
		0x6666, 0x8801,
		gJoyBox3ButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// PSX/USB Pad
	{
		"PS2 Gamepad",
		"PSX/USB Pad",
		0x8631, 0x1128,
		gRadioshackButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// PS3 SIXAXIS Controller
	{
		"PS3 Controller",
		"PLAYSTATION(R)3 Controller",
		0x054C, 0x0268,
		gSixaxisButtonID,
		gPS2ButtonNames,
		0
	},

	// Random PC Gamepad (this one needs fleshing out... and i dont know the button names)
	{
		"USB Gamepad",
		"USB GamePad",
		0x0E8F, 0x3013,
		gUSBGamepadButtonID,
		gPS2ButtonNames,
		0
	},

	// Twin Shock
	{
		"Twin Shock Gamepad",
		"Twin Shock",
		0x1241, 0x5004,
		gTwinShockButtonID,
		gTwinShockButtonNames,
		0
	},

	// UNIVERSAL Rumble Wheel
	{
		"PS2 Gamepad",
		"UNIVERSAL Rumble Wheel",
		0x0E8F, 0x103F,
		gUNIVERSALRumbleWheelButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// Mad Catz Gamepad
	{
		"Mad Catz Gamepad",
		"Controller (MadCatz GamePad)",
		0x0738, 0x4716,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		0
	},

	// Saitek Gamepads
	{
		"Saitek P990 Gamepad",
		"Saitek P990 Dual Analog Pad",
		0x06A3, 0x040B,
		gSaitek990ButtonID,
		gSaitek990ButtonNames,
		0
	},

	{
		"Saitek P880 Gamepad",
		"p880",
		0x07B5, 0x0312,
		gSaitek880ButtonID,
		gSaitek880ButtonNames,
		0
	},

	// TigerGame PS2 Gamepad Adapters
	{
		"PS2 Gamepad",
		"TigerGame PS/PS2 Game Controller Adapter",
		0x6666, 0x8802,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"TigerGame PS/PS2 Game Controller Adapter",
		0x6666, 0x8804,
		gTitaniumButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// PS2 Adapter
	{
		"PS2 Gamepad",
		"Psx Gamepad 1",
		0x045E, 0x1100,
		gUnusualPS2AdapterButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"Macro Vibration Joystick",
		0x0079, 0x0006,
		gGGE909ButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"PS2->USB Gamepad Adapter",
		0x19FA, 0x8D91,
		gJoyBox3ButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"USB  Joystick",
		0x1345, 0x0003,
		gRadioshackButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS2 Gamepad",
		"USB Game Device",
		0x04B4, 0xC681,
		gGGE909ButtonID,	// BEST GUESS: this should possibly use the gGGE909VariantButtonID instead (can't check)
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// Gravis GamePad Pro USB
	{
		"GamePad Pro",
		"GamePad Pro USB",
		0x0428, 0x4001,
		gGamePadProButtonID,
		gGamePadProButtonNames,
		0
	},

	// XInput Guitar
	{
		"Guitar Controller",
		"Guitar Controller",
		0, 0,
		gXGuitarButtonID,
		gXGuitarButtonNames,
		MFGF_IsGuitar | MFGF_Guitar_HasTilt | MFGF_DontUseSphericalDeadzone
	},

	// PS3/USB Corded Gamepad
	{
		"PS3 Gamepad",
		"PS3/USB Corded Gamepad",
		0x046D, 0xCAD1,
		gLogitechDualActionButtonID,
		gPS2ButtonNames,
		0
	},

	{
		"PS3 Gamepad",
		"NYKO AirFlo Controller",
		0x124B, 0x4D01,
		gAirFloButtonID,
		gAirFlo,
		0
	},

	{
		"PS2 Gamepad",
		"axe 4 bouton 13 Manette de jeu avec commande de pouce",
		0x146B, 0x0306,
		gPowerWaveButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},


	{
		"XBox 360 Gamepad",
		"XUSB Gamepad (Controller)",
		0x046D, 0xC242,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		0
	},

	{
		"PS2 Gamepad",
		"USB Joystick",
		0x12BD, 0xC001,
		gRandomAdapterButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	{
		"PS3 Gamepad",
		"PS3/USB Cordless Gamepad",
		0x046D, 0xCAD2,
		gLogitechDualActionButtonID,
		gPS2ButtonNames,
		0
	},

	// PS Vibration Feedback Converter
	{
		"PS2 Gamepad",
		"PS Vibration Feedback Converter",
		0x0C12, 0x0005,
		gPSVibrationFeedbackButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// PS to USB (2P) Convert Cable
	{
		"PS2 Gamepad",
		"PS to USB (2P) Convert Cable",
		0x0E8F, 0x1006,
		gPSVibrationFeedbackButtonID,
		gPS2ButtonNames,
		MFGF_IsAdapter | MFGF_PS2
	},

	// Speed-Link SL-6535 USB Pad
	{
		"SpeedLink SL6535 Gamepad",
		"Speed-Link SL-6535 USB Pad",
		0x0E8F, 0x0012,
		gGGE909VariantButtonID,
		gGGE909ButtonNames,
		0
	},

	// OUYA controller
	{
		"OUYA Gamepad",
		"OUYA Game Controller",
		0x2836, 0x0001,
		gOUYAButtonID,
		gOUYAButtonNames,
		0
	},

	// RB 360 Guitar
	{
		"RockBand Guitar",
		"Controller (Harmonix Guitar for Xbox 360)", // Also: "Harmonix Guitar for Xbox 360 (Controller)"
		0x1BAD, 0x0002,
		g360RBGuitarButtonID,
		g360RBGuitarButtonNames,
		MFGF_IsGuitar | MFGF_Guitar_HasSolo | MFGF_Guitar_HasPickupSwitch | MFGF_DontUseSphericalDeadzone
	},

	// RB 360 Drums
	{
		"RockBand Drums",
		"Controller (Harmonix Drum Kit for Xbox 360)",
		0x1BAD, 0x0003,
		g360RBDrumsButtonID,
		g360RBDrumsButtonNames,
		MFGF_IsDrums
	},

	// Xinput Drums
	{
		"Drum Kit Controller",
		"Drum Kit Controller",
		0, 0,
		gXDrumsButtonID,
		g360RBDrumsButtonNames,
		MFGF_IsDrums
	},

	// XInput gamepad
	{
		"XBox 360 Gamepad",
		"Game Controller",
		0, 0,
		gXGamepadButtonID,
		gXBox360ButtonNames,
		0
	},

	// XInput Arcade Stick
	{
		"Arcade Stick",
		"Arcade Stick",
		0, 0,
		gXGamepadButtonID,
		gXBox360ButtonNames,
		0
	},

	// GH:WT PS3 Guitar
	{
		"GH4 Guitar",
		"Guitar Hero4 for PlayStation (R) 3",
		0x12BA, 0x0100,
		gPS3GH4GuitarButtonID,
		gPS3GH4GuitarButtonNames,
		MFGF_IsGuitar | MFGF_Guitar_HasSlider | MFGF_DontUseSphericalDeadzone
	},

	// GH:WT PS3 Drums
	{
		"GH4 Drums",
		"GuitarHero for Playstation (R) 3",
		0x12BA, 0x0120,
		gPS3GH4DrumsButtonID,
		gPS3GH4DrumsButtonNames,
		MFGF_IsDrums | MFGF_Drums_Has5Drums | MFGF_DontUseSphericalDeadzone
	},

	// GH5 Xbox 360 Drums
	{
		"GH5 Drums",
		"Drum Kit (Xbox 360 Wireless Receiver for Windows)",
		0x1430, 0x0719,
		g360GH4DrumsButtonID,
		g360GH4DrumsButtonNames,
		MFGF_IsDrums | MFGF_Drums_Has5Drums | MFGF_DontUseSphericalDeadzone
	},

	// GH5 Xbox 360 Guitar ** HACK: this shares the same USB id's with the drums! there is a hack in MFInput_PC.cpp to skip to the next one if axiis are present **
	{
		"GH5 Guitar",
		"Drum Kit (Xbox 360 Wireless Receiver for Windows)",
		0x1430, 0x0719,
		g360GH5GuitarButtonID,
		gXPlorerButtonNames,
		MFGF_IsGuitar | MFGF_Guitar_HasSlider | MFGF_DontUseSphericalDeadzone
	},

/*
	RB PS3:
		GREEN FRET - 1
		RED FRET - 2
		YELLOW FRET - 3
		BLUE FRET - 0
		ORANGE FRET - 4
		START - 9
		SELECT - 8
		FX TRIGGER - AID_Clamp | AID_Rz

	GH3 PS3:
		Green - 1
		Red - 2
		Yellow - 0
		Blue - 3
		Orange - 4
		Start - 9
		Select - 8

	PS3 RB Drums:

*/

	// "Harmonix Guitar for PlayStation®3" 12BA:0200
	// "Guitar Hero3 for PlayStation (R) 3" 12BA:0100
	// "Harmonix Drum Kit for PlayStation(R)3" 12BA:0210

	// "MP-8868 Dual USB Joypad" 0409:005A
	// "Microsoft SideWinder game pad" 045E:0003
};

MFGamepadInfo *pGamepadMappingRegistry = gGamepadDescriptors;

void MFInputPC_LinkGamepadRegistry()
{
	int numDescriptors = sizeof(gGamepadDescriptors)/sizeof(gGamepadDescriptors[0]);
	for(int a=0; a<numDescriptors; ++a)
	{
		if(a < numDescriptors-1)
			gGamepadDescriptors[a].pNext = &gGamepadDescriptors[a+1];
		else
			gGamepadDescriptors[a].pNext = NULL;
	}
}

#endif
