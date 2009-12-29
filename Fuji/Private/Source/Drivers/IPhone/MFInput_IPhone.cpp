#include "Fuji.h"

#if MF_INPUT == MF_DRIVER_IPHONE

#include "MFInput_Internal.h"
#include "MFInput_IPhone.h"

/*** Extern functions ***/

/*** Structure definitions ***/

//struct Contact
//{
//	float x, y;
//	int contactID;
//};

/*** Globals ***/

static Contact gContacts[20];
static int gNumContacts = 0;
static float gAcceleration[3] = { 0.f, 0.f, 0.f };
static float gBearing = 0.f; // compas bearing [0 = North -> 1 Clockwise]
static bool gbDidShake = false;

/**** Platform Specific Functions ****/

void MFInput_InitModulePlatformSpecific()
{
	MFZeroMemory(gContacts, sizeof(gContacts));
}

void MFInput_DeinitModulePlatformSpecific()
{
}

void MFInput_UpdatePlatformSpecific()
{
}

uint32 MFInput_GetDeviceFlags(int device, int deviceID)
{
	return 0;
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	if(id != 0)
		return IDS_Unavailable;

	switch(device)
	{
		case IDD_Accelerometer:
		case IDD_TouchPanel:
			return IDS_Ready;
	}

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

	if(id == 0)
	{
		pAccelerometerState->values[Acc_XAxis] = gAcceleration[0];
		pAccelerometerState->values[Acc_YAxis] = gAcceleration[1];
		pAccelerometerState->values[Acc_ZAxis] = gAcceleration[2];
		pAccelerometerState->values[Acc_Acceleration] = MakeVector(gAcceleration[0], gAcceleration[1], gAcceleration[2]).Magnitude3();		
	}
}

void MFInput_GetTouchPanelStateInternal(int id, MFTouchPanelState *pTouchPanelState)
{
	MFZeroMemory(pTouchPanelState, sizeof(MFTouchPanelState));
	
	if(id == 0)
	{
		pTouchPanelState->numContacts = gNumContacts;
		for(int a=0; a<gNumContacts; ++a)
		{
			pTouchPanelState->contacts[a].x = gContacts[a].x;
			pTouchPanelState->contacts[a].y = gContacts[a].y;
			pTouchPanelState->contacts[a].tapCount = gContacts[a].tapCount;
			pTouchPanelState->contacts[a].phase = gContacts[a].phase;
			pTouchPanelState->contacts[a].flags = gContacts[a].flags;
		}

		pTouchPanelState->bDidShake = gbDidShake;
		gbDidShake = false;
	}
}

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	if(sourceID != 0)
		return NULL;

	switch(source)
	{
		case IDD_Accelerometer:
			return "iPhone Accelerometer";
		case IDD_TouchPanel:
			return "iPhone Screen";
	}

	return NULL;
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	return "iPhone";
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}

void MFInputIPhone_SetContacts(int numContacts, Contact *pContacts)
{
	gNumContacts = numContacts;

	for(int a=0; a<numContacts; ++a)
		gContacts[a] = pContacts[a];
}

void MFInputIPhone_SetAcceleration(float x, float y, float z)
{
	gAcceleration[0] = x;
	gAcceleration[1] = y;
	gAcceleration[2] = z;
}

void MFInputIPhone_Shake()
{
	gbDidShake = true;
}

#endif
