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
#include "MFShader_Internal.h"

//#include "../Shaders/Registers.h"
//#include "../Shaders/MatStandard_s.h"
//#include "../Shaders/MatStandard_a.h"


extern IDirect3DDevice9 *pd3dDevice;
extern D3DCAPS9 gD3D9DeviceCaps;

MFShader *pVS_s = NULL;
MFShader *pVS_a = NULL;

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

#define r_wvpm 0
#define r_wm 4
#define r_wvm 8
#define r_vpm 12
#define r_tex 16
#define r_modelColour 18
#define r_colourMask 19
#define r_animating 20
#define r_numWeights 0
#define r_animMats 46

static const char gVertexShader[] = "														\n\
float4x4 mLocalToScreen : register(c0);														\n\
float4x4 mLocalToWorld : register(c4);														\n\
float4x4 mLocalToView : register(c8);														\n\
float4x4 mWorldToScreen : register(c12);													\n\
float4 mAnimMats[70*3] : register(c46);														\n\
float4 mTexMatrix[2] : register(c16);														\n\
																							\n\
float4 gModelColour : register(c18);														\n\
float4 gColourMask : register(c19);															\n\
																							\n\
//int gNumWeights : register(i0);															\n\
																							\n\
struct VS_INPUT																				\n\
{																							\n\
	float4 pos		: POSITION;																\n\
	float3 norm		: NORMAL;																\n\
	float4 uv		: TEXCOORD; 															\n\
	float4 colour	: COLOR0;																\n\
#if defined(_ANIMATION)																		\n\
	float4 weights	: BLENDWEIGHT;															\n\
	float4 indices	: BLENDINDICES;															\n\
#endif																						\n\
};																							\n\
																							\n\
struct VS_OUTPUT																			\n\
{																							\n\
	float4 pos		: POSITION;																\n\
	float4 colour	: COLOR0;																\n\
    																						\n\
	float4 uv		: TEXCOORD0;															\n\
	float4 uv2		: TEXCOORD1;															\n\
};																							\n\
																							\n\
VS_OUTPUT main(in VS_INPUT input)															\n\
{																							\n\
	VS_OUTPUT output;																		\n\
																							\n\
#if defined(_ANIMATION)																		\n\
	int i;																					\n\
	float3 t0, t1, t2, t3;																	\n\
	input.indices = input.indices * 255 + float4(0.5, 0.5, 0.5, 0.5);						\n\
																							\n\
	i = input.indices.x;																	\n\
	t0.x = dot(input.pos, mAnimMats[i]);													\n\
	t0.y = dot(input.pos, mAnimMats[i+1]);													\n\
	t0.z = dot(input.pos, mAnimMats[i+2]);													\n\
	i = input.indices.y;																	\n\
	t1.x = dot(input.pos, mAnimMats[i]);													\n\
	t1.y = dot(input.pos, mAnimMats[i+1]);													\n\
	t1.z = dot(input.pos, mAnimMats[i+2]);													\n\
	i = input.indices.z;																	\n\
	t2.x = dot(input.pos, mAnimMats[i]);													\n\
	t2.y = dot(input.pos, mAnimMats[i+1]);													\n\
	t2.z = dot(input.pos, mAnimMats[i+2]);													\n\
	i = input.indices.w;																	\n\
	t3.x = dot(input.pos, mAnimMats[i]);													\n\
	t3.y = dot(input.pos, mAnimMats[i+1]);													\n\
	t3.z = dot(input.pos, mAnimMats[i+2]);													\n\
																							\n\
	float4 pos;																				\n\
	pos.xyz = (t0*input.weights.x + t1*input.weights.y + t2*input.weights.z + t3*input.weights.w);	\n\
	pos.w = 1;																				\n\
																							\n\
	// transform pos																		\n\
	output.pos = mul(pos, mWorldToScreen);													\n\
#else																						\n\
	// transform pos																		\n\
	output.pos = mul(input.pos, mLocalToScreen);											\n\
#endif																						\n\
																							\n\
	// do lighting																			\n\
																							\n\
	// apply texture matrix																	\n\
	output.uv.x = dot(input.uv, mTexMatrix[0]);												\n\
	output.uv.yzw = dot(input.uv, mTexMatrix[1]);											\n\
	output.uv2 = output.uv;																	\n\
																							\n\
	// output colour and apply colour mask													\n\
	output.colour.xyz = input.colour.xyz*gModelColour.xyz*gColourMask.x + gColourMask.yyy;	\n\
	output.colour.w = input.colour.w*gModelColour.w*gColourMask.z + gColourMask.w;			\n\
																							\n\
	return output;																			\n\
}																							\n";

// direct3d management fucntions
void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix)
{
	MFMatrix mat = animationMatrix;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(r_animMats + boneID*3, (float*)&mat, 3);
}

void MFRendererPC_SetMatrix(int constantSlot, const MFMatrix &matrix)
{
	MFMatrix mat = matrix;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(constantSlot, (float*)&mat, 4);
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

int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
	MFCALLSTACK;

	pVS_s = MFShader_CreateFromString(MFST_VertexShader, gVertexShader, NULL, "MFMat_Standard_VS_static");

	MFShaderMacro macros[] = { { "_ANIMATION", "1" }, { NULL, NULL } };
	pVS_a = MFShader_CreateFromString(MFST_VertexShader, gVertexShader, macros, "MFMat_Standard_VS_animated");

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;

	MFShader_Release(pVS_s);
	MFShader_Release(pVS_a);
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
		pd3dDevice->SetVertexShaderConstantF(r_wvpm, (float*)&mat, 4);
	}

	if(state.pMatrixStatesSet[MFSCM_ViewProjection] != state.pMatrixStates[MFSCM_ViewProjection])
	{
		MFMatrix *pVP = state.getDerivedMatrix(MFSCM_ViewProjection);
		state.pMatrixStates[MFSCM_ViewProjection] = pVP;

		MFMatrix mat;
		mat.Transpose(*pVP);
		pd3dDevice->SetVertexShaderConstantF(r_vpm, (float*)&mat, 4);
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
		for(uint32 b=0; b<state.matrixBatch.numMatrices; b++)
			MFRendererPC_SetAnimationMatrix(b, state.animation.pMatrices[state.matrixBatch.pIndices[b]]);

		pd3dDevice->SetVertexShader((IDirect3DVertexShader9*)pVS_a->pPlatformData);
	}
	else
		pd3dDevice->SetVertexShader((IDirect3DVertexShader9*)pVS_s->pPlatformData);

	return 0;
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

#endif // MF_RENDERER
