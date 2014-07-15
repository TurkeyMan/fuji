#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_SDL2

#include "MFWindow_Internal.h"
#include "MFDisplay_Internal.h"

#include <SDL2/SDL.h>


struct MFWindow_SDL2 : MFWindow
{
	SDL_Window* window;
};


void MFWindow_InitModulePlatformSpecific()
{
}

void MFWindow_DeinitModulePlatformSpecific()
{
}

MF_API MFWindow *MFWindow_Create(MFWindowParams *pParams)
{
	Uint32 flags = SDL_WINDOW_OPENGL |
			(pParams->bFullscreen ? SDL_WINDOW_FULLSCREEN : 0) |
			(pParams->flags & MFWF_WindowFrame ? 0 : SDL_WINDOW_BORDERLESS) |
			(pParams->flags & MFWF_CanResize ? SDL_WINDOW_RESIZABLE : 0) |
			(pParams->flags & MFWF_AlwaysOnTop ? 0 : 0);

	SDL_Window* window = SDL_CreateWindow(
		pParams->pWindowTitle,             // window title
//		pParams->x,                        // initial x position
//		pParams->y,                        // initial y position
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		pParams->width,                    // width, in pixels
		pParams->height,                   // height, in pixels
		flags                              // flags - see below
	);

	if(!window)
	{
		MFDebug_Warn(1, MFStr("Couldn't create window: %s", SDL_GetError()));
		return NULL;
	}

	MFWindow_SDL2 *pWindow = (MFWindow_SDL2*)MFHeap_AllocAndZero(sizeof(MFWindow_SDL2));
	pWindow->params = *pParams;
	pWindow->window = window;

	return pWindow;
}

MF_API MFWindow *MFWindow_BindExisting(void *pWindowHandle)
{
	MFWindow_SDL2 *pWindow = (MFWindow_SDL2*)MFHeap_AllocAndZero(sizeof(MFWindow_SDL2));

	MFDebug_Assert(false, "TODO!");
	// TODO: fill out params from window data...
//	pWindow->params.x =

	return pWindow;
}

MF_API void MFWindow_Destroy(MFWindow *_pWindow)
{
	MFWindow_SDL2 *pWindow = (MFWindow_SDL2*)_pWindow;

	// TODO: destroy associated display here?

	SDL_DestroyWindow(pWindow->window);
	MFHeap_Free(pWindow);
}

MF_API void MFWindow_Update(MFWindow *_pWindow, const MFWindowParams *pParams)
{
//	MFWindow_SDL2 *pWindow = (MFWindow_SDL2*)_pWindow;
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
	MFWindow_SDL2 *pWindow = (MFWindow_SDL2*)_pWindow;
	return (void*)pWindow->window;
}

#endif
