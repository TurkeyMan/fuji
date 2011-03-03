#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_D3D11
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_D3D11
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_D3D11
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_D3D11
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_D3D11
	#define MFRenderer_BeginFrame MFRenderer_BeginFrame_D3D11
	#define MFRenderer_EndFrame MFRenderer_EndFrame_D3D11
	#define MFRenderer_SetClearColour MFRenderer_SetClearColour_D3D11
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_D3D11
	#define MFRenderer_GetViewport MFRenderer_GetViewport_D3D11
	#define MFRenderer_SetViewport MFRenderer_SetViewport_D3D11
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_D3D11
	#define MFRenderer_SetRenderTarget MFRenderer_SetRenderTarget_D3D11
	#define MFRenderer_SetDeviceRenderTarget MFRenderer_SetDeviceRenderTarget_D3D11
	#define MFRenderer_GetTexelCenterOffset MFRenderer_GetTexelCenterOffset_D3D11
#endif

#include "MFTexture_Internal.h"

#include <d3d11.h>

static MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);

extern HWND apphWnd;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;

void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay()
{
	HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( apphWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
    };
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = apphWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

	return 0;
}

void MFRenderer_DestroyDisplay()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}

void MFRenderer_ResetDisplay()
{
}

void MFRenderer_BeginFrame()
{
	MFCALLSTACK;

}

void MFRenderer_EndFrame()
{
	MFCALLSTACK;
    
	g_pSwapChain->Present( 0, 0 );
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

	float ClearColor[4] = { gClearColour.x, gClearColour.y, gClearColour.z, gClearColour.w }; // RGBA
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );
}

void MFRenderer_GetViewport(MFRect *pRect)
{
}

void MFRenderer_SetViewport(MFRect *pRect)
{

}

void MFRenderer_ResetViewport()
{

}

void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget)
{

}

void MFRenderer_SetDeviceRenderTarget()
{
}

float MFRenderer_GetTexelCenterOffset()
{
	return 0.5f;
}
//
//// direct3d management fucntions
//void MFRendererPC_SetTexture(int stage, IDirect3DTexture9 *pTexture)
//{
//}
//
//void MFRendererPC_SetVertexShader(IDirect3DVertexShader9 *pVertexShader)
//{
//}
//
//void MFRendererPC_SetStreamSource(int stream, IDirect3DVertexBuffer9 *pVertexBuffer, int offset, int stride)
//{
//}
//
//void MFRendererPC_SetIndices(IDirect3DIndexBuffer9 *pIndexBuffer)
//{
//}
//
//void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix)
//{
//}
//
//void MFRendererPC_SetWorldToScreenMatrix(const MFMatrix &worldToScreen)
//{
//}
//
//void MFRendererPC_SetTextureMatrix(const MFMatrix &textureMatrix)
//{
//}
//
//void MFRendererPC_SetModelColour(const MFVector &colour)
//{
//}
//
//void MFRendererPC_SetColourMask(float colourModulate, float colourAdd, float alphaModulate, float alphaAdd)
//{
//}
//
//void MFRendererPC_SetNumWeights(int numWeights)
//{
//}
//
//int MFRendererPC_GetNumWeights()
//{
//	return 0;
//}
//
//void MFRendererPC_ApplyGPUStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultGPUStates()
//{
//
//}
//
//void MFRendererPC_ApplyRenderStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultRenderStates()
//{
//
//}
//
//void MFRendererPC_SetRenderState(D3DRENDERSTATETYPE type, uint32 value)
//{
//}
//
//void MFRendererPC_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRendererPC_ApplyTextureStageStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultTextureStageStates()
//{
//
//}
//
//void MFRendererPC_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value)
//{
//}
//
//void MFRendererPC_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRendererPC_ApplySamplerStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultSamplerStates()
//{
//
//}
//
//void MFRendererPC_SetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 value)
//{
//}
//
//void MFRendererPC_GetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRendererPC_ConvertFloatToPCVF(const float *pFloat, char *pData, PCVF_Type type, int *pNumBytesWritten)
//{
//}
//
//void MFRendererPC_ConvertPCVFToFloat(const char *pData, float *pFloat, PCVF_Type type, int *pNumComponentsWritten)
//{
//}


#endif // MF_RENDERER
