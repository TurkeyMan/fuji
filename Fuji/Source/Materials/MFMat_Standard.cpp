#include "Fuji.h"
#include "MFSystem.h"
#include "MFIni.h"

#include "MFRenderState.h"
#include "MFMaterial_Internal.h"
#include "MFTexture_Internal.h"
#include "Materials/MFMat_Standard_Internal.h"

static MFEnumKey sBlendKeys[] =
{
	{ "none",			MFMatStandard_Blend_None },
	{ "alpha",			MFMatStandard_Blend_Alpha },
	{ "additive",		MFMatStandard_Blend_Additive },
	{ "subtractive",	MFMatStandard_Blend_Subtractive },
	{ NULL, 0 }
};

static MFEnumKey sCullKeys[] =
{
	{ "none",		MFMatStandard_Cull_None },
	{ "ccw",		MFMatStandard_Cull_CCW },
	{ "cw",			MFMatStandard_Cull_CW },
	{ "default",	MFMatStandard_Cull_Default },
	{ NULL, 0 }
};

static MFEnumKey sTextureKeys[] =
{
	{ "diffuse",	MFMatStandard_Tex_DifuseMap },
	{ "diffuse2",	MFMatStandard_Tex_DiffuseMap2 },
	{ "normal",		MFMatStandard_Tex_NormalMap },
	{ "detail",		MFMatStandard_Tex_DetailMap },
	{ "env",		MFMatStandard_Tex_EnvMap },
	{ "light",		MFMatStandard_Tex_LightMap },
	{ "bump",		MFMatStandard_Tex_BumpMap },
	{ "reflection",	MFMatStandard_Tex_ReflectionMap },
	{ "specular",	MFMatStandard_Tex_SpecularMap },
	{ NULL, 0 }
};

static MFEnumKey sTextureAddreessKeys[] =
{
	{ "addressu",		MFMatStandard_TexFlag_AddressU },
	{ "addressv",		MFMatStandard_TexFlag_AddressV },
	{ "addressw",		MFMatStandard_TexFlag_AddressW },
	{ NULL, 0 }
};

static MFEnumKey sTextureFilterKeys[] =
{
	{ "magfilter",		MFMatStandard_TexFlag_MagFilter },
	{ "minfilter",		MFMatStandard_TexFlag_MinFilter },
	{ "mipfilter",		MFMatStandard_TexFlag_MipFilter },
	{ NULL, 0 }
};

static MFEnumKey sTextureAddressValueKeys[] =
{
	{ "wrap",			MFMatStandard_TexAddress_Wrap },
	{ "mirror",			MFMatStandard_TexAddress_Mirror },
	{ "clamp",			MFMatStandard_TexAddress_Clamp },
	{ "border",			MFMatStandard_TexAddress_Border },
	{ "mirroronce",		MFMatStandard_TexAddress_MirrorOnce },
	{ NULL, 0 }
};

static MFEnumKey sTextureFilterValueKeys[] =
{
	{ "none",			MFMatStandard_TexFilter_None },
	{ "nearest",		MFMatStandard_TexFilter_Point },
	{ "linear",			MFMatStandard_TexFilter_Linear },
	{ "anisotropic",	MFMatStandard_TexFilter_Anisotropic },
	{ NULL, 0 }
};

static MFMaterialParameterInfo::ParameterDetails boolValue[] = { { MFParamType_Bool, 1 } };
static MFMaterialParameterInfo::ParameterDetails intValue[] = { { MFParamType_Int, 1 } };
static MFMaterialParameterInfo::ParameterDetails floatValue[] = { { MFParamType_Float } };
static MFMaterialParameterInfo::ParameterDetails colourValue[] = { { MFParamType_Colour } };
static MFMaterialParameterInfo::ParameterDetails stringValue[] = { { MFParamType_String } };
static MFMaterialParameterInfo::ParameterDetails matrixValue[] = { { MFParamType_Matrix } };

static MFMaterialParameterInfo::ParameterDetails blend[] = { { MFParamType_Enum, 1, sBlendKeys } };
static MFMaterialParameterInfo::ParameterDetails cull[] = { { MFParamType_Enum, 1, sCullKeys } };
static MFMaterialParameterInfo::ParameterDetails textureaddress[] = { { MFParamType_Enum, MFMatStandard_TexAddress_Wrap, sTextureAddressValueKeys } };
static MFMaterialParameterInfo::ParameterDetails texturefilter[] = { { MFParamType_Enum, MFMatStandard_TexFilter_Linear, sTextureFilterValueKeys } };
static MFMaterialParameterInfo::ParameterDetails animated[] = { { MFParamType_Int }, { MFParamType_Int }, { MFParamType_Float } };
static MFMaterialParameterInfo::ParameterDetails tile[] = { { MFParamType_Int }, { MFParamType_Int }, { MFParamType_Int }, { MFParamType_Int } };

static MFMaterialParameterInfo::ParameterDetails additive[] = { { MFParamType_Constant, MFMatStandard_Blend_Additive } };
static MFMaterialParameterInfo::ParameterDetails subtractive[] = { { MFParamType_Constant, MFMatStandard_Blend_Subtractive } };
static MFMaterialParameterInfo::ParameterDetails alpha[] = { { MFParamType_Constant, MFMatStandard_Blend_Alpha } };

MFMaterialParameterInfo parameterInformation[] =
{
	// name				paramIndex						argIndex hiword							argIndex													values...
	{ "lit",			MFMatStandard_Lit,				{ MFParamType_None },					{ MFParamType_None },										boolValue, 1 },
	{ "prelit",			MFMatStandard_Prelit,			{ MFParamType_None },					{ MFParamType_None },										boolValue, 1 },
	{ "diffusecolour",	MFMatStandard_DiffuseColour,	{ MFParamType_None },					{ MFParamType_None },										colourValue, 1 },
	{ "ambientcolour",	MFMatStandard_AmbientColour,	{ MFParamType_None },					{ MFParamType_None },										colourValue, 1 },
	{ "specularcolour",	MFMatStandard_SpecularColour,	{ MFParamType_None },					{ MFParamType_None },										colourValue, 1 },
	{ "specularpower",	MFMatStandard_SpecularPower,	{ MFParamType_None },					{ MFParamType_None },										floatValue, 1 },
	{ "emissivecolour",	MFMatStandard_EmissiveColour,	{ MFParamType_None },					{ MFParamType_None },										colourValue, 1 },
	{ "mask",			MFMatStandard_Mask,				{ MFParamType_None },					{ MFParamType_None },										boolValue, 1 },
	{ "alpharef",		MFMatStandard_AlphaRef,			{ MFParamType_None },					{ MFParamType_None },										floatValue, 1 },
	{ "cullmode",		MFMatStandard_CullMode,			{ MFParamType_None },					{ MFParamType_None },										cull, 1 },
	{ "zread",			MFMatStandard_ZRead,			{ MFParamType_None },					{ MFParamType_None },										boolValue, 1 },
	{ "zwrite",			MFMatStandard_ZWrite,			{ MFParamType_None },					{ MFParamType_None },										boolValue, 1 },
	{ "blend",			MFMatStandard_Blend,			{ MFParamType_None },					{ MFParamType_None },										blend, 1 },
	{ "celshading",		MFMatStandard_CelShading,		{ MFParamType_None },					{ MFParamType_None },										boolValue, 1 },
	{ "phong",			MFMatStandard_Phong,			{ MFParamType_None },					{ MFParamType_None },										boolValue, 1 },

	{ "effect",			MFMatStandard_Effect,			{ MFParamType_None },					{ MFParamType_None },										stringValue, 1 },

	{ "texture",		MFMatStandard_Texture,			{ MFParamType_None },					{ MFParamType_Enum, 0, sTextureKeys },						stringValue, 1 },
	{ "textureaddress",	MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_Enum, 0, sTextureAddreessKeys },				textureaddress, 1 },
	{ "texturefilter",	MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_Enum, 0, sTextureFilterKeys },				texturefilter, 1 },
	{ "bordercolour",	MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_None, MFMatStandard_TexFlag_BorderColour },	colourValue, 1 },
	{ "texturematrix",	MFMatStandard_TextureMatrix,	{ MFParamType_None },					{ MFParamType_None },										matrixValue, 1 },

	{ "animated",		MFMatStandard_Animated,			{ MFParamType_None },					{ MFParamType_None },										animated, sizeof(animated)/sizeof(MFMaterialParameterInfo::ParameterDetails) },
	{ "tile",			MFMatStandard_Tile,				{ MFParamType_None },					{ MFParamType_None },										tile, sizeof(tile)/sizeof(MFMaterialParameterInfo::ParameterDetails) },

	// handy macros
	{ "additive",		MFMatStandard_Blend,			{ MFParamType_None },					{ MFParamType_None },										additive, 1 },
	{ "subtractive",	MFMatStandard_Blend,			{ MFParamType_None },					{ MFParamType_None },										subtractive, 1 },
	{ "alpha",			MFMatStandard_Blend,			{ MFParamType_None },					{ MFParamType_None },										alpha, 1 },

	{ "addressu",		MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_Constant, MFMatStandard_TexFlag_AddressU },	textureaddress, 1 },
	{ "addressv",		MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_Constant, MFMatStandard_TexFlag_AddressV },	textureaddress, 1 },
	{ "minfilter",		MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_Constant, MFMatStandard_TexFlag_MinFilter },	texturefilter, 1 },
	{ "magfilter",		MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_Constant, MFMatStandard_TexFlag_MagFilter },	texturefilter, 1 },
	{ "mipfilter",		MFMatStandard_TextureFlags,		{ MFParamType_Enum, 0, sTextureKeys },	{ MFParamType_Constant, MFMatStandard_TexFlag_MipFilter },	texturefilter, 1 },

	{ "diffusemap",		MFMatStandard_Texture,			{ MFParamType_None },					{ MFParamType_Constant, MFMatStandard_Tex_DifuseMap },		stringValue, 1 },
	{ "diffusemap2",	MFMatStandard_Texture,			{ MFParamType_None },					{ MFParamType_Constant, MFMatStandard_Tex_DiffuseMap2 },	stringValue, 1 },
	{ "normalmap",		MFMatStandard_Texture,			{ MFParamType_None },					{ MFParamType_Constant, MFMatStandard_Tex_NormalMap },		stringValue, 1 },
	{ "detailmap",		MFMatStandard_Texture,			{ MFParamType_None },					{ MFParamType_Constant, MFMatStandard_Tex_DetailMap },		stringValue, 1 },
	{ "envmap",			MFMatStandard_Texture,			{ MFParamType_None },					{ MFParamType_Constant, MFMatStandard_Tex_EnvMap },			stringValue, 1 },
	{ "lightmap",		MFMatStandard_Texture,			{ MFParamType_None },					{ MFParamType_Constant, MFMatStandard_Tex_LightMap },		stringValue, 1 }
};

void MFMat_Standard_Register()
{
	MFMaterialCallbacks matStandardCallbacks;

	matStandardCallbacks.pRegisterMaterial = MFMat_Standard_RegisterMaterial;
	matStandardCallbacks.pUnregisterMaterial = MFMat_Standard_UnregisterMaterial;
	matStandardCallbacks.pCreateInstance = MFMat_Standard_CreateInstance;
	matStandardCallbacks.pDestroyInstance = MFMat_Standard_DestroyInstance;
	matStandardCallbacks.pUpdate = MFMat_Standard_Update;
	matStandardCallbacks.pBuildStateBlock = MFMat_Standard_BuildStateBlock;
	matStandardCallbacks.pBegin = MFMat_Standard_Begin;
	matStandardCallbacks.pSetParameter = MFMat_Standard_SetParameter;
	matStandardCallbacks.pGetParameter = MFMat_Standard_GetParameter;
	matStandardCallbacks.pGetNumParams = MFMat_Standard_GetNumParams;
	matStandardCallbacks.pGetParameterInfo = MFMat_Standard_GetParameterInfo;

	MFMaterial_RegisterMaterialType("Standard", &matStandardCallbacks, sizeof(MFMat_Standard_Data));
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend | 1<<6 /* back face culling */;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;

	pData->alphaRef = 1.0f;

//	pData->pEffect = MFEffect_Create("standard.mfx");

	MFMat_Standard_CreateInstancePlatformSpecific(pMaterial);
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFMat_Standard_DestroyInstancePlatformSpecific(pMaterial);

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	if(pData->pEffect)
		MFEffect_Release(pData->pEffect);

	for(uint32 a=0; a<pData->textureCount; a++)
		MFTexture_Release(pData->textures[a].pTexture);

	if(pData->pRasteriserState)
		MFRasteriserState_Release(pData->pRasteriserState);
	if(pData->pDepthStencilState)
		MFDepthStencilState_Release(pData->pDepthStencilState);
	if(pData->pBlendState)
		MFBlendState_Release(pData->pBlendState);
	for(int a=0; a<8; ++a)
	{
		if(pData->pSamplerState[a])
			MFSamplerState_Release(pData->pSamplerState[a]);
	}
}

void MFMat_Standard_Update(MFMaterial *pMaterial)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	if(pData->materialType & MF_Animating)
	{
		pData->curTime += MFSystem_TimeDelta();

		while(pData->curTime >= pData->frameTime)
		{
			pData->curTime -= pData->frameTime;

			pData->curFrame++;
			pData->curFrame = pData->curFrame % (pData->uFrames*pData->vFrames);

			pData->textureMatrix.SetTrans3(MakeVector(MFRcp((float)pData->uFrames) * (float)(pData->curFrame%pData->uFrames), MFRcp((float)pData->vFrames) * (float)(pData->curFrame/pData->vFrames), 0.0f));
		}
	}
}

void MFMat_Standard_BuildStateBlock(MFMaterial *pMaterial)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFStateBlock_Clear(pMaterial->pMaterialState);

	if(pData->pRasteriserState)
		MFRasteriserState_Release(pData->pRasteriserState);
	if(pData->pDepthStencilState)
		MFDepthStencilState_Release(pData->pDepthStencilState);
	if(pData->pBlendState)
		MFBlendState_Release(pData->pBlendState);
	for(int a=0; a<8; ++a)
	{
		if(pData->pSamplerState[a])
			MFSamplerState_Release(pData->pSamplerState[a]);
	}

	MFStateBlock_SetVector(pMaterial->pMaterialState, MFSCV_MaterialDiffuseColour, pData->diffuse);
	MFStateBlock_SetVector(pMaterial->pMaterialState, MFSCV_AmbientColour, pData->ambient);
	MFStateBlock_SetVector(pMaterial->pMaterialState, MFSCV_User0, MakeVector(1, 0, 1, 0));

	MFStateBlock_SetMatrix(pMaterial->pMaterialState, MFSCM_UV0, pData->textureMatrix);

	MFStateBlock_SetBool(pMaterial->pMaterialState, MFSCB_Opaque, pData->opaque != 0);

	bool bAlphaTest = !!(pData->materialType & MF_Mask);
	MFStateBlock_SetBool(pMaterial->pMaterialState, MFSCB_AlphaTest, bAlphaTest);
	if(bAlphaTest)
		MFStateBlock_SetVector(pMaterial->pMaterialState, MFSCV_RenderState, MakeVector(pData->alphaRef, 0, 0, 0));

	// rasteriser state
	MFRasteriserStateDesc rasteriserDesc;

	switch((pData->materialType & MF_CullMode) >> 6)
	{
		case 0: rasteriserDesc.cullMode = MFCullMode_None;	break;
		case 1: rasteriserDesc.cullMode = MFCullMode_CCW;	break;
		case 2: rasteriserDesc.cullMode = MFCullMode_CW;	break;
		case 3: rasteriserDesc.cullMode = MFCullMode_CCW;	break; // 'default' = CCW
	}

	pData->pRasteriserState = MFRasteriserState_Create(&rasteriserDesc);
	MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_RasteriserState, pData->pRasteriserState);

	// blend state
	MFBlendStateDesc blendDesc;
	blendDesc.bIndependentBlendEnable = false;

	bool bPremultipliedAlpha = !!(pData->textures[pData->diffuseMapIndex].pTexture->pTemplateData->flags & TEX_PreMultipliedAlpha);
	MFStateBlock_SetBool(pMaterial->pMaterialState, MFSCB_User0, bPremultipliedAlpha);

	switch(pData->materialType&MF_BlendMask)
	{
		case 0:
			blendDesc.renderTarget[0].bEnable = false;
			break;
		case MF_AlphaBlend:
			blendDesc.renderTarget[0].bEnable = true;
			blendDesc.renderTarget[0].blendOp = MFBlendOp_Add;
			blendDesc.renderTarget[0].srcBlend = bPremultipliedAlpha ? MFBlendArg_One : MFBlendArg_SrcAlpha;
			blendDesc.renderTarget[0].destBlend = MFBlendArg_InvSrcAlpha;
			break;
		case MF_Additive:
			blendDesc.renderTarget[0].bEnable = true;
			blendDesc.renderTarget[0].blendOp = MFBlendOp_Add;
			blendDesc.renderTarget[0].srcBlend = bPremultipliedAlpha ? MFBlendArg_One : MFBlendArg_SrcAlpha;
			blendDesc.renderTarget[0].destBlend = MFBlendArg_One;
			break;
		case MF_Subtractive:
			blendDesc.renderTarget[0].bEnable = true;
			blendDesc.renderTarget[0].blendOp = MFBlendOp_RevSubtract;
			blendDesc.renderTarget[0].srcBlend = bPremultipliedAlpha ? MFBlendArg_One : MFBlendArg_SrcAlpha;
			blendDesc.renderTarget[0].destBlend = MFBlendArg_One;
			break;
	}

	pData->pBlendState = MFBlendState_Create(&blendDesc);
	MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_BlendState, pData->pBlendState);

	// depth stencil state
	MFDepthStencilStateDesc depthStencilDesc;

	if(!(pData->materialType&MF_NoZRead) || !(pData->materialType&MF_NoZWrite))
	{
		depthStencilDesc.bDepthEnable = true;
		depthStencilDesc.depthFunc = (pData->materialType&MF_NoZRead) ? MFComparisonFunc_Always : MFComparisonFunc_LessEqual;
		depthStencilDesc.depthWriteMask = (pData->materialType&MF_NoZWrite) ? MFDepthWriteMask_Zero : MFDepthWriteMask_All;
	}
	else
		depthStencilDesc.bDepthEnable = false;

	pData->pDepthStencilState = MFDepthStencilState_Create(&depthStencilDesc);
	MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_DepthStencilState, pData->pDepthStencilState);

	// build samplers
	for(uint32 a=0; a<pData->textureCount; ++a)
	{
		if(!pData->textures[a].pTexture)
			continue;

		MFSamplerStateDesc desc;
		desc.minFilter = (MFTexFilter)pData->textures[a].minFilter;
		desc.magFilter = (MFTexFilter)pData->textures[a].magFilter;
		desc.mipFilter = (MFTexFilter)pData->textures[a].mipFilter;
		desc.addressU = (MFTexAddressMode)pData->textures[a].addressU;
		desc.addressV = (MFTexAddressMode)pData->textures[a].addressV;
		desc.addressW = (MFTexAddressMode)pData->textures[a].addressW;
		desc.borderColour.FromPackedColour(pData->textures[a].borderColour);

		pData->pSamplerState[a] = MFSamplerState_Create(&desc);
	}

	// add textures to stateblock
	MFDebug_Assert(pData->diffuseMapIndex == 0, "Diffuse map must be index 0!");
	if(pData->textures[0].pTexture)
	{
		MFStateBlock_SetTexture(pMaterial->pMaterialState, MFSCT_Diffuse, pData->textures[0].pTexture);
		MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_SamplerState(MFSCT_Diffuse), pData->pSamplerState[0]);
	}
	if(pData->normalMapIndex != 0 && pData->textures[pData->normalMapIndex].pTexture)
	{
		MFStateBlock_SetTexture(pMaterial->pMaterialState, MFSCT_NormalMap, pData->textures[pData->normalMapIndex].pTexture);
		MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_SamplerState(MFSCT_NormalMap), pData->pSamplerState[pData->normalMapIndex]);
	}
	if(pData->specularMapIndex != 0 && pData->textures[pData->specularMapIndex].pTexture)
	{
		MFStateBlock_SetTexture(pMaterial->pMaterialState, MFSCT_SpecularMap, pData->textures[pData->specularMapIndex].pTexture);
		MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_SamplerState(MFSCT_SpecularMap), pData->pSamplerState[pData->specularMapIndex]);
	}
	if(pData->detailMapIndex != 0 && pData->textures[pData->detailMapIndex].pTexture)
	{
		MFStateBlock_SetTexture(pMaterial->pMaterialState, MFSCT_DetailMap, pData->textures[pData->detailMapIndex].pTexture);
		MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_SamplerState(MFSCT_DetailMap), pData->pSamplerState[pData->detailMapIndex]);
	}
	if(pData->envMapIndex != 0 && pData->textures[pData->envMapIndex].pTexture)
	{
		MFStateBlock_SetTexture(pMaterial->pMaterialState, MFSCT_EnvironmentMap, pData->textures[pData->envMapIndex].pTexture);
		MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_SamplerState(MFSCT_EnvironmentMap), pData->pSamplerState[pData->envMapIndex]);
	}
	if(pData->lightMapIndex != 0 && pData->textures[pData->lightMapIndex].pTexture)
	{
		MFStateBlock_SetTexture(pMaterial->pMaterialState, MFSCT_LightMap, pData->textures[pData->lightMapIndex].pTexture);
		MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_SamplerState(MFSCT_LightMap), pData->pSamplerState[pData->lightMapIndex]);
	}
	if(pData->displacementMapIndex != 0 && pData->textures[pData->displacementMapIndex].pTexture)
	{
		MFStateBlock_SetTexture(pMaterial->pMaterialState, MFSCT_Vertex0, pData->textures[pData->displacementMapIndex].pTexture);
		MFStateBlock_SetRenderState(pMaterial->pMaterialState, MFSCRS_SamplerState(MFSCT_Vertex0), pData->pSamplerState[pData->displacementMapIndex]);
	}
}

void MFMat_Standard_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, uintp value)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	switch(parameterIndex)
	{
		case MFMatStandard_Lit:
			pData->materialType = (pData->materialType & ~MF_Lit) | (value ? MF_Lit : 0);
			break;
		case MFMatStandard_Prelit:
			pData->materialType = (pData->materialType & ~MF_Lit) | (!value ? MF_Lit : 0);
			break;
		case MFMatStandard_DiffuseColour:
			pData->diffuse = *(MFVector*)value;
			pData->opaque = pData->diffuse.w >= 1.f && pData->textures[pData->diffuseMapIndex].pTexture &&
				(pData->textures[pData->diffuseMapIndex].pTexture->pTemplateData->flags & TEX_AlphaMask) == 0;
			break;
		case MFMatStandard_AmbientColour:
			pData->ambient = *(MFVector*)value;
			break;
		case MFMatStandard_SpecularColour:
			pData->specular = *(MFVector*)value;
			break;
		case MFMatStandard_SpecularPower:
			pData->specularPow = *(float*)value;
			break;
		case MFMatStandard_EmissiveColour:
			pData->illum = *(MFVector*)value;
			break;
		case MFMatStandard_Mask:
			pData->materialType = (pData->materialType & ~MF_Mask) | (value ? MF_Mask : 0);
			break;
		case MFMatStandard_AlphaRef:
			pData->alphaRef = *(float*)value;
			break;
		case MFMatStandard_CullMode:
			pData->materialType = (pData->materialType & ~MF_CullMode) | (((uint32)value & 0x3) << 6);
			break;
		case MFMatStandard_ZRead:
			pData->materialType = (pData->materialType & ~MF_NoZRead) | (value ? 0 : MF_NoZRead);
			break;
		case MFMatStandard_ZWrite:
			pData->materialType = (pData->materialType & ~MF_NoZWrite) | (value ? 0 : MF_NoZWrite);
			break;
		case MFMatStandard_Blend:
			pData->materialType = (pData->materialType & ~MF_BlendMask) | ((int)(value) << 1);
			break;
		case MFMatStandard_Effect:
			if(pData->pEffect)
				MFEffect_Release(pData->pEffect);
			pData->pEffect = MFEffect_Create((const char *)value);
			break;
		case MFMatStandard_Texture:
			pData->textures[pData->textureCount].mipFilter = MFMatStandard_TexFilter_Linear;
			pData->textures[pData->textureCount].minFilter = MFMatStandard_TexFilter_Linear;
			pData->textures[pData->textureCount].magFilter = MFMatStandard_TexFilter_Linear;
			switch(argIndex)
			{
				case MFMatStandard_Tex_DifuseMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->diffuseMapIndex = pData->textureCount;
					pData->textureCount++;

					pData->opaque = pData->diffuse.w >= 1.f && pData->textures[pData->diffuseMapIndex].pTexture &&
						(pData->textures[pData->diffuseMapIndex].pTexture->pTemplateData->flags & TEX_AlphaMask) == 0;
					break;
				case MFMatStandard_Tex_DiffuseMap2:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->diffuseMap2Index = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_NormalMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->normalMapIndex = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_DetailMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->detailMapIndex = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_EnvMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->envMapIndex = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_LightMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->lightMapIndex = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_BumpMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->bumpMapIndex = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_ReflectionMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->reflectionMapIndex = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_SpecularMap:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->specularMapIndex = pData->textureCount;
					pData->textureCount++;
					break;
				case MFMatStandard_Tex_Texture:
					pData->textures[pData->textureCount].pTexture = MFTexture_Create((const char *)value);
					pData->textureCount++;
					break;
			}
			break;
		case MFMatStandard_TextureFlags:
			switch(argIndex & 0xFFFF)
			{
				case MFMatStandard_TexFlag_AddressU:
					pData->textures[argIndex >> 16].addressU = value;
					break;
				case MFMatStandard_TexFlag_AddressV:
					pData->textures[argIndex >> 16].addressV = value;
					break;
				case MFMatStandard_TexFlag_AddressW:
					pData->textures[argIndex >> 16].addressW = value;
					break;
				case MFMatStandard_TexFlag_MagFilter:
					pData->textures[argIndex >> 16].magFilter = value;
					break;
				case MFMatStandard_TexFlag_MinFilter:
					pData->textures[argIndex >> 16].minFilter = value;
					break;
				case MFMatStandard_TexFlag_MipFilter:
					pData->textures[argIndex >> 16].mipFilter = value;
					break;
				case MFMatStandard_TexFlag_BorderColour:
					pData->textures[argIndex >> 16].borderColour = ((MFVector&)value).ToPackedColour();
					break;
			}
			break;
		case MFMatStandard_CelShading:
			pData->materialType |= MF_CelShading;
			break;
		case MFMatStandard_Phong:
			pData->materialType |= MF_LitPerPixel;
			break;
		case MFMatStandard_Animated:
		{
			MFMat_Standard_AnimParams *pAnim = (MFMat_Standard_AnimParams*)value;

			pData->materialType |= MF_Animating;

			pData->uFrames = pAnim->hFrames;
			pData->vFrames = pAnim->vFrames;
			pData->frameTime = pAnim->frameTime;

			pData->textureMatrix.SetScale(MakeVector(MFRcp((float)pAnim->hFrames), MFRcp((float)pAnim->vFrames), 1.0f));
			break;
		}
		case MFMatStandard_Tile:
		{
			MFMat_Standard_TileParams *pTile = (MFMat_Standard_TileParams*)value;

			pData->uFrames = pTile->hFrames;
			pData->vFrames = pTile->vFrames;
			pData->curFrame = pTile->vFrame*pTile->hFrames + pTile->hFrame;

			float hScale = MFRcp((float)pTile->hFrames);
			float vScale = MFRcp((float)pTile->vFrames);
			pData->textureMatrix.SetScale(MakeVector(hScale, vScale, 1.0f));
			pData->textureMatrix.SetTrans3(MakeVector(hScale * (float)pTile->hFrame, vScale * (float)pTile->vFrame, 0.0f));
			break;
		}
	}
}

uintp MFMat_Standard_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	switch(parameterIndex)
	{
		case MFMatStandard_Lit:
			return (pData->materialType & MF_Lit) ? 1 : 0;
		case MFMatStandard_Prelit:
			return (pData->materialType & MF_Lit) ? 0 : 1;
		case MFMatStandard_DiffuseColour:
			*(MFVector*)pValue = pData->diffuse;
			break;
		case MFMatStandard_AmbientColour:
			*(MFVector*)pValue = pData->ambient;
			break;
		case MFMatStandard_SpecularColour:
			*(MFVector*)pValue = pData->specular;
			break;
		case MFMatStandard_SpecularPower:
			*(float*)pValue = pData->specularPow;
			break;
		case MFMatStandard_EmissiveColour:
			*(MFVector*)pValue = pData->illum;
			break;
		case MFMatStandard_Mask:
			return *(uintp*)pValue = (pData->materialType & MF_Mask) ? 1 : 0;
		case MFMatStandard_CullMode:
			return (pData->materialType & MF_CullMode) >> 6;
		case MFMatStandard_ZRead:
			return (pData->materialType & MF_NoZRead) ? 0 : 1;
		case MFMatStandard_ZWrite:
			return (pData->materialType & MF_NoZWrite) ? 0 : 1;
		case MFMatStandard_Effect:
			return (uintp)pData->pEffect;
		case MFMatStandard_Texture:
			switch(argIndex)
			{
				case MFMatStandard_Tex_DifuseMap:
					return (uintp)pData->textures[pData->diffuseMapIndex].pTexture;
				case MFMatStandard_Tex_DiffuseMap2:
					return (uintp)pData->textures[pData->diffuseMap2Index].pTexture;
				case MFMatStandard_Tex_NormalMap:
					return (uintp)pData->textures[pData->normalMapIndex].pTexture;
				case MFMatStandard_Tex_DetailMap:
					return (uintp)pData->textures[pData->detailMapIndex].pTexture;
				case MFMatStandard_Tex_EnvMap:
					return (uintp)pData->textures[pData->envMapIndex].pTexture;
				case MFMatStandard_Tex_LightMap:
					return (uintp)pData->textures[pData->lightMapIndex].pTexture;
				case MFMatStandard_Tex_BumpMap:
					return (uintp)pData->textures[pData->bumpMapIndex].pTexture;
				case MFMatStandard_Tex_ReflectionMap:
					return (uintp)pData->textures[pData->reflectionMapIndex].pTexture;
				case MFMatStandard_Tex_SpecularMap:
					return (uintp)pData->textures[pData->specularMapIndex].pTexture;
				case MFMatStandard_Tex_Texture:
					return (uintp)pData->textures[(uintp)pValue].pTexture;
			}
			break;
		case MFMatStandard_TextureFlags:
			switch(argIndex & 0xFFFF)
			{
				case MFMatStandard_TexFlag_AddressU:
					return pData->textures[argIndex >> 16].addressU;
				case MFMatStandard_TexFlag_AddressV:
					return pData->textures[argIndex >> 16].addressV;
				case MFMatStandard_TexFlag_AddressW:
					return pData->textures[argIndex >> 16].addressW;
				case MFMatStandard_TexFlag_MagFilter:
					return pData->textures[argIndex >> 16].magFilter;
				case MFMatStandard_TexFlag_MinFilter:
					return pData->textures[argIndex >> 16].minFilter;
				case MFMatStandard_TexFlag_MipFilter:
					return pData->textures[argIndex >> 16].mipFilter;
				case MFMatStandard_TexFlag_BorderColour:
					return pData->textures[argIndex >> 16].borderColour;
			}
			break;
		case MFMatStandard_TextureMatrix:
			*(MFMatrix*)pValue = pData->textureMatrix;
			break;
		default:
			MFDebug_Assert(false, "Not written!!!");
			break;
	}

	return 0;
}

int MFMat_Standard_GetNumParams()
{
	return sizeof(parameterInformation)/sizeof(MFMaterialParameterInfo);
}

MFMaterialParameterInfo* MFMat_Standard_GetParameterInfo(int parameterIndex)
{
	MFDebug_Assert((uint32)parameterIndex < sizeof(parameterInformation)/sizeof(MFMaterialParameterInfo), MFStr("Invalid parameter id %d.", parameterIndex));

	return &parameterInformation[parameterIndex];
}

