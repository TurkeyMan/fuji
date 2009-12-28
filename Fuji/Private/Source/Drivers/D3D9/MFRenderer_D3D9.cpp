#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_D3D9
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_D3D9
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_D3D9
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_D3D9
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_D3D9
	#define MFRenderer_BeginFrame MFRenderer_BeginFrame_D3D9
	#define MFRenderer_EndFrame MFRenderer_EndFrame_D3D9
	#define MFRenderer_SetClearColour MFRenderer_SetClearColour_D3D9
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_D3D9
	#define MFRenderer_GetViewport MFRenderer_GetViewport_D3D9
	#define MFRenderer_SetViewport MFRenderer_SetViewport_D3D9
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_D3D9
	#define MFRenderer_SetRenderTarget MFRenderer_SetRenderTarget_D3D9
	#define MFRenderer_SetDeviceRenderTarget MFRenderer_SetDeviceRenderTarget_D3D9
	#define MFRenderer_GetTexelCenterOffset MFRenderer_GetTexelCenterOffset_D3D9
#endif

#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"

#include "MFRenderer_Internal.h"
#include "MFRenderer_D3D9.h"

#include "Shaders/Registers.h"

#include <d3d9.h>

static IDirect3D9 *d3d9;
IDirect3DDevice9 *pd3dDevice;

static IDirect3DSurface9 *pRenderTarget = NULL;
static IDirect3DSurface9 *pZTarget = NULL;

static D3DCAPS9 deviceCaps;

extern HWND apphWnd;

static MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);

static int gNumWeights = 0;
static MFRect gCurrentViewport;

void MFRenderer_InitModulePlatformSpecific()
{
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		MessageBox(NULL,"Unable to Create the D3D Device. You probably need to update DirectX.","Error!",MB_OK|MB_ICONERROR);
		return;
	}

	d3d9->GetDeviceCaps(0, D3DDEVTYPE_HAL, &deviceCaps);
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
	ZeroMemory(&present, sizeof(present));

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

	if(deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT && ((deviceCaps.VertexShaderVersion >> 8) & 0xFF) >= 2)
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

	pd3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

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
	D3DPRESENT_PARAMETERS present;
	ZeroMemory(&present, sizeof(present));

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
		case D3D_OK:
			MFRenderer_ResetViewport();
	}
}

void MFRenderer_BeginFrame()
{
	MFCALLSTACK;

	HRESULT hr = pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void MFRenderer_EndFrame()
{
	MFCALLSTACK;

	pd3dDevice->EndScene();
	pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void MFRenderer_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void MFRenderer_ClearScreen(uint32 flags)
{
	MFCALLSTACKc;

	pd3dDevice->Clear(0, NULL, ((flags&CS_Colour) ? D3DCLEAR_TARGET : NULL)|((flags&CS_ZBuffer) ? D3DCLEAR_ZBUFFER : NULL)|((flags&CS_Stencil) ? D3DCLEAR_STENCIL : NULL), gClearColour.ToPackedColour(), 1.0f, 0);
}

void MFRenderer_GetViewport(MFRect *pRect)
{
	*pRect = gCurrentViewport;
}

void MFRenderer_SetViewport(MFRect *pRect)
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

void MFRenderer_ResetViewport()
{
	MFCALLSTACK;

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)gDisplay.width;
	gCurrentViewport.height = (float)gDisplay.height;

	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = gDisplay.width;
	vp.Height = gDisplay.height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}

void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget)
{
	MFDebug_Assert(pRenderTarget->pTemplateData->flags & TEX_RenderTarget, "Texture is not a render target!");

	if(pRenderTarget)
	{
		IDirect3DTexture9 *pRT = (IDirect3DTexture9*)pRenderTarget->pInternalData;

		IDirect3DSurface9 *pSurface;
		pRT->GetSurfaceLevel(0, &pSurface);
		pd3dDevice->SetRenderTarget(0, pSurface);
		pSurface->Release();
	}

	if(pZTarget)
	{
		IDirect3DTexture9 *pRT = (IDirect3DTexture9*)pRenderTarget->pInternalData;

		IDirect3DSurface9 *pSurface;
		pRT->GetSurfaceLevel(0, &pSurface);
		pd3dDevice->SetDepthStencilSurface(pSurface);
		pSurface->Release();
	}
	else
	{
		pd3dDevice->SetDepthStencilSurface(NULL);
	}
}

void MFRenderer_SetDeviceRenderTarget()
{
	pd3dDevice->SetRenderTarget(0, pRenderTarget);
	pd3dDevice->SetDepthStencilSurface(pZTarget);
}

float MFRenderer_GetTexelCenterOffset()
{
	return 0.5f;
}

// direct3d management fucntions
void MFRendererPC_SetTexture(int stage, IDirect3DTexture9 *pTexture)
{
	pd3dDevice->SetTexture(stage, pTexture);
}

void MFRendererPC_SetVertexShader(IDirect3DVertexShader9 *pVertexShader)
{
	pd3dDevice->SetVertexShader(pVertexShader);
}

void MFRendererPC_SetStreamSource(int stream, IDirect3DVertexBuffer9 *pVertexBuffer, int offset, int stride)
{
	pd3dDevice->SetStreamSource(stream, pVertexBuffer, offset, stride);
}

void MFRendererPC_SetIndices(IDirect3DIndexBuffer9 *pIndexBuffer)
{
	pd3dDevice->SetIndices(pIndexBuffer);
}

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

void MFRendererPC_SetColourMask(float colourModulate, float colourAdd, float alphaModulate, float alphaAdd)
{
	float mask[4] = { colourModulate, colourAdd, alphaModulate, alphaAdd };
	pd3dDevice->SetVertexShaderConstantF(r_colourMask, mask, 1);
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

void MFRendererPC_ApplyGPUStates()
{

}

void MFRendererPC_SetDefaultGPUStates()
{

}

void MFRendererPC_ApplyRenderStates()
{

}

void MFRendererPC_SetDefaultRenderStates()
{

}

void MFRendererPC_SetRenderState(D3DRENDERSTATETYPE type, uint32 value)
{
	pd3dDevice->SetRenderState(type, value);
}

void MFRendererPC_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetRenderState(type, (DWORD*)pValue);
}

void MFRendererPC_ApplyTextureStageStates()
{

}

void MFRendererPC_SetDefaultTextureStageStates()
{

}

void MFRendererPC_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value)
{
	pd3dDevice->SetTextureStageState(stage, type, value);
}

void MFRendererPC_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetTextureStageState(stage, type, (DWORD*)pValue);
}

void MFRendererPC_ApplySamplerStates()
{

}

void MFRendererPC_SetDefaultSamplerStates()
{

}

void MFRendererPC_SetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 value)
{
	pd3dDevice->SetSamplerState(sampler, type, value);
}

void MFRendererPC_GetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetSamplerState(sampler, type, (DWORD*)pValue);
}

void MFRendererPC_ConvertFloatToPCVF(const float *pFloat, char *pData, PCVF_Type type, int *pNumBytesWritten)
{
	int numBytes = 0;

	switch(type)
	{
		case PCVF_Float4:
			((float*)pData)[3] = pFloat[3];
			numBytes = 4;
		case PCVF_Float3:
			((float*)pData)[2] = pFloat[2];
			numBytes += 4;
		case PCVF_Float2:
			((float*)pData)[1] = pFloat[1];
			numBytes += 4;
		case PCVF_Float1:
			((float*)pData)[0] = pFloat[0];
			numBytes += 4;
			break;

		case PCVF_D3DColor:
			*(uint32*)pData = (((uint32)(pFloat[3] * 255.0f) & 0xFF) << 24) |
							  (((uint32)(pFloat[0] * 255.0f) & 0xFF) << 16) |
							  (((uint32)(pFloat[1] * 255.0f) & 0xFF) << 8) |
							   ((uint32)(pFloat[2] * 255.0f) & 0xFF);
			numBytes = 4;
			break;

		case PCVF_UByte4:
			pData[0] = (uint8)pFloat[0];
			pData[1] = (uint8)pFloat[1];
			pData[2] = (uint8)pFloat[2];
			pData[3] = (uint8)pFloat[3];
			numBytes = 4;
			break;

		case PCVF_Short4:
			((int16*)pData)[2] = (int16)pFloat[2];
			((int16*)pData)[3] = (int16)pFloat[3];
			numBytes = 4;
		case PCVF_Short2:
			((int16*)pData)[0] = (int16)pFloat[0];
			((int16*)pData)[1] = (int16)pFloat[1];
			numBytes += 4;
			break;

		case PCVF_UByte4N:
			break;

		case PCVF_Short2N:
			break;

		case PCVF_Short4N:
			break;

		case PCVF_UShort2N:
			break;

		case PCVF_UShort4N:
			break;

		case PCVF_UDec3:
			break;

		case PCVF_Dec3N:
			break;

		case PCVF_Float16_2:
			break;

		case PCVF_Float16_4:
			break;

		default:
			MFDebug_Assert(false, "Invalid PCVF_Type");
	}

	if(pNumBytesWritten)
		*pNumBytesWritten = numBytes;
}

void MFRendererPC_ConvertPCVFToFloat(const char *pData, float *pFloat, PCVF_Type type, int *pNumComponentsWritten)
{
	int numComponents = 0;

	switch(type)
	{
		case PCVF_Float4:
			pFloat[3] = ((float*)pData)[3];
			++numComponents;
		case PCVF_Float3:
			pFloat[2] = ((float*)pData)[2];
			++numComponents;
		case PCVF_Float2:
			pFloat[1] = ((float*)pData)[1];
			++numComponents;
		case PCVF_Float1:
			pFloat[0] = ((float*)pData)[0];
			++numComponents;
			break;

		case PCVF_D3DColor:
			break;

		case PCVF_UByte4:
			break;

		case PCVF_Short2:
			break;

		case PCVF_Short4:
			break;

		case PCVF_UByte4N:
			break;

		case PCVF_Short2N:
			break;

		case PCVF_Short4N:
			break;

		case PCVF_UShort2N:
			break;

		case PCVF_UShort4N:
			break;

		case PCVF_UDec3:
			break;

		case PCVF_Dec3N:
			break;

		case PCVF_Float16_2:
			break;

		case PCVF_Float16_4:
			break;

		default:
			MFDebug_Assert(false, "Invalid PCVF_Type");
	}

	if(pNumComponentsWritten)
		*pNumComponentsWritten = numComponents;
}

#endif // MF_RENDERER
