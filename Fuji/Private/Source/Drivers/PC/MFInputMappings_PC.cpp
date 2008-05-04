#include "Fuji.h"

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
	AID_Z | AID_Negative  // Button_ThumbRY
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
	AID_X,                // Button_ThumbLX
	AID_Y | AID_Negative, // Button_ThumbLY
	AID_Z,               // Button_ThumbRX
	AID_Rz | AID_Negative  // Button_ThumbRY
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
};

static const int gXPlorerButtonID[GamepadType_Max] =
{
	2,	// Button_A
	1,	// Button_B
	4,	// Button_X
	3,	// Button_Y
	-1,	// Button_White
	-1,	// Button_Black
	AID_Ry | AID_Clamp | AID_Negative,	// Button_LeftTrigger
	0,	// Button_RightTrigger
	7,	// Button_Start
	6,	// Button_Back
	-1,	// Button_LeftThumb
	-1,	// Button_RightThumb
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_Rx | AID_Full,					// Button_ThumbLX
	-1,	// Button_ThumbLY
	-1, // AID_Z,	// Button_ThumbRX // the spherical deadzone plays havoc with this value
	AID_Ry | AID_Negative,	// Button_ThumbRY
};

static const int gXGuitarButtonID[GamepadType_Max] =
{
	2,	// Button_A
	1,	// Button_B
	4,	// Button_X
	3,	// Button_Y
	-1,	// Button_White
	-1,	// Button_Black
	AID_Ry | AID_Clamp | AID_Negative,	// Button_LeftTrigger
	0,	// Button_RightTrigger
	7,	// Button_Start
	6,	// Button_Back
	-1,	// Button_LeftThumb
	-1,	// Button_RightThumb
	POV_Up,	// Button_DUp
	POV_Down,	// Button_DDown
	POV_Left,	// Button_DLeft
	POV_Right,	// Button_DRight
	AID_Rx | AID_Full,					// Button_ThumbLX
	-1,	// Button_ThumbLY
	-1, // AID_Z,	// Button_ThumbRX // the spherical deadzone plays havoc with this value
	AID_Ry | AID_Negative,	// Button_ThumbRY
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
	-1	// Button_ThumbRY
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
	AID_Z | AID_Negative  // Button_ThumbRY
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
	"Right Y-Axis"  // Button_ThumbRY
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
	"Right Y-Axis"  // Button_ThumbRY
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
	"Right Y-Axis"  // Button_ThumbRY
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
	"Right Y-Axis"  // Button_ThumbRY
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
	"Right Y-Axis"  // Button_ThumbRY
};

static const char * gXPlorerButtonNames[GamepadType_Max] =
{
	"Blue",			// Button_A
	"Red",			// Button_B
	"Orange",		// Button_X
	"Yellow",		// Button_Y
	"Unavailable",	// Button_White
	"Unavailable",	// Button_Black
	"Rock Out",		// Button_LeftTrigger
	"Green",		// Button_RightTrigger
	"Start",		// Button_Start
	"Back",			// Button_Back
	"Unavailable",	// Button_LeftThumb
	"Unavailable",	// Button_RightThumb
	"Up",			// Button_DUp
	"Down",			// Button_DDown
	"Left",			// Button_DLeft
	"Right",		// Button_DRight
	"Whammy",		// Button_ThumbLX
	"Unavailable",	// Button_ThumbLY
	"Motion",		// Button_ThumbRX
	"Tilt"			// Button_ThumbRY
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
	"Unavailable"   // Button_ThumbRY
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
	"Right Y-Axis"  // Button_ThumbRY
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
		0, 0, // 0x0738, 0x4516, // i think the S-Pad and 3rd party pads are different..
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

	{
		"PS2 Gamepad",
		"TWIN PS TO PC CONVERTER",
		0x0810, 0x0001,
		gRadioshackButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[24]
	},

	{
		"PS2 Gamepad",
		"Unknown PS2 gamepad adapter",
		0x0F30, 0x010A,
		gRadioshackButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[25]
	},

	{
		"PS2 Gamepad",
		"MP-8866 Dual USB Joypad",
		0x0925, 0x8866,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[26]
	},

	// Logitech Cordless RumblePad 2
	{
		"RumblePad 2",
		"Logitech Cordless RumblePad 2",
		0x046d, 0xc219,
		gLogitechDualActionButtonID,
		gLogitechDualActionButtonNames,
		&gGamepadDescriptors[27]
	},

	// Guitar Hero X-Plorer
	{
		"X-Plorer Guitar",
		"Controller (Guitar Hero X-plorer  )",
		0x1430, 0x4748,
		gXPlorerButtonID,
		gXPlorerButtonNames,
		&gGamepadDescriptors[28]
	},

	// PS TO PC CONVERTOR
	{
		"PS2 Gamepad",
		"PS TO PC CONVERTOR",
		0x19FA, 0x8D01,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[29]
	},

	// GGE90x PC Recoil Pad
	{
		"GGE908 Recoil Pad",
		"GGE908 PC Recoil Pad",
		0x0F30, 0x0107,
		gGGE909ButtonID,
		gGGE909ButtonNames,
		&gGamepadDescriptors[30]
	},

	{
		"GGE909 Recoil Pad",
		"GGE909 PC Recoil Pad",
		0x0F30, 0x010B,
		gGGE909ButtonID,
		gGGE909ButtonNames,
		&gGamepadDescriptors[31]
	},

	// Super Joy Box 3 Pro
	{
		"PS2 Gamepad",
		"Super Joy Box 3 Pro",
		0x6666, 0x8801,
		gJoyBox3ButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[32]
	},

	// PSX/USB Pad
	{
		"PS2 Gamepad",
		"PSX/USB Pad",
		0x8631, 0x1128,
		gRadioshackButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[33]
	},

	// PS3 SIXAXIS Controller
	{
		"PS3 Controller",
		"PLAYSTATION(R)3 Controller",
		0x054C, 0x0268,
		gSixaxisButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[34]
	},

	// Random PC Gamepad (this one needs fleshing out... and i dont know the button names)
	{
		"USB Gamepad",
		"USB GamePad",
		0x0E8F, 0x3013,
		gUSBGamepadButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[35]
	},

	// Twin Shock
	{
		"Twin Shock Gamepad",
		"Twin Shock",
		0x1241, 0x5004,
		gTwinShockButtonID,
		gTwinShockButtonNames,
		&gGamepadDescriptors[36]
	},

	// UNIVERSAL Rumble Wheel
	{
		"PS2 Gamepad",
		"UNIVERSAL Rumble Wheel",
		0x0E8F, 0x103F,
		gUNIVERSALRumbleWheelButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[37]
	},

	// Mad Catz Gamepad
	{
		"Mad Catz Gamepad",
		"Controller (MadCatz GamePad)",
		0x0738, 0x4716,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		&gGamepadDescriptors[38]
	},

	// Saitek Gamepads
	{
		"Saitek P990 Gamepad",
		"Saitek P990 Dual Analog Pad",
		0x06A3, 0x040B,
		gSaitek990ButtonID,
		gSaitek990ButtonNames,
		&gGamepadDescriptors[39]
	},

	{
		"Saitek P880 Gamepad",
		"p880",
		0x07B5, 0x0312,
		gSaitek880ButtonID,
		gSaitek880ButtonNames,
		&gGamepadDescriptors[40]
	},

	// TigerGame PS2 Gamepad Adapters
	{
		"PS2 Gamepad",
		"TigerGame PS/PS2 Game Controller Adapter",
		0x6666, 0x8802,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[41]
	},

	{
		"PS2 Gamepad",
		"TigerGame PS/PS2 Game Controller Adapter",
		0x6666, 0x8804,
		gTitaniumButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[42]
	},

	// PS2 Adapter
	{
		"PS2 Gamepad",
		"Psx Gamepad 1",
		0x045E, 0x1100,
		gUnusualPS2AdapterButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[43]
	},

	{
		"PS2 Gamepad",
		"Macro Vibration Joystick",
		0x0079, 0x0006,
		gGGE909ButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[44]
	},

	{
		"PS2 Gamepad",
		"PS2->USB Gamepad Adapter",
		0x19FA, 0x8D91,
		gJoyBox3ButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[45]
	},

	{
		"PS2 Gamepad",
		"USB  Joystick",
		0x1345, 0x0003,
		gRadioshackButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[46]
	},

	{
		"PS2 Gamepad",
		"USB Game Device",
		0x04B4, 0xC681,
		gGGE909ButtonID,	// BEST GUESS: this should possibly use the gGGE909VariantButtonID instead (can't check)
		gPS2ButtonNames,
		&gGamepadDescriptors[47]
	},

	// Gravis GamePad Pro USB
	{
		"GamePad Pro",
		"GamePad Pro USB",
		0x0428, 0x4001,
		gGamePadProButtonID,
		gGamePadProButtonNames,
		&gGamepadDescriptors[48]
	},

	// Guitar Hero X-Plorer
	{
		"Guitar Controller",
		"Guitar Hero Controller",
		0, 0,
		gXGuitarButtonID,
		gXPlorerButtonNames,
		&gGamepadDescriptors[49]
	},

	// PS3/USB Corded Gamepad
	{
		"PS3 Gamepad",
		"PS3/USB Corded Gamepad",
		0x046D, 0xCAD1,
		gLogitechDualActionButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[50]
	},

	{
		"PS3 Gamepad",
		"NYKO AirFlo Controller",
		0x124B, 0x4D01,
		gAirFloButtonID,
		gAirFlo,
		&gGamepadDescriptors[51]
	},

	{
		"PS2 Gamepad",
		"axe 4 bouton 13 Manette de jeu avec commande de pouce",
		0x146B, 0x0306,
		gPowerWaveButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[52]
	},


	{
		"XBox 360 Gamepad",
		"XUSB Gamepad (Controller)",
		0x046D, 0xC242,
		gXBox360ButtonID,
		gXBox360ButtonNames,
		&gGamepadDescriptors[53]
	},

	{
		"PS2 Gamepad",
		"USB Joystick",
		0x12BD, 0xC001,
		gRandomAdapterButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[54]
	},

	{
		"PS3 Gamepad",
		"PS3/USB Cordless Gamepad",
		0x046D, 0xCAD2,
		gLogitechDualActionButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[55]
	},

	// PS Vibration Feedback Converter
	{
		"PS2 Gamepad",
		"PS Vibration Feedback Converter",
		0x0C12, 0x0005,
		gPSVibrationFeedbackButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[56]
	},

	// PS to USB (2P) Convert Cable
	{
		"PS2 Gamepad",
		"PS to USB (2P) Convert Cable",
		0x0E8F, 0x1006,
		gPSVibrationFeedbackButtonID,
		gPS2ButtonNames,
		&gGamepadDescriptors[57]
	},

	// Speed-Link SL-6535 USB Pad
	{
		"SpeedLink SL6535 Gamepad",
		"Speed-Link SL-6535 USB Pad",
		0x0E8F, 0x0012,
		gGGE909VariantButtonID,
		gGGE909ButtonNames,
		NULL
	},

	// "Harmonix Guitar for Xbox 360 (Controller)" 1BAD:0002
	// "Controller (Harmonix Guitar for Xbox 360)" 1BAD:0002
	// "Harmonix Guitar for PlayStation®3" 12BA:0200
	// "Guitar Hero3 for PlayStation (R) 3" 12BA:0100
	// "Controller (Harmonix Drum Kit for Xbox 360)" 1BAD:0003
	// "Harmonix Drum Kit for PlayStation(R)3" 12BA:0210

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

	RB 360:
		Green - 0
		Red - 1
		Yellow - 3
		Blue - 2
		Orange - 4
		Start - 7
		Back - 6
		Whammy - AID_Full | AID_Rx

*/
	// "MP-8868 Dual USB Joypad" 0409:005A

	// "Microsoft SideWinder game pad" 045E:0003
};

MFGamepadInfo *pGamepadMappingRegistry = gGamepadDescriptors;

#endif
