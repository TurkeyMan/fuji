#include "Fuji.h"

#if (MF_RENDERER == MF_DRIVER_D3D11) || defined(MF_RENDERPLUGIN_D3D11)

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

#include "MFRenderer.h"
#include "MFRenderer_D3D11.h"
#include "MFTexture_Internal.h"
#include "Shaders/Registers.h"
#include "MFVertex.h"
#include "MFMesh_Internal.h"

#include <d3d11.h>

#pragma comment(lib, "d3d11")


struct CBWorld
{
	MFMatrix mWorldToScreen;
	MFMatrix mLocalToWorld;
};

static MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);

extern HWND apphWnd;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D*		g_pDepthStencil = NULL;
ID3D11DepthStencilView*	g_pDepthStencilView = NULL;

ID3D11DepthStencilState* g_pDepthStencilState = NULL;
D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

ID3D11Buffer* g_pConstantBufferWorld = NULL;
static CBWorld cbWorld;

//---------------------------------------------------------------------------------------------------------------------
// Utils
//---------------------------------------------------------------------------------------------------------------------
extern int gVertexDataStride[MFVDF_Max];
//---------------------------------------------------------------------------------------------------------------------
static const char* s_SemanticName[MFVET_Max] =
{
	"POSITION",		// MFVE_Position,
	"NORMAL",		// MFVE_Normal
	"COLOR",		// MFVE_Colour
	"TEXCOORD",		// MFVE_TexCoord
	"BINORMAL",		// MFVE_Binormal
	"TANGENT",		// MFVE_Tangent
	"BLENDINDICES", // MFVE_Indices
	"BLENDWEIGHT",	// MFVE_Weights
};
//---------------------------------------------------------------------------------------------------------------------
const char* MFRenderer_D3D11_GetSemanticName(MFVertexElementType type)
{
	return s_SemanticName[type];
}
//---------------------------------------------------------------------------------------------------------------------
static const DXGI_FORMAT s_MFVDF_To_DXGI[MFVDF_Max] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT, // MFVDF_Float4
	DXGI_FORMAT_R32G32B32_FLOAT,	// MFVDF_Float3
	DXGI_FORMAT_R32G32_FLOAT,		// MFVDF_Float2
	DXGI_FORMAT_R32_FLOAT,			// MFVDF_Float1
	DXGI_FORMAT_R8G8B8A8_UINT,		// MFVDF_UByte4_RGBA
	DXGI_FORMAT_R8G8B8A8_UNORM,		// MFVDF_UByte4N_RGBA
	DXGI_FORMAT_B8G8R8A8_UNORM,		// MFVDF_UByte4N_BGRA
	DXGI_FORMAT_R16G16B16A16_SINT,	// MFVDF_SShort4
	DXGI_FORMAT_R16G16_SINT,		// MFVDF_SShort2
	DXGI_FORMAT_R16G16B16A16_SNORM, // MFVDF_SShort4N
	DXGI_FORMAT_R16G16_SNORM,		// MFVDF_SShort2N
	DXGI_FORMAT_R16G16B16A16_UINT,	// MFVDF_UShort4
	DXGI_FORMAT_R32G32_UINT,		// MFVDF_UShort2
	DXGI_FORMAT_R16G16B16A16_UNORM, // MFVDF_UShort4N
	DXGI_FORMAT_R16G16_UNORM,		// MFVDF_UShort2N
	DXGI_FORMAT_R16G16B16A16_FLOAT,	// MFVDF_Float16_4
	DXGI_FORMAT_R16G16_FLOAT		// MFVDF_Float16_2
};
//---------------------------------------------------------------------------------------------------------------------
DXGI_FORMAT MFRenderer_D3D11_GetFormat(MFVertexDataFormat format)
{
	return s_MFVDF_To_DXGI[format];
}
//---------------------------------------------------------------------------------------------------------------------
static const DXGI_FORMAT s_MFMVDF_To_DXGI[MFMVDT_Max] =
{
	DXGI_FORMAT_R32_FLOAT,			//MFMVDT_Float1,
	DXGI_FORMAT_R32G32_FLOAT,		//MFMVDT_Float2,
	DXGI_FORMAT_R32G32B32_FLOAT,	//MFMVDT_Float3,
	DXGI_FORMAT_R32G32B32A32_FLOAT,	//MFMVDT_Float4,
	DXGI_FORMAT_B8G8R8A8_UNORM,		//MFMVDT_ColourBGRA,
	DXGI_FORMAT_R8G8B8A8_UINT,		//MFMVDT_UByte4,
	DXGI_FORMAT_R8G8B8A8_UNORM,		//MFMVDT_UByte4N,
	DXGI_FORMAT_R16G16_SINT,		//MFMVDT_Short2,
	DXGI_FORMAT_R16G16B16A16_SINT,	//MFMVDT_Short4,
	DXGI_FORMAT_R16G16_SNORM,		//MFMVDT_Short2N,
	DXGI_FORMAT_R16G16B16A16_SNORM,	//MFMVDT_Short4N,
	DXGI_FORMAT_R16G16_UNORM,		//MFMVDT_UShort2N,
	DXGI_FORMAT_R16G16B16A16_UNORM,	//MFMVDT_UShort4N,
	DXGI_FORMAT_R10G10B10A2_UINT,	//MFMVDT_UDec3,
	DXGI_FORMAT_R10G10B10A2_UNORM,	//MFMVDT_Dec3N,
	DXGI_FORMAT_R16G16_FLOAT,		//MFMVDT_Float16_2,
	DXGI_FORMAT_R16G16B16A16_FLOAT,	//MFMVDT_Float16_4,
};
//---------------------------------------------------------------------------------------------------------------------
DXGI_FORMAT MFRenderer_D3D11_GetFormat(MFMeshVertexDataType format)
{
	return s_MFMVDF_To_DXGI[format];
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_InitModulePlatformSpecific()
{
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_DeinitModulePlatformSpecific()
{
}
//---------------------------------------------------------------------------------------------------------------------
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
    MFZeroMemory( &sd, sizeof( sd ) );
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

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

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

	
	g_pImmediateContext->VSSetConstantBuffers(n_cbWorld, 1, &g_pConstantBufferWorld);
	g_pImmediateContext->PSSetConstantBuffers(n_cbWorld, 1, &g_pConstantBufferWorld);

	MFZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

	g_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_pDepthStencilState);

	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_DestroyDisplay()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

	if (g_pConstantBufferWorld) g_pConstantBufferWorld->Release();

	if (g_pDepthStencilState) g_pDepthStencilState->Release();
	
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
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_ResetDisplay()
{
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_BeginFrame()
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_EndFrame()
{
	MFCALLSTACK;
    
	g_pSwapChain->Present( 0, 0 );
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_ClearScreen(uint32 flags)
{
	MFCALLSTACKc;
	
	const bool bClearDepth = (flags & CS_ZBuffer) != 0;
	const bool bClearStencil = (flags & CS_Stencil) != 0;
	const bool bClearColour = (flags & CS_Colour) != 0;

	if (bClearColour)
	{
		float ClearColor[4] = { gClearColour.x, gClearColour.y, gClearColour.z, gClearColour.w }; // RGBA
		g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	}
	
    if (bClearDepth | bClearStencil)
    {
		uint32 clearFlags = (bClearDepth ? D3D11_CLEAR_DEPTH : 0) | (bClearStencil ? D3D11_CLEAR_STENCIL : 0);
		g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, clearFlags, 1.0f, 0);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_GetViewport(MFRect *pRect)
{
}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_SetViewport(MFRect *pRect)
{

}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_ResetViewport()
{

}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget)
{

}
//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_SetDeviceRenderTarget()
{
}
//---------------------------------------------------------------------------------------------------------------------
float MFRenderer_GetTexelCenterOffset()
{
	return 0.5f;
}
//---------------------------------------------------------------------------------------------------------------------
//
//// direct3d management fucntions
//void MFRenderer_D3D11_SetTexture(int stage, IDirect3DTexture9 *pTexture)
//{
//}
//
//void MFRenderer_D3D11_SetVertexShader(IDirect3DVertexShader9 *pVertexShader)
//{
//}
//
//void MFRenderer_D3D11_SetStreamSource(int stream, IDirect3DVertexBuffer9 *pVertexBuffer, int offset, int stride)
//{
//}
//
//void MFRenderer_D3D11_SetIndices(IDirect3DIndexBuffer9 *pIndexBuffer)
//{
//}
//
//void MFRenderer_D3D11_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix)
//{
//}

//---------------------------------------------------------------------------------------------------------------------
void MFRenderer_D3D11_SetWorldToScreenMatrix(const MFMatrix &worldToScreen)
{
	cbWorld.mWorldToScreen.Transpose(worldToScreen);

	g_pImmediateContext->UpdateSubresource(g_pConstantBufferWorld, 0, NULL, &cbWorld, 0, 0);
}
//---------------------------------------------------------------------------------------------------------------------
//void MFRenderer_D3D11_SetTextureMatrix(const MFMatrix &textureMatrix)
//{
//}
//
//void MFRenderer_D3D11_SetModelColour(const MFVector &colour)
//{
//}
//
//void MFRenderer_D3D11_SetColourMask(float colourModulate, float colourAdd, float alphaModulate, float alphaAdd)
//{
//}
//
//void MFRenderer_D3D11_SetNumWeights(int numWeights)
//{
//}
//
//int MFRenderer_D3D11_GetNumWeights()
//{
//	return 0;
//}
//
//void MFRenderer_D3D11_ApplyGPUStates()
//{
//
//}
//
//void MFRenderer_D3D11_SetDefaultGPUStates()
//{
//
//}
//
//void MFRenderer_D3D11_ApplyRenderStates()
//{
//
//}
//
//void MFRenderer_D3D11_SetDefaultRenderStates()
//{
//
//}
//
//void MFRenderer_D3D11_SetRenderState(D3DRENDERSTATETYPE type, uint32 value)
//{
//}
//
//void MFRenderer_D3D11_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRenderer_D3D11_ApplyTextureStageStates()
//{
//
//}
//
//void MFRenderer_D3D11_SetDefaultTextureStageStates()
//{
//
//}
//
//void MFRenderer_D3D11_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value)
//{
//}
//
//void MFRenderer_D3D11_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRenderer_D3D11_ApplySamplerStates()
//{
//
//}
//
//void MFRenderer_D3D11_SetDefaultSamplerStates()
//{
//
//}
//
//void MFRenderer_D3D11_SetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 value)
//{
//}
//
//void MFRenderer_D3D11_GetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRenderer_D3D11_ConvertFloatToPCVF(const float *pFloat, char *pData, PCVF_Type type, int *pNumBytesWritten)
//{
//}
//
//void MFRenderer_D3D11_ConvertPCVFToFloat(const char *pData, float *pFloat, PCVF_Type type, int *pNumComponentsWritten)
//{
//}
//---------------------------------------------------------------------------------------------------------------------

#endif // MF_RENDERER
