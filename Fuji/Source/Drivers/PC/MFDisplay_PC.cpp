#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_WIN32

#if !defined(WM_INPUT)
	#define WM_INPUT 0x00FF
#endif

#include "MFDisplay_Internal.h"
#include "MFRenderer_Internal.h"
#include "DebugMenu_Internal.h"
#include "MFSystem.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <dbt.h>

void MFDisplay_ResetDisplay();

void MFInputPC_Acquire(bool acquire);
#if defined(ALLOW_RAW_INPUT)
int HandleRawMouseMessage(HANDLE hDevice);
#endif

uint8 gWindowsKeys[256];

bool isortho = false;
float fieldOfView;

extern MFInitParams gInitParams;

extern bool gAppHasFocus;

extern HINSTANCE apphInstance;
HWND apphWnd;
int wndX = 24, wndY = 64;

bool initialised = false;

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

	gDisplay.fullscreenWidth = modeList[currentMode].width;
	gDisplay.fullscreenHeight = modeList[currentMode].height;
	gDisplay.wide = modeList[currentMode].bWidescreen;

	gDisplay.width = gDisplay.fullscreenWidth;
	gDisplay.height = gDisplay.fullscreenHeight;

	if(gDisplay.windowed)
	{
		int xframe = GetSystemMetrics(SM_CXFRAME)*2;
		int yframe = GetSystemMetrics(SM_CYFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
		MoveWindow(apphWnd, wndX, wndY, gDisplay.width + xframe, gDisplay.height + yframe, true);
	}
	else
	{
		MFDisplay_ResetDisplay();
	}
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

int MFDisplayPC_HandleWindowMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!gpEngineInstance->bIsInitialised)
		return 1;

	switch(message)
	{
		case WM_ACTIVATE:
		{
			int activate = LOWORD(wParam);

			MFInputPC_Acquire(activate != WA_INACTIVE);

			if(activate != WA_INACTIVE)
			{
				if(!gDisplay.windowed)
					MFDisplay_ResetDisplay();

				// scan key states...

				// using GetKeyState() for windows keystates

				// or read the state from directinput

				gAppHasFocus = true;

				MFSystemCallbackFunction pCallback = MFSystem_GetSystemCallback(MFCB_GainedFocus);
				if(pCallback)
					pCallback();
			}
			else
			{
				gAppHasFocus = false;

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
					if(initialised && lParam == VK_RETURN)
					{
						if(gDisplay.windowed)
							MFDisplay_SetDisplayMode(gDisplay.fullscreenWidth, gDisplay.fullscreenHeight, true);
						else
							MFDisplay_SetDisplayMode(gDisplay.width, gDisplay.height, false);
						MFDisplay_ResetDisplay();
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
			if(initialised && gDisplay.windowed)
			{
				wndX = LOWORD(lParam);
				wndY = HIWORD(lParam);
			}
			break;

		case WM_SIZE:
			if(initialised && gDisplay.windowed && wParam != SIZE_MINIMIZED)
			{
				RECT r, cr;
				GetWindowRect(apphWnd, &r);
				GetClientRect(apphWnd, &cr);

				wndX = r.left < 640 ? r.left : 0;
				wndY = r.top < 480 ? r.top : 0;

				gDisplay.width = cr.right - cr.left;
				gDisplay.height = cr.bottom - cr.top;
				MFDisplay_ResetDisplay();
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
	MFCALLSTACK;

	if(!MFDisplayPC_HandleWindowMessages(hWnd, message, wParam, lParam))
		return 0;

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void MFDisplay_DestroyWindow()
{
	MFCALLSTACK;

	MFRenderer_DestroyDisplay();

	if(apphWnd && !DestroyWindow(apphWnd))
	{
		MessageBox(NULL, "Could Not Release Window.", "Error", MB_OK|MB_ICONERROR);
	}

	apphWnd = NULL;
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	MFRect rect;
	MFDisplay_GetNativeRes(&rect);

	gDisplay.fullscreenWidth = (int)rect.width;
	gDisplay.fullscreenHeight = (int)rect.height;
	gDisplay.width = width;
	gDisplay.height = height;
	gDisplay.refreshRate = rate;
	gDisplay.colourDepth = 32;
	gDisplay.windowed = !gInitParams.display.bFullscreen; // Hacked like a boss
	gDisplay.wide = false;
	gDisplay.progressive = true;

	if(gInitParams.hWnd)
	{
		apphWnd = (HWND)gInitParams.hWnd;
	}
	else
	{
		WNDCLASS wc;

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC)WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = apphInstance;
		wc.hIcon = gDefaults.display.pIcon ? LoadIcon(apphInstance, gDefaults.display.pIcon) : NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "FujiWin";

		if(!RegisterClass(&wc))
		{
			MessageBox(NULL,"Failed To Register The Window Class.","Error!",MB_OK|MB_ICONERROR);
			return 2;
		}

		RECT rect;
		rect.left=(long)0;
		rect.right=(long)width;
		rect.top=(long)0;
		rect.bottom=(long)height;

		DWORD dwStyle, dwExStyle;
		int x, y;
		if(!gDisplay.windowed)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
			x = 0;
			y = 0;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW;
			x = wndX;
			y = wndY;
		}

		AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

		apphWnd = CreateWindowEx(dwExStyle, "FujiWin", gDefaults.display.pWindowTitle, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, x, y, rect.right-rect.left, rect.bottom-rect.top, NULL, NULL, apphInstance, NULL);
		if(!apphWnd)
		{
			MessageBox(NULL,"Failed To Create Window.","Error!",MB_OK|MB_ICONERROR);
			return 3;
		}

		MFRenderer_SetDisplayMode(width, height, !gDisplay.windowed);

		ShowWindow(apphWnd, SW_SHOW);
		SetForegroundWindow(apphWnd);
		SetFocus(apphWnd);
	}

	MFRenderer_CreateDisplay();

	initialised = true;

	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);
	sprintf(pCurrentRes, "%dx%d", modeList[currentMode].width, modeList[currentMode].height);

	MFZeroMemory(gWindowsKeys, sizeof(gWindowsKeys));

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFCALLSTACK;

	MFRenderer_ResetDisplay();
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;

	MFRenderer_DestroyDisplay();

	DestroyWindow(apphWnd);
	UnregisterClass("FujiWin", apphInstance);
}

bool MFDisplay_SetDisplayMode(int width, int height, bool bFullscreen)
{
	// WIN32 windows need to have the window styles changed when swapping in and out of fullscreen mode
	// fullscreen apps need to use a window with no frame
	// windowed apps may want any number of window decorations and stuff
	if(gDisplay.windowed == bFullscreen)
	{
		gDisplay.windowed = !bFullscreen;

		// change of display mode...
		RECT rect;
		rect.left=(long)0;
		rect.right=(long)width;
		rect.top=(long)0;
		rect.bottom=(long)height;

		DWORD dwStyle, dwExStyle;
		int x, y;
		if(bFullscreen)
		{
			gDisplay.fullscreenWidth = width;
			gDisplay.fullscreenHeight = height;
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
			x = 0;
			y = 0;
		}
		else
		{
			gDisplay.width = width;
			gDisplay.height = height;
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW;
			x = wndX;
			y = wndY;
		}

		AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

		SetWindowLong(apphWnd, GWL_STYLE, dwStyle);
		SetWindowLong(apphWnd, GWL_EXSTYLE, dwExStyle);
		SetWindowPos(apphWnd, HWND_NOTOPMOST, x, y, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	}

	return MFRenderer_SetDisplayMode(width, height, bFullscreen);
}

void MFDisplay_GetNativeRes(MFRect *pRect)
{
	DEVMODE dm;
	if(EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
	{
		pRect->x = pRect->y = 0.f;
		pRect->width = (float)dm.dmPelsWidth;
		pRect->height = (float)dm.dmPelsHeight;
	}
	else
	{
		pRect->x = pRect->y = 0.f;
		pRect->width = 640.f;
		pRect->height = 480.f;
	}
}

void MFDisplay_GetDefaultRes(MFRect *pRect)
{
	pRect->x = pRect->y = 0.f;
	pRect->width = 800.f;
	pRect->height = 480.f;
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

#endif // MF_DISPLAY
