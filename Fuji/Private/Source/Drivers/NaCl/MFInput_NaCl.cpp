#include "Fuji.h"

#if MF_INPUT == MF_DRIVER_NACL

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

MF_API uint32 MFInput_GetDeviceFlags(int device, int deviceID)
{
	return 0;
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	MFZeroMemory(pGamepadState, sizeof(MFGamepadState));
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFZeroMemory(pKeyState, sizeof(MFKeyState));
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFZeroMemory(pMouseState, sizeof(MFMouseState));
}

void MFInput_GetAccelerometerStateInternal(int id, MFAccelerometerState *pAccelerometerState)
{
	MFZeroMemory(pAccelerometerState, sizeof(MFAccelerometerState));
}

void MFInput_GetTouchPanelStateInternal(int id, MFTouchPanelState *pTouchPanelState)
{
	MFZeroMemory(pTouchPanelState, sizeof(MFTouchPanelState));
}

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	return "NaCl Input Device";
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	return "NaCl Input Button";
}

MF_API bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}

#endif
