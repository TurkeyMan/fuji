#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_D3D9
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_D3D9
	#define MFMat_Standard_Begin MFMat_Standard_Begin_D3D9
	#define MFMat_Standard_CreateInstancePlatformSpecific MFMat_Standard_CreateInstancePlatformSpecific_D3D9
	#define MFMat_Standard_DestroyInstancePlatformSpecific MFMat_Standard_DestroyInstancePlatformSpecific_D3D9
#endif

#include "MFRenderState_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "../MFRenderer_D3D9.h"
#include "Materials/MFMat_Standard_Internal.h"

#include "../Shaders/Registers.h"
#include "../Shaders/MatStandard_s.h"
#include "../Shaders/MatStandard_a.h"


extern IDirect3DDevice9 *pd3dDevice;
extern D3DCAPS9 gD3D9DeviceCaps;

IDirect3DVertexShader9 *pVS_s = NULL;
IDirect3DVertexShader9 *pVS_a = NULL;

static const DWORD sFilterModes[MFTexFilter_Max] =
{
    D3DTEXF_NONE,
    D3DTEXF_POINT,
    D3DTEXF_LINEAR,
    D3DTEXF_ANISOTROPIC,
//	D3DTEXF_NONE // MFTexFilter_Text_1Bit - unsupported!
};

static const DWORD sAddressModes[MFTexAddressMode_Max] =
{
    D3DTADDRESS_WRAP,
    D3DTADDRESS_MIRROR,
    D3DTADDRESS_CLAMP,
    D3DTADDRESS_BORDER,
    D3DTADDRESS_MIRRORONCE
};


int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
	MFCALLSTACK;

	pd3dDevice->CreateVertexShader(reinterpret_cast<const DWORD*>(g_vs_main_s), &pVS_s);
	pd3dDevice->CreateVertexShader(reinterpret_cast<const DWORD*>(g_vs_main_a), &pVS_a);

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;

	pVS_s->Release();
	pVS_a->Release();
}

static void MFMat_Standard_SetSamplerState(int sampler, MFSamplerState *pSamp)
{
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_MINFILTER, sFilterModes[pSamp->stateDesc.minFilter]);
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_MAGFILTER, sFilterModes[pSamp->stateDesc.magFilter]);
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_MIPFILTER, sFilterModes[pSamp->stateDesc.mipFilter]);
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_ADDRESSU, sAddressModes[pSamp->stateDesc.addressU]);
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_ADDRESSV, sAddressModes[pSamp->stateDesc.addressV]);
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_ADDRESSW, sAddressModes[pSamp->stateDesc.addressW]);

	if(pSamp->stateDesc.magFilter == MFTexFilter_Anisotropic || pSamp->stateDesc.minFilter == MFTexFilter_Anisotropic || pSamp->stateDesc.mipFilter == MFTexFilter_Anisotropic)
		pd3dDevice->SetSamplerState(sampler, D3DSAMP_MAXANISOTROPY, pSamp->stateDesc.maxAnisotropy);

	if(pSamp->stateDesc.addressU == MFTexAddressMode_Border || pSamp->stateDesc.addressV == MFTexAddressMode_Border || pSamp->stateDesc.addressW == MFTexAddressMode_Border)
		pd3dDevice->SetSamplerState(sampler, D3DSAMP_BORDERCOLOR, pSamp->stateDesc.borderColour.ToPackedColour());

	pd3dDevice->SetSamplerState(sampler, D3DSAMP_MAXMIPLEVEL, (DWORD)pSamp->stateDesc.maxLOD);
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_MIPMAPLODBIAS , (DWORD)pSamp->stateDesc.mipLODBias);
}

int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	MFCALLSTACK;

	if(state.pMatrixStatesSet[MFSCM_WorldViewProjection] != state.pMatrixStates[MFSCM_WorldViewProjection])
	{
		MFMatrix *pWVP = state.getDerivedMatrix(MFSCM_WorldViewProjection);
		state.pMatrixStates[MFSCM_WorldViewProjection] = pWVP;

		MFMatrix mat;
		mat.Transpose(*pWVP);
		pd3dDevice->SetVertexShaderConstantF(r_wvp, (float*)&mat, 4);
	}

	if(state.pMatrixStatesSet[MFSCM_UV0] != state.pMatrixStates[MFSCM_UV0])
	{
		MFMatrix *pUV0 = state.pMatrixStates[MFSCM_UV0];
		state.pMatrixStatesSet[MFSCM_UV0] = pUV0;

		MFMatrix mat;
		mat.Transpose(*pUV0);
		pd3dDevice->SetVertexShaderConstantF(r_tex, (float*)&mat, 2);
	}

	if(state.pVectorStatesSet[MFSCV_MaterialDiffuseColour] != state.pVectorStates[MFSCV_MaterialDiffuseColour])
	{
		MFVector *pDiffuseColour = state.pVectorStates[MFSCV_MaterialDiffuseColour];
		state.pVectorStatesSet[MFSCV_MaterialDiffuseColour] = pDiffuseColour;

		pd3dDevice->SetVertexShaderConstantF(r_modelColour, (float*)pDiffuseColour, 1);
	}

	bool bDetailPresent = state.isSet(MFSB_CT_Bool, MFSCB_DetailMapSet);
	bool bDetailChanged = !!state.boolChanged(MFSCB_DetailMapSet);
	bool bDiffusePresent = state.isSet(MFSB_CT_Bool, MFSCB_DiffuseSet);
	bool bDiffuseChanged = !!state.boolChanged(MFSCB_DiffuseSet);
	int diffuseIndex = bDetailPresent ? 1 : 0;

	if(bDetailPresent)
	{
		// set detail map
		MFTexture *pDetail = state.pTextures[MFSCT_DetailMap];
		if(state.pTexturesSet[MFSCT_DetailMap] != pDetail || bDetailChanged)
		{
			state.pTexturesSet[MFSCT_DetailMap] = pDetail;
			pd3dDevice->SetTexture(0, (IDirect3DTexture9*)pDetail->pInternalData);
		}

		// set detail map sampler
		MFSamplerState *pDetailSamp = (MFSamplerState*)state.pRenderStates[MFSCRS_DetailMapSamplerState];
		if(state.pRenderStatesSet[MFSCRS_DetailMapSamplerState] != pDetailSamp || bDetailChanged)
		{
			state.pRenderStatesSet[MFSCRS_DetailMapSamplerState] = pDetailSamp;
			MFMat_Standard_SetSamplerState(0, pDetailSamp);
		}
	}
	else
	{
		if(state.pTexturesSet[MFSCT_DetailMap] != NULL || bDetailChanged)
		{
			state.pTexturesSet[MFSCT_DetailMap] = NULL;
			pd3dDevice->SetTexture(1, NULL);
		}
	}

	if(bDiffusePresent)
	{
		// set diffuse map
		MFTexture *pDiffuse = state.pTextures[MFSCT_Diffuse];
		if(state.pTexturesSet[MFSCT_Diffuse] != pDiffuse || bDetailChanged)
		{
			state.pTexturesSet[MFSCT_Diffuse] = pDiffuse;
			pd3dDevice->SetTexture(diffuseIndex, (IDirect3DTexture9*)pDiffuse->pInternalData);
		}

		// set diffuse map sampler
		MFSamplerState *pDiffuseSamp = (MFSamplerState*)state.pRenderStates[MFSCRS_DiffuseSamplerState];
		if(state.pRenderStatesSet[MFSCRS_DiffuseSamplerState] != pDiffuseSamp || bDetailChanged)
		{
			state.pRenderStatesSet[MFSCRS_DiffuseSamplerState] = pDiffuseSamp;
			MFMat_Standard_SetSamplerState(diffuseIndex, pDiffuseSamp);
		}
	}
	else
	{
		if(state.pTexturesSet[MFSCT_Diffuse] != NULL || bDetailChanged)
		{
			state.pTexturesSet[MFSCT_Diffuse] = NULL;
			pd3dDevice->SetTexture(diffuseIndex, NULL);
		}
	}

	// configure the texture combiner (can we cache this?)
	if(bDetailChanged || bDiffuseChanged || state.boolChanged(MFSCB_User0))
	{
		const uint32 mask = MFBIT(MFSCB_DetailMapSet) | MFBIT(MFSCB_DiffuseSet) | MFBIT(MFSCB_User0);
		state.boolsSet = (state.boolsSet & ~mask) | (state.bools & mask);

		if(bDetailPresent)
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
			pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		}
		else
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			if(state.getBool(MFSCB_User0))
			{
				// we need to scale the colour intensity by the vertex alpha since it wont happen during the blend.
				pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
				pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

//				pd3dDevice->SetTextureStageState(1, D3DTSS_CONSTANT, 0);

				pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
				pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEMP);
				pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
			}
			else
			{
				pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
		}
	}

	// blend state
	MFBlendState *pBlendState = (MFBlendState*)state.pRenderStates[MFSCRS_BlendState];
	if(state.pRenderStatesSet[MFSCRS_BlendState] != pBlendState)
	{
		state.pRenderStatesSet[MFSCRS_BlendState] = pBlendState;
		IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)pBlendState->pPlatformData;
		pSB->Apply();
	}

	// rasteriser state
	MFRasteriserState *pRasteriserState = (MFRasteriserState*)state.pRenderStates[MFSCRS_RasteriserState];
	if(state.pRenderStatesSet[MFSCRS_RasteriserState] != pRasteriserState)
	{
		state.pRenderStatesSet[MFSCRS_RasteriserState] = pRasteriserState;
		IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)pRasteriserState->pPlatformData;
		pSB->Apply();
	}

	// depth/stencil state
	MFDepthStencilState *pDSState = (MFDepthStencilState*)state.pRenderStates[MFSCRS_DepthStencilState];
	if(state.pRenderStatesSet[MFSCRS_DepthStencilState] != pDSState)
	{
		state.pRenderStatesSet[MFSCRS_DepthStencilState] = pDSState;
		IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)pDSState->pPlatformData;
		pSB->Apply();
	}

	// setup alpha test
	if(state.boolChanged(MFSCB_AlphaTest) || (state.pVectorStatesSet[MFSCV_RenderState] != state.pVectorStates[MFSCV_RenderState] && state.getBool(MFSCB_AlphaTest)))
	{
		MFVector *pRS = state.pVectorStates[MFSCV_RenderState];
		state.pVectorStatesSet[MFSCV_RenderState] = pRS;
		state.boolsSet = (state.boolsSet & ~MFBIT(MFSCB_AlphaTest)) | (state.bools & MFBIT(MFSCB_AlphaTest));

		if(state.getBool(MFSCB_AlphaTest))
		{
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
//			pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL); // this is set globally, never changes
			pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)MFClamp(0.f, pRS->x * 255.f, 255.f));
		}
		else
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	// set alpha ref
	if(state.pVectorStatesSet[MFSCV_RenderState] != state.pVectorStates[MFSCV_RenderState] && state.getBool(MFSCB_AlphaTest))
	{
		MFVector *pRS = state.pVectorStates[MFSCV_RenderState];
		state.pVectorStatesSet[MFSCV_RenderState] = pRS;

		pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)MFClamp(0.f, pRS->x * 255.f, 255.f));
	}

	// set clour/alpha scales
	if(state.pVectorStatesSet[MFSCV_User0] != state.pVectorStates[MFSCV_User0])
	{
		MFVector *pMask = state.pVectorStates[MFSCV_User0];
		state.pVectorStatesSet[MFSCV_User0] = pMask;

		pd3dDevice->SetVertexShaderConstantF(r_colourMask, (float*)pMask, 1);
	}

	// set animation matrices
	if(state.getBool(MFSCB_Animated))
	{
//		for(int b=0; b<gNumBonesInBatch; b++)
//			MFRendererPC_SetAnimationMatrix(b, pAnimMats[pCurrentBatch[b]]);

		pd3dDevice->SetVertexShader(pVS_a);
	}
	else
		pd3dDevice->SetVertexShader(pVS_s);

	return 0;
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

#endif // MF_RENDERER
