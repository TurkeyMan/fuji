#include "Common.h"
#include "Input.h"
#include "MFVector.h"

// FIXME

void Input_InitModule()
{
}

void Input_DeinitModule()
{
}

void Input_Update()
{
}

float Input_ReadGamepad(int controlID, uint32 type)
{
	return(0.0f);
}

bool Input_WasPressed(int controlID, uint32 type)
{
	return(false);
}

bool Input_IsConnected(int controlID)
{
	return(true);
}

void SetGamepadEventHandler(EventFunc pEventFunc)
{
}

int Input_GetNumKeyboards()
{
	return(1);
}

bool Input_ReadKeyboard(uint32 key, int keyboardID)
{
	return(false);
}

bool Input_WasKeyPressed(uint32 key, int keyboardID)
{
	return(false);
}

void SetKeyboardEventHandler(EventFunc pEventFunc)
{
}

int Input_GetNumPointers()
{
	return(1);
}

bool Input_ReadMouseKey(uint32 key, int mouseID)
{
	return(false);
}

bool Input_WasMousePressed(uint32 key, int mouseID)
{
	return(false);
}

void SetMouseEventHandler(EventFunc pEventFunc)
{
}

void SetMouseMode(uint32 mouseMode)
{
}

MFVector Input_ReadMousePos(int mouseID)
{
}

char* Input_EnumerateString(ButtonMapping *pMap)
{
}
