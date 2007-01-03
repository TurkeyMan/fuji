#include "Fuji.h"

#include "MFVector.h"
#include "MFInput_Internal.h"
#include "MFHeap.h"
#include "MFIni.h"

/*** Structure definitions ***/

/*** Globals ***/

int	gGamepadCount	= 0;
int	gKeyboardCount	= 0;
int	gMouseCount		= 0;

char gKeyState[256];

bool gExclusiveMouse = false;
float deadZone = 0.3f;

float mouseMultiplier = 1.0f;

static const char * const gPS2Buttons[] =
{
// PS2 controller enums
	"X",
	"Circle",
	"Box",
	"Triangle",
	"L1",
	"R1",
	"L2",
	"R2",
	"Start",
	"Select",
	"L3",
	"R3",

// general controller enums
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Left X-Axis",
	"Left Y-Axis",
	"Right X-Axis",
	"Right Y-Axis"
};

/**** Platform Specific Functions ****/
#include <stdio.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include "libpad.h"

static char padBuf[256] __attribute__((aligned(64)));
static char actAlign[6];
static int actuators;

struct padButtonStatus buttons;
u32 new_pad;


// Using the X* variety of modules doesnt work for me, so I stick to the vanilla versions
#define ROM_PADMAN

static int
loadModules(void)
{
    int ret;

#ifdef ROM_PADMAN
    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
#else
    ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
#endif
    if (ret < 0) {
	printf("sifLoadModule sio failed: %d\n", ret);
	return -1;
    }

#ifdef ROM_PADMAN
    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
#else
    ret = SifLoadModule("rom0:XPADMAN", 0, NULL);
#endif
    if (ret < 0) {
	printf("sifLoadModule pad failed: %d\n", ret);
	return -1;
    }
    return 0;
}

int waitPadReady(int port, int slot)
{
    int state;

    state = padGetState(port, slot);
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
	state=padGetState(port, slot);
    }
    return 0;
}

int
initializePad(int port, int slot)
{

    int ret;
    int modes;
    int i;

    waitPadReady(port, slot);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);


    // If modes == 0, this is not a Dual shock controller 
    // (it has no actuator engines)
    if (modes == 0) {
//	printf("This is a digital controller?\n");
	return 1;
    }

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
	if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
	    break;
	i++;
    } while (i < modes);
    if (i >= modes) {
//	printf("This is no Dual Shock controller\n");
	return 1;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
//	printf("This is no Dual Shock controller??\n");
	return 1;
    }


    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);

    if (actuators != 0) {
	actAlign[0] = 0;   // Enable small engine
	actAlign[1] = 1;   // Enable big engine
	actAlign[2] = 0xff;
	actAlign[3] = 0xff;
	actAlign[4] = 0xff;
	actAlign[5] = 0xff;

	waitPadReady(port, slot);
    }
    else {
//	printf("Did not find any actuators.\n");
    }

    waitPadReady(port, slot);

    return 1;
}


void MFInput_InitModulePlatformSpecific()
{
	int port, slot;
	int ret;
    
	MFCALLSTACK;
	SifInitRpc(0);
	if(loadModules()){
	    printf("UNABLE to load modules\n");
	    return;
	}
	if(padInit(0)){
	    printf("UNABLE to init pad\n");
	    return;
	}

	port = 0; // 0 -> Connector 1, 1 -> Connector 2
	slot = 0; // Always zero if not using multitap

//	printf("PortMax: %d\n", padGetPortMax());
//	printf("SlotMax: %d\n", padGetSlotMax(port));

	if((ret = padPortOpen(port, slot, padBuf)) == 0) {
	    printf("padOpenPort failed: %d\n", ret);
	    return;
	}

	if(!initializePad(port, slot)) {
	    printf("pad initalization failed!\n");
	    return ;
	}
	
}

void MFInput_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;
}

void MFInput_UpdatePlatformSpecific()
{
    int ret;
    int port =0, slot = 0;

    MFCALLSTACK;
    ret=padGetState(port, slot);
    while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
	if(ret==PAD_STATE_DISCONN) {
	    printf("Pad(%d, %d) is disconnected\n", port, slot);
	    printf("What now?\n");
	    while(1)
		;
	}
	ret=padGetState(port, slot);
    }

    ret = padRead(port, slot, &buttons);
                    
    if (ret != 0) {
	new_pad = 0xffff ^ buttons.btns;
	return;
    }
    
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	if(device == IDD_Gamepad && id < 2)
		return IDS_Disconnected;

	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	MFCALLSTACK;

	MFZeroMemory(pGamepadState, sizeof(MFGamepadState));
	
	pGamepadState->values[Button_P2_Cross]    = (new_pad & PAD_CROSS )?1.0f:0.0f;
	pGamepadState->values[Button_P2_Circle]   = (new_pad & PAD_CIRCLE)?1.0f:0.0f;
	pGamepadState->values[Button_P2_Box]      = (new_pad & PAD_SQUARE)?1.0f:0.0f;
	pGamepadState->values[Button_P2_Triangle] = (new_pad & PAD_TRIANGLE)?1.0f:0.0f;
	
	pGamepadState->values[Button_P2_L1] = (new_pad & PAD_L1)?1.0f:0.0f;
	pGamepadState->values[Button_P2_R1] = (new_pad & PAD_R1)?1.0f:0.0f;
	pGamepadState->values[Button_P2_L2] = (new_pad & PAD_L2)?1.0f:0.0f;
	pGamepadState->values[Button_P2_R2] = (new_pad & PAD_R2)?1.0f:0.0f;
	
	pGamepadState->values[Button_P2_Start]  = (new_pad & PAD_START)?1.0f:0.0f;
	pGamepadState->values[Button_P2_Select] = (new_pad & PAD_SELECT)?1.0f:0.0f;

	pGamepadState->values[Button_P2_L3] = (new_pad & PAD_L3)?1.0f:0.0f;
	pGamepadState->values[Button_P2_R3] = (new_pad & PAD_R3)?1.0f:0.0f;

	pGamepadState->values[Button_DUp]    = (new_pad & PAD_UP)?1.0f:0.0f;
	pGamepadState->values[Button_DDown]  = (new_pad & PAD_DOWN)?1.0f:0.0f;
	pGamepadState->values[Button_DLeft]  = (new_pad & PAD_LEFT)?1.0f:0.0f;
	pGamepadState->values[Button_DRight] = (new_pad & PAD_RIGHT)?1.0f:0.0f;

	pGamepadState->values[Axis_LX] = (float)buttons.ljoy_h /127.5f - 1.0f;
	pGamepadState->values[Axis_LY] = -((float)buttons.ljoy_v/127.5f - 1.0f);
	pGamepadState->values[Axis_RX] = (float)buttons.rjoy_h /127.5f - 1.0f;
	pGamepadState->values[Axis_RY] = -((float)buttons.rjoy_v/127.5f - 1.0f);

}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFCALLSTACK;

	MFZeroMemory(pKeyState, sizeof(MFKeyState));
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFCALLSTACK;

	MFZeroMemory(pMouseState, sizeof(MFMouseState));
}

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	switch(source)
	{
		case IDD_Gamepad:
			return "DualShock 2";
		case IDD_Mouse:
			return "Mouse";
		case IDD_Keyboard:
			return "Keyboard";
		default:
			break;
	}

	return NULL;
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	MFDebug_Assert(sourceID < 2, "Only two gamepads available on PS2..."); // multitap??

	return gPS2Buttons[button];
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	switch(keyboardState)
	{
		case KSS_NumLock:
			break;

		case KSS_CapsLock:
			break;

		case KSS_ScrollLock:
			break;

		case KSS_Insert:
			break;
	}

	return 0;
}
