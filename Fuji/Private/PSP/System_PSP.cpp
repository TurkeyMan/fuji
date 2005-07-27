#include "Common.h"
#include "System_Internal.h"

#include "MFFileSystem_Internal.h"
#include "DebugMenu_Internal.h"
#include "View_Internal.h"
#include "Display_Internal.h"
#include "Input_Internal.h"
#include "Font.h"
#include "Primitive.h"

#include <pspkernel.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <pspsdk.h>
#include <pspdebug.h>
#include <psppower.h>

#include <time.h>


/* Define the module info section */
PSP_MODULE_INFO("FUJIPSP", 0x1000, 0, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

extern int gQuit;
extern int gRestart;

uint32 USBState = 0;

// USB Debug Menu Item...
void USBConnectionCallback(MenuObject *pMenu, void *pData);
void ClockRateCallback(MenuObject *pMenu, void *pData);

const char *pToggleStrings[] = { "Disabled", "Enabled", NULL };
const char *pClockRateStrings[] = { "222mhz", "266mhz", "333mhz", NULL };

MenuItemIntString usbMode(pToggleStrings, gDefaults.misc.enableUSBOnStartup ? 1 : 0);
MenuItemIntString clockRate(pClockRateStrings, 0);
MenuItemIntString showSystemInfo(pToggleStrings, 0);
MenuItemIntString showUSBStatus(pToggleStrings, 1);
MenuItemIntString showPowerStatus(pToggleStrings, 1);

/**
 * Function that is called from _init in kernelmode before the
 * main thread is started in usermode.
 */
__attribute__ ((constructor))
void loaderInit()
{
    pspKernelSetKernelPC();
    pspSdkInstallNoDeviceCheckPatch();
    pspDebugInstallKprintfHandler(NULL);
}

/* Exit callback */
int ExitCallback(int count, int arg, void *common)
{
	// terminate the app
	gQuit = 1;
	gRestart = 0;

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize size, void *arg)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", ExitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("Update Thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

//helper function to make things easier
int LoadStartModule(char *path)
{
	uint32 loadResult;
	uint32 startResult;
	int status;

	loadResult = sceKernelLoadModule(path, 0, NULL);
	if(loadResult & 0x80000000)
		return -1;
	else
		startResult = sceKernelStartModule(loadResult, 0, NULL, &status, NULL);

	if(loadResult != startResult)
		return -2;

	return 0;
}

void InitUSB()
{
	uint32 retVal;

	//start necessary drivers
	LoadStartModule("flash0:/kd/semawm.prx");
	LoadStartModule("flash0:/kd/usbstor.prx");
	LoadStartModule("flash0:/kd/usbstormgr.prx");
	LoadStartModule("flash0:/kd/usbstorms.prx");
	LoadStartModule("flash0:/kd/usbstorboot.prx");

	//setup USB drivers
	retVal = sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	if(retVal != 0)
	{
		LOGD(STR("Error starting USB Bus driver (0x%08X)\n", retVal));
	}

	retVal = sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
	if(retVal != 0)
	{
		LOGD(STR("Error starting USB Mass Storage driver (0x%08X)\n", retVal));
	}

	retVal = sceUsbstorBootSetCapacity(0x800000);
	if(retVal != 0)
	{
		LOGD(STR("Error setting capacity with USB Mass Storage driver (0x%08X)\n", retVal));
	}

	if(gDefaults.misc.enableUSBOnStartup)
		sceUsbActivate(0x1c8);

	USBState = sceUsbGetState();
}

void DeinitUSB()
{
	uint32 retVal;

	if(USBState & PSP_USB_ACTIVATED)
	{
		retVal = sceUsbDeactivate();

		if(retVal != 0)
			LOGD(STR("Error calling sceUsbDeactivate (0x%08X)\n", retVal));
	}

	retVal = sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);

	if(retVal != 0)
		LOGD(STR("Error stopping USB Mass Storage driver (0x%08X)\n", retVal));

	retVal = sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);

	if(retVal != 0)
		LOGD(STR("Error stopping USB BUS driver (0x%08X)\n", retVal));
}

int main()
{
	srand((uint32)clock());

	SetupCallbacks();

	pspDebugScreenInit();
//	pspDebugScreenSetBackColor(0xFF400000);
//	pspDebugScreenClear();

	System_GameLoop();

	sceKernelExitGame();
	return 0;
}

void System_InitModulePlatformSpecific()
{
	DebugMenu_AddItem("Show System Info", "Fuji Options", &showSystemInfo, NULL, NULL);
	DebugMenu_AddItem("Clock Rate", "Fuji Options", &clockRate, ClockRateCallback, NULL);
	DebugMenu_AddItem("USB Connection", "Fuji Options", &usbMode, USBConnectionCallback, NULL);
	DebugMenu_AddItem("Show USB Status", "Fuji Options", &showUSBStatus, NULL, NULL);
	DebugMenu_AddItem("Show Power Status", "Fuji Options", &showPowerStatus, NULL, NULL);

	InitUSB();
}

void System_DeinitModulePlatformSpecific()
{
	DeinitUSB();
}

void System_UpdatePlatformSpecific()
{
	USBState = sceUsbGetState();
}

void System_DrawPlatformSpecific()
{
	if(showUSBStatus)
	{
		if(USBState & PSP_USB_ACTIVATED)
		{
			MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(Mat_USB));

			MFPrimitive(PT_TriStrip);
			MFBegin(4);
			MFSetColour((USBState & PSP_USB_CONNECTION_ESTABLISHED) ? Vector(1,1,0,1) : Vector(0,0,0,1));
			MFSetTexCoord1(0,1);
			MFSetPosition(10, 26, 0);
			MFSetTexCoord1(0,0);
			MFSetPosition(10, 10, 0);
			MFSetTexCoord1(1,1);
			MFSetPosition(42, 26, 0);
			MFSetTexCoord1(1,0);
			MFSetPosition(42, 10, 0);
			MFEnd();

			MFMaterial_SetMaterial(MFMaterial_GetStockMaterial((USBState & PSP_USB_CABLE_CONNECTED) ? Mat_Connected : Mat_Disconnected));

			MFPrimitive(PT_TriStrip);
			MFBegin(4);
			MFSetColour(Vector4::one);
			MFSetTexCoord1(0,1);
			MFSetPosition(43, 26, 0);
			MFSetTexCoord1(0,0);
			MFSetPosition(43, 10, 0);
			MFSetTexCoord1(1,1);
			MFSetPosition(59, 26, 0);
			MFSetTexCoord1(1,0);
			MFSetPosition(59, 10, 0);
			MFEnd();
		}
	}

	if(showPowerStatus)
	{
		float batteryPercent = (float)scePowerGetBatteryLifePercent() * (1.0f / 100.0f);
		bool powerConnected = !!scePowerIsPowerOnline();
		bool charging = !!scePowerIsBatteryCharging();

		MFMaterial_SetMaterial(NULL);

		MFPrimitive(PT_TriStrip);
		MFBegin(4);
		MFSetColour(Vector4::one);
		MFSetPosition(390, 262, 0);
		MFSetPosition(390, 246, 0);
		MFSetPosition(454, 262, 0);
		MFSetPosition(454, 246, 0);
		MFEnd();
		MFBegin(4);
		MFSetColour(0,0,0,0.8f);
		MFSetPosition(391, 261, 0);
		MFSetPosition(391, 247, 0);
		MFSetPosition(453, 261, 0);
		MFSetPosition(453, 247, 0);
		MFEnd();
		MFBegin(4);
		MFSetColour(1,batteryPercent,0,1);
		MFSetPosition(391, 261, 0);
		MFSetPosition(391, 247, 0);
		MFSetPosition(391 + batteryPercent*62.0f, 261, 0);
		MFSetPosition(391 + batteryPercent*62.0f, 247, 0);
		MFEnd();

		if(charging)
		{
			MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(Mat_Charging));

			MFPrimitive(PT_TriStrip);
			MFBegin(4);
			MFSetColour(Vector4::one);
			MFSetTexCoord1(0,1);
			MFSetPosition(454, 262, 0);
			MFSetTexCoord1(0,0);
			MFSetPosition(454, 246, 0);
			MFSetTexCoord1(1,1);
			MFSetPosition(470, 262, 0);
			MFSetTexCoord1(1,0);
			MFSetPosition(470, 246, 0);
			MFEnd();
		}

		if(powerConnected)
		{
			MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(Mat_Power));

			MFPrimitive(PT_TriStrip);
			MFBegin(4);
			MFSetColour(Vector4::one);
			MFSetTexCoord1(0,1);
			MFSetPosition(372, 262, 0);
			MFSetTexCoord1(0,0);
			MFSetPosition(372, 246, 0);
			MFSetTexCoord1(1,1);
			MFSetPosition(388, 262, 0);
			MFSetTexCoord1(1,0);
			MFSetPosition(388, 246, 0);
			MFEnd();
		}
	}

	if(showSystemInfo)
	{
		int cpuFreq = scePowerGetCpuClockFrequency();
		int ramFreq = 0;
		int busFreq = scePowerGetBusClockFrequency();

		int batteryPercent = scePowerGetBatteryLifePercent();
		int batteryLifeTime = scePowerGetBatteryLifeTime();
		int hours = batteryLifeTime / 60;
		int minutes = batteryLifeTime - (hours*60);
		int batteryTemp = scePowerGetBatteryTemp();
		float batteryVolts = (float)scePowerGetBatteryVolt() / 1000.0f;

		bool powerConnected = !!scePowerIsPowerOnline();
		bool batteryAvailable = !!scePowerIsBatteryExist();
		bool charging = !!scePowerIsBatteryCharging();

		bool usbEnabled = !!(USBState & PSP_USB_ACTIVATED);
		bool usbConnected = !!(USBState & PSP_USB_CABLE_CONNECTED);
		bool usbActive = !!(USBState & PSP_USB_CONNECTION_ESTABLISHED);

		MFMaterial_SetMaterial(NULL);

		MFPrimitive(PT_TriStrip);
		MFBegin(4);
		MFSetColour(0,0,0,0.8f);
		MFSetPosition(70, 252, 0);
		MFSetPosition(70, 20, 0);
		MFSetPosition(410, 252, 0);
		MFSetPosition(410, 20, 0);
		MFEnd();

		Font_DrawText(gpDebugFont, 90.0f, 30.0f, 0, 20.0f, Vector(1,1,0,1), "System Status");

		Font_DrawTextf(gpDebugFont, 100.0f, 60.0f, 0, 20.0f, Vector4::one, "Clock/Ram/Bus Rate: %d/%d/%d", cpuFreq, ramFreq, busFreq);

		Font_DrawTextf(gpDebugFont, 100.0f, 90.0f, 0, 20.0f, Vector4::one, "Power Source: %s", powerConnected ? "External" : "Battery");
		Font_DrawTextf(gpDebugFont, 100.0f, 110.0f, 0, 20.0f, Vector4::one, "Battery %s", batteryAvailable ? STR("Connected%s", charging ? " (Charging...)" : "") : "Disconnected");

		Font_DrawTextf(gpDebugFont, 100.0f, 140.0f, 0, 20.0f, Vector4::one, "Battery Level: %s", batteryAvailable ? STR("%d%%%s", batteryPercent, powerConnected ? "" : STR(" (%dh %dm)", hours, minutes)) : "N/A");
		Font_DrawTextf(gpDebugFont, 100.0f, 160.0f, 0, 20.0f, Vector4::one, "Battery Temperature: %s", batteryAvailable ? STR("%dc", batteryTemp) : "N/A");
		Font_DrawTextf(gpDebugFont, 100.0f, 180.0f, 0, 20.0f, Vector4::one, "Battery Voltage: %s", batteryAvailable ? STR("%.1fv", batteryVolts) : "N/A");

		Font_DrawTextf(gpDebugFont, 100.0f, 210.0f, 0, 20.0f, Vector4::one, "USB: %s, %s, %s", usbEnabled ? "Enabled" : "Disabled", usbConnected ? "Connected" : "Disconnected", usbActive ? "Active" : "Inactive");
	}
}

void USBConnectionCallback(MenuObject *pMenu, void *pData)
{
	MenuItemIntString *pMI = (MenuItemIntString*)pMenu;

	if(pMI->data)
	{
		if(!(USBState & PSP_USB_ACTIVATED))
			sceUsbActivate(0x1c8);
	}
	else
	{
		if(USBState & PSP_USB_ACTIVATED)
			sceUsbDeactivate();
	}
}

void ClockRateCallback(MenuObject *pMenu, void *pData)
{
	MenuItemIntString *pMI = (MenuItemIntString*)pMenu;

	int clockRate = 222;
	int busRate = 111;

	if(pMI->data == 1)
	{
		clockRate = 266;
		busRate = 133;
	}
	else if(pMI->data == 2)
	{
		clockRate = 333;
		busRate = 166;
	}

	// do something with the clock rate..
//	scePowerSetCpuClockFrequency(clockRate);
//	scePowerSetBusClockFrequency(clockRate);

	scePowerSetClockFrequency(clockRate, clockRate, busRate);
}

// RTC functions
uint64 RDTSC()
{
	return (uint64)clock();
}

uint64 GetTSCFrequency()
{
	return CLOCKS_PER_SEC * 1000;
}
