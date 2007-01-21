#include "Fuji.h"
#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

IDirect3D8 *d3d8;
IDirect3DDevice8 *pd3dDevice;

bool isortho = false;
float fieldOfView;

static bool gbWidescreen, gbLetterBox;

extern MFVector gClearColour;

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	DWORD avPack = XGetAVPack();
	DWORD vidMode = XGetVideoStandard();
	DWORD vidFlags = XGetVideoFlags();

	// log the AV pack (just an FYI)
	switch(avPack)
	{
		case XC_AV_PACK_SCART:		MFDebug_Log(2, "MFDisplay: Video output using Scart AV Pack"); break;
		case XC_AV_PACK_HDTV:		MFDebug_Log(2, "MFDisplay: Video output using High Def Pack"); break;
		case XC_AV_PACK_RFU:		MFDebug_Log(2, "MFDisplay: Video output using RF Unit"); break;
		case XC_AV_PACK_SVIDEO:		MFDebug_Log(2, "MFDisplay: Video output using Advanced AV Pack"); break;
		case XC_AV_PACK_STANDARD:	MFDebug_Log(2, "MFDisplay: Video output using Standard AV Pack"); break;
		default:					MFDebug_Log(2, "MFDisplay: Video output using Unknown AV Pack"); break;
	}

	// calculate the frame buffer size and display mode from the dashboard info
	gbWidescreen = !!(vidFlags & (XC_VIDEO_FLAGS_WIDESCREEN));
	gbLetterBox = !!(vidFlags & (XC_VIDEO_FLAGS_LETTERBOX));
	wide = gbWidescreen;

	if(vidMode == XC_VIDEO_STANDARD_PAL_I)
	{
		// PAL modes (poor PAL users dont get high def) :(
		if(vidFlags & XC_VIDEO_FLAGS_PAL_60Hz)
		{
			width = 720;
			height = 480;
			rate = 60;
			progressive = false;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to PAL60%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
		else
		{
			width = 720;
			height = 576;
			rate = 50;
			progressive = false;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to PAL (576i)%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
	}
	else
	{
		// NTSC modes
		if(vidFlags & XC_VIDEO_FLAGS_HDTV_1080i)
		{
			width = 1920;
			height = 1080;
			rate = 60;
			progressive = false;
			MFDebug_Log(2, "MFDisplay: Video Mode set to 1080i");
		}
		else if(vidFlags & XC_VIDEO_FLAGS_HDTV_720p)
		{
			width = 1280;
			height = 720;
			rate = 60;
			progressive = true;
			MFDebug_Log(2, "MFDisplay: Video Mode set to 720p");
		}
		else if(vidFlags & XC_VIDEO_FLAGS_HDTV_480p)
		{
			width = 720;
			height = 480;
			rate = 60;
			progressive = true;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to 480p%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
		else
		{
			width = 720;
			height = 480;
			rate = 60;
			progressive = false;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to NTSC (480i)%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
	}

	// update display parameters
	gDisplay.width = width;
	gDisplay.height = height;
	gDisplay.progressive = progressive;
	gDisplay.refreshRate = rate;
	gDisplay.wide = wide;

	// create D3D interface
	d3d8 = Direct3DCreate8(D3D_SDK_VERSION);
	if(!d3d8)
		return 1;

	// create the D3D device
	D3DPRESENT_PARAMETERS presentparams;
	HRESULT hr;

	MFZeroMemory(&presentparams, sizeof(D3DPRESENT_PARAMETERS));
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
	if(hr != D3D_OK)
		return 2;

	// clear frame buffers to black
	MFVector oldColour = gClearColour;
	MFDisplay_SetClearColour(0,0,0,0);

	MFDisplay_BeginFrame();
	MFDisplay_ClearScreen(CS_All);
	MFDisplay_EndFrame();
	MFDisplay_BeginFrame();
	MFDisplay_ClearScreen(CS_All);
	MFDisplay_EndFrame();
	if(triplebuffer)
	{
		MFDisplay_BeginFrame();
		MFDisplay_ClearScreen(CS_All);
		MFDisplay_EndFrame();
	}

	MFDisplay_SetClearColour(oldColour.x, oldColour.y, oldColour.z, oldColour.w);

	// setup the initial viewport (letterbox)
	MFDisplay_ResetViewport();

	return 0;
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;

	pd3dDevice->Release();
	d3d8->Release();
}

void MFDisplay_BeginFrame()
{
	MFCALLSTACK;

	pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void MFDisplay_EndFrame()
{
	MFCALLSTACK;

	pd3dDevice->EndScene();
	pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void MFDisplay_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void MFDisplay_ClearScreen(uint32 flags)
{
	MFCALLSTACK;

	pd3dDevice->Clear(0, NULL, ((flags&CS_Colour) ? D3DCLEAR_TARGET : NULL)|((flags&CS_ZBuffer) ? D3DCLEAR_ZBUFFER : NULL)|((flags&CS_Stencil) ? D3DCLEAR_STENCIL : NULL), gClearColour.ToPackedColour(), 1.0f, 0);
}

void MFDisplay_SetViewport(float x, float y, float width, float height)
{
	MFCALLSTACK;

	D3DVIEWPORT8 vp;
	vp.X = (DWORD)((x / 640.0f) * (float)gDisplay.width);
	vp.Y = (DWORD)(((gbLetterBox ? y + 60.0f : y) / 480.0f) * (float)gDisplay.height);
	vp.Width = (DWORD)((width / 640.0f) * (float)gDisplay.width);
	vp.Height = (DWORD)(((gbLetterBox ? height * 0.75f : height) / 480.0f) * (float)gDisplay.height);
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}

void MFDisplay_ResetViewport()
{
	MFCALLSTACK;

	D3DVIEWPORT8 vp;
	vp.X = 0;
	vp.Y = gbLetterBox ? (DWORD)(gDisplay.height * 0.125f) : 0;
	vp.Width = gDisplay.width;
	vp.Height = gbLetterBox ? (DWORD)(gDisplay.height * 0.75f) : gDisplay.height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}

float MFDisplay_GetNativeAspectRatio()
{
	return (gbWidescreen || gbLetterBox) ? MFAspect_16x9 : MFAspect_4x3;
}

bool MFDisplay_IsWidescreen()
{
	return gbWidescreen || gbLetterBox;
}
