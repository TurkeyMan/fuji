#include "Common.h"
#include "Display.h"
#include "DebugMenu.h"
#include "Input_PC.h"

#include <stdio.h>

void Display_ResetDisplay();

IDirect3D9 *d3d9;
IDirect3DDevice9 *pd3dDevice;

bool isortho = false;
float fieldOfView;

extern Vector4 gClearColour;

extern HINSTANCE apphInstance;
HWND apphWnd;
int wndX = 0, wndY = 0;

bool initialised = false;

// i will dynamically construct a list later from supported resolution
// provided by directx, verified by the monitor driver
const int resList[][2] = { {320, 240}, {400, 300}, {640, 480}, {800, 600}, {1024, 768}, {1152, 864}, {1280, 720}, {1280,800}, {1280, 1024}, {1600, 1200}, {1920, 1080}, {1920, 1200}, {1920, 1440} };
const int numModes = sizeof(resList) / (sizeof(int)*2);

// debug menu resolution setting
char pCurrentRes[16] = "####x####";
char *resStrings[] = { "-", pCurrentRes, "+", NULL };

MenuItemIntString resSelect(resStrings, 1);
MenuItemStatic applyDisplayMode;

int currentMode = 2;

// apply display mode callback
void ApplyDisplayModeCallback(MenuObject *pMenu, void *pData)
{
	CALLSTACK;

	display.fullscreenWidth = resList[currentMode][0];
	display.fullscreenHeight = resList[currentMode][1];

	display.wide = false;

	if(resList[currentMode][1] == 720 || resList[currentMode][1] == 800 || resList[currentMode][1] == 1080 && (resList[currentMode][0] == 1920 || resList[currentMode][1] == 1200))
		display.wide = true;

	display.width = display.fullscreenWidth;
	display.height = display.fullscreenHeight;

	if(display.windowed)
	{
		int xframe = GetSystemMetrics(SM_CXFRAME)*2;
		int yframe = GetSystemMetrics(SM_CYFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
		MoveWindow(apphWnd, wndX, wndY, display.width + xframe, display.height + yframe, true);
	}
	else
	{
		Display_ResetDisplay();
	}
}

// resolution change callback
void ChangeResCallback(MenuObject *pMenu, void *pData)
{
	CALLSTACK;

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
		sprintf(resStrings[1], "720p", resList[currentMode][0], resList[currentMode][1]);
	else if(resList[currentMode][1] == 1080)
		sprintf(resStrings[1], "1080p", resList[currentMode][0], resList[currentMode][1]);
	else
		sprintf(resStrings[1], "%dx%d", resList[currentMode][0], resList[currentMode][1]);
	pRes->data = 1;
}

// windows WndProc
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CALLSTACK;

	switch(message)
	{
		case WM_ACTIVATE:
			Input_Acquire(wParam != WA_INACTIVE);

			if(wParam != WA_INACTIVE)
			{
				if(!display.windowed)
				{
					Display_ResetDisplay();
				}
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
						display.windowed = !display.windowed;
						Display_ResetDisplay();

						if(display.windowed)
						{
							int xframe = GetSystemMetrics(SM_CXFRAME)*2;
							int yframe = GetSystemMetrics(SM_CYFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
							MoveWindow(apphWnd, wndX, wndY, display.width + xframe, display.height + yframe, true);
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
			if(initialised && display.windowed)
			{
				wndX = LOWORD(lParam);
				wndY = LOWORD(lParam);
			}
			break;

		case WM_SIZE:
			if(initialised && display.windowed && wParam != SIZE_MINIMIZED)
			{
				RECT r, cr;
				GetWindowRect(apphWnd, &r);
				GetClientRect(apphWnd, &cr);

				wndX = r.left < 640 ? r.left : 0;
				wndY = r.top < 480 ? r.top : 0;

				display.width = cr.right - cr.left;
				display.height = cr.bottom - cr.top;
				Display_ResetDisplay();
			}
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			break;
	}

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Display_DestroyWindow()
{
	CALLSTACK;

	if(pd3dDevice)
	{
		pd3dDevice->Release();
		pd3dDevice=NULL;
	}

	if(apphWnd && !DestroyWindow(apphWnd))
	{
		MessageBox(NULL, "Could Not Release Window.", "Error", MB_OK|MB_ICONERROR);
	}

	apphWnd = NULL;
}

int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	CALLSTACK;

	display.fullscreenWidth = display.width = width;
	display.fullscreenHeight = display.height = height;
	display.refreshRate = rate;
	display.colourDepth = 32;
	display.windowed = true;
	display.wide = false;
	display.progressive = true;

	WNDCLASS wc;

	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		MessageBox(NULL,"Unable to Create the D3D Device.","Error!",MB_OK|MB_ICONERROR);
		return 1;
	}

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = apphInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "FujiWin";

	if(!RegisterClass(&wc))
	{
		MessageBox(NULL,"Failed To Register The Window Class.","Error!",MB_OK|MB_ICONERROR);
		return 2;
	}

	int xframe = GetSystemMetrics(SM_CXFRAME)*2;
	int yframe = GetSystemMetrics(SM_CYFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);

	apphWnd = CreateWindowEx(NULL, "FujiWin", "Fuji Window", WS_POPUP|WS_OVERLAPPEDWINDOW, wndX, wndY, display.width + xframe, display.height + yframe, NULL, NULL, apphInstance, NULL);
    if(!apphWnd)
	{
		MessageBox(NULL,"Failed To Create Window.","Error!",MB_OK|MB_ICONERROR);
		return 3;
	}

	ShowWindow(apphWnd, SW_SHOW);
	SetFocus(apphWnd);

	D3DCAPS9 deviceCaps;
	d3d9->GetDeviceCaps(0, D3DDEVTYPE_HAL, &deviceCaps);

	D3DFORMAT PixelFormat;

	D3DPRESENT_PARAMETERS present;
	ZeroMemory(&present, sizeof(present));

	if(!display.windowed)
	{
		present.SwapEffect						= D3DSWAPEFFECT_FLIP;
		present.Windowed						= FALSE;
		present.BackBufferFormat				= (display.colourDepth == 32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		present.BackBufferWidth					= display.width;
		present.BackBufferHeight				= display.height;
		present.BackBufferCount					= 2;
		present.EnableAutoDepthStencil			= TRUE;
		present.AutoDepthStencilFormat			= D3DFMT_D24S8;
//		present.AutoDepthStencilFormat			= (display.zBufferBits == 32) ? D3DFMT_D24S8 : D3DFMT_D16;
		present.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
		present.PresentationInterval			= D3DPRESENT_INTERVAL_ONE;
//		present.PresentationInterval			= display.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		present.hDeviceWindow					= apphWnd;

		PixelFormat = present.BackBufferFormat;
	}
	else
	{
		D3DDISPLAYMODE d3ddm;
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		present.SwapEffect              = D3DSWAPEFFECT_COPY;
		present.Windowed                = TRUE;
		present.BackBufferFormat        = d3ddm.Format;
		present.EnableAutoDepthStencil	= TRUE;
		present.AutoDepthStencilFormat	= D3DFMT_D24S8;
		present.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
//		present.AutoDepthStencilFormat	= (display.zBufferBits == 32) ? D3DFMT_D24S8 : D3DFMT_D16;
//		present.PresentationInterval	= display.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		present.hDeviceWindow			= apphWnd;

		PixelFormat = d3ddm.Format;
	}

	int b=0;
	DWORD processing = D3DCREATE_MULTITHREADED;

	if(deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		processing |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		LOGD("Warning: Hardware does not support HardwareVertexProcessing, Attempting to use SoftwareVertexProcessing instead..");
		processing |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	for(int a=0; a<2&&!b; a++)
	{
		if(d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, PixelFormat, PixelFormat, true)==D3D_OK)
		{
			if(FAILED(d3d9->CreateDevice(0, D3DDEVTYPE_HAL, apphWnd, processing, &present, &pd3dDevice)))
			{
				LOGD("Error: Failed to create Direct3D device. Cant create game window.");
				Display_DestroyWindow();
				MessageBox(NULL,"Failed to create Direct3D device.\nCant create game window.","Error!",MB_OK|MB_ICONERROR);
				return 4;
			}
			else b=1;
		}
		else
		{
			if(a == 0)
			{
				MessageBox(NULL,"Unsuitable display mode.\nAttempting default.","Error!",MB_OK|MB_ICONERROR);
			}
			else
			{
				MessageBox(NULL,"No suitable hardware supported Display Mode could be found.\nCant create game window.","Error!",MB_OK|MB_ICONERROR);
				Display_DestroyWindow();
				return 5;
			}
		}
	}

	pd3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

/*
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d9) return 1;

	memset(&presentparams, 0, sizeof(D3DPRESENT_PARAMETERS));
	presentparams.BackBufferWidth = width;
	presentparams.BackBufferHeight = height;
	presentparams.BackBufferFormat = (bpp == 32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
	presentparams.BackBufferCount = triplebuffer ? 2 : 1;
	presentparams.MultiSampleType = D3DMULTISAMPLE_NONE;
	presentparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentparams.EnableAutoDepthStencil = TRUE;
	presentparams.AutoDepthStencilFormat = D3DFMT_D24S8;
	presentparams.FullScreen_RefreshRateInHz = rate; 
	presentparams.FullScreen_PresentationInterval = vsync ? D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE : D3DPRESENT_INTERVAL_IMMEDIATE;
	presentparams.Flags = (wide ? D3DPRESENTFLAG_WIDESCREEN : NULL) | (progressive ? D3DPRESENTFLAG_PROGRESSIVE : NULL);

	hr = d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentparams, &pd3dDevice);
	if(hr != D3D_OK) return 2;
*/
	initialised = true;

	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);
	sprintf(resStrings[1], "%dx%d", resList[currentMode][0], resList[currentMode][1]);

	return 0;
}

void Display_ResetDisplay()
{
	CALLSTACK;

	D3DFORMAT PixelFormat;

	D3DPRESENT_PARAMETERS present;
	ZeroMemory(&present, sizeof(present));

	if(!display.windowed)
	{
		present.SwapEffect						= D3DSWAPEFFECT_FLIP;
		present.Windowed						= FALSE;
		present.BackBufferFormat				= (display.colourDepth == 32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		present.BackBufferWidth					= display.fullscreenWidth;
		present.BackBufferHeight				= display.fullscreenHeight;
		present.BackBufferCount					= 1;
		present.EnableAutoDepthStencil			= TRUE;
		present.AutoDepthStencilFormat			= D3DFMT_D24S8;
//		present.AutoDepthStencilFormat			= (display.zBufferBits == 32) ? D3DFMT_D24S8 : D3DFMT_D16;
		present.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
		present.PresentationInterval			= D3DPRESENT_INTERVAL_ONE;
//		present.PresentationInterval			= display.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		present.hDeviceWindow					= apphWnd;

		PixelFormat = present.BackBufferFormat;
	}
	else
	{
		D3DDISPLAYMODE d3ddm;
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		present.SwapEffect              = D3DSWAPEFFECT_COPY;
		present.Windowed                = TRUE;
		present.BackBufferFormat        = d3ddm.Format;
		present.BackBufferWidth			= display.width;
		present.BackBufferHeight		= display.height;
		present.BackBufferCount			= 1;
		present.EnableAutoDepthStencil	= TRUE;
		present.AutoDepthStencilFormat	= D3DFMT_D24S8;
		present.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
//		present.AutoDepthStencilFormat	= (display.zBufferBits == 32) ? D3DFMT_D24S8 : D3DFMT_D16;
//		present.PresentationInterval	= display.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		present.hDeviceWindow			= apphWnd;

		PixelFormat = d3ddm.Format;
	}

	HRESULT hr;

	hr = pd3dDevice->Reset(&present);

	switch(hr)
	{
		case D3DERR_DEVICELOST:
			MessageBox(0, "Reset: D3DERR_DEVICELOST", "Error!", MB_OK|MB_ICONERROR);
			break;
		case D3DERR_DRIVERINTERNALERROR:
			MessageBox(0, "Reset: D3DERR_DRIVERINTERNALERROR", "Error!", MB_OK|MB_ICONERROR);
			break;
		case D3DERR_INVALIDCALL:
			MessageBox(0, "Reset: D3DERR_INVALIDCALL", "Error!", MB_OK|MB_ICONERROR);
			break;
	}
}

void Display_DestroyDisplay()
{
	CALLSTACK;

	pd3dDevice->Release();
	d3d9->Release();
}

void Display_BeginFrame()
{
	CALLSTACK;

	pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void Display_EndFrame()
{
	CALLSTACK;

	pd3dDevice->EndScene();
	pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Display_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void Display_ClearScreen(uint32 flags)
{
	CALLSTACKc;

	pd3dDevice->Clear(0, NULL, (CS_Colour ? D3DCLEAR_TARGET : NULL)|(CS_ZBuffer ? D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL : NULL), gClearColour.ToPackedColour(), 1.0f, 0);
}

void SetViewport(float x, float y, float width, float height)
{
	CALLSTACK;

	D3DVIEWPORT9 vp;
	vp.X = (DWORD)((x / 640.0f) * (float)display.width);
	vp.Y = (DWORD)((y / 480.0f) * (float)display.height);
	vp.Width = (DWORD)((width / 640.0f) * (float)display.width);
	vp.Height = (DWORD)((height / 480.0f) * (float)display.height);
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}

void ResetViewport()
{
	CALLSTACK;

	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = display.width;
	vp.Height = display.height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}
