#if !defined(_INPUT_H)
#define _INPUT_H

enum InputType
{
	Button_A,
	Button_B,
	Button_X,
	Button_Y,
	Button_Black,
	Button_White,
	Button_LTrig,
	Button_RTrig,
	Button_Start,
	Button_Back,
	Button_LThumb,
	Button_RThumb,
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