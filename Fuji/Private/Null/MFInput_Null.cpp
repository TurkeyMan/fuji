#include "Fuji.h"
#include "MFInput_Internal.h"

/*** Structure definitions ***/

/*** Globals ***/

/**** Platform Specific Functions ****/

void MFInput_InitModulePlatformSpecific()
{
}

void MFInput_DeinitModulePlatformSpecific()
{
}

void MFInput_UpdatePlatformSpecific()
{
}

void MFInput_GetDeviceStatusInternal(int device, int id, MFDeviceStatus *pDeviceStatus)
{
	pDeviceStatus->available = false;
	pDeviceStatus->status = IDS_Disconnected;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	memset(pGamepadState, 0, sizeof(MFGamepadState));
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	memset(pKeyState, 0, sizeof(MFKeyState));
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	memset(pMouseState, 0, sizeof(MFMouseState));
}

const char* MFInput_GetDeviceName(int source, int sourceID)
{
	return "Null Input Device";
}

const char* MFInput_GetGamepadButtonName(int button, int sourceID)
{
	return "Null Input Device";
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}
