#include "Common.h"
#include "Input.h"
#include "Vector3.h"

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
}

bool Input_WasPressed(int controlID, uint32 type)
{
}

bool Input_IsConnected(int controlID)
{
}

void SetGamepadEventHandler(EventFunc pEventFunc)
{
}

int Input_GetNumKeyboards()
{
}

bool Input_ReadKeyboard(uint32 key, int keyboardID)
{
}

bool Input_WasKeyPressed(uint32 key, int keyboardID)
{
}

void SetKeyboardEventHandler(EventFunc pEventFunc)
{
}

int Input_GetNumPointers()
{
}

bool Input_ReadMouseKey(uint32 key, int mouseID)
{
}

bool Input_WasMousePressed(uint32 key, int mouseID)
{
}

void SetMouseEventHandler(EventFunc pEventFunc)
{
}

void SetMouseMode(uint32 mouseMode)
{
}

Vector3 Input_ReadMousePos(int mouseID)
{
}

char* Input_EnumerateString(ButtonMapping *pMap)
{
}
