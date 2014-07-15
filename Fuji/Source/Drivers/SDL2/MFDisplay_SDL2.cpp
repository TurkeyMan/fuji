#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_SDL2

#include "MFDisplay_Internal.h"
#include "MFWindow_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFView.h"
#include "MFSystem.h"
#include "DebugMenu.h"
#include "MFHeap.h"

#include <SDL2/SDL.h>
#include <stdio.h>


extern MFDisplay *gpCurrentDisplay;

static int gNumDisplayDevices = 0;
static MFDisplayAdaptorDesc *gpDisplayAdaptors = NULL;

extern MFInitParams gInitParams;


void MFDisplay_InitModulePlatformSpecific()
{
	SDL_InitSubSystem(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
//	SDL_VideoInit(NULL);

//	gNumDisplayDevices = 0;
//	gpDisplayAdaptors = (MFDisplayAdaptorDesc*)MFHeap_Alloc(sizeof(MFDisplayAdaptorDesc)*0);


	// build our internal list of available video modes
/*
	GetModes(&modes, false);//!gDisplay.windowed);
	while(!FindMode(modes, width, height))
	{
		if(!gDisplay.windowed)
		{
			// no fullscreen mode, try windowed mode instead
			MFDebug_Warn(1, "No suitable modes for fullscreen mode, trying windowed mode");

			gDisplay.windowed = true;

			FreeModes();
			GetModes(&modes, false);
		}
		else
		{
			// default is some sort of custom mode that doesn't appear in the windowed mode list
			// HACK: we'll add it to the end..
			modes[numModes].width = width;
			modes[numModes].height = height;
			currentMode = numModes;
			++numModes;
			break;
		}
	}
*/

//	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
//	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);
}

void MFDisplay_DeinitModulePlatformSpecific()
{
	if(gpDisplayAdaptors)
		MFHeap_Free(gpDisplayAdaptors);

//	SDL_VideoQuit();
	SDL_QuitSubSystem(SDL_INIT_VIDEO|SDL_INIT_EVENTS);

//	FreeModes();
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
	pDisplaySettings->flags = 0;//MFDF_CanResizeWindow;
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
/*
	// Set full screen mode, if necessary
	if(!gDisplay.windowed && numModes > 1)
	{
		if(!XF86VidModeSwitchToMode(xdisplay, screen, vidModes[currentMode]))
		{
			MFDebug_Error("Unable to switch screenmodes, defaulting to windowed mode");
			MFDisplay_DestroyDisplay();
			return 1;
		}
	}
*/
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

/*
	if(!gDisplay.windowed && numModes > 1)
	{
		if(!XF86VidModeSwitchToMode(xdisplay, screen, vidModes[currentMode]))
		{
			MFDebug_Error("Unable to set screen mode");
			MFDisplay_DestroyDisplay();
			return 1;
		}

		XGrabPointer(xdisplay, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);

		XFlush(xdisplay);
		// A little trick to make sure the entire window is on the screen
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, width - 1, height - 1);
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, 0, 0);
		XFlush(xdisplay);
	}
*/
	return pDisplay;
}

MF_API bool MFDisplay_Reset(MFDisplay *pDisplay, const MFDisplaySettings *pSettings)
{
/*
	sizeHints->flags = PSize | PMinSize | PMaxSize;
	sizeHints->min_width = sizeHints->max_width = sizeHints->base_width = gDisplay.width;
	sizeHints->min_height = sizeHints->max_height = sizeHints->base_height = gDisplay.height;

	XSetWMNormalHints(xdisplay, window, sizeHints);
	XResizeWindow(xdisplay, window, gDisplay.width, gDisplay.height);

	if(!gDisplay.windowed && numModes > 1)
	{
		XF86VidModeSwitchToMode(xdisplay, screen, vidModes[currentMode]);

		XGrabPointer(xdisplay, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
		XFlush(xdisplay);

		// A little trick to make sure the entire window is on the screen
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, gDisplay.width - 1, gDisplay.height - 1);
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, 0, 0);
		XFlush(xdisplay);
	}
*/

	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

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

	return true;
}

MF_API void MFDisplay_Destroy(MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	if(gpCurrentDisplay == pDisplay)
		gpCurrentDisplay = NULL;

/*
	if((!gDisplay.windowed) && (originalVidMode != NULL) && xdisplay != NULL && numModes > 1)
	{
		XF86VidModeSwitchToMode(xdisplay, screen, originalVidMode);
	}
*/

	MFRenderer_DestroyDisplay(pDisplay);

	// TODO: destroy window...??
}

void MFDisplay_HandleEventsSDL2()
{
	MFCALLSTACK;

}

#endif
