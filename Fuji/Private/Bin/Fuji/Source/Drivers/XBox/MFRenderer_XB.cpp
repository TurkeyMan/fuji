#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_XBOX

#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"

#include "MFRenderer_Internal.h"
#include "MFRenderer_XB.h"


IDirect3D8 *d3d8;
IDirect3DDevice8 *pd3dDevice;
D3DCAPS8 deviceCaps;

MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);

MFRect gCurrentViewport;

extern bool gbLetterBox;


void MFRenderer_InitModulePlatformSpecific()
{
	// create D3D interface
	d3d8 = Direct3DCreate8(D3D_SDK_VERSION);

	if(!d3d8)
		MFDebug_Assert(false ,"Unable to Create the D3D Device.");

	d3d8->GetDeviceCaps(0, D3DDEVTYPE_HAL, &deviceCaps);
}

void MFRenderer_DeinitModulePlatformSpecific()
{
	if(d3d8)
		d3d8->Release();
}

int MFRenderer_CreateDisplay()
{
	// create the D3D device
	D3DPRESENT_PARAMETERS presentparams;
	HRESULT hr;

	MFZeroMemory(&presentparams, sizeof(D3DPRESENT_PARAMETERS));
	presentparams.BackBufferWidth = gDisplay.width;
	presentparams.BackBufferHeight = gDisplay.height;
	presentparams.BackBufferFormat = (gDisplay.colourDepth == 32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
	presentparams.BackBufferCount = 1;
	presentparams.MultiSampleType = D3DMULTISAMPLE_NONE;
	presentparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentparams.EnableAutoDepthStencil = TRUE;
	presentparams.AutoDepthStencilFormat = D3DFMT_D24S8;
	presentparams.FullScreen_RefreshRateInHz = gDisplay.refreshRate; 
	presentparams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;// : D3DPRESENT_INTERVAL_IMMEDIATE;
	presentparams.Flags = (gDisplay.wide ? D3DPRESENTFLAG_WIDESCREEN : NULL) | (gDisplay.progressive ? D3DPRESENTFLAG_PROGRESSIVE : NULL);

	hr = d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentparams, &pd3dDevice);
	if(hr != D3D_OK)
		return 2;

	// clear frame buffers to black
	MFVector oldColour = gClearColour;
	MFRenderer_SetClearColour(0,0,0,0);
	MFRenderer_BeginFrame();
	MFRenderer_ClearScreen(CS_All);
	MFRenderer_EndFrame();
	MFRenderer_BeginFrame();
	MFRenderer_ClearScreen(CS_All);
	MFRenderer_EndFrame();
	MFRenderer_SetClearColour(oldColour.x, oldColour.y, oldColour.z, oldColour.w);

	return 0;
}

void MFRenderer_DestroyDisplay()
{
	if(pd3dDevice)
	{
		pd3dDevice->Release();
		pd3dDevice = NULL;
	}
}

void MFRenderer_ResetDisplay()
{
	MFRenderer_ResetViewport();
}

void MFRenderer_BeginFrame()
{
	MFCALLSTACK;

	pd3dDevice->BeginScene();

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

	float letterboxOffset = (float)gDisplay.height * 0.125f;
	D3DVIEWPORT8 vp;
	vp.X = (DWORD)pRect->x;
	vp.Y = (DWORD)(gbLetterBox ? pRect->y + letterboxOffset : pRect->y);
	vp.Width = (DWORD)pRect->width;
	vp.Height = (DWORD)(gbLetterBox ? pRect->height * 0.75f : pRect->height);
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
	gCurrentViewport.height = gbLetterBox ? (float)gDisplay.height * 0.75f : (float)gDisplay.height;

	float letterboxOffset = (float)gDisplay.height * 0.125f;
	D3DVIEWPORT8 vp;
	vp.X = 0;
	vp.Y = gbLetterBox ? (DWORD)letterboxOffset : 0;
	vp.Width = (DWORD)gCurrentViewport.width;
	vp.Height = (DWORD)gCurrentViewport.height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	pd3dDevice->SetViewport(&vp);
}

// direct3d management fucntions
void MFRendererXB_SetTexture(int stage, IDirect3DTexture8 *pTexture)
{
	pd3dDevice->SetTexture(stage, pTexture);
}

void MFRendererXB_SetVertexShader(uint32 vsHandle)
{
	pd3dDevice->SetVertexShader(vsHandle);
}

void MFRendererXB_SetStreamSource(int stream, IDirect3DVertexBuffer8 *pVertexBuffer, int stride)
{
	pd3dDevice->SetStreamSource((uint32)stream, pVertexBuffer, stride);
}

void MFRendererXB_SetIndices(IDirect3DIndexBuffer8 *pIndexBuffer, int baseIndex)
{
	pd3dDevice->SetIndices(pIndexBuffer, baseIndex);
}

void MFRendererXB_SetTextureMatrix(const MFMatrix &textureMatrix)
{
	MFMatrix texMat = textureMatrix;
	texMat.SetZAxis3(texMat.GetTrans());
	pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX*)&texMat);
}

void MFRendererXB_ApplyGPUStates()
{

}

void MFRendererXB_SetDefaultGPUStates()
{

}

void MFRendererXB_ApplyRenderStates()
{

}

void MFRendererXB_SetDefaultRenderStates()
{

}

void MFRendererXB_SetRenderState(D3DRENDERSTATETYPE type, uint32 value)
{
	pd3dDevice->SetRenderState(type, value);
}

void MFRendererXB_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetRenderState(type, (DWORD*)pValue);
}

void MFRendererXB_ApplyTextureStageStates()
{

}

void MFRendererXB_SetDefaultTextureStageStates()
{

}

void MFRendererXB_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value)
{
	pd3dDevice->SetTextureStageState(stage, type, value);
}

void MFRendererXB_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetTextureStageState(stage, type, (DWORD*)pValue);
}

void MFRendererXB_ConvertFloatToXBVF(const float *pFloat, char *pData, XBVF_Type type, int *pNumBytesWritten)
{
	int numBytes = 0;

	switch(type)
	{
		case XBVF_Float4:
			((float*)pData)[3] = pFloat[3];
			numBytes = 4;
		case XBVF_Float3:
			((float*)pData)[2] = pFloat[2];
			numBytes += 4;
		case XBVF_Float2:
			((float*)pData)[1] = pFloat[1];
			numBytes += 4;
		case XBVF_Float1:
			((float*)pData)[0] = pFloat[0];
			numBytes += 4;
			break;

		case XBVF_D3DColor:
			*(uint32*)pData = (((uint32)(pFloat[3] * 255.0f) & 0xFF) << 24) |
							  (((uint32)(pFloat[0] * 255.0f) & 0xFF) << 16) |
							  (((uint32)(pFloat[1] * 255.0f) & 0xFF) << 8) |
							   ((uint32)(pFloat[2] * 255.0f) & 0xFF);
			numBytes = 4;
			break;

		case XBVF_Short4:
			((int16*)pData)[2] = (int16)pFloat[2];
			((int16*)pData)[3] = (int16)pFloat[3];
			numBytes = 4;
		case XBVF_Short2:
			((int16*)pData)[0] = (int16)pFloat[0];
			((int16*)pData)[1] = (int16)pFloat[1];
			numBytes += 4;
			break;

		case XBVF_Short1N:
			break;

		case XBVF_Short2N:
			break;

		case XBVF_Short3N:
			break;

		case XBVF_Short4N:
			break;

		case XBVF_NormPacked:
			break;

		case XBVF_Short1:
			break;

		case XBVF_Short3:
			break;

		case XBVF_UByte1N:
			break;

		case XBVF_UByte2N:
			break;

		case XBVF_UByte3N:
			break;

		case XBVF_UByte4N:
			break;

		case XBVF_Float2H:
			break;

		default:
			MFDebug_Assert(false, "Invalid XBVF_Type");
	}

	if(pNumBytesWritten)
		*pNumBytesWritten = numBytes;
}

void MFRendererXB_ConvertXBVFToFloat(const char *pData, float *pFloat, XBVF_Type type, int *pNumComponentsWritten)
{
	int numComponents = 0;

	switch(type)
	{
		case XBVF_Float4:
			pFloat[3] = ((float*)pData)[3];
			++numComponents;
		case XBVF_Float3:
			pFloat[2] = ((float*)pData)[2];
			++numComponents;
		case XBVF_Float2:
			pFloat[1] = ((float*)pData)[1];
			++numComponents;
		case XBVF_Float1:
			pFloat[0] = ((float*)pData)[0];
			++numComponents;
			break;

		case XBVF_D3DColor:
			break;

		case XBVF_Short2:
			break;

		case XBVF_Short4:
			break;

		case XBVF_Short1N:
			break;

		case XBVF_Short2N:
			break;

		case XBVF_Short3N:
			break;

		case XBVF_Short4N:
			break;

		case XBVF_NormPacked:
			break;

		case XBVF_Short1:
			break;

		case XBVF_Short3:
			break;

		case XBVF_UByte1N:
			break;

		case XBVF_UByte2N:
			break;

		case XBVF_UByte3N:
			break;

		case XBVF_UByte4N:
			break;

		case XBVF_Float2H:
			break;

		default:
			MFDebug_Assert(false, "Invalid XBVF_Type");
	}

	if(pNumComponentsWritten)
		*pNumComponentsWritten = numComponents;
}

#endif
