#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_WIN32

#if !defined(WM_INPUT)
	#define WM_INPUT 0x00FF
#endif

#include "MFDisplay_Internal.h"
#include "MFWindow_Internal.h"
#include "MFRenderer_Internal.h"
#include "DebugMenu_Internal.h"
#include "MFSystem.h"

#include <windows.h>
#include <stdio.h>

extern MFDisplay *gpCurrentDisplay;

static int gNumDisplayDevices = 0;
static MFDisplayAdaptorDesc *gpDisplayAdaptors = NULL;

extern MFInitParams gInitParams;

// i will dynamically construct a list later from supported resolution
// provided by directx, verified by the monitor driver
struct DisplayMode
{
	int width, height;
	bool bWidescreen;
};
const DisplayMode modeList[] = { {320, 240, false}, {400, 300, false}, {480, 272, true}, {640, 480, false}, {800, 600, false}, {1024, 768, false}, {1152, 864, false}, {1280, 720, true}, {1280,800, true}, {1280, 1024, false}, {1600, 1200, false}, {1920, 1080, true}, {1920, 1200, true}, {1920, 1440, false} };
const int numModes = sizeof(modeList) / (sizeof(modeList[0]));

// debug menu resolution setting
char pCurrentRes[16] = "####x####";
const char *resStrings[] = { "-", pCurrentRes, "+", NULL };

MenuItemIntString resSelect(resStrings, 1);
MenuItemStatic applyDisplayMode;

int currentMode = 2;

// apply display mode callback
void ApplyDisplayModeCallback(MenuObject *pMenu, void *pData)
{
	MFCALLSTACK;

	gpCurrentDisplay->fullscreenWidth = modeList[currentMode].width;
	gpCurrentDisplay->fullscreenHeight = modeList[currentMode].height;
	gpCurrentDisplay->windowWidth = modeList[currentMode].width;
	gpCurrentDisplay->windowHeight = modeList[currentMode].height;

	MFDisplaySettings settings = *MFDisplay_GetDisplaySettings(gpCurrentDisplay);
	settings.width = modeList[currentMode].width;
	settings.height = modeList[currentMode].height;
	MFDisplay_Reset(gpCurrentDisplay, &settings);
}

// resolution change callback
void ChangeResCallback(MenuObject *pMenu, void *pData)
{
	MFCALLSTACK;

	MenuItemIntString *pRes = static_cast<MenuItemIntString*>(pMenu);

	if(pRes->data == 1)
	{
		ApplyDisplayModeCallback(NULL, NULL);
		return;
	}
	else if(pRes->data == 0)
	{
		currentMode = (currentMode == 0) ? numModes-1 : currentMode-1;
	}
	else if(pRes->data == 2)
	{
		currentMode = (currentMode == numModes-1) ? 0 : currentMode+1;
	}

	if(modeList[currentMode].height == 720)
		sprintf(pCurrentRes, "720p");
	else if(modeList[currentMode].height == 1080)
		sprintf(pCurrentRes, "1080p");
	else
		sprintf(pCurrentRes, "%dx%d", modeList[currentMode].width, modeList[currentMode].height);
	pRes->data = 1;
}


void MFDisplay_InitModulePlatformSpecific()
{
	DISPLAY_DEVICE device;
	device.cb = sizeof(DISPLAY_DEVICE);

	while(EnumDisplayDevices(NULL, gNumDisplayDevices, &device, 0))
		++gNumDisplayDevices;

	gpDisplayAdaptors = (MFDisplayAdaptorDesc*)MFHeap_Alloc(sizeof(MFDisplayAdaptorDesc)*gNumDisplayDevices);

	for(int i=0; i<gNumDisplayDevices; ++i)
	{
		EnumDisplayDevices(NULL, i, &device, 0);

		gpDisplayAdaptors[i].pDeviceId = MFString_Dup(device.DeviceName);
		gpDisplayAdaptors[i].pName = MFString_Dup(device.DeviceString);
		gpDisplayAdaptors[i].bSupportsFullscreen = true;
		gpDisplayAdaptors[i].bSupportsWindowed = (device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) ? true : false;

		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(device.DeviceName, ENUM_CURRENT_SETTINGS, &devMode);

		// enumerate display modes
		int numModes = 0;
		while(EnumDisplaySettings(device.DeviceName, numModes, &devMode))
		{
			// displaymode

			++numModes;
		}
	}

	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);
	sprintf(pCurrentRes, "%dx%d", modeList[currentMode].width, modeList[currentMode].height);
}

void MFDisplay_DeinitModulePlatformSpecific()
{
	MFHeap_Free(gpDisplayAdaptors);
}

void MFDisplay_LostFocus(MFDisplay *pDisplay)
{
	pDisplay->bHasFocus = false;

	MFRenderer_LostFocus(pDisplay);
}

void MFDisplay_GainedFocus(MFDisplay *pDisplay)
{
	pDisplay->bHasFocus = true;

	MFRenderer_GainedFocus(pDisplay);
}

MF_API int MFDisplay_GetNumMonitors()
{
	return 0;
}

MF_API const MFMonitorDesc *MFDisplay_GetMonitorDesc(int monitor)
{
	return NULL;
}

MF_API int MFDisplay_GetNumDisplayAdaptors()
{
	return gNumDisplayDevices;
}

MF_API const MFDisplayAdaptorDesc *MFDisplay_GetDisplayAdaptorDesc(int adaptor)
{
	if(adaptor >= 0 && adaptor < gNumDisplayDevices)
	{
		MFDebug_Warn(2, "Invalid adaptor!");
		return NULL;
	}
	return &gpDisplayAdaptors[adaptor];
}

MF_API int MFDisplay_GetDisplayModeCount(int monitor)
{
	return 0;
}

MF_API const MFDisplayModeDesc * MFDisplay_GetDisplayMode(int monitor, int index)
{
	return NULL;
}

MF_API void MFDisplay_GetDefaults(MFDisplaySettings *pDisplaySettings)
{
	MFZeroMemory(pDisplaySettings, sizeof(*pDisplaySettings));
	pDisplaySettings->displayAdaptor = 0;
	pDisplaySettings->monitor = 0;
	pDisplaySettings->mode = MFDM_Progressive;
	pDisplaySettings->aspect = MFDA_Default;
	pDisplaySettings->cable = MFDC_Unknown;
	pDisplaySettings->bVSync = true;
	pDisplaySettings->numBuffers = 2;
	pDisplaySettings->flags = MFDF_CanResizeWindow;
	pDisplaySettings->pWindow = NULL;

	pDisplaySettings->backBufferFormat = ImgFmt_SelectDefault;
	pDisplaySettings->depthStencilFormat = ImgFmt_SelectDepth;

#if defined(MF_RETAIL)
	// TODO: should use the native or desktop res?
	pDisplaySettings->bFullscreen = true;
	pDisplaySettings->width = (int)gInitParams.display.displayRect.width;
	pDisplaySettings->height = (int)gInitParams.display.displayRect.height;
	pDisplaySettings->refreshRate = 0;
#else
	pDisplaySettings->bFullscreen = false;
	pDisplaySettings->width = (int)gInitParams.display.displayRect.width;
	pDisplaySettings->height = (int)gInitParams.display.displayRect.height;
	pDisplaySettings->refreshRate = 0;
#endif
}

MF_API MFDisplay *MFDisplay_Create(const char *pName, const MFDisplaySettings *pDisplaySettings)
{
	bool bDidCreateWindow = false;

	MFWindow *pWindow = pDisplaySettings->pWindow;
	if(!pWindow)
	{
		MFWindowParams params;
		params.x = (int)gInitParams.display.displayRect.x;
		params.y = (int)gInitParams.display.displayRect.y;
		params.width = pDisplaySettings->width;
		params.height = pDisplaySettings->height;
		params.pWindowTitle = gInitParams.pAppTitle;
		params.monitor = pDisplaySettings->monitor;
		params.bFullscreen = pDisplaySettings->bFullscreen;
		params.flags = MFWF_WindowFrame | ((pDisplaySettings->flags & MFDF_CanResizeWindow) ? MFWF_CanResize : 0);

		pWindow = MFWindow_Create(&params);

		if(!pWindow)
		{
			MFDebug_Warn(1, "Couldn't create default Fuji window!");
			return NULL;
		}

		bDidCreateWindow = true;
	}

	MFDisplay *pDisplay = (MFDisplay*)MFHeap_Alloc(sizeof(MFDisplay));
	pDisplay->settings = *pDisplaySettings;
	pDisplay->settings.pWindow = pWindow;

	pDisplay->fullscreenWidth = pDisplay->windowWidth = pDisplay->settings.width;
	pDisplay->fullscreenHeight = pDisplay->windowHeight = pDisplay->settings.height;
	pDisplay->aspectRatio = (float)pDisplay->settings.width / (float)pDisplay->settings.height;
	pDisplay->bHasFocus = pWindow->bHasFocus;
	pDisplay->bIsVisible = true;
	pDisplay->orientation = MFDO_Normal;

	if(MFRenderer_CreateDisplay(pDisplay))
	{
		MFDebug_Warn(2, "Couldn't create display!");
		MFHeap_Free(pDisplay);
		if(bDidCreateWindow)
			MFWindow_Destroy(pWindow);
		return NULL;
	}

	MFWindow_AssociateDisplay(pWindow, pDisplay);

	return pDisplay;
}

MF_API bool MFDisplay_Reset(MFDisplay *pDisplay, const MFDisplaySettings *pSettings)
{
	// HACK! bail if re-entering function...
	static bool bReseting = false;
	if(bReseting)
		return false;
	bReseting = true;

	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	const MFDisplaySettings *pS = pSettings ? pSettings : &pDisplay->settings;
	MFDebug_Warn(4, MFStr("MFDisplay_Reset(%d, %d, %s)", pS->width, pS->height, pS->bFullscreen ? "true" : "false"));

	if(pSettings)
	{
		MFWindowParams params = *MFWindow_GetWindowParameters(pSettings->pWindow);

		params.bFullscreen = pSettings->bFullscreen;
		params.width = pSettings->width;
		params.height = pSettings->height;
		params.flags = (params.flags & ~MFWF_CanResize) | ((pSettings->flags & MFDF_CanResizeWindow) ? MFWF_CanResize : 0);

		MFWindow_Update(pSettings->pWindow, &params);
	}

	if(!pSettings)
		pSettings = &pDisplay->settings;

	if(!MFRenderer_ResetDisplay(pDisplay, pSettings))
	{
		// TODO: Oh no! shall we revert to previous settings?
		bReseting = false;
		return false;
	}

	pDisplay->settings = *pSettings;
	if(pDisplay->settings.bFullscreen)
	{
		pDisplay->fullscreenWidth = pDisplay->settings.width;
		pDisplay->fullscreenHeight = pDisplay->settings.height;
	}
	else
	{
		pDisplay->windowWidth = pDisplay->settings.width;
		pDisplay->windowHeight = pDisplay->settings.height;
	}

	pDisplay->aspectRatio = (float)pDisplay->settings.width / (float)pDisplay->settings.height;

	bReseting = false;
	return true;
}

MF_API void MFDisplay_Destroy(MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	if(gpCurrentDisplay == pDisplay)
		gpCurrentDisplay = NULL;
}

#endif // MF_DISPLAY
