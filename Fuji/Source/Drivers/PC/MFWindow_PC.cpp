#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_WIN32

#include "MFWindow_Internal.h"
#include "MFDisplay_Internal.h"

#if !defined(WM_INPUT)
	#define WM_INPUT 0x00FF
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dbt.h>

void MFInputPC_Acquire(bool acquire);
#if defined(ALLOW_RAW_INPUT)
int HandleRawMouseMessage(HANDLE hDevice);
#endif

struct MFWindow_PC : MFWindow
{
	HWND hWnd;
};

extern HINSTANCE apphInstance; // from WinMain
static HINSTANCE hInstance = NULL;

uint8 gWindowsKeys[256];


int MFWindowPC_HandleWindowMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!gpEngineInstance->bIsInitialised)
		return 1;

	MFWindow_PC *pWindow = (MFWindow_PC*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(message)
	{
		case WM_ACTIVATE:
		{
			int activate = LOWORD(wParam);

			MFInputPC_Acquire(activate != WA_INACTIVE);

			if(activate != WA_INACTIVE)
			{
				if(pWindow->params.bFullscreen)
					MFDisplay_Reset(pWindow->pDisplay);

				// scan key states...

				// using GetKeyState() for windows keystates

				// or read the state from directinput

				pWindow->bHasFocus = true;

				MFDisplay_GainedFocus(pWindow->pDisplay);

				MFSystemCallbackFunction pCallback = MFSystem_GetSystemCallback(MFCB_GainedFocus);
				if(pCallback)
					pCallback();
			}
			else
			{
				pWindow->bHasFocus = false;

				MFDisplay_LostFocus(pWindow->pDisplay);

				MFSystemCallbackFunction pCallback = MFSystem_GetSystemCallback(MFCB_LostFocus);
				if(pCallback)
					pCallback();
			}
			break;
		}

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_KEYMENU:
					if(pWindow && lParam == VK_RETURN)
					{
						MFDisplaySettings display = *MFDisplay_GetDisplaySettings(pWindow->pDisplay);
						display.bFullscreen = !display.bFullscreen;
						if(display.bFullscreen)
						{
							display.width = pWindow->pDisplay->fullscreenWidth;
							display.height = pWindow->pDisplay->fullscreenHeight;
						}
						else
						{
							display.width = pWindow->pDisplay->windowWidth;
							display.height = pWindow->pDisplay->windowHeight;
						}
						MFDisplay_Reset(pWindow->pDisplay, &display);
					}

				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

		case WM_SETCURSOR:
			/*if(CurrentDisplay.FullScreen)
			{
				SetCursor(NULL);
				return TRUE;
			}*/
			break;

		case WM_MOVE:
			if(pWindow && !pWindow->params.bFullscreen)
			{
				pWindow->params.x = LOWORD(lParam);
				pWindow->params.y = HIWORD(lParam);
			}
			break;

		case WM_SIZE:
			if(pWindow && !pWindow->params.bFullscreen)
			{
				switch(wParam)
				{
					case SIZE_MINIMIZED:
						pWindow->state = MFWinState_Minimised;
						pWindow->pDisplay->bIsVisible = false;
						break;
					case SIZE_MAXIMIZED:
						pWindow->state = MFWinState_Maximised;
						pWindow->pDisplay->bIsVisible = true;
						break;
					case SIZE_MAXHIDE:
						pWindow->pDisplay->bIsVisible = false;
						break;
					case SIZE_MAXSHOW:
						pWindow->pDisplay->bIsVisible = true;
						break;
					case SIZE_RESTORED:
						pWindow->state = MFWinState_Normal;
						break;
				}
				if(wParam != SIZE_MINIMIZED)
				{
					RECT r, cr;
					GetWindowRect(hWnd, &r);
					GetClientRect(hWnd, &cr);

					pWindow->params.x = r.left;
					pWindow->params.y = r.top;

					pWindow->params.width = cr.right - cr.left;
					pWindow->params.height = cr.bottom - cr.top;

					MFDisplaySettings display = *MFDisplay_GetDisplaySettings(pWindow->pDisplay);
					display.width = pWindow->params.width;
					display.height = pWindow->params.height;
					MFDisplay_Reset(pWindow->pDisplay, &display);
				}
			}
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_DEVICECHANGE:
		{
			if(wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE)
			{
				void DeviceChange(DEV_BROADCAST_DEVICEINTERFACE *pDevInf, bool connect);

				PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
				switch(pHdr->dbch_devicetype)
				{
					case DBT_DEVTYP_DEVICEINTERFACE:
					{
						PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
						DeviceChange(pDevInf, wParam == DBT_DEVICEARRIVAL ? true : false);
					}
				}
			}
		}
		break;

#if defined(ALLOW_RAW_INPUT)
		case WM_INPUT:
			if(gDefaults.input.allowMultipleMice)
				HandleRawMouseMessage((HRAWINPUT)lParam);
			break;
#endif

		case WM_KEYDOWN:
			gWindowsKeys[wParam] = 1;
			break;

		case WM_KEYUP:
			gWindowsKeys[wParam] = 0;
			break;
	}

	return 1;
}

// windows WndProc
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!MFWindowPC_HandleWindowMessages(hWnd, message, wParam, lParam))
		return 0;

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void MFWindow_InitModulePlatformSpecific()
{
	MFZeroMemory(gWindowsKeys, sizeof(gWindowsKeys));

	hInstance = apphInstance;

	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = (WNDPROC)WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = gDefaults.display.pIcon ? LoadIcon(hInstance, gDefaults.display.pIcon) : NULL;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "FujiWin";
	windowClass.hIconSm = NULL;

	if(!RegisterClassEx(&windowClass))
		MessageBox(NULL, "Failed To Register The Window Class.","Error!", MB_OK|MB_ICONERROR);
}

void MFWindow_DeinitModulePlatformSpecific()
{
	UnregisterClass("FujiWin", hInstance);
}

MF_API MFWindow *MFWindow_Create(MFWindowParams *pParams)
{
	MFWindow_PC *pWindow = (MFWindow_PC*)MFHeap_AllocAndZero(sizeof(MFWindow_PC));
	pWindow->params = *pParams;

	// create window
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

	pWindow->hWnd = CreateWindowEx(dwExStyle, "FujiWin", gDefaults.display.pWindowTitle, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, NULL, NULL, hInstance, NULL);
	if(!pWindow->hWnd)
	{
		MessageBox(NULL,"Failed To Create Window.","Error!",MB_OK|MB_ICONERROR);
		MFHeap_Free(pWindow);
		return NULL;
	}

	SetWindowLongPtr(pWindow->hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);

	ShowWindow(pWindow->hWnd, SW_SHOW);
	SetForegroundWindow(pWindow->hWnd);
	SetFocus(pWindow->hWnd);

	return pWindow;
}

MF_API MFWindow *MFWindow_BindExisting(void *pWindowHandle)
{
	MFWindow_PC *pWindow = (MFWindow_PC*)MFHeap_AllocAndZero(sizeof(MFWindow_PC));
	pWindow->hWnd = (HWND)pWindowHandle;

	MFDebug_Assert(false, "TODO!");
	// TODO: fill out params from window data...
//	pWindow->params.x = 

	SetWindowLongPtr(pWindow->hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);

	return pWindow;
}

MF_API void MFWindow_Destroy(MFWindow *_pWindow)
{
	MFWindow_PC *pWindow = (MFWindow_PC*)_pWindow;

	// TODO: destroy associated display here?

	if(!DestroyWindow(pWindow->hWnd))
		MFDebug_Warn(1, "Couldn't destroy window!");

	MFHeap_Free(pWindow);
}

MF_API void MFWindow_Update(MFWindow *_pWindow, const MFWindowParams *pParams)
{
	MFDebug_Warn(4, MFStr("MFWindow_Update(%d, %d, %d, %d)", pParams->x, pParams->y, pParams->width, pParams->height));

	MFWindow_PC *pWindow = (MFWindow_PC*)_pWindow;

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
}

MF_API void *MFWindow_GetSystemWindowHandle(MFWindow *_pWindow)
{
	MFWindow_PC *pWindow = (MFWindow_PC*)_pWindow;
	return pWindow->hWnd;
}

#endif
