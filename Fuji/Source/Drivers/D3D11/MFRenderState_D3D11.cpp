#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFRenderState_InitModulePlatformSpecific MFRenderState_InitModulePlatformSpecific_D3D11
	#define MFRenderState_DeinitModulePlatformSpecific MFRenderState_DeinitModulePlatformSpecific_D3D11
	#define MFBlendState_CreatePlatformSpecific MFBlendState_CreatePlatformSpecific_D3D11
	#define MFBlendState_DestroyPlatformSpecific MFBlendState_DestroyPlatformSpecific_D3D11
	#define MFSamplerState_CreatePlatformSpecific MFSamplerState_CreatePlatformSpecific_D3D11
	#define MFSamplerState_DestroyPlatformSpecific MFSamplerState_DestroyPlatformSpecific_D3D11
	#define MFDepthStencilState_CreatePlatformSpecific MFDepthStencilState_CreatePlatformSpecific_D3D11
	#define MFDepthStencilState_DestroyPlatformSpecific MFDepthStencilState_DestroyPlatformSpecific_D3D11
	#define MFRasteriserState_CreatePlatformSpecific MFRasteriserState_CreatePlatformSpecific_D3D11
	#define MFRasteriserState_DestroyPlatformSpecific MFRasteriserState_DestroyPlatformSpecific_D3D11
#endif

#include "MFRenderState_Internal.h"
#include "MFRenderer_D3D11.h"


static const D3D11_FILTER_TYPE sFilterType[MFTexFilter_Max] =
{
	D3D11_FILTER_TYPE_POINT,
	D3D11_FILTER_TYPE_POINT,
	D3D11_FILTER_TYPE_LINEAR,
	D3D11_FILTER_TYPE_LINEAR,
};

static const D3D11_TEXTURE_ADDRESS_MODE sTexAddress[MFTexAddressMode_Max] =
{
	D3D11_TEXTURE_ADDRESS_WRAP,			// MFMatStandard_TexAddress_Wrap
	D3D11_TEXTURE_ADDRESS_MIRROR,		// MFMatStandard_TexAddress_Mirror
	D3D11_TEXTURE_ADDRESS_CLAMP,		// MFMatStandard_TexAddress_Clamp
	D3D11_TEXTURE_ADDRESS_BORDER,		// MFMatStandard_TexAddress_Border
	D3D11_TEXTURE_ADDRESS_MIRROR_ONCE	// MFMatStandard_TexAddress_MirrorOnce
};

static const D3D11_BLEND_OP sBlendOp[MFBlendOp_BlendOpCount] =
{
    D3D11_BLEND_OP_ADD,
    D3D11_BLEND_OP_SUBTRACT,
    D3D11_BLEND_OP_REV_SUBTRACT,
    D3D11_BLEND_OP_MIN,
    D3D11_BLEND_OP_MAX
};

static const D3D11_BLEND sBlendArg[MFBlendArg_Max] =
{
	D3D11_BLEND_ZERO,
	D3D11_BLEND_ONE,
	D3D11_BLEND_SRC_COLOR,
	D3D11_BLEND_INV_SRC_COLOR,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_DEST_COLOR,
	D3D11_BLEND_INV_DEST_COLOR,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_INV_DEST_ALPHA,
	D3D11_BLEND_SRC_ALPHA_SAT,
	D3D11_BLEND_BLEND_FACTOR,
	D3D11_BLEND_INV_BLEND_FACTOR,
	D3D11_BLEND_SRC1_COLOR,
	D3D11_BLEND_INV_SRC1_COLOR,
	D3D11_BLEND_SRC1_ALPHA,
	D3D11_BLEND_INV_SRC1_ALPHA,
};

static const D3D11_COMPARISON_FUNC sCompareFunc[MFComparisonFunc_Max] =
{
	D3D11_COMPARISON_NEVER,
	D3D11_COMPARISON_LESS,
	D3D11_COMPARISON_EQUAL,
	D3D11_COMPARISON_LESS_EQUAL,
	D3D11_COMPARISON_GREATER,
	D3D11_COMPARISON_NOT_EQUAL,
	D3D11_COMPARISON_GREATER_EQUAL,
	D3D11_COMPARISON_ALWAYS
};

static const D3D11_FILL_MODE sFillMode[MFFillMode_Max] =
{
	D3D11_FILL_SOLID,
	D3D11_FILL_WIREFRAME,
	D3D11_FILL_WIREFRAME	// MFFillMode_Point not supported in D3D11!
};

static const D3D11_DEPTH_WRITE_MASK sWriteMask[MFDepthWriteMask_Max] =
{
	D3D11_DEPTH_WRITE_MASK_ZERO,
	D3D11_DEPTH_WRITE_MASK_ALL
};

static const D3D11_STENCIL_OP sStencilOp[MFStencilOp_Max] =
{
	D3D11_STENCIL_OP_KEEP,
	D3D11_STENCIL_OP_ZERO,
	D3D11_STENCIL_OP_REPLACE,
	D3D11_STENCIL_OP_INCR_SAT,
	D3D11_STENCIL_OP_DECR_SAT,
	D3D11_STENCIL_OP_INVERT,
	D3D11_STENCIL_OP_INCR,
	D3D11_STENCIL_OP_DECR
};


extern ID3D11Device* g_pd3dDevice;


void MFRenderState_InitModulePlatformSpecific()
{
}

void MFRenderState_DeinitModulePlatformSpecific()
{
}

bool MFBlendState_CreatePlatformSpecific(MFBlendState *pBS)
{
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = pBS->stateDesc.bAlphaToCoverageEnable;
	blendDesc.IndependentBlendEnable = pBS->stateDesc.bIndependentBlendEnable;
	for(int a=0; a<8; ++a)
	{
		blendDesc.RenderTarget[a].BlendEnable = pBS->stateDesc.renderTarget[a].bEnable;
		blendDesc.RenderTarget[a].SrcBlend = sBlendArg[pBS->stateDesc.renderTarget[a].srcBlend];
		blendDesc.RenderTarget[a].DestBlend = sBlendArg[pBS->stateDesc.renderTarget[a].destBlend];
		blendDesc.RenderTarget[a].BlendOp = sBlendOp[pBS->stateDesc.renderTarget[a].blendOp];
		blendDesc.RenderTarget[a].SrcBlendAlpha = sBlendArg[pBS->stateDesc.renderTarget[a].srcBlendAlpha];
		blendDesc.RenderTarget[a].DestBlendAlpha = sBlendArg[pBS->stateDesc.renderTarget[a].destBlendAlpha];
		blendDesc.RenderTarget[a].BlendOpAlpha = sBlendOp[pBS->stateDesc.renderTarget[a].blendOpAlpha];
		blendDesc.RenderTarget[a].RenderTargetWriteMask = pBS->stateDesc.renderTarget[a].writeMask;
	}

	ID3D11BlendState *pD3D11BS;
	HRESULT hr = g_pd3dDevice->CreateBlendState(&blendDesc, &pD3D11BS);
	pBS->pPlatformData = pD3D11BS;

	return SUCCEEDED(hr);
}

void MFBlendState_DestroyPlatformSpecific(MFBlendState *pBS)
{
	ID3D11BlendState *pD3D11BS = (ID3D11BlendState*)pBS->pPlatformData;
	pD3D11BS->Release();
}

bool MFSamplerState_CreatePlatformSpecific(MFSamplerState *pSS)
{
	D3D11_SAMPLER_DESC samplerDesc;
	if(pSS->stateDesc.magFilter == MFTexFilter_Anisotropic || pSS->stateDesc.minFilter == MFTexFilter_Anisotropic || pSS->stateDesc.mipFilter == MFTexFilter_Anisotropic)
		samplerDesc.Filter = D3D11_ENCODE_ANISOTROPIC_FILTER(false);
	else
		samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(sFilterType[pSS->stateDesc.minFilter], sFilterType[pSS->stateDesc.magFilter], sFilterType[pSS->stateDesc.mipFilter], false);
	samplerDesc.AddressU = sTexAddress[pSS->stateDesc.addressU];
	samplerDesc.AddressV = sTexAddress[pSS->stateDesc.addressV];
	samplerDesc.AddressW = sTexAddress[pSS->stateDesc.addressW];
	samplerDesc.MipLODBias = pSS->stateDesc.mipLODBias;
	samplerDesc.MaxAnisotropy = pSS->stateDesc.maxAnisotropy;
	samplerDesc.ComparisonFunc = sCompareFunc[pSS->stateDesc.comparisonFunc];
	samplerDesc.BorderColor[0] = pSS->stateDesc.borderColour.x;
	samplerDesc.BorderColor[1] = pSS->stateDesc.borderColour.y;
	samplerDesc.BorderColor[2] = pSS->stateDesc.borderColour.z;
	samplerDesc.BorderColor[3] = pSS->stateDesc.borderColour.w;
	samplerDesc.MinLOD = pSS->stateDesc.minLOD;
	samplerDesc.MaxLOD = pSS->stateDesc.maxLOD;

	ID3D11SamplerState *pD3D11SS;
	HRESULT hr = g_pd3dDevice->CreateSamplerState(&samplerDesc, &pD3D11SS);
	pSS->pPlatformData = pD3D11SS;

	return SUCCEEDED(hr);
}

void MFSamplerState_DestroyPlatformSpecific(MFSamplerState *pSS)
{
	ID3D11SamplerState *pD3D11SS = (ID3D11SamplerState*)pSS->pPlatformData;
	pD3D11SS->Release();
}

bool MFDepthStencilState_CreatePlatformSpecific(MFDepthStencilState *pDSS)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = pDSS->stateDesc.bDepthEnable;
	depthStencilDesc.DepthWriteMask = sWriteMask[pDSS->stateDesc.depthWriteMask];
	depthStencilDesc.DepthFunc = sCompareFunc[pDSS->stateDesc.depthFunc];
	depthStencilDesc.StencilEnable = pDSS->stateDesc.bStencilEnable;
	depthStencilDesc.StencilReadMask = pDSS->stateDesc.stencilReadMask;
	depthStencilDesc.StencilWriteMask = pDSS->stateDesc.stencilWriteMask;
	depthStencilDesc.FrontFace.StencilFailOp = sStencilOp[pDSS->stateDesc.frontFace.stencilFailOp];
	depthStencilDesc.FrontFace.StencilDepthFailOp = sStencilOp[pDSS->stateDesc.frontFace.stencilDepthFailOp];
	depthStencilDesc.FrontFace.StencilPassOp = sStencilOp[pDSS->stateDesc.frontFace.stencilPassOp];
	depthStencilDesc.FrontFace.StencilFunc = sCompareFunc[pDSS->stateDesc.frontFace.stencilFunc];
	depthStencilDesc.BackFace.StencilFailOp = sStencilOp[pDSS->stateDesc.backFace.stencilFailOp];
	depthStencilDesc.BackFace.StencilDepthFailOp = sStencilOp[pDSS->stateDesc.backFace.stencilDepthFailOp];
	depthStencilDesc.BackFace.StencilPassOp = sStencilOp[pDSS->stateDesc.backFace.stencilPassOp];
	depthStencilDesc.BackFace.StencilFunc = sCompareFunc[pDSS->stateDesc.backFace.stencilFunc];

	ID3D11DepthStencilState *pD3D11DSS;
	HRESULT hr = g_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &pD3D11DSS);
	pDSS->pPlatformData = pD3D11DSS;

	return SUCCEEDED(hr);
}

void MFDepthStencilState_DestroyPlatformSpecific(MFDepthStencilState *pDSS)
{
	ID3D11DepthStencilState *pD3D11DSS = (ID3D11DepthStencilState*)pDSS->pPlatformData;
	pD3D11DSS->Release();
}

bool MFRasteriserState_CreatePlatformSpecific(MFRasteriserState *pRS)
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	switch(pRS->stateDesc.fillMode)
	{
		case MFCullMode_None:
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			rasterizerDesc.FrontCounterClockwise = false;
			break;
		case MFCullMode_CCW:
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = false;
			break;
		case MFCullMode_CW:
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = true;
			break;
		default:
			MFDebug_Assert(false, "Invalid cull mode!");
	}
	rasterizerDesc.FillMode = sFillMode[pRS->stateDesc.fillMode];
	rasterizerDesc.DepthBias = pRS->stateDesc.depthBias;
	rasterizerDesc.DepthBiasClamp = pRS->stateDesc.depthBiasClamp;
	rasterizerDesc.SlopeScaledDepthBias = pRS->stateDesc.slopeScaledDepthBias;
	rasterizerDesc.DepthClipEnable = pRS->stateDesc.bDepthClipEnable;
	rasterizerDesc.ScissorEnable = pRS->stateDesc.bScissorEnable;
	rasterizerDesc.MultisampleEnable = pRS->stateDesc.bMultisampleEnable;
	rasterizerDesc.AntialiasedLineEnable = pRS->stateDesc.bAntialiasedLineEnable;

	ID3D11RasterizerState *pD3D11RS;
	HRESULT hr = g_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &pD3D11RS);
	pRS->pPlatformData = pD3D11RS;

	return SUCCEEDED(hr);
}

void MFRasteriserState_DestroyPlatformSpecific(MFRasteriserState *pRS)
{
	ID3D11RasterizerState *pD3D11RS = (ID3D11RasterizerState*)pRS->pPlatformData;
	pD3D11RS->Release();
}

#endif // MF_RENDERER
