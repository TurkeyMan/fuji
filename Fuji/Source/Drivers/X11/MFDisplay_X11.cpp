#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_X11

#include "MFDisplay_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFView.h"
#include "MFSystem.h"
#include "DebugMenu.h"
#include "MFHeap.h"
#include "X11_linux.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>

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

Display *xdisplay = NULL;
int screen = 0;
Window window = 0;
Window rootWindow;
XF86VidModeModeInfo *originalVidMode = NULL;
XF86VidModeModeInfo **vidModes = NULL;
XSizeHints *sizeHints = NULL;
Colormap colorMap = 0;
Atom wm_delete_window;

uint8 gXKeys[65535];
XMouseState gXMouse;

static bool GetModes(Resolution **_modes, bool fullscreen);
static void SetSingleMode(Resolution **modes);
static void FreeModes();
static bool FindMode(Resolution *modes, int width, int height);
static void MFDisplay_ResetDisplay();

static Bool WaitForNotify(Display *d, XEvent *e, char *arg)
{
	return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}

static bool GetModes(Resolution **_modes, bool fullscreen)
{
	MFCALLSTACK;

	int numModeLines;

	if(!fullscreen)
	{
		modes = defaultModes;

		for(uint32 i = 0; modes[i].width != 0; ++i)
		{
			++numModes;
		}
	}
	else
	{
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
	}

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
	gDisplay.width = modes[currentMode].width;
	gDisplay.height = modes[currentMode].height;

	gDisplay.fullscreenWidth = gDisplay.width;
	gDisplay.fullscreenHeight = gDisplay.height;

	MFDisplay_ResetDisplay();
}

void MFDisplay_DestroyWindow()
{
	MFCALLSTACK;
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	MFZeroMemory(gXKeys, sizeof(gXKeys));
	MFZeroMemory(&gXMouse, sizeof(gXMouse));
	gXMouse.x = -1;

	gDisplay.fullscreenWidth = gDisplay.width = width;
	gDisplay.fullscreenHeight = gDisplay.height = height;
	gDisplay.refreshRate = 0;
	gDisplay.colourDepth = 0; /* Use default.  Chances are, it's something sane */
	gDisplay.windowed = true;
	gDisplay.wide = false;
	gDisplay.progressive = true;

	if(!(xdisplay = XOpenDisplay(NULL)))
	{
		MFDebug_Error("Unable to open display");
		MFDisplay_DestroyDisplay();
		return 1;
	}

	screen = DefaultScreen(xdisplay);
	rootWindow = RootWindow(xdisplay, screen);

	// build our internal list of available video modes
	GetModes(&modes, !gDisplay.windowed);
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

	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);

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

	XVisualInfo *MFRenderer_GetVisualInfo();
	XVisualInfo *visualInfo = MFRenderer_GetVisualInfo();
	if(!visualInfo)
		return 1;

	if(!(colorMap = XCreateColormap(xdisplay, rootWindow, visualInfo->visual, AllocNone)))
	{
		MFDebug_Error("Unable to create colourmap");
		XFree(visualInfo);
		MFDisplay_DestroyDisplay();
		return 1;
	}

	XSetWindowAttributes windowAttrs;
	windowAttrs.colormap = colorMap;
	windowAttrs.cursor = None;
	windowAttrs.event_mask = StructureNotifyMask;
	windowAttrs.border_pixel = BlackPixel(xdisplay, screen);
	windowAttrs.background_pixel = BlackPixel(xdisplay, screen);

	if(!(window = XCreateWindow(xdisplay, rootWindow, 0, 0, width, height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWBackPixel | CWBorderPixel | CWCursor | CWColormap | CWEventMask, &windowAttrs)))
	{
		MFDebug_Error("Unable to create X Window");
		XFree(visualInfo);
		MFDisplay_DestroyDisplay();
		return 1;
	}

	// Tell the window manager not to allow our window to be resized.  But some window managers can ignore me and do it anyway.  Typical X-Windows.
	if((sizeHints = XAllocSizeHints()) == NULL)
	{
		MFDebug_Error("Unable to alloc XSizeHints structure, out of memory?");
		XFree(visualInfo);
		MFDisplay_DestroyDisplay();
		return 1;
	}

	sizeHints->flags = PSize | PMinSize | PMaxSize;
    sizeHints->min_width = sizeHints->max_width = sizeHints->base_width = width;
    sizeHints->min_height = sizeHints->max_height = sizeHints->base_height = height;

	XSetWMNormalHints(xdisplay, window, sizeHints);

	// Window title
	XStoreName(xdisplay, window, gDefaults.display.pWindowTitle);

	XWMHints *wmHints;
	if((wmHints = XAllocWMHints()) == NULL)
	{
		MFDebug_Error("Unable to alloc XWMHints structure, out of memory?");
		XFree(visualInfo);
		MFDisplay_DestroyDisplay();
		return 1;
	}

	wmHints->flags = InputHint | StateHint;
	wmHints->input = true;
	wmHints->initial_state = NormalState;
	if(!XSetWMHints(xdisplay, window, wmHints))
	{
		MFDebug_Error("Unable to set WM hints for window");
		XFree(visualInfo);
		MFDisplay_DestroyDisplay();
		return 1;
	}

	XFree(wmHints);
	XFree(visualInfo);

	// Tell the window manager that I want to be notified if the window's closed
	wm_delete_window = XInternAtom(xdisplay, "WM_DELETE_WINDOW", false);
	if(!XSetWMProtocols(xdisplay, window, &wm_delete_window, 1))
	{
		MFDebug_Error("Unable to set Window Manager protocols");
		MFDisplay_DestroyDisplay();
		return 1;
	}

	XSelectInput(xdisplay, window, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | ExposureMask);

	if(!XMapRaised(xdisplay, window))
	{
		MFDebug_Error("Unable to map new window");
		MFDisplay_DestroyDisplay();
		return 1;
	}

	// Wait for the window to be mapped, etc. The documentation doesn't indicate that this is necessary, but every GLX program I've ever seen does it, so I assume it is.
	XEvent event;
	XIfEvent(xdisplay, &event, WaitForNotify, (char *)window);

	MFRenderer_CreateDisplay();

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

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFCALLSTACK;

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

	MFRenderer_ResetDisplay();
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;

	if((!gDisplay.windowed) && (originalVidMode != NULL) && xdisplay != NULL && numModes > 1)
	{
		XF86VidModeSwitchToMode(xdisplay, screen, originalVidMode);
	}

	MFRenderer_DestroyDisplay();

	if(sizeHints != NULL)
	{
		XFree(sizeHints);
		sizeHints = NULL;
	}

	if(window != 0)
	{
		XDestroyWindow(xdisplay, window);
		window = 0;
	}

	if(colorMap != 0)
	{
		XFreeColormap(xdisplay, colorMap);
		colorMap = 0L;
	}

	if(xdisplay != NULL)
	{
		XCloseDisplay(xdisplay);
		xdisplay = NULL;
	}

	FreeModes();
}

void MFDisplay_GetNativeRes(MFRect *pRect)
{
	// TODO: need to find the current desktop res in linux

	pRect->x = pRect->y = 0.f;
	pRect->width = 1280.f;
	pRect->height = 720.f;
}

void MFDisplay_GetDefaultRes(MFRect *pRect)
{
	pRect->x = pRect->y = 0.f;
	pRect->width = 1280.f;
	pRect->height = 720.f;
}

float MFDisplay_GetNativeAspectRatio()
{
	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);

	return rect.width / rect.height;
}

bool MFDisplay_IsWidescreen()
{
	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);

	return rect.width / rect.height >= 1.6f;
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
