#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_X11

#include "MFDisplay_Internal.h"
#include "MFWindow_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFView.h"
#include "MFSystem.h"
#include "DebugMenu.h"
#include "MFHeap.h"

#include "X11_linux.h"
#include <stdio.h>


extern MFDisplay *gpCurrentDisplay;

static int gNumDisplayDevices = 0;
static MFDisplayAdaptorDesc *gpDisplayAdaptors = NULL;

extern MFInitParams gInitParams;


Display *xdisplay = NULL;
int screen = 0;
Window rootWindow;
Atom wm_delete_window;


uint8 gXKeys[65535];
XMouseState gXMouse;


//-------------------------------------------------------------------
// Typedefs
struct Resolution
{
	int width, height;
	float refresh;
};

// Data definition
static Resolution defaultModes[] =
{
	{320, 240, 0.0f},
	{480, 272, 0.0f},
	{640, 480, 0.0f},
	{800, 600, 0.0f},
	{1024, 576, 0.0f},
	{1024, 768, 0.0f},
	{1280, 720, 0.0f},
	{1280, 960, 0.0f},
	{1280, 1024, 0.0f},
	{1920, 1080, 0.0f},
	{1920, 1200, 0.0f},
	{0, 0, 0.0f},
	{0, 0, 0.0f}
};

char modeString[24] = "";
const char *resMenuItems[] = {"-", modeString, "+", NULL};

MenuItemIntString resSelect(resMenuItems, 1);
MenuItemStatic applyDisplayMode;

// Variables
Resolution *modes = NULL;
MenuItemIntString *modesMenu;
int32 currentMode = -1, selectedMode = -1;
int32 numModes;

XF86VidModeModeInfo *originalVidMode = NULL;
XF86VidModeModeInfo **vidModes = NULL;

/*
static bool GetModes(Resolution **_modes, bool fullscreen);
static void SetSingleMode(Resolution **modes);
static void FreeModes();
static bool FindMode(Resolution *modes, int width, int height);

static bool GetModes(Resolution **_modes, bool fullscreen)
{
	MFCALLSTACK;

	int numModeLines;

	// scan hardware display modes
	int throwaway;
	if(!XF86VidModeQueryExtension(xdisplay, &throwaway, &throwaway))
	{
		SetSingleMode(_modes);
		return true;
	}

	if((!XF86VidModeGetAllModeLines(xdisplay, screen, &numModeLines, &vidModes)) || numModeLines < 2)
	{
		SetSingleMode(_modes);
		return true;
	}

	originalVidMode = vidModes[0];
	numModes = (uint32)numModeLines;

	modes = (Resolution *)MFHeap_Alloc(sizeof(Resolution) * (numModes + 1));

	for(int32 i = 0; i < numModes; i++)
	{
		modes[i].width = vidModes[i]->hdisplay;
		modes[i].height = vidModes[i]->vdisplay;
		modes[i].refresh = ((float)vidModes[i]->dotclock / (float)vidModes[i]->htotal) / (float)vidModes[i]->htotal;
	}

	modes[numModes].width = 0;
	modes[numModes].height = 0;

	if(_modes != NULL)
		*_modes = modes;

	return true;
}

static void SetSingleMode(Resolution **modes)
{
	*modes = (Resolution *)MFHeap_Alloc(sizeof(Resolution) * 2);

	(*modes)[0].width = DisplayWidth(xdisplay, screen);
	(*modes)[0].height = DisplayHeight(xdisplay, screen);
	(*modes)[0].refresh = 0;

	(*modes)[1].width = 0;
	(*modes)[1].height = 0;
	(*modes)[1].refresh = 0;

	originalVidMode = vidModes[0];

	numModes = 1;
}

static void FreeModes()
{
	MFCALLSTACK;

	if(modes != NULL)
	{
		if(modes != defaultModes)
		{
			MFHeap_Free(modes);
		}

		modes = NULL;
	}

	if(vidModes != NULL)
	{
		XFree(vidModes);
		vidModes = NULL;
	}
}

static bool FindMode(Resolution *modes, int width, int height)
{
	currentMode = -1;
	for(int32 i=0; i < numModes; i++)
	{
		if(width == modes[i].width && height == modes[i].height)
		{
			currentMode = i;
			break;
		}
	}

	return currentMode != -1;
}
*/

void ChangeResCallback(MenuObject *pMenu, void *pData)
{
	MenuItemIntString *pRes = (MenuItemIntString *)pMenu;

	if(pRes->data == 0)
	{
		selectedMode--;
		if(selectedMode < 0)
		{
			selectedMode = numModes - 1;
		}
	}
	else if(pRes->data == 2)
	{
		selectedMode++;
		if(selectedMode == numModes)
		{
			selectedMode = 0;
		}
	}

	pRes->data = 1;
	snprintf(modeString, 24, "%dx%d (%.02fMhz)", modes[selectedMode].width, modes[selectedMode].height, modes[selectedMode].refresh);
}

void ApplyDisplayModeCallback(MenuObject *pMenu, void *pData)
{
	currentMode = selectedMode;

	MFDisplaySettings settings = gpCurrentDisplay->settings;
	settings.width = modes[currentMode].width;
	settings.height = modes[currentMode].height;

	MFDisplay_Reset(gpCurrentDisplay, &settings);
}
//-------------------------------------------------------------------


void MFDisplay_InitModulePlatformSpecific()
{
	MFZeroMemory(gXKeys, sizeof(gXKeys));
	MFZeroMemory(&gXMouse, sizeof(gXMouse));
	gXMouse.x = -1;

//	gNumDisplayDevices = 0;
//	gpDisplayAdaptors = (MFDisplayAdaptorDesc*)MFHeap_Alloc(sizeof(MFDisplayAdaptorDesc)*0);

	xdisplay = XOpenDisplay(NULL);
	if(!xdisplay)
	{
		MFDebug_Error("Unable to open display");
		return;
	}

	screen = DefaultScreen(xdisplay);
	rootWindow = RootWindow(xdisplay, screen);

	wm_delete_window = XInternAtom(xdisplay, "WM_DELETE_WINDOW", false);

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

	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);
}

void MFDisplay_DeinitModulePlatformSpecific()
{
	if(gpDisplayAdaptors)
		MFHeap_Free(gpDisplayAdaptors);

//	FreeModes();

	XCloseDisplay(xdisplay);
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

void MFDisplay_HandleEventsX11()
{
	MFCALLSTACK;

	XEvent event;

	while(XPending(xdisplay))
	{
		XNextEvent(xdisplay, &event);
		switch(event.type)
		{
			case ClientMessage:
			{
				Atom atom;

				if(event.xclient.format == 8)
				{
					atom = event.xclient.data.b[0];
				}
				else if(event.xclient.format == 16)
				{
					atom = event.xclient.data.s[0];
				}
				else if(event.xclient.format == 32)
				{
					atom = event.xclient.data.l[0];
				}
				else
				{
					atom = 0;
				}

				if(atom == wm_delete_window)
				{
					gpEngineInstance->bQuit = 1;
				}
				break;
			}
			case KeyPress:
			{
				XKeyEvent *pEv = (XKeyEvent*)&event;
				KeySym ks = XLookupKeysym(pEv, 0);
				if(ks<=65535)
					gXKeys[ks] = 1;
				break;
			}
			case KeyRelease:
			{
				XKeyEvent *pEv = (XKeyEvent*)&event;
				KeySym ks = XLookupKeysym(pEv, 0);
				if(ks<=65535)
					gXKeys[ks] = 0;
				break;
			}
			case ButtonPress:
			{
				XButtonEvent *pEv = (XButtonEvent*)&event;
				gXMouse.buttons[pEv->button] = 1;
				if(pEv->button == 4)
					gXMouse.wheel -= 1;
				if(pEv->button == 5)
					gXMouse.wheel += 1;
				break;
			}
			case ButtonRelease:
			{
				XButtonEvent *pEv = (XButtonEvent*)&event;
				gXMouse.buttons[pEv->button] = 0;
				break;
			}
			case MotionNotify:
			{
				XMotionEvent *pEv = (XMotionEvent*)&event;
				if(gXMouse.x == -1)
				{
					gXMouse.prevX = pEv->x;
					gXMouse.prevY = pEv->y;
				}
				gXMouse.x = pEv->x;
				gXMouse.y = pEv->y;
				break;
			}
			default:
				break;
		}
	}
}

#endif
