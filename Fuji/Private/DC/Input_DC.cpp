#include "Common.h"
#include "Input.h"
#include "Vector3.h"

/*** Structures ***/

/*** Function Declarations ***/

/*** Global Variables ***/

float deadZone = 0.3f;

/*** Functions ***/

void Input_InitModule()
{
	CALLSTACK;

}

void Input_DeinitModule()
{
	CALLSTACK;

}

void Input_Update()
{
	CALLSTACKc;

}

float Input_ReadGamepad(int controlID, uint32 type)
{
	CALLSTACKc;


	return 0.0f;
}

bool Input_WasPressed(int controlID, uint32 type)
{
	CALLSTACKc;


	return false;
}

// "Is Pad Connected" Function?
bool Input_IsConnected(int controlID)
{
	CALLSTACK;

	return false;
}

void SetGamepadEventHandler(EventFunc pEventFunc)
{
	
}

int Input_GetNumKeyboards()
{
	return 0;
}

bool Input_ReadKeyboard(int keyboardID, uint32 key)
{
	return false;
}

bool Input_WasKeyPressed(int keyboardID, uint32 key)
{
	return false;
}

void SetKeyboardEventHandler(EventFunc pEventFunc)
{

}

int Input_GetNumPointers()
{
	return 0;
}

bool Input_ReadMouseKey(int mouseID, uint32 key)
{
	return false;
}

bool Input_WasMousePressed(int mouseID, uint32 key)
{
	return false;
}

void SetMouseEventHandler(EventFunc pEventFunc)
{

}

void SetMouseMode(uint32 mouseMode)
{

}

Vector3 Input_ReadMousePos(int mouseID)
{
	return Vector(0.0f, 0.0f, 0.0f);
}

