#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFRenderState_InitModulePlatformSpecific MFRenderState_InitModulePlatformSpecific_D3D9
	#define MFRenderState_DeinitModulePlatformSpecific MFRenderState_DeinitModulePlatformSpecific_D3D9
	#define MFBlendState_CreatePlatformSpecific MFBlendState_CreatePlatformSpecific_D3D9
	#define MFBlendState_DestroyPlatformSpecific MFBlendState_DestroyPlatformSpecific_D3D9
	#define MFSamplerState_CreatePlatformSpecific MFSamplerState_CreatePlatformSpecific_D3D9
	#define MFSamplerState_DestroyPlatformSpecific MFSamplerState_DestroyPlatformSpecific_D3D9
	#define MFDepthStencilState_CreatePlatformSpecific MFDepthStencilState_CreatePlatformSpecific_D3D9
	#define MFDepthStencilState_DestroyPlatformSpecific MFDepthStencilState_DestroyPlatformSpecific_D3D9
	#define MFRasteriserState_CreatePlatformSpecific MFRasteriserState_CreatePlatformSpecific_D3D9
	#define MFRasteriserState_DestroyPlatformSpecific MFRasteriserState_DestroyPlatformSpecific_D3D9
#endif

#include "MFRenderState_Internal.h"
#include "MFRenderer_D3D9.h"


static const DWORD sBlendOp[MFBlendOp_BlendOpCount] =
{
    D3DBLENDOP_ADD,
    D3DBLENDOP_SUBTRACT,
    D3DBLENDOP_REVSUBTRACT,
    D3DBLENDOP_MIN,
    D3DBLENDOP_MAX
};

static const DWORD sBlendArg[MFBlendArg_Max] =
{
	D3DBLEND_ZERO,
	D3DBLEND_ONE,
	D3DBLEND_SRCCOLOR,
	D3DBLEND_INVSRCCOLOR,
	D3DBLEND_SRCALPHA,
	D3DBLEND_INVSRCALPHA,
	D3DBLEND_DESTCOLOR,
	D3DBLEND_INVDESTCOLOR,
	D3DBLEND_DESTALPHA,
	D3DBLEND_INVDESTALPHA,
	D3DBLEND_SRCALPHASAT,
	D3DBLEND_BLENDFACTOR,
	D3DBLEND_INVBLENDFACTOR,
	D3DBLEND_SRCCOLOR2,
	D3DBLEND_INVSRCCOLOR2,
	D3DBLEND_ZERO,
	D3DBLEND_ZERO,
};

static const DWORD sCompareFunc[MFComparisonFunc_Max] =
{
	D3DCMP_NEVER,
	D3DCMP_LESS,
	D3DCMP_EQUAL,
	D3DCMP_LESSEQUAL,
	D3DCMP_GREATER,
	D3DCMP_NOTEQUAL,
	D3DCMP_GREATEREQUAL,
	D3DCMP_ALWAYS,
};

static const DWORD sStencilOp[MFStencilOp_Max] =
{
	D3DSTENCILOP_KEEP,
	D3DSTENCILOP_ZERO,
	D3DSTENCILOP_REPLACE,
	D3DSTENCILOP_INCRSAT,
	D3DSTENCILOP_DECRSAT,
	D3DSTENCILOP_INVERT,
	D3DSTENCILOP_INCR,
	D3DSTENCILOP_DECR
};

static const DWORD sCullMode[MFCullMode_Max] =
{
	D3DCULL_NONE,
	D3DCULL_CCW,
	D3DCULL_CW,
};

static const DWORD sFillMode[MFFillMode_Max] =
{
	D3DFILL_SOLID,
	D3DFILL_WIREFRAME,
	D3DFILL_POINT
};


extern IDirect3DDevice9 *pd3dDevice;
extern D3DCAPS9 gD3D9DeviceCaps;


void MFRenderState_InitModulePlatformSpecific()
{
}

void MFRenderState_DeinitModulePlatformSpecific()
{
}

void MFRenderState_Release()
{
	// clean up before reset
	MFResourceIterator *pI = MFResource_EnumerateFirst();
	while(pI)
	{
		MFResource *pResource = MFResource_Get(pI);

		int rt = MFResource_GetType(pResource);
		if(rt == MFRT_BlendState)
			MFBlendState_DestroyPlatformSpecific((MFBlendState*)pResource);
		else if(rt == MFRT_RasteriserState)
			MFRasteriserState_DestroyPlatformSpecific((MFRasteriserState*)pResource);
		else if(rt == MFRT_DepthStencilState)
			MFDepthStencilState_DestroyPlatformSpecific((MFDepthStencilState*)pResource);

		pI = MFResource_EnumerateNext(pI);
	}
}

void MFRenderState_Recreate()
{
	// recreate after reset
	MFResourceIterator *pI = MFResource_EnumerateFirst();
	while(pI)
	{
		MFResource *pResource = MFResource_Get(pI);

		int rt = MFResource_GetType(pResource);
		if(rt == MFRT_BlendState)
			MFBlendState_CreatePlatformSpecific((MFBlendState*)pResource);
		else if(rt == MFRT_RasteriserState)
			MFRasteriserState_CreatePlatformSpecific((MFRasteriserState*)pResource);
		else if(rt == MFRT_DepthStencilState)
			MFDepthStencilState_CreatePlatformSpecific((MFDepthStencilState*)pResource);

		pI = MFResource_EnumerateNext(pI);
	}
}

bool MFBlendState_CreatePlatformSpecific(MFBlendState *pBS)
{
	pd3dDevice->BeginStateBlock();

	MFBlendStateDesc::RenderTargetBlendDesc &target = pBS->stateDesc.renderTarget[0];
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, target.bEnable ? TRUE : FALSE);
	if(target.bEnable)
	{
		pd3dDevice->SetRenderState(D3DRS_BLENDOP, sBlendOp[target.blendOp]);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, sBlendArg[target.srcBlend]);
		pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, sBlendArg[target.srcBlendAlpha]);
		pd3dDevice->SetRenderState(D3DRS_BLENDOPALPHA, sBlendOp[target.blendOpAlpha]);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, sBlendArg[target.destBlend]);
		pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, sBlendArg[target.destBlendAlpha]);
	}

	IDirect3DStateBlock9 *pSB;
	pd3dDevice->EndStateBlock(&pSB);
	pBS->pPlatformData = pSB;
	return true;
}

void MFBlendState_DestroyPlatformSpecific(MFBlendState *pBS)
{
	IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)pBS->pPlatformData;
	if(pSB)
		pSB->Release();
	pBS->pPlatformData = NULL;
}

bool MFSamplerState_CreatePlatformSpecific(MFSamplerState *pSS)
{
	// we can't record a stateblock for the sampler state...
	// the stateblock would also record the texture unit index with sampler states :/

	return true;
}

void MFSamplerState_DestroyPlatformSpecific(MFSamplerState *pSS)
{
}

bool MFDepthStencilState_CreatePlatformSpecific(MFDepthStencilState *pDSS)
{
	pd3dDevice->BeginStateBlock();

	bool bDepthTest = pDSS->stateDesc.bDepthEnable;
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, bDepthTest ? D3DZB_TRUE : D3DZB_FALSE);
	if(bDepthTest)
	{
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, pDSS->stateDesc.depthWriteMask == MFDepthWriteMask_Zero ? FALSE : TRUE);
		pd3dDevice->SetRenderState(D3DRS_ZFUNC, sCompareFunc[pDSS->stateDesc.depthFunc]);
	}

	bool bStencil = pDSS->stateDesc.bStencilEnable;
	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, bStencil ? TRUE : FALSE);
	if(bStencil)
	{
		pd3dDevice->SetRenderState(D3DRS_STENCILMASK, pDSS->stateDesc.stencilReadMask);
		pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, pDSS->stateDesc.stencilWriteMask);

		pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, sCompareFunc[pDSS->stateDesc.frontFace.stencilFunc]);
		pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, sStencilOp[pDSS->stateDesc.frontFace.stencilFailOp]);
		pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL, sStencilOp[pDSS->stateDesc.frontFace.stencilDepthFailOp]);
		pd3dDevice->SetRenderState(D3DRS_STENCILPASS, sStencilOp[pDSS->stateDesc.frontFace.stencilPassOp]);

		if(gD3D9DeviceCaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED)
		{
			pd3dDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, sCompareFunc[pDSS->stateDesc.backFace.stencilFunc]);
			pd3dDevice->SetRenderState(D3DRS_CCW_STENCILFAIL, sStencilOp[pDSS->stateDesc.backFace.stencilFailOp]);
			pd3dDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, sStencilOp[pDSS->stateDesc.backFace.stencilDepthFailOp]);
			pd3dDevice->SetRenderState(D3DRS_CCW_STENCILPASS, sStencilOp[pDSS->stateDesc.backFace.stencilPassOp]);
		}
	}

	IDirect3DStateBlock9 *pSB;
	pd3dDevice->EndStateBlock(&pSB);
	pDSS->pPlatformData = pSB;
	return true;
}

void MFDepthStencilState_DestroyPlatformSpecific(MFDepthStencilState *pDSS)
{
	IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)pDSS->pPlatformData;
	if(pSB)
		pSB->Release();
	pDSS->pPlatformData = NULL;
}

bool MFRasteriserState_CreatePlatformSpecific(MFRasteriserState *pRS)
{
	pd3dDevice->BeginStateBlock();

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, sCullMode[pRS->stateDesc.cullMode]);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, sFillMode[pRS->stateDesc.fillMode]);
	pd3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, pRS->stateDesc.bAntialiasedLineEnable ? TRUE : FALSE);

//	pRS->stateDesc.bMultisampleEnable;

	pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, pRS->stateDesc.bScissorEnable ? TRUE : FALSE);
//	pRS->stateDesc.bDepthClipEnable;

	// revisit the depth bias states...
//	pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, pRS->stateDesc.depthBias);
//	pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, pRS->stateDesc.slopeScaledDepthBias);
//	pRS->stateDesc.depthBiasClamp;

	IDirect3DStateBlock9 *pSB;
	pd3dDevice->EndStateBlock(&pSB);
	pRS->pPlatformData = pSB;
	return true;
}

void MFRasteriserState_DestroyPlatformSpecific(MFRasteriserState *pRS)
{
	IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)pRS->pPlatformData;
	if(pSB)
		pSB->Release();
	pRS->pPlatformData = NULL;
}

#endif // MF_RENDERER
