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

void Input_InitModule();
void Input_DeinitModule();
void Input_Update();

float Input_ReadGamepad(int controlID, uint32 type);
bool Input_WasPressed(int controlID, uint32 type);

bool Input_IsConnected(int controlID);
#endif