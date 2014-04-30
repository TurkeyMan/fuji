#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_X11

#include "MFWindow_Internal.h"
#include "MFDisplay_Internal.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "X11_linux.h"


struct MFWindow_X11 : MFWindow
{
	Window window;

	Colormap colorMap;
	XSizeHints *sizeHints;
};

extern Display *xdisplay;
extern int screen;
extern Window rootWindow;
extern Atom wm_delete_window;


static Bool WaitForNotify(Display *d, XEvent *e, char *arg)
{
	return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}


void MFWindow_InitModulePlatformSpecific()
{
}

void MFWindow_DeinitModulePlatformSpecific()
{
}

MF_API MFWindow *MFWindow_Create(MFWindowParams *pParams)
{
	// TODO: move this call into here...
	XVisualInfo *MFRenderer_GetVisualInfo();
	XVisualInfo *visualInfo = MFRenderer_GetVisualInfo();
	if(!visualInfo)
		return NULL;

	Colormap colorMap = XCreateColormap(xdisplay, rootWindow, visualInfo->visual, AllocNone);
	if(!colorMap)
	{
		MFDebug_Error("Unable to create colourmap");
		XFree(visualInfo);
		return NULL;
	}

	XSetWindowAttributes windowAttrs;
	windowAttrs.colormap = colorMap;
	windowAttrs.cursor = None;
	windowAttrs.event_mask = StructureNotifyMask;
	windowAttrs.border_pixel = BlackPixel(xdisplay, screen);
	windowAttrs.background_pixel = BlackPixel(xdisplay, screen);

	Window window = XCreateWindow(xdisplay, rootWindow, 0, 0, pParams->width, pParams->height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWBackPixel | CWBorderPixel | CWCursor | CWColormap | CWEventMask, &windowAttrs);
	if(!window)
	{
		MFDebug_Error("Unable to create X Window");
		XFree(visualInfo);
		return NULL;
	}

	// Tell the window manager not to allow our window to be resized.  But some window managers can ignore me and do it anyway.  Typical X-Windows.
	XSizeHints *sizeHints = XAllocSizeHints();
	if(!sizeHints)
	{
		MFDebug_Error("Unable to alloc XSizeHints structure, out of memory?");
		XFree(visualInfo);
		return NULL;
	}

	sizeHints->flags = PSize | PMinSize | PMaxSize;
	sizeHints->min_width = sizeHints->max_width = sizeHints->base_width = pParams->width;
	sizeHints->min_height = sizeHints->max_height = sizeHints->base_height = pParams->height;

	XSetWMNormalHints(xdisplay, window, sizeHints);

	// Window title
	XStoreName(xdisplay, window, pParams->pWindowTitle);

	XWMHints *wmHints = XAllocWMHints();
	if(!wmHints)
	{
		MFDebug_Error("Unable to alloc XWMHints structure, out of memory?");
		XFree(visualInfo);
		return NULL;
	}

	wmHints->flags = InputHint | StateHint;
	wmHints->input = true;
	wmHints->initial_state = NormalState;

	int r = XSetWMHints(xdisplay, window, wmHints);

	XFree(wmHints);
	XFree(visualInfo);

	if(!r)
	{
		MFDebug_Error("Unable to set WM hints for window");
		return NULL;
	}

	// Tell the window manager that I want to be notified if the window's closed
	if(!XSetWMProtocols(xdisplay, window, &wm_delete_window, 1))
	{
		MFDebug_Error("Unable to set Window Manager protocols");
		return NULL;
	}

	XSelectInput(xdisplay, window, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | ExposureMask);

	if(!XMapRaised(xdisplay, window))
	{
		MFDebug_Error("Unable to map new window");
		return NULL;
	}

	// Wait for the window to be mapped, etc. The documentation doesn't indicate that this is necessary, but every GLX program I've ever seen does it, so I assume it is.
	XEvent event;
	XIfEvent(xdisplay, &event, WaitForNotify, (char *)window);

	// We have a window!
	MFWindow_X11 *pWindow = (MFWindow_X11*)MFHeap_AllocAndZero(sizeof(MFWindow_X11));
	pWindow->params = *pParams;
	pWindow->window = window;
	pWindow->colorMap = colorMap;
	pWindow->sizeHints = sizeHints;

	return pWindow;
}

MF_API MFWindow *MFWindow_BindExisting(void *pWindowHandle)
{
	MFWindow_X11 *pWindow = (MFWindow_X11*)MFHeap_AllocAndZero(sizeof(MFWindow_X11));

	MFDebug_Assert(false, "TODO!");
	// TODO: fill out params from window data...
//	pWindow->params.x =

	return pWindow;
}

MF_API void MFWindow_Destroy(MFWindow *_pWindow)
{
	MFWindow_X11 *pWindow = (MFWindow_X11*)_pWindow;

	// TODO: destroy associated display here?

	XFree(pWindow->sizeHints);
	XDestroyWindow(xdisplay, pWindow->window);
	XFreeColormap(xdisplay, pWindow->colorMap);

	MFHeap_Free(pWindow);
}

MF_API void MFWindow_Update(MFWindow *_pWindow, const MFWindowParams *pParams)
{
	MFWindow_X11 *pWindow = (MFWindow_X11*)_pWindow;
/*
	if(MFString_Compare(pWindow->params.pWindowTitle, pParams->pWindowTitle) != 0)
		SetWindowText(pWindow->hWnd, pParams->pWindowTitle);

	RECT rect;
	rect.left = 0;
	rect.right = (long)pParams->width;
	rect.top = 0;
	rect.bottom = (long)pParams->height;

	DWORD dwStyle = WS_POPUP;
	DWORD dwExStyle = WS_EX_APPWINDOW;
	if(!pParams->bFullscreen)
	{
		if(pParams->flags & MFWF_WindowFrame)
		{
			dwExStyle |= WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			if(pParams->flags & MFWF_CanResize)
				dwStyle |= WS_THICKFRAME;
		}
		if(pParams->flags & MFWF_AlwaysOnTop)
			dwExStyle |= WS_EX_TOPMOST;

		AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
		rect.right += -rect.left + (long)pParams->x;
		rect.left = (long)pParams->x;
		rect.bottom += -rect.top + (long)pParams->y;
		rect.top = (long)pParams->y;
	}

	bool bFullscreenChanged = pWindow->params.bFullscreen != pParams->bFullscreen;
	bool bFrameChanged = pWindow->params.flags != pParams->flags || bFullscreenChanged;
	bool bNoMove = !bFullscreenChanged && pWindow->params.x == rect.left && pWindow->params.y == rect.top;
	bool bNoResize = !bFrameChanged && pWindow->params.width == pParams->width && pWindow->params.height == pParams->height;

	pWindow->params = *pParams;

	UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER;
	flags |= bNoMove ? SWP_NOMOVE : 0;
	flags |= bNoResize ? SWP_NOSIZE : 0;

	if(bFrameChanged)
	{
		SetWindowLong(pWindow->hWnd, GWL_STYLE, dwStyle);
		SetWindowLong(pWindow->hWnd, GWL_EXSTYLE, dwExStyle);
		flags |= SWP_FRAMECHANGED;

		int showState = !pParams->bFullscreen && pWindow->state == MFWinState_Maximised ? SW_MAXIMIZE : SW_SHOW;

		SetWindowPos(pWindow->hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, flags);

		ShowWindow(pWindow->hWnd, showState);
		SetForegroundWindow(pWindow->hWnd);
		SetFocus(pWindow->hWnd);
	}
	else if(!(bNoMove && bNoResize))
	{
		SetWindowPos(pWindow->hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, flags);
	}
*/
}

MF_API void *MFWindow_GetSystemWindowHandle(MFWindow *_pWindow)
{
	MFWindow_X11 *pWindow = (MFWindow_X11*)_pWindow;
	return (void*)pWindow->window;
}

#endif
