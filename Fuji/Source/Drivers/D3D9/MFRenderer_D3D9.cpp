#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_D3D9
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_D3D9
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_D3D9
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_D3D9
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_D3D9
	#define MFRenderer_SetDisplayMode MFRenderer_SetDisplayMode_D3D9
	#define MFRenderer_BeginFramePlatformSpecific MFRenderer_BeginFramePlatformSpecific_D3D9
	#define MFRenderer_EndFramePlatformSpecific MFRenderer_EndFramePlatformSpecific_D3D9
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_D3D9
	#define MFRenderer_SetViewport MFRenderer_SetViewport_D3D9
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_D3D9
	#define MFRenderer_GetDeviceRenderTarget MFRenderer_GetDeviceRenderTarget_D3D9
	#define MFRenderer_GetDeviceDepthStencil MFRenderer_GetDeviceDepthStencil_D3D9
	#define MFRenderer_SetRenderTarget MFRenderer_SetRenderTarget_D3D9
	#define MFRenderer_GetTexelCenterOffset MFRenderer_GetTexelCenterOffset_D3D9

	//MFTexture
	#define MFTexture_Release MFTexture_Release_D3D9
	#define MFTexture_Recreate MFTexture_Recreate_D3D9
#endif

#include "MFVertex_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFView_Internal.h"
#include "MFSystem_Internal.h"

#include "MFRenderer_Internal.h"
#include "MFRenderer_D3D9.h"

#include "Shaders/Registers.h"

#include <D3Dcommon.h>

#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")

extern MFSystemCallbackFunction pSystemCallbacks[MFCB_Max];

static IDirect3D9 *d3d9;
IDirect3DDevice9 *pd3dDevice;

static IDirect3DSurface9 *pRenderTarget = NULL;
static IDirect3DSurface9 *pZTarget = NULL;

static MFTexture gDeviceRenderTarget;
static MFTextureTemplateData gDeviceRenderTargetTemplate;
static MFTextureSurfaceLevel gDeviceRenderTargetSurface;
static MFTexture gDeviceZTarget;
static MFTextureTemplateData gDeviceZTargetTemplate;
static MFTextureSurfaceLevel gDeviceZTargetSurface;

D3DCAPS9 gD3D9DeviceCaps;

extern HWND apphWnd;

static int gNumWeights = 0;
static MFRect gCurrentViewport;

void MFRenderer_D3D9_SetDebugName(IDirect3DResource9* pResource, const char* pName)
{
#if !defined(MF_RETAIL)
	if (pResource)
		pResource->SetPrivateData(WKPDID_D3DDebugObjectName, pName, MFString_Length(pName), 0);
#endif
}

void MFRenderer_InitModulePlatformSpecific()
{
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		MessageBox(NULL,"Unable to Create the D3D Device. You probably need to update DirectX.","Error!",MB_OK|MB_ICONERROR);
		return;
	}

	d3d9->GetDeviceCaps(0, D3DDEVTYPE_HAL, &gD3D9DeviceCaps);

	MFZeroMemory(&gDeviceRenderTarget, sizeof(gDeviceRenderTarget));
	MFZeroMemory(&gDeviceRenderTargetTemplate, sizeof(gDeviceRenderTargetTemplate));
	MFZeroMemory(&gDeviceRenderTargetSurface, sizeof(gDeviceRenderTargetSurface));
	MFZeroMemory(&gDeviceZTarget, sizeof(gDeviceZTarget));
	MFZeroMemory(&gDeviceZTargetTemplate, sizeof(gDeviceZTargetTemplate));
	MFZeroMemory(&gDeviceZTargetSurface, sizeof(gDeviceZTargetSurface));

	gDeviceRenderTarget.pTemplateData = &gDeviceRenderTargetTemplate;
	gDeviceRenderTarget.pName = "Device Render Target";
	gDeviceRenderTargetTemplate.pSurfaces = &gDeviceRenderTargetSurface;
	gDeviceRenderTargetTemplate.flags = TEX_RenderTarget;
	gDeviceRenderTargetTemplate.mipLevels = 1;

	gDeviceZTarget.pTemplateData = &gDeviceZTargetTemplate;
	gDeviceZTarget.pName = "Device Depth Stencil";
	gDeviceZTargetTemplate.pSurfaces = &gDeviceZTargetSurface;
	gDeviceZTargetTemplate.flags = TEX_RenderTarget;
	gDeviceZTargetTemplate.mipLevels = 1;
}

void MFRenderer_DeinitModulePlatformSpecific()
{
	if(d3d9)
	{
		d3d9->Release();
	}
}

int MFRenderer_CreateDisplay()
{
	D3DPRESENT_PARAMETERS present;
	MFZeroMemory(&present, sizeof(present));

	if(!gDisplay.windowed)
	{
		present.SwapEffect					= D3DSWAPEFFECT_FLIP;
		present.Windowed					= FALSE;
		present.BackBufferFormat			= (gDisplay.colourDepth == 32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		present.BackBufferWidth				= gDisplay.width;
		present.BackBufferHeight			= gDisplay.height;
		present.BackBufferCount				= 2;
		present.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	}
	else
	{
		D3DDISPLAYMODE d3ddm;
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		// check if this format is supported as a back buffer
		if(d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, d3ddm.Format, d3ddm.Format, TRUE) != D3D_OK)
		{
			d3ddm.Format = D3DFMT_X8R8G8B8;
			if(d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, d3ddm.Format, d3ddm.Format, TRUE) != D3D_OK)
				d3ddm.Format = D3DFMT_R5G6B5;
		}

		present.SwapEffect			= D3DSWAPEFFECT_COPY;
		present.Windowed			= TRUE;
		present.BackBufferFormat	= d3ddm.Format;
	}

	D3DFORMAT PixelFormat = present.BackBufferFormat;
	bool z16 = d3d9->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, PixelFormat, PixelFormat, D3DFMT_D24S8) != D3D_OK;

	present.EnableAutoDepthStencil	= TRUE;
//	present.AutoDepthStencilFormat	= (display.zBufferBits == 32) ? D3DFMT_D24S8 : D3DFMT_D16;
	present.AutoDepthStencilFormat	= z16 ? D3DFMT_D16 : D3DFMT_D24S8;
//	present.PresentationInterval	= display.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	present.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
	present.hDeviceWindow			= apphWnd;

	int b=0;
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

	for(int a=0; a<3&&!b; a++)
	{
		if(d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, PixelFormat, PixelFormat, gDisplay.windowed)==D3D_OK)
		{
			HRESULT hr = d3d9->CreateDevice(0, D3DDEVTYPE_HAL, apphWnd, processing, &present, &pd3dDevice);
			if(FAILED(hr))
			{
				if(a == 0)
				{
					MessageBox(NULL,"Failed to create Direct3D device.\nAttempting default display settings.","Error!",MB_OK|MB_ICONERROR);
					present.BackBufferFormat = PixelFormat = D3DFMT_X8R8G8B8;
					present.AutoDepthStencilFormat = D3DFMT_D24S8;
				}
				if(a == 1)
				{
					present.BackBufferFormat = PixelFormat = D3DFMT_R5G6B5;
					present.AutoDepthStencilFormat = D3DFMT_D16;
				}
				else
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
					pMessage = MFStr("Failed to create Direct3D device with error: %s.\nCant create game window.", pMessage);
					MFDebug_Error(pMessage);
					MFRenderer_DestroyDisplay();
					MessageBox(NULL, pMessage, "Error!", MB_OK|MB_ICONERROR);
					return 4;
				}
			}
			else b=1;
		}
		else
		{
			if(a == 0)
			{
				MessageBox(NULL,"Unsuitable display mode.\nAttempting default.","Error!",MB_OK|MB_ICONERROR);
				present.BackBufferFormat = PixelFormat = D3DFMT_X8R8G8B8;
				present.AutoDepthStencilFormat = D3DFMT_D24S8;
			}
			if(a == 1)
			{
				present.BackBufferFormat = PixelFormat = D3DFMT_R5G6B5;
				present.AutoDepthStencilFormat = D3DFMT_D16;
			}
			else
			{
				MessageBox(NULL,"No suitable hardware supported Display Mode could be found.\nCant create game window.","Error!",MB_OK|MB_ICONERROR);
				MFRenderer_DestroyDisplay();
				return 5;
			}
		}
	}

	pd3dDevice->GetRenderTarget(0, &pRenderTarget);
	pd3dDevice->GetDepthStencilSurface(&pZTarget);

	gDeviceRenderTarget.pTemplateData->imageFormat = (PixelFormat == D3DFMT_R5G6B5) ? ImgFmt_R5G6B5 : ImgFmt_A8R8G8B8;
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].width = present.BackBufferWidth;
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].height = present.BackBufferHeight;
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].bitsPerPixel = MFImage_GetBitsPerPixel(gDeviceRenderTarget.pTemplateData->imageFormat);
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].pImageData = (char*)pRenderTarget;

	gDeviceZTarget.pTemplateData->imageFormat = (present.AutoDepthStencilFormat == D3DFMT_D16) ? ImgFmt_D16 : ImgFmt_D24S8;
	gDeviceZTarget.pTemplateData->pSurfaces[0].width = present.BackBufferWidth;
	gDeviceZTarget.pTemplateData->pSurfaces[0].height = present.BackBufferHeight;
	gDeviceZTarget.pTemplateData->pSurfaces[0].bitsPerPixel = MFImage_GetBitsPerPixel(gDeviceRenderTarget.pTemplateData->imageFormat);
	gDeviceZTarget.pTemplateData->pSurfaces[0].pImageData = (char*)pZTarget;

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

	return 0;
}

void MFRenderer_DestroyDisplay()
{
	if(pRenderTarget)
	{
		pRenderTarget->Release();
		pRenderTarget = NULL;
	}

	if(pZTarget)
	{
		pZTarget->Release();
		pZTarget = NULL;
	}

	if(pd3dDevice)
	{
		pd3dDevice->Release();
		pd3dDevice=NULL;
	}
}

void MFRenderer_ResetDisplay()
{
	// free resources
	void MFTexture_Release();
	MFTexture_Release();
	void MFRenderState_Release();
	MFRenderState_Release();

	if(pRenderTarget)
	{
		pRenderTarget->Release();
		pRenderTarget = NULL;
	}
	if(pZTarget)
	{
		pZTarget->Release();
		pZTarget = NULL;
	}

	// setup present params
	D3DPRESENT_PARAMETERS present;
	MFZeroMemory(&present, sizeof(present));

	if(!gDisplay.windowed)
	{
		present.SwapEffect						= D3DSWAPEFFECT_FLIP;
		present.Windowed						= FALSE;
		present.BackBufferFormat				= (gDisplay.colourDepth == 32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		present.BackBufferWidth					= gDisplay.fullscreenWidth;
		present.BackBufferHeight				= gDisplay.fullscreenHeight;
		present.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
	}
	else
	{
		D3DDISPLAYMODE d3ddm;
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		// check if this format is supported as a back buffer
		if(d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, d3ddm.Format, d3ddm.Format, TRUE) != D3D_OK)
		{
			d3ddm.Format = D3DFMT_X8R8G8B8;
			if(d3d9->CheckDeviceType(0, D3DDEVTYPE_HAL, d3ddm.Format, d3ddm.Format, TRUE) != D3D_OK)
				d3ddm.Format = D3DFMT_R5G6B5;
		}

		present.SwapEffect              = D3DSWAPEFFECT_COPY;
		present.Windowed                = TRUE;
		present.BackBufferFormat        = d3ddm.Format;
		present.BackBufferWidth			= gDisplay.width;
		present.BackBufferHeight		= gDisplay.height;
	}

	// set the zbuffer format
	bool z16 = d3d9->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, present.BackBufferFormat, present.BackBufferFormat, D3DFMT_D24S8) != D3D_OK;
	present.EnableAutoDepthStencil = TRUE;
//	present.AutoDepthStencilFormat = (display.zBufferBits == 32) ? D3DFMT_D24S8 : D3DFMT_D16;
	present.AutoDepthStencilFormat = z16 ? D3DFMT_D16 : D3DFMT_D24S8;
	present.BackBufferCount        = 1;
	present.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
//	present.PresentationInterval   = display.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	present.hDeviceWindow          = apphWnd;

	if (!pd3dDevice)
		return;

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

	// recreate resources
	pd3dDevice->GetRenderTarget(0, &pRenderTarget);
	pd3dDevice->GetDepthStencilSurface(&pZTarget);

	gDeviceRenderTarget.pTemplateData->imageFormat = (present.BackBufferFormat == D3DFMT_R5G6B5) ? ImgFmt_R5G6B5 : ImgFmt_A8R8G8B8;
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].width = present.BackBufferWidth;
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].height = present.BackBufferHeight;
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].bitsPerPixel = MFImage_GetBitsPerPixel(gDeviceRenderTarget.pTemplateData->imageFormat);
	gDeviceRenderTarget.pTemplateData->pSurfaces[0].pImageData = (char*)pRenderTarget;

	gDeviceZTarget.pTemplateData->imageFormat = (present.AutoDepthStencilFormat == D3DFMT_D16) ? ImgFmt_D16 : ImgFmt_D24S8;
	gDeviceZTarget.pTemplateData->pSurfaces[0].width = present.BackBufferWidth;
	gDeviceZTarget.pTemplateData->pSurfaces[0].height = present.BackBufferHeight;
	gDeviceZTarget.pTemplateData->pSurfaces[0].bitsPerPixel = MFImage_GetBitsPerPixel(gDeviceRenderTarget.pTemplateData->imageFormat);
	gDeviceZTarget.pTemplateData->pSurfaces[0].pImageData = (char*)pZTarget;

	if(SUCCEEDED(hr))
	{
		void MFTexture_Recreate();
		MFTexture_Recreate();
		void MFRenderState_Recreate();
		MFRenderState_Recreate();

		if(pSystemCallbacks[MFCB_DisplayReset])
			pSystemCallbacks[MFCB_DisplayReset]();
	}
}

bool MFRenderer_SetDisplayMode(int width, int height, bool bFullscreen)
{
	// D3D handles this automatically in Reset()
	gDisplay.windowed = !bFullscreen;
	if(bFullscreen)
	{
		gDisplay.fullscreenWidth = width;
		gDisplay.fullscreenHeight = height;
	}
	else
	{
		gDisplay.width = width;
		gDisplay.height = height;
	}

	if(pd3dDevice)
		MFRenderer_ResetDisplay();
	return true;
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
						// try and reset the display...
						MFRenderer_ResetDisplay();
						break;
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

	pd3dDevice->Clear(0, NULL, ((flags & MFRCF_Colour) ? D3DCLEAR_TARGET : NULL)|((flags & MFRCF_ZBuffer) ? D3DCLEAR_ZBUFFER : NULL)|((flags & MFRCF_Stencil) ? D3DCLEAR_STENCIL : NULL), colour.ToPackedColour(), z, stencil);
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

	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	if(gDisplay.windowed)
	{
		vp.Width = gDisplay.width;
		vp.Height = gDisplay.height;
	}
	else
	{
		vp.Width = gDisplay.fullscreenWidth;
		vp.Height = gDisplay.fullscreenHeight;
	}
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)vp.Width;
	gCurrentViewport.height = (float)vp.Height;

	pd3dDevice->SetViewport(&vp);
}

MF_API MFTexture* MFRenderer_GetDeviceRenderTarget()
{
	return &gDeviceRenderTarget;
}

MF_API MFTexture* MFRenderer_GetDeviceDepthStencil()
{
	return &gDeviceZTarget;
}

MF_API void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget)
{
	MFDebug_Assert(pRenderTarget->pTemplateData->flags & TEX_RenderTarget, "Texture is not a render target!");

	if(pRenderTarget)
	{
		IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)pRenderTarget->pTemplateData->pSurfaces[0].pImageData;
		pd3dDevice->SetRenderTarget(0, pSurface);
	}

	if(pZTarget)
	{
		IDirect3DSurface9 *pSurface = (IDirect3DSurface9*)pZTarget->pTemplateData->pSurfaces[0].pImageData;
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


// direct3d management fucntions
void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix)
{
	MFMatrix mat = animationMatrix;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(r_animMats + boneID*3, (float*)&mat, 3);
}

void MFRendererPC_SetWorldToScreenMatrix(const MFMatrix &worldToScreen)
{
	MFMatrix mat = worldToScreen;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(r_wvp, (float*)&mat, 4);
}

void MFRendererPC_SetTextureMatrix(const MFMatrix &textureMatrix)
{
	MFMatrix mat = textureMatrix;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(r_tex, (float*)&mat, 2);
}

void MFRendererPC_SetModelColour(const MFVector &colour)
{
	pd3dDevice->SetVertexShaderConstantF(r_modelColour, colour, 1);
}

void MFRendererPC_SetNumWeights(int numWeights)
{
	gNumWeights = numWeights;

//	int i[4] = { numWeights, numWeights, numWeights, numWeights };
//	pd3dDevice->SetVertexShaderConstantI(r_numWeights, i, 1);
	pd3dDevice->SetVertexShaderConstantF(r_animating, numWeights ? MFVector::identity : MFVector::one, 1);
	pd3dDevice->SetVertexShaderConstantF(r_animating + 1, numWeights ? MFVector::one : MFVector::identity, 1);
}

int MFRendererPC_GetNumWeights()
{
	return gNumWeights;
}

#endif // MF_RENDERER
