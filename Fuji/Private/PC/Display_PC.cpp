#include "Common.h"
#include "Display.h"
#include "DebugMenu.h"

IDirect3D9 *d3d9;
IDirect3DDevice9 *pd3dDevice;

bool isortho = false;
float fieldOfView;

extern HINSTANCE apphInstance;
HWND apphWnd;

void Display_InitModule()
{
	CALLSTACK("Display_InitModule");

	int error;

	// create the display
	error = CreateDisplay(640, 480, 32, 60, true, false, false, false);
	if(error) return;

	DebugMenu_AddMenu("Display Options", "Fuji Options");
}

void Display_DeinitModule()
{
	CALLSTACK("Display_DeinitModule");

	DestroyDisplay();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CALLSTACK("WndProc");

	switch(message)
	{
		case WM_ACTIVATE:
			break;

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_KEYMENU:
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

		case WM_CREATE:
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

int CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	CALLSTACK("CreateDisplay");

	display.width = width;
	display.height = height;
	display.refreshRate = rate;
	display.colourDepth = 32;
	display.windowed = true;

	WNDCLASS wc;

	if(!(d3d9 = Direct3DCreate9(D3D_SDK_VERSION)))
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

	if(!(apphWnd = CreateWindowEx(NULL, "FujiWin", "Fuji Window", WS_POPUP|WS_OVERLAPPEDWINDOW, 0, 0, display.width + xframe, display.height + yframe, NULL, NULL, apphInstance, NULL)))
	{
		MessageBox(NULL,"Failed To Create Window.","Error!",MB_OK|MB_ICONERROR);
		return 3;
	}

	ShowWindow(apphWnd, SW_SHOW);
	SetFocus(apphWnd);

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
	for(int a=0; a<2&&!b; a++)
	{
		if(d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, PixelFormat, PixelFormat, true)==D3D_OK)
		{
			if(FAILED(d3d9->CreateDevice(0, D3DDEVTYPE_HAL, apphWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED, &present, &pd3dDevice)))
			{
				Display_DestroyWindow();
				MessageBox(NULL,"No suitable hardware supported Display Mode could be found.\nCant create game window.","Error!",MB_OK|MB_ICONERROR);
				return 4;
			}
			else b=1;
		}
		else
		{
			if(a=0)
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
	return 0;
}

void DestroyDisplay()
{
	CALLSTACK("DestroyDisplay");

	pd3dDevice->Release();
	d3d9->Release();
}

void Display_BeginFrame()
{
	CALLSTACK("Display_BeginFrame");

	pd3dDevice->BeginScene();
}

void Display_EndFrame()
{
	CALLSTACK("Display_EndFrame");

	pd3dDevice->EndScene();
	pd3dDevice->Present(NULL, NULL, NULL, NULL);

}

void ClearScreen()
{
	CALLSTACK("ClearScreen");

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000030, 1.0f, 0);
}

void SetProjection(float fov)
{
	CALLSTACK("SetProjection");

	D3DXMATRIX proj;

	fieldOfView = fov;

	// construct and apply perspective projection
	D3DXMatrixPerspectiveFovLH(&proj, fov, ((float)display.width/(float)display.height), 0.1f, 1000.0f);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

bool SetOrtho(bool enable, float width, float height)
{
	CALLSTACK("SetOrtho");

	D3DXMATRIX proj;

	bool t = isortho;
	isortho = enable;

	if(enable)
	{
		float extend = 0.0f;

		// correct for widescreen
//		if(display.wide) extend = (((width/1.333333333f)*1.77777777778f)-width)/2.0f;

		// construct and apply ortho projection
		D3DXMatrixOrthoOffCenterLH(&proj, -extend, width + extend, height, 0, 0.0f, 1000.0f);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);
	}
	else
	{
		SetProjection(fieldOfView);
	}

	return t;
}

void SetViewport(float x, float y, float width, float height)
{
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
	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = display.width;
	vp.Height = display.height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}
