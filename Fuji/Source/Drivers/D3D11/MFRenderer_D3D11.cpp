#include "Fuji_Internal.h"

#if (MF_RENDERER == MF_DRIVER_D3D11) || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_D3D11
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_D3D11
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_D3D11
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_D3D11
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_D3D11
	#define MFRenderer_LostFocus MFRenderer_LostFocus_D3D11
	#define MFRenderer_GainedFocus MFRenderer_GainedFocus_D3D11
	#define MFRenderer_BeginFramePlatformSpecific MFRenderer_BeginFramePlatformSpecific_D3D11
	#define MFRenderer_EndFramePlatformSpecific MFRenderer_EndFramePlatformSpecific_D3D11
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_D3D11
	#define MFRenderer_SetViewport MFRenderer_SetViewport_D3D11
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_D3D11
	#define MFRenderer_GetDeviceRenderTarget MFRenderer_GetDeviceRenderTarget_D3D11
	#define MFRenderer_GetDeviceDepthStencil MFRenderer_GetDeviceDepthStencil_D3D11
	#define MFRenderer_SetRenderTargetPlatformSpecific MFRenderer_SetRenderTargetPlatformSpecific_D3D11
	#define MFRenderer_GetTexelCenterOffset MFRenderer_GetTexelCenterOffset_D3D11
	#define MFRendererInternal_SortElements MFRendererInternal_SortElements_D3D11
#endif

#include "MFRenderer_Internal.h"
#include "MFRenderer_D3D11.h"
#include "MFTexture_Internal.h"
#include "MFDisplay_Internal.h"
#include "Shaders/Registers.h"
#include "MFVertex.h"
#include "MFMesh_Internal.h"
#include "MFWindow.h"

#include <d3d11.h>

#pragma comment(lib, "d3d11")


struct CBWorld
{
	MFMatrix mWorldToScreen;
	MFMatrix mLocalToWorld;
};

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D*		g_pDepthStencil = NULL;
ID3D11DepthStencilView*	g_pDepthStencilView = NULL;

ID3D11Buffer* g_pConstantBufferWorld = NULL;
static CBWorld cbWorld;


// Utils

void MFRenderer_D3D11_SetDebugName(ID3D11DeviceChild* pResource, const char* pName)
{
#if !defined(MF_RETAIL)
	if (pResource)
		pResource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)MFString_Length(pName), pName);
#endif
}

void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay(MFDisplay *pDisplay)
{
	HWND hWnd = (HWND)MFWindow_GetSystemWindowHandle(pDisplay->settings.pWindow);

	HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( hWnd, &rc );
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
    MFZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
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

	MFRenderer_D3D11_SetDebugName(g_pImmediateContext, "MFRenderer global device context");

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

	MFRenderer_D3D11_SetDebugName(pBackBuffer, "MFRenderer back buffer");

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

	MFRenderer_D3D11_SetDebugName(g_pRenderTargetView, "MFRenderer render target view");

	// Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

	MFRenderer_D3D11_SetDebugName(g_pDepthStencil, "MFRenderer depth stencil buffer");

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

	MFRenderer_D3D11_SetDebugName(g_pDepthStencilView, "MFRenderer depth stencil view");

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

	//--

	cbWorld.mWorldToScreen = MFMatrix::identity;
	cbWorld.mLocalToWorld = MFMatrix::identity;
	
	D3D11_BUFFER_DESC desc;
	MFZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(cbWorld);
	desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data;
	MFZeroMemory(&data, sizeof(data));
	data.pSysMem = &cbWorld;

	g_pd3dDevice->CreateBuffer(&desc, &data, &g_pConstantBufferWorld);

	MFRenderer_D3D11_SetDebugName(g_pConstantBufferWorld, "MFRenderer world constant buffer");

	g_pImmediateContext->VSSetConstantBuffers(n_cbWorld, 1, &g_pConstantBufferWorld);
	g_pImmediateContext->PSSetConstantBuffers(n_cbWorld, 1, &g_pConstantBufferWorld);

	return 0;
}

void MFRenderer_DestroyDisplay(MFDisplay *pDisplay)
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

	if (g_pConstantBufferWorld) g_pConstantBufferWorld->Release();

	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();

    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();

	//{
	//	ID3D11Debug* pDebug = NULL;
	//	g_pd3dDevice->QueryInterface(&pDebug);

	//	pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	//	pDebug->Release();
	//}

    if (g_pd3dDevice) g_pd3dDevice->Release();
}

bool MFRenderer_ResetDisplay(MFDisplay *pDisplay, const MFDisplaySettings *pSettings)
{
	return true;
}

void MFRenderer_LostFocus(MFDisplay *pDisplay)
{
}

void MFRenderer_GainedFocus(MFDisplay *pDisplay)
{
}

bool MFRenderer_BeginFramePlatformSpecific()
{
	MFCALLSTACK;
	return true;
}

void MFRenderer_EndFramePlatformSpecific()
{
	MFCALLSTACK;
   
	g_pSwapChain->Present( 0, 0 );
}

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags, const MFVector &colour, float z, int stencil)
{
	MFCALLSTACKc;
	
	const bool bClearDepth = (flags & MFRCF_Depth) != 0;
	const bool bClearStencil = (flags & MFRCF_Stencil) != 0;
	const bool bClearColour = (flags & MFRCF_Colour) != 0;

	if (bClearColour)
	{
		float clearColour[4] = { colour.x, colour.y, colour.z, colour.w }; // RGBA
		g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColour);
	}

    if (bClearDepth | bClearStencil)
    {
		uint32 clearFlags = (bClearDepth ? D3D11_CLEAR_DEPTH : 0) | (bClearStencil ? D3D11_CLEAR_STENCIL : 0);
		g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, clearFlags, z, (UINT8)stencil);
	}
}

MF_API void MFRenderer_SetViewport(MFRect *pRect)
{

}

MF_API void MFRenderer_ResetViewport()
{

}

MF_API MFRenderTarget* MFRenderer_GetDeviceRenderTarget()
{
	return NULL;
}

void MFRenderer_SetRenderTargetPlatformSpecific(MFRenderTarget *pRenderTarget)
{
//    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );
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

void MFRenderer_D3D11_SetWorldToScreenMatrix(const MFMatrix &worldToScreen)
{
	cbWorld.mWorldToScreen.Transpose(worldToScreen);

	g_pImmediateContext->UpdateSubresource(g_pConstantBufferWorld, 0, NULL, &cbWorld, 0, 0);
}

#endif // MF_RENDERER
