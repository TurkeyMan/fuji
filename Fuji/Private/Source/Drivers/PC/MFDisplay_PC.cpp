#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_WIN32

#define _WIN32_WINNT 0x501
#define WM_INPUT 0x00FF

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
extern bool gFujiInitialised;

extern HINSTANCE apphInstance;
HWND apphWnd;
int wndX = 24, wndY = 64;

bool initialised = false;

// i will dynamically construct a list later from supported resolution
// provided by directx, verified by the monitor driver
const int resList[][2] = { {320, 240}, {400, 300}, {480, 272}, {640, 480}, {800, 600}, {1024, 768}, {1152, 864}, {1280, 720}, {1280,800}, {1280, 1024}, {1600, 1200}, {1920, 1080}, {1920, 1200}, {1920, 1440} };
const int numModes = sizeof(resList) / (sizeof(int)*2);

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

	gDisplay.fullscreenWidth = resList[currentMode][0];
	gDisplay.fullscreenHeight = resList[currentMode][1];

	gDisplay.wide = false;

	if(resList[currentMode][1] == 720 || resList[currentMode][1] == 800 || resList[currentMode][1] == 1080 && (resList[currentMode][0] == 1920 || resList[currentMode][1] == 1200))
		gDisplay.wide = true;

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

	if(resList[currentMode][1] == 720)
		sprintf(pCurrentRes, "720p", resList[currentMode][0], resList[currentMode][1]);
	else if(resList[currentMode][1] == 1080)
		sprintf(pCurrentRes, "1080p", resList[currentMode][0], resList[currentMode][1]);
	else
		sprintf(pCurrentRes, "%dx%d", resList[currentMode][0], resList[currentMode][1]);
	pRes->data = 1;
}

int MFDisplayPC_HandleWindowMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!gFujiInitialised)
		return 1;

	switch(message)
	{
		case WM_ACTIVATE:
			MFInputPC_Acquire(wParam != WA_INACTIVE);

			if(wParam != WA_INACTIVE)
			{
				if(!gDisplay.windowed)
				{
					MFDisplay_ResetDisplay();
				}

				// scan key states...

				// using GetKeyState() for windows keystates

				// or read the state from directinput
			}
			else
			{
			}
			break;

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_KEYMENU:
					if(initialised && lParam == VK_RETURN)
					{
						gDisplay.windowed = !gDisplay.windowed;
						MFDisplay_ResetDisplay();

						if(gDisplay.windowed)
						{
							int xframe = GetSystemMetrics(SM_CXFRAME)*2;
							int yframe = GetSystemMetrics(SM_CYFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
							MoveWindow(apphWnd, wndX, wndY, gDisplay.width + xframe, gDisplay.height + yframe, true);
						}
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
				wndY = LOWORD(lParam);
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

	gDisplay.fullscreenWidth = gDisplay.width = width;
	gDisplay.fullscreenHeight = gDisplay.height = height;
	gDisplay.refreshRate = rate;
	gDisplay.colourDepth = 32;
	gDisplay.windowed = true;
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

		DWORD dwStyle = WS_POPUP|WS_OVERLAPPEDWINDOW;
		DWORD dwExStyle = 0;

#if MF_RENDERER != MF_DRIVER_D3D9
		if(!gDisplay.windowed)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW;
		}
#endif

		AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

		apphWnd = CreateWindowEx(dwExStyle, "FujiWin", gDefaults.display.pWindowTitle, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, wndX, wndY, rect.right-rect.left, rect.bottom-rect.top, NULL, NULL, apphInstance, NULL);
		if(!apphWnd)
		{
			MessageBox(NULL,"Failed To Create Window.","Error!",MB_OK|MB_ICONERROR);
			return 3;
		}

#if MF_RENDERER != MF_DRIVER_D3D9
		// if we're not using D3D, we'll have to manage the display mode ourselves...
		if(!gDisplay.windowed)
		{
			ShowCursor(FALSE);

			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = gDisplay.fullscreenWidth;
			dmScreenSettings.dmPelsHeight = gDisplay.fullscreenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				if(MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
				{
					gDisplay.windowed = true;
				}
				else
				{
					MessageBox(NULL, "Program Will Now Close.", "Error!", MB_OK|MB_ICONSTOP);
					return FALSE;
				}
			}
		}
#endif

		ShowWindow(apphWnd, SW_SHOW);
		SetForegroundWindow(apphWnd);
		SetFocus(apphWnd);
	}

	MFRenderer_CreateDisplay();

	initialised = true;

	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);
	sprintf(pCurrentRes, "%dx%d", resList[currentMode][0], resList[currentMode][1]);

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
