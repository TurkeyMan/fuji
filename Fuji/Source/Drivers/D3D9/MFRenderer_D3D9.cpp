#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_D3D9
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_D3D9
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_D3D9
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_D3D9
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_D3D9
	#define MFRenderer_LostFocus MFRenderer_LostFocus_D3D9
	#define MFRenderer_GainedFocus MFRenderer_GainedFocus_D3D9
	#define MFRenderer_BeginFramePlatformSpecific MFRenderer_BeginFramePlatformSpecific_D3D9
	#define MFRenderer_EndFramePlatformSpecific MFRenderer_EndFramePlatformSpecific_D3D9
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_D3D9
	#define MFRenderer_SetViewport MFRenderer_SetViewport_D3D9
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_D3D9
	#define MFRenderer_GetDeviceRenderTarget MFRenderer_GetDeviceRenderTarget_D3D9
	#define MFRenderer_GetDeviceDepthStencil MFRenderer_GetDeviceDepthStencil_D3D9
	#define MFRenderer_SetRenderTargetPlatformSpecific MFRenderer_SetRenderTargetPlatformSpecific_D3D9
	#define MFRenderer_GetTexelCenterOffset MFRenderer_GetTexelCenterOffset_D3D9
	#define MFRendererInternal_SortElements MFRendererInternal_SortElements_D3D9

	//MFTexture
	#define MFTexture_Release MFTexture_Release_D3D9
	#define MFTexture_Recreate MFTexture_Recreate_D3D9
#endif

#include "MFWindow_Internal.h"
#include "MFVertex_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFView_Internal.h"
#include "MFSystem_Internal.h"
#include "MFRenderTarget_Internal.h"

#include "MFRenderer_Internal.h"
#include "MFRenderer_D3D9.h"

#include <D3Dcommon.h>

#pragma comment(lib, "d3d9")

extern D3DFORMAT gD3D9Format[ImgFmt_Max];

static IDirect3D9 *d3d9;
IDirect3DDevice9 *pd3dDevice;

static IDirect3DSurface9 *gpRenderTarget = NULL;
static IDirect3DSurface9 *gpZTarget = NULL;

static MFRenderTarget *gpDeviceRenderTarget = NULL;
static MFTexture *gpDeviceColourTarget = NULL;
static MFTexture *gpDeviceZTarget = NULL;

D3DCAPS9 gD3D9DeviceCaps;

static MFRect gCurrentViewport;

void MFRenderer_D3D9_SetDebugName(IDirect3DResource9* pResource, const char* pName)
{
#if !defined(MF_RETAIL)
	if (pResource)
		pResource->SetPrivateData(WKPDID_D3DDebugObjectName, pName, (DWORD)MFString_Length(pName), 0);
#endif
}

void MFRenderer_InitModulePlatformSpecific()
{
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		MessageBoxA(NULL,"Unable to Create the D3D Device. You probably need to update DirectX.","Error!",MB_OK|MB_ICONERROR);
		return;
	}

	d3d9->GetDeviceCaps(0, D3DDEVTYPE_HAL, &gD3D9DeviceCaps);
}

void MFRenderer_DeinitModulePlatformSpecific()
{
	if(d3d9)
	{
		d3d9->Release();
	}
}

static bool ChooseFormats(MFImageFormat &backBuffer, MFImageFormat &depthStencil, bool bFullscreen)
{
	bool bAutoBackBuffer = (backBuffer & ImgFmt_SelectDefault) != 0;
	bool bAutoDepth = (depthStencil & ImgFmt_SelectDefault) != 0;
	bool bDepthWithStencil = (depthStencil & ImgFmt_SelectDepthStencil) == ImgFmt_SelectDepthStencil;

	// find a suitable back buffer format
	if(bAutoBackBuffer)
	{
		backBuffer = ImgFmt_A8R8G8B8;
	}

	// find a suitable zbuffer format
	if(bAutoDepth)
	{
		MFDebug_Assert((depthStencil & ImgFmt_SelectDepthStencil) >= ImgFmt_SelectDepth, "Invalid depth/stencil buffer format!");

		if(bDepthWithStencil)
		{
			// with stencil
			depthStencil = ImgFmt_D24S8;
		}
		else
		{
			// without stencil
			if((depthStencil & 0x3) == 1)	// SelectNicest
				depthStencil = ImgFmt_D32F;
			else
				depthStencil = ImgFmt_D24X8;
		}
	}

	D3DFORMAT adapterFormat = backBuffer == ImgFmt_A8R8G8B8 ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8;
	if(d3d9->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, adapterFormat, gD3D9Format[backBuffer], gD3D9Format[depthStencil]) != D3D_OK)
	{
		// try and find formats that work...
		struct AutoFormat
		{
			MFImageFormat bb, ds;
		};
		AutoFormat autoFormatsStencil[] =
		{
			{ ImgFmt_A8R8G8B8, ImgFmt_D24S8 },
			{ ImgFmt_R5G6B5, ImgFmt_D24S8 },
			{ ImgFmt_Unknown, ImgFmt_Unknown }
		};
		AutoFormat autoFormatsNoStencil[] =
		{
			{ ImgFmt_A8R8G8B8, ImgFmt_D24X8 },
			{ ImgFmt_A8R8G8B8, ImgFmt_D16 },
			{ ImgFmt_R5G6B5, ImgFmt_D24X8 },
			{ ImgFmt_R5G6B5, ImgFmt_D16 },
			{ ImgFmt_Unknown, ImgFmt_Unknown }
		};

		AutoFormat *pAutoFormats = bDepthWithStencil ? autoFormatsStencil : autoFormatsNoStencil;

		int i=0;
		while(pAutoFormats[i].bb != D3DFMT_UNKNOWN)
		{
			backBuffer = bAutoBackBuffer ? pAutoFormats[i].bb : backBuffer;
			depthStencil = bAutoDepth ? pAutoFormats[i].ds : depthStencil;

			adapterFormat = backBuffer == ImgFmt_A8R8G8B8 ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8;
			if(d3d9->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, adapterFormat, gD3D9Format[backBuffer], gD3D9Format[depthStencil]) == D3D_OK &&
				d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, adapterFormat, gD3D9Format[backBuffer], FALSE) == D3D_OK)
				break;
		}
		if(pAutoFormats[i].bb == ImgFmt_Unknown)
		{
			return false;
		}
	}

	return true;
}

static MFImageFormat GetDesktopFormat(UINT adapter, D3DFORMAT *pFormat)
{
	// get active display mode
	D3DDISPLAYMODE d3ddm;
	d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if(pFormat)
		*pFormat = d3ddm.Format;
	switch(d3ddm.Format)
	{
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8: return ImgFmt_A8R8G8B8;
		case D3DFMT_R5G6B5: return ImgFmt_R5G6B5;
		default:
			MFDebug_Assert(false, "Unexpected back buffer format!");
			return ImgFmt_Unknown;
	}
}

int MFRenderer_CreateDisplay(MFDisplay *pDisplay)
{
	D3DPRESENT_PARAMETERS present;
	MFZeroMemory(&present, sizeof(present));

	MFImageFormat backBufferFormat = pDisplay->settings.backBufferFormat;
	MFImageFormat depthStencilFormat = pDisplay->settings.depthStencilFormat;

	if((backBufferFormat & ImgFmt_SelectDefault) | (depthStencilFormat & ImgFmt_SelectDefault))
	{
		if(!ChooseFormats(backBufferFormat, depthStencilFormat, pDisplay->settings.bFullscreen))
		{
			MFDebug_Warn(2, "Incompatible render/depth formats.");
			return 1;
		}
	}

	if(pDisplay->settings.bFullscreen)
	{
		present.SwapEffect					= D3DSWAPEFFECT_FLIP;
		present.Windowed					= FALSE;
		present.BackBufferFormat			= gD3D9Format[backBufferFormat];
		present.BackBufferWidth				= pDisplay->settings.width;
		present.BackBufferHeight			= pDisplay->settings.height;
		present.BackBufferCount				= pDisplay->settings.numBuffers;
		present.FullScreen_RefreshRateInHz	= pDisplay->settings.refreshRate;
	}
	else
	{
		present.SwapEffect	= D3DSWAPEFFECT_COPY;
		present.Windowed	= TRUE;

		backBufferFormat = GetDesktopFormat(D3DADAPTER_DEFAULT, &present.BackBufferFormat);
	}

	present.EnableAutoDepthStencil	= TRUE;
	present.AutoDepthStencilFormat	= gD3D9Format[depthStencilFormat];
	present.PresentationInterval	= pDisplay->settings.bVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	present.hDeviceWindow			= (HWND)MFWindow_GetSystemWindowHandle(pDisplay->settings.pWindow);

	DWORD processing = D3DCREATE_MULTITHREADED;
	if(gD3D9DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT && ((gD3D9DeviceCaps.VertexShaderVersion >> 8) & 0xFF) >= 2)
	{
		processing |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		MFDebug_Warn(2, "Hardware does not support HardwareVertexProcessing, Attempting to use SoftwareVertexProcessing instead..");
		processing |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	HRESULT hr = d3d9->CreateDevice(0, D3DDEVTYPE_HAL, present.hDeviceWindow, processing, &present, &pd3dDevice);
	if(FAILED(hr))
	{
		const char *pMessage = "UNKNOWN ERROR";
		switch(hr)
		{
			case D3DERR_DEVICELOST:
				pMessage = "D3DERR_DEVICELOST";
				break;
			case D3DERR_INVALIDCALL:
				pMessage = "D3DERR_INVALIDCALL";
				break;
			case D3DERR_NOTAVAILABLE:
				pMessage = "D3DERR_NOTAVAILABLE";
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				pMessage = "D3DERR_OUTOFVIDEOMEMORY";
				break;
		}
		pMessage = MFStr("Failed to create Direct3D device with error: %s.\nCant create display!", pMessage);
		MFDebug_Error(pMessage);
		MessageBoxA(NULL, pMessage, "Error!", MB_OK|MB_ICONERROR);
		return 2;
	}

	pDisplay->settings.backBufferFormat = backBufferFormat;
	pDisplay->settings.depthStencilFormat = depthStencilFormat;

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (gD3D9DeviceCaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED) ? TRUE : FALSE);

	// we always use this alpha test for 'masking'...
	pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	D3DVIEWPORT9 vp;
	pd3dDevice->GetViewport(&vp);
	gCurrentViewport.x = (float)vp.X;
	gCurrentViewport.y = (float)vp.Y;
	gCurrentViewport.width = (float)vp.Width;
	gCurrentViewport.height = (float)vp.Height;

	pd3dDevice->GetRenderTarget(0, &gpRenderTarget);
	pd3dDevice->GetDepthStencilSurface(&gpZTarget);

	MFTextureDesc texDesc = { MFTexType_2D, backBufferFormat, (int)present.BackBufferWidth, (int)present.BackBufferHeight, 0, 0, 1, MFTCF_RenderTarget };
	gpDeviceColourTarget = MFTexture_InitTexture(&texDesc, MFRD_OpenGL, 0);
	gpDeviceColourTarget->pName = "Device Colour Target";
	gpDeviceColourTarget->pSurfaces[0].platformData = (uint64)(size_t)gpRenderTarget;

	texDesc.format = depthStencilFormat;
	gpDeviceZTarget = MFTexture_InitTexture(&texDesc, MFRD_OpenGL, 0);
	gpDeviceZTarget->pName = "Device Depth Stencil";
	gpDeviceZTarget->pSurfaces[0].platformData = (uint64)(size_t)gpZTarget;

	MFRenderTargetDesc desc;
	desc.pName = "Device Render Target";
	desc.width = (int)present.BackBufferWidth;
	desc.height = (int)present.BackBufferHeight;
	desc.colourTargets[0].pSurface = gpDeviceColourTarget;
	desc.depthStencil.pSurface = gpDeviceZTarget;
	gpDeviceRenderTarget = MFRenderTarget_Create(&desc);

	return 0;
}

void MFRenderer_DestroyDisplay(MFDisplay *pDisplay)
{
	MFRenderTarget_Release(gpDeviceRenderTarget);

	if(gpRenderTarget)
	{
		gpRenderTarget->Release();
		gpRenderTarget = NULL;
	}

	if(gpZTarget)
	{
		gpZTarget->Release();
		gpZTarget = NULL;
	}

	if(pd3dDevice)
	{
		pd3dDevice->Release();
		pd3dDevice=NULL;
	}
}

bool MFRenderer_ResetDisplay(MFDisplay *pDisplay, const MFDisplaySettings *pSettings)
{
	// free resources
	void MFTexture_Release();
	MFTexture_Release();
	void MFRenderState_Release();
	MFRenderState_Release();

	if(gpRenderTarget)
	{
		gpRenderTarget->Release();
		gpRenderTarget = NULL;
	}
	if(gpZTarget)
	{
		gpZTarget->Release();
		gpZTarget = NULL;
	}

	if(!pd3dDevice)
		return false;

	// setup present params
	D3DPRESENT_PARAMETERS present;
	MFZeroMemory(&present, sizeof(present));

	MFImageFormat backBufferFormat = pSettings->backBufferFormat;
	MFImageFormat depthStencilFormat = pSettings->depthStencilFormat;

	if((backBufferFormat & ImgFmt_SelectDefault) | (depthStencilFormat & ImgFmt_SelectDefault))
	{
		if(!ChooseFormats(backBufferFormat, depthStencilFormat, pSettings->bFullscreen))
		{
			MFDebug_Warn(2, "Incompatible render/depth formats.");
			return 1;
		}
	}

	if(pSettings->bFullscreen)
	{
		present.SwapEffect						= D3DSWAPEFFECT_FLIP;
		present.Windowed						= FALSE;
		present.BackBufferFormat				= gD3D9Format[backBufferFormat];
		present.BackBufferWidth					= pSettings->width;
		present.BackBufferHeight				= pSettings->height;
		present.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
	}
	else
	{
		present.SwapEffect              = D3DSWAPEFFECT_COPY;
		present.Windowed                = TRUE;
		present.BackBufferWidth			= pSettings->width;
		present.BackBufferHeight		= pSettings->height;

		// TODO: This is broken! If swapping away from fullscreen, this will capture the fullscreen mode rather than the desktop mode... >_<
		backBufferFormat = GetDesktopFormat(D3DADAPTER_DEFAULT, &present.BackBufferFormat);
	}

	present.EnableAutoDepthStencil	= TRUE;
	present.AutoDepthStencilFormat	= gD3D9Format[depthStencilFormat];
	present.PresentationInterval	= pSettings->bVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	present.hDeviceWindow			= (HWND)MFWindow_GetSystemWindowHandle(pSettings->pWindow);

	// reset the device
	HRESULT hr;
	hr = pd3dDevice->Reset(&present);

	switch(hr)
	{
		case D3DERR_DEVICELOST:
			MFDebug_Warn(1, "Reset(): D3DERR_DEVICELOST");
			break;
		case D3DERR_DRIVERINTERNALERROR:
			MFDebug_Warn(1, "Reset(): D3DERR_DRIVERINTERNALERROR");
			break;
		case D3DERR_INVALIDCALL:
			MFDebug_Warn(1, "Reset(): D3DERR_INVALIDCALL");
			break;
		case D3D_OK:
			MFRenderer_ResetViewport();
	}

	if(FAILED(hr))
		return false;

	// recreate resources
	pd3dDevice->GetRenderTarget(0, &gpRenderTarget);
	pd3dDevice->GetDepthStencilSurface(&gpZTarget);

	gpDeviceColourTarget->imageFormat = backBufferFormat;
	gpDeviceColourTarget->pSurfaces[0].width = (int)present.BackBufferWidth;
	gpDeviceColourTarget->pSurfaces[0].height = (int)present.BackBufferHeight;
	gpDeviceColourTarget->pSurfaces[0].bitsPerPixel = (uint8)MFImage_GetBitsPerPixel(backBufferFormat);
	gpDeviceColourTarget->pSurfaces[0].platformData = (uint64)(size_t)gpRenderTarget;

	gpDeviceZTarget->imageFormat = depthStencilFormat;
	gpDeviceZTarget->pSurfaces[0].width = (int)present.BackBufferWidth;
	gpDeviceZTarget->pSurfaces[0].height = (int)present.BackBufferHeight;
	gpDeviceZTarget->pSurfaces[0].bitsPerPixel = (uint8)MFImage_GetBitsPerPixel(depthStencilFormat);
	gpDeviceZTarget->pSurfaces[0].platformData = (uint64)(size_t)gpZTarget;

	gpDeviceRenderTarget->width = (int)present.BackBufferWidth;
	gpDeviceRenderTarget->height = (int)present.BackBufferHeight;

	void MFTexture_Recreate();
	MFTexture_Recreate();
	void MFRenderState_Recreate();
	MFRenderState_Recreate();

	MFSystemCallbackFunction pCallback = MFSystem_GetSystemCallback(MFCB_DisplayReset);
	if(pCallback)
		pCallback();

	return true;
}

void MFRenderer_LostFocus(MFDisplay *pDisplay)
{
}

void MFRenderer_GainedFocus(MFDisplay *pDisplay)
{
	if(pDisplay->settings.bFullscreen)
		MFRenderer_ResetDisplay(pDisplay, &pDisplay->settings);
}

bool MFRenderer_BeginFramePlatformSpecific()
{
	MFCALLSTACK;

	HRESULT hr = pd3dDevice->BeginScene();
	if(FAILED(hr))
		return false;

	return true;
}

void MFRenderer_EndFramePlatformSpecific()
{
	MFCALLSTACK;

	pd3dDevice->EndScene();
	HRESULT hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
	if(FAILED(hr))
	{
		switch(hr)
		{
			case D3DERR_DEVICEREMOVED:
				MFDebug_Warn(1, "D3DERR_DEVICEREMOVED");
				break;
			default:
			{
				hr = pd3dDevice->TestCooperativeLevel();
				switch(hr)
				{
					case D3DERR_DEVICENOTRESET:
					{
						// try and reset the display...
						MFDisplay *pDisplay = MFDisplay_GetCurrent();
						MFRenderer_ResetDisplay(pDisplay, &pDisplay->settings);
						break;
					}
					case D3DERR_DEVICELOST:
						// this is usual when the window is hidden
						break;
					case D3DERR_DRIVERINTERNALERROR:
						MFDebug_Warn(1, "D3DERR_DRIVERINTERNALERROR");
						break;
					default:
						MFDebug_Warn(1, "Unknown error!");
						break;
				}
				break;
			}
		}
	}
}

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags, const MFVector &colour, float z, int stencil)
{
	MFCALLSTACKc;

	pd3dDevice->Clear(0, NULL, ((flags & MFRCF_Colour) ? D3DCLEAR_TARGET : NULL)|((flags & MFRCF_Depth) ? D3DCLEAR_ZBUFFER : NULL)|((flags & MFRCF_Stencil) ? D3DCLEAR_STENCIL : NULL), colour.ToPackedColour(), z, stencil);
}

MF_API void MFRenderer_SetViewport(MFRect *pRect)
{
	MFCALLSTACK;

	gCurrentViewport = *pRect;

	D3DVIEWPORT9 vp;
	vp.X = (DWORD)pRect->x;
	vp.Y = (DWORD)pRect->y;
	vp.Width = (DWORD)pRect->width;
	vp.Height = (DWORD)pRect->height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}

MF_API void MFRenderer_ResetViewport()
{
	MFCALLSTACK;

	MFDisplay *pDisplay = MFDisplay_GetCurrent();

	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = pDisplay->settings.width;
	vp.Height = pDisplay->settings.height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)vp.Width;
	gCurrentViewport.height = (float)vp.Height;

	pd3dDevice->SetViewport(&vp);
}

MF_API MFRenderTarget* MFRenderer_GetDeviceRenderTarget()
{
	return gpDeviceRenderTarget;
}

void MFRenderer_SetRenderTargetPlatformSpecific(MFRenderTarget *pRenderTarget)
{
	for(int a=0; a<8; ++a)
	{
		if(pRenderTarget->pColourTargets[a])
		{
			IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)(size_t)pRenderTarget->pColourTargets[a]->pSurfaces[0].platformData;
			pd3dDevice->SetRenderTarget(a, pSurface);
		}
		else
		{
			pd3dDevice->SetRenderTarget(a, NULL);
		}
	}

	if(pRenderTarget->pDepthStencil)
	{
		IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)(size_t)pRenderTarget->pDepthStencil->pSurfaces[0].platformData;
		pd3dDevice->SetDepthStencilSurface(pSurface);
	}
	else
	{
		pd3dDevice->SetDepthStencilSurface(NULL);
	}
}

MF_API float MFRenderer_GetTexelCenterOffset()
{
	return 0.5f;
}

static int SortDefault(const void *p1, const void *p2)
{
	MFRenderElement *pE1 = (MFRenderElement*)p1;
	MFRenderElement *pE2 = (MFRenderElement*)p2;

	int pred = pE1->primarySortKey - pE2->primarySortKey;
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterial - (char*)pE1->pMaterial);
	if(pred) return pred;
	pred = (int)((char*)pE2->pViewState - (char*)pE1->pViewState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pEntityState - (char*)pE1->pEntityState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialOverrideState - (char*)pE1->pMaterialOverrideState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialState - (char*)pE1->pMaterialState);
	return pred;
}

static int SortBackToFront(const void *p1, const void *p2)
{
	MFRenderElement *pE1 = (MFRenderElement*)p1;
	MFRenderElement *pE2 = (MFRenderElement*)p2;

	int pred = pE1->primarySortKey - pE2->primarySortKey;
	if(pred) return pred;
    pred = pE2->zSort - pE1->zSort;
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterial - (char*)pE1->pMaterial);
	if(pred) return pred;
	pred = (int)((char*)pE2->pViewState - (char*)pE1->pViewState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pEntityState - (char*)pE1->pEntityState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialOverrideState - (char*)pE1->pMaterialOverrideState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialState - (char*)pE1->pMaterialState);
	return pred;
}

static int SortFrontToBack(const void *p1, const void *p2)
{
	MFRenderElement *pE1 = (MFRenderElement*)p1;
	MFRenderElement *pE2 = (MFRenderElement*)p2;

	int pred = pE1->primarySortKey - pE2->primarySortKey;
	if(pred) return pred;
    pred = pE1->zSort - pE2->zSort;
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterial - (char*)pE1->pMaterial);
	if(pred) return pred;
	pred = (int)((char*)pE2->pViewState - (char*)pE1->pViewState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pEntityState - (char*)pE1->pEntityState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialOverrideState - (char*)pE1->pMaterialOverrideState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialState - (char*)pE1->pMaterialState);
	return pred;
}

static MFRenderSortFunction gSortFunctions[MFRL_SM_Max] =
{
  SortDefault,
  SortFrontToBack,
  SortBackToFront
};

void MFRendererInternal_SortElements(MFRenderLayer &layer)
{
	qsort(layer.elements.getPointer(), layer.elements.size(), sizeof(MFRenderElement), gSortFunctions[layer.sortMode]);
}

//void MFRendererInternal_RenderElement(MFRendererState &state, MFRenderElement &element)

#endif // MF_RENDERER
