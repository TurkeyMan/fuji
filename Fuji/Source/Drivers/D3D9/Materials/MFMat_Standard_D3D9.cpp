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
#include "MFEffect_Internal.h"

//#include "Mat_Standard_mfx.h"
//#include "Fuji_h.h"

extern IDirect3DDevice9 *pd3dDevice;

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
	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
}

static void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix)
{
	MFMatrix mat = animationMatrix;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(112 + boneID*3, (float*)&mat, 3);
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

//	pd3dDevice->SetSamplerState(sampler, D3DSAMP_MAXMIPLEVEL, (DWORD)pSamp->stateDesc.maxLOD);
	pd3dDevice->SetSamplerState(sampler, D3DSAMP_MIPMAPLODBIAS , *(DWORD*)&pSamp->stateDesc.mipLODBias);
}

int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFEffectTechnique *pTechnique = NULL;
	if(pData->pEffect)
		pTechnique = MFEffect_GetTechnique(pData->pEffect, state);
	MFDebug_Assert(pTechnique, "No technique!");

	MFShader *pVS = pTechnique->shaders[MFST_VertexShader].pShader;
	MFShader *pPS = pTechnique->shaders[MFST_PixelShader].pShader;

	if(pTechnique != state.pTechniqueSet)
	{
		state.pTechniqueSet = pTechnique;

		MFDebug_Assert(pVS && pPS, "Missing shader!");
		pd3dDevice->SetVertexShader((IDirect3DVertexShader9*)pVS->pPlatformData);
		pd3dDevice->SetPixelShader((IDirect3DPixelShader9*)pPS->pPlatformData);
	}

	// bools
	uint32 boolState = state.bools & state.rsSet[MFSB_CT_Bool];

	uint32 req = pTechnique->renderStateRequirements[MFSB_CT_Bool];
	if(req)
	{
		uint32 vsReq = pVS->renderStateRequirements[MFSB_CT_Bool];
		uint32 psReq = pPS->renderStateRequirements[MFSB_CT_Bool];
		if((state.boolsSet & req) != (boolState & req))
		{
			BOOL bools[32];
			for(uint32 i=0, b=1; i<MFSCB_Max; ++i, b<<=1)
				bools[i] = (boolState & b) != 0;

			if((state.boolsSet & vsReq) != (boolState & vsReq))
				pd3dDevice->SetVertexShaderConstantB(0, bools,  32);
			if((state.boolsSet & psReq) != (boolState & psReq))
				pd3dDevice->SetPixelShaderConstantB(0, bools,  32);
		}
	}

	// matrices
	req = pTechnique->renderStateRequirements[MFSB_CT_Matrix];
	if(req)
	{
		uint32 vsReq = pVS->renderStateRequirements[MFSB_CT_Matrix];
		uint32 psReq = pPS->renderStateRequirements[MFSB_CT_Matrix];

		uint32 i;
		while(MFUtil_BitScanReverse(req, &i))
		{
			uint32 b = MFBIT(i);
			req ^= b;

			if(state.pMatrixStatesSet[i] != state.pMatrixStates[i])
			{
				MFMatrix *pM;
				if(i > MFSCM_DerivedStart)
					pM = state.getDerivedMatrix((MFStateConstant_Matrix)i);
				else
					pM = state.pMatrixStates[i];
				state.pMatrixStatesSet[i] = pM;

				MFMatrix mat;
				mat.Transpose(*pM);
				if(vsReq & b)
					pd3dDevice->SetVertexShaderConstantF(i*4, (float*)&mat, 4);
				if(psReq & b)
					pd3dDevice->SetPixelShaderConstantF(i*4, (float*)&mat, 4);
			}
		}
	}

	// vectors
	req = pTechnique->renderStateRequirements[MFSB_CT_Vector];
	if(req)
	{
		uint32 vsReq = pVS->renderStateRequirements[MFSB_CT_Vector];
		uint32 psReq = pPS->renderStateRequirements[MFSB_CT_Vector];

		uint32 i;
		while(MFUtil_BitScanReverse(req, &i))
		{
			uint32 b = MFBIT(i);
			req ^= b;

			if(state.pVectorStatesSet[i] != state.pVectorStates[i])
			{
				MFVector *pV = state.pVectorStates[i];
				state.pVectorStatesSet[i] = pV;
				if(vsReq & b)
					pd3dDevice->SetVertexShaderConstantF(80 + i, (float*)pV, 1);
				if(psReq & b)
					pd3dDevice->SetPixelShaderConstantF(80 + i, (float*)pV, 1);
			}
		}
	}

	// textures
	req = pTechnique->renderStateRequirements[MFSB_CT_Texture];
	uint32 i;
	while(MFUtil_BitScanReverse(req, &i))
	{
		req ^= MFBIT(i);

		MFTexture *pT = state.pTextures[i];
		if(state.pTexturesSet[i] != pT)
		{
			state.pTexturesSet[i] = pT;
			pd3dDevice->SetTexture(i, (IDirect3DTexture9*)pT->pInternalData);
		}

		MFSamplerState *pS = (MFSamplerState*)state.pRenderStates[MFSCRS_DiffuseSamplerState + i];
		if(state.pRenderStatesSet[MFSCRS_DiffuseSamplerState + i] != pS)
		{
			state.pRenderStatesSet[MFSCRS_DiffuseSamplerState + i] = pS;
			MFMat_Standard_SetSamplerState(i, pS);
		}
	}

	// renderstates

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

	// set animation matrices
	if(state.getBool(MFSCB_Animated))
	{
		for(uint32 b=0; b<state.matrixBatch.numMatrices; b++)
			MFRendererPC_SetAnimationMatrix(b, state.animation.pMatrices[state.matrixBatch.pIndices[b]]);
	}

	// set viewport
	if(state.pViewportSet != state.pViewport)
	{
		if(!state.pViewport)
			MFRenderer_ResetViewport();
		else
			MFRenderer_SetViewport(state.pViewport);
		state.pViewportSet = state.pViewport;
	}

	state.boolsSet = boolState;

	return 0;
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

#endif // MF_RENDERER
