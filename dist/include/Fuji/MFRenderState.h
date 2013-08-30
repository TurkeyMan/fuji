/**
 * @file MFRenderState.h
 * @brief API's to manage renderer state.
 * @author Manu Evans
 * @defgroup MFRenderState Rendering State Management
 * @{
 */

#pragma once
#if !defined(_MFRENDERSTATE_H)
#define _MFRENDERSTATE_H

#include "MFMatrix.h"
#include <float.h>

struct MFTexture;

enum MFStateConstant_Matrix
{
	MFSCM_Unknown = -1,

	MFSCM_World = 0,
	MFSCM_Camera,
	MFSCM_Projection,
	MFSCM_ShadowMap,
	MFSCM_Fuji0,
	MFSCM_Fuji1,
	MFSCM_User0,
	MFSCM_User1,
	MFSCM_User2,
	MFSCM_User3,
	MFSCM_UV0,
	MFSCM_UV1,
	MFSCM_UV2,
	MFSCM_UV3,
	MFSCM_View,
	MFSCM_WorldView,
	MFSCM_ViewProjection,
	MFSCM_WorldViewProjection,
	MFSCM_InverseWorld,
	MFSCM_InverseViewProjection,

	MFSCM_Max,

	MFSCM_ManagedStart = MFSCM_World,
	MFSCM_NumManaged = MFSCM_ShadowMap - MFSCM_ManagedStart,

	MFSCM_DerivedStart = MFSCM_View,
	MFSCM_NumDerived = MFSCM_Max - MFSCM_DerivedStart,

	MFSCM_NumSetable = MFSCM_DerivedStart,

	MFSCM_FujiMatrixCount = MFSCM_User0 - MFSCM_Fuji0,
	MFSCM_UserMatrixCount = MFSCM_UV0 - MFSCM_User0,
	MFSCM_UVMatrixCount = MFSCM_View - MFSCM_UV0,

	MFSCM_ForceInt = 0x7FFFFFFF
};

#define MFSCM_FujiMat(i) (MFStateConstant_Matrix)(MFSCM_FujiMat0 + i)
#define MFSCM_UserMat(i) (MFStateConstant_Matrix)(MFSCM_UserMat0 + i)
#define MFSCM_UV(i) (MFStateConstant_Matrix)(MFSCM_UV0 + i)

enum MFStateConstant_Vector
{
	MFSCV_Unknown = -1,

	MFSCV_Time = 0,				/**< x = game time in seconds, y = tile delta, z = 1 / x, w = 1 / y */
	MFSCV_FogColour,
	MFSCV_FogParams1,
	MFSCV_FogParams2,
	MFSCV_RenderState,			/**< x = alpha ref, ...? */
	MFSCV_MaterialDiffuseColour,
	MFSCV_DiffuseColour,
	MFSCV_AmbientColour,

	MFSCV_Fuji0, MFSCV_Fuji1, MFSCV_Fuji2, MFSCV_Fuji3, MFSCV_Fuji4, MFSCV_Fuji5, MFSCV_Fuji6,

	MFSCV_LightCounts,			/**< x = directional, y = omni count, z = spot count */

	MFSCV_User0, MFSCV_User1, MFSCV_User2, MFSCV_User3, MFSCV_User4, MFSCV_User5, MFSCV_User6, MFSCV_User7,
	MFSCV_User8, MFSCV_User9, MFSCV_User10, MFSCV_User11, MFSCV_User12, MFSCV_User13, MFSCV_User14, MFSCV_User15,

	MFSCV_Max,

	MFSCV_FujiVectorCount = MFSCV_LightCounts - MFSCV_Fuji0,
	MFSCV_UserectorCount = MFSCV_Max - MFSCV_User0,

	MFSCV_ForceInt = 0x7FFFFFFF
};

#define MFSCV_Fuji(i) (MFStateConstant_Vector)(MFSCV_Fuji0 + i)
#define MFSCV_User(i) (MFStateConstant_Vector)(MFSCV_User0 + i)

enum MFStateConstant_Texture
{
	MFSCT_Unknown = -1,

	MFSCT_Diffuse = 0,
	MFSCT_NormalMap,
	MFSCT_SpecularMap,
	MFSCT_DetailMap,
	MFSCT_OpacityMap,
	MFSCT_EnvironmentMap,
	MFSCT_SpecularPowerMap,
	MFSCT_EmissiveMap,
	MFSCT_LightMap,
	MFSCT_ShadowBuffer,
	MFSCT_Projector,
	MFSCT_User0,
	MFSCT_User1,
	MFSCT_User2,
	MFSCT_User3,
	MFSCT_User4,
	MFSCT_Vertex0,
	MFSCT_Vertex1,
	MFSCT_Vertex2,

	MFSCT_Max,

	MFSCV_FujiTextureCount = 0,
	MFSCV_UserTextureCount = MFSCT_Vertex0 - MFSCT_User0,
	MFSCV_VertexTextureCount = MFSCT_Max - MFSCT_Vertex0,

	MFSCT_ForceInt = 0x7FFFFFFF
};

#define MFSCT_Fuji(i) (MFStateConstant_Texture)(MFSCT_Max + i)
#define MFSCT_User(i) (MFStateConstant_Texture)(MFSCT_User0 + i)
#define MFSCT_Vertex(i) (MFStateConstant_Texture)(MFSCT_Vertex0 + i)

enum MFStateConstant_Bool
{
	MFSCB_Unknown = -1,

	MFSCB_Animated = 0,
	MFSCB_ZPrime,
	MFSCB_ShadowGeneration,
	MFSCB_ShadowReceiving,
	MFSCB_Opaque,
	MFSCB_AlphaTest,
	MFSCB_Fuji0,
	MFSCB_Fuji1,
	MFSCB_Fuji2,
	MFSCB_User0,
	MFSCB_User1,
	MFSCB_User2,
	MFSCB_User3,

	MFSCB_DiffuseSet,
	MFSCB_NormalMapSet,
	MFSCB_SpecularMapSet,
	MFSCB_DetailMapSet,
	MFSCB_OpacityMapSet,
	MFSCB_EnvironmentMapSet,
	MFSCB_SpecularPowerMapSet,
	MFSCB_EmissiveMapSet,
	MFSCB_LightMapSet,
	MFSCB_ShadowBufferSet,
	MFSCB_ProjectorSet,
	MFSCB_UserTex0Set,
	MFSCB_UserTex1Set,
	MFSCB_UserTex2Set,
	MFSCB_UserTex3Set,
	MFSCB_UserTex4Set,
	MFSCB_VertexTex0Set,
	MFSCB_VertexTex1Set,
	MFSCB_VertexTex2Set,

	MFSCB_Max,

	MFSCB_FujiBoolCount = MFSCB_User0 - MFSCB_Fuji0,
	MFSCB_UserBoolCount = MFSCB_DiffuseSet - MFSCB_User0,

	MFSCB_ForceInt = 0x7FFFFFFF
};

#define MFSCB_Fuji(i) (MFStateConstant_Bool)(MFSCB_Fuji0 + i)
#define MFSCB_User(i) (MFStateConstant_Bool)(MFSCB_User0 + i)
#define MFSCB_TexSet(i) (MFStateConstant_Bool)(MFSCB_DiffuseSet + i)

enum MFStateConstant_RenderState
{
	MFSCRS_Unknown= -1,

	MFSCRS_BlendState = 0,
	MFSCRS_DepthStencilState,
	MFSCRS_RasteriserState,

	MFSCRS_DiffuseSamplerState,
	MFSCRS_NormalMapSamplerState,
	MFSCRS_SpecularMapSamplerState,
	MFSCRS_DetailMapSamplerState,
	MFSCRS_OpacityMapSamplerState,
	MFSCRS_EnvironmentMapSamplerState,
	MFSCRS_SpecularPowerMapSamplerState,
	MFSCRS_EmissiveMapSamplerState,
	MFSCRS_LightMapSamplerState,
	MFSCRS_ShadowBufferSamplerState,
	MFSCRS_ProjectorSamplerState,
	MFSCRS_UserTex0SamplerState,
	MFSCRS_UserTex1SamplerState,
	MFSCRS_UserTex2SamplerState,
	MFSCRS_UserTex3SamplerState,
	MFSCRS_UserTex4SamplerState,
	MFSCRS_VertexTex0SamplerState,
	MFSCRS_VertexTex1SamplerState,
	MFSCRS_VertexTex2SamplerState,

	MFSCRS_VertexDeclaration,
	MFSCRS_VertexBuffer0,
	MFSCRS_VertexBuffer1,
	MFSCRS_VertexBuffer2,
	MFSCRS_VertexBuffer3,
	MFSCRS_VertexBuffer4,
	MFSCRS_VertexBuffer5,
	MFSCRS_VertexBuffer6,
	MFSCRS_VertexBuffer7,
	MFSCRS_IndexBuffer,

	MFSCRS_Max,

	MFSCRS_VertexBufferCount = MFSCRS_VertexBuffer0 - MFSCRS_IndexBuffer,

	MFSCRS_ForceInt = 0x7FFFFFFF
};

#define MFSCRS_SamplerState(tex) (MFStateConstant_RenderState)(MFSCRS_DiffuseSamplerState + tex)
#define MFSCRS_VertexBuffer(i) (MFStateConstant_RenderState)(MFSCRS_VertexBuffer0 + i)

enum MFStateConstant_Miscellaneous
{
	MFSCMisc_AnimationMatrices,
	MFSCMisc_MatrixBatch,
	MFSCMisc_Light0,
	MFSCMisc_Light1,
	MFSCMisc_Light2,
	MFSCMisc_Light3,
	MFSCMisc_Light4,
	MFSCMisc_Light5,
	MFSCMisc_Light6,
	MFSCMisc_Light7,
	MFSCMisc_Light8,
	MFSCMisc_Light9,
	MFSCMisc_Light10,
	MFSCMisc_Light11,
	MFSCMisc_Light12,
	MFSCMisc_Light13,
	MFSCMisc_Light14,
	MFSCMisc_Light15,
	MFSCMisc_Light16,
	MFSCMisc_GPUEvent,

	MFSCMisc_Max,

	MFSCMisc_LightCount = MFSCMisc_GPUEvent - MFSCMisc_Light0,

	MFSCMisc_ForceInt = 0x7FFFFFFF
};

#define MFSCMisc_Light(i) (MFStateConstant_Miscellaneous)(MFSCMisc_Light0 + i)

struct MFStateConstant_AnimationMatrices
{
	MFMatrix *pMatrices;
	size_t numMatrices;
};

struct MFStateConstant_MatrixBatch
{
	uint16 *pIndices;
	uint32 numMatrices;
};

enum MFBlendOp
{
	MFBlendOp_Unknown = -1,

	MFBlendOp_Add = 0,
	MFBlendOp_Subtract,
	MFBlendOp_RevSubtract,
	MFBlendOp_Min,
	MFBlendOp_Max,

	MFBlendOp_BlendOpCount,
	MFBlendOp_ForceInt = 0x7FFFFFFF
};

enum MFBlendArg
{
	MFBlendArg_Unknown = -1,

	MFBlendArg_Zero = 0,
	MFBlendArg_One,
	MFBlendArg_SrcColour,
	MFBlendArg_InvSrcColour,
	MFBlendArg_SrcAlpha,
	MFBlendArg_InvSrcAlpha,
	MFBlendArg_DestColour,
	MFBlendArg_InvDestColour,
	MFBlendArg_DestAlpha,
	MFBlendArg_InvDestAlpha,
	MFBlendArg_SrcAlphaSaturate,

	MFBlendArg_BlendFactor,
	MFBlendArg_InvBlendFactor,
	MFBlendArg_Src1Colour,
	MFBlendArg_InvSrc1Colour,
	MFBlendArg_Src1Alpha,
	MFBlendArg_InvSrc1Alpha,

	MFBlendArg_Max,
	MFBlendArg_ForceInt = 0x7FFFFFFF
};

enum MFColourWriteEnable
{
	MFColourWriteEnable_None = 0,

	MFColourWriteEnable_Red = 1,
	MFColourWriteEnable_Green = 2,
	MFColourWriteEnable_Blue = 4,
	MFColourWriteEnable_Alpha = 8,

	MFColourWriteEnable_All = (MFColourWriteEnable_Red | MFColourWriteEnable_Green | MFColourWriteEnable_Blue | MFColourWriteEnable_Alpha),
};

enum MFTexFilter
{
	MFTexFilter_Unknown = -1,

	MFTexFilter_None,
	MFTexFilter_Point,
	MFTexFilter_Linear,
	MFTexFilter_Anisotropic,
//	MFTexFilter_Text_1Bit,

	MFTexFilter_Max,
	MFTexFilter_ForceInt = 0x7FFFFFFF
};

enum MFTexAddressMode
{
	MFTexAddressMode_Unknown = -1,

	MFTexAddressMode_Wrap = 0,
	MFTexAddressMode_Mirror,
	MFTexAddressMode_Clamp,
	MFTexAddressMode_Border,
	MFTexAddressMode_MirrorOnce,

	MFTexAddressMode_Max,
	MFTexAddressMode_ForceInt = 0x7FFFFFFF
};

enum MFComparisonFunc
{
	MFComparisonFunc_Unknown = -1,

	MFComparisonFunc_Never = 0,
	MFComparisonFunc_Less,
	MFComparisonFunc_Equal,
	MFComparisonFunc_LessEqual,
	MFComparisonFunc_Greater,
	MFComparisonFunc_NotEqual,
	MFComparisonFunc_GreaterEqual,
	MFComparisonFunc_Always,

	MFComparisonFunc_Max,
	MFComparisonFunc_ForceInt = 0x7FFFFFFF
};

enum MFDepthWriteMask
{
	MFDepthWriteMask_Unknown = -1,

	MFDepthWriteMask_Zero = 0,
	MFDepthWriteMask_All = 1,

	MFDepthWriteMask_Max,
	MFDepthWriteMask_ForceInt = 0x7FFFFFFF
};

enum MFStencilOp
{
	MFStencilOp_Unknown = -1,

	MFStencilOp_Keep = 0,
	MFStencilOp_Zero,
	MFStencilOp_Replace,
	MFStencilOp_IncrementClamp,
	MFStencilOp_DecrementClamp,
	MFStencilOp_Invert,
	MFStencilOp_Increment,
	MFStencilOp_Decrement,

	MFStencilOp_Max,
	MFStencilOp_ForceInt = 0x7FFFFFFF
};

enum MFFillMode
{
	MFFillMode_Unknown = -1,

	MFFillMode_Solid = 0,
	MFFillMode_Wireframe,
	MFFillMode_Point,

	MFFillMode_Max,
	MFFillMode_ForceInt = 0x7FFFFFFF
};

enum MFCullMode
{
	MFCullMode_Unknown = -1,

	MFCullMode_None = 0,
	MFCullMode_CCW,
	MFCullMode_CW,

	MFCullMode_Max,
	MFCullMode_ForceInt = 0x7FFFFFFF
};

enum MFStateBlockConstantType
{
	MFSB_CT_Bool = 0,
	MFSB_CT_Vector,
	MFSB_CT_Matrix,
	MFSB_CT_Texture,
	MFSB_CT_RenderState,
	MFSB_CT_Misc,

	MFSB_CT_Unknown = 7,

	MFSB_CT_TypeCount,
	MFSB_CT_ForceInt = 0x7FFFFFFF
};

struct MFBlendState;

struct MFBlendStateDesc
{
	struct RenderTargetBlendDesc
	{
		MFBlendOp blendOp;					/**< This blend operation defines how to combine the RGB data sources */
		MFBlendArg srcBlend;				/**< This blend option specifies the first RGB data source and includes an optional pre-blend operation */
		MFBlendArg destBlend;				/**< This blend option specifies the second RGB data source and includes an optional pre-blend operation */
		MFBlendOp blendOpAlpha;				/**< This blend operation defines how to combine the alpha data sources */
		MFBlendArg srcBlendAlpha;			/**< This blend option specifies the first alpha data source and includes an optional pre-blend operation. Blend options that end in _COLOR are not allowed */
		MFBlendArg destBlendAlpha;			/**< This blend option specifies the second alpha data source and includes an optional pre-blend operation. Blend options that end in _COLOR are not allowed */
		uint8 writeMask;					/**< A write mask */
		bool bEnable;						/**< Enable (or disable) blending */

		RenderTargetBlendDesc()
		{
			blendOp = MFBlendOp_Add;
			srcBlend = MFBlendArg_SrcAlpha;
			destBlend = MFBlendArg_InvSrcAlpha;
			blendOpAlpha = MFBlendOp_Add;
			srcBlendAlpha = MFBlendArg_One;
			destBlendAlpha = MFBlendArg_Zero;
			writeMask = MFColourWriteEnable_All;
			bEnable = false;
		}
	};

	bool bAlphaToCoverageEnable;			/**< Use alpha-to-coverage as a multisampling technique when setting a pixel to a rendertarget. */
	bool bIndependentBlendEnable;			/**< Enables independent blending to simultaneous render targets. If false, only the renderTarget[0] members are used. RenderTarget[1..7] are ignored. */
	RenderTargetBlendDesc renderTarget[8];	/**< Array of RenderTargetBlendDesc; corresponding to the eight rendertargets that can be set to the output-merger stage at one time. */

	MFBlendStateDesc()
	{
		bAlphaToCoverageEnable = false;
		bIndependentBlendEnable = false;
	}
};

struct MFSamplerState;

struct MFSamplerStateDesc
{
	MFTexFilter minFilter;
	MFTexFilter magFilter;
	MFTexFilter mipFilter;
	MFTexAddressMode addressU;
	MFTexAddressMode addressV;
	MFTexAddressMode addressW;
	bool bEnableComparison;
	MFComparisonFunc comparisonFunc;
	uint32 maxAnisotropy;
	float mipLODBias;
	float minLOD;
	float maxLOD;
	MFVector borderColour;

	MFSamplerStateDesc()
	{
		minFilter = MFTexFilter_Linear;
		magFilter = MFTexFilter_Linear;
		mipFilter = MFTexFilter_Linear;
		addressU = MFTexAddressMode_Wrap;
		addressV = MFTexAddressMode_Wrap;
		addressW = MFTexAddressMode_Wrap;
		bEnableComparison = false;
		comparisonFunc = MFComparisonFunc_Never;
		maxAnisotropy = 16;
		mipLODBias = 0.0f;
		minLOD = -FLT_MAX;
		maxLOD = FLT_MAX;
		borderColour = MFVector::zero;
	}
};

struct MFDepthStencilState;

struct MFDepthStencilStateDesc
{
	struct StencilOpDesc
	{
		MFStencilOp stencilFailOp;
		MFStencilOp stencilDepthFailOp;
		MFStencilOp stencilPassOp;
		MFComparisonFunc stencilFunc;

		StencilOpDesc()
		{
			stencilFailOp = MFStencilOp_Keep;
			stencilDepthFailOp = MFStencilOp_Keep;
			stencilPassOp = MFStencilOp_Keep;
			stencilFunc = MFComparisonFunc_Always;
		}
	};

	bool bDepthEnable;
	MFComparisonFunc depthFunc;
	MFDepthWriteMask depthWriteMask;
	bool bStencilEnable;
	uint8 stencilReadMask;
	uint8 stencilWriteMask;
	StencilOpDesc frontFace;
	StencilOpDesc backFace;

	MFDepthStencilStateDesc()
	{
		bDepthEnable = true;
		depthFunc = MFComparisonFunc_Less;
		depthWriteMask = MFDepthWriteMask_All;
		bStencilEnable = false;
		stencilReadMask = 0xFF;
		stencilWriteMask = 0xFF;
		backFace.stencilFunc = MFComparisonFunc_Never;
	}
};

struct MFRasteriserState;

struct MFRasteriserStateDesc
{
	MFFillMode fillMode;
	MFCullMode cullMode;
	int depthBias;
	float depthBiasClamp;
	float slopeScaledDepthBias;
	bool bDepthClipEnable;
	bool bScissorEnable;
	bool bMultisampleEnable;
	bool bAntialiasedLineEnable;

	// Set Fuji defaults
	MFRasteriserStateDesc()
	{
		fillMode = MFFillMode_Solid;
		cullMode = MFCullMode_CCW;
		depthBias = 0;
		depthBiasClamp = 0.f;
		slopeScaledDepthBias = 0.f;
		bDepthClipEnable = true;
		bScissorEnable = false;
		bMultisampleEnable = false;
		bAntialiasedLineEnable = false;
	}
};

struct MFStateBlock;

MF_API MFBlendState* MFBlendState_Create(MFBlendStateDesc *pDesc);
MF_API int MFBlendState_Release(MFBlendState *pBlendState);

MF_API MFSamplerState* MFSamplerState_Create(MFSamplerStateDesc *pDesc);
MF_API int MFSamplerState_Release(MFSamplerState *pSamplerState);

MF_API MFDepthStencilState* MFDepthStencilState_Create(MFDepthStencilStateDesc *pDesc);
MF_API int MFDepthStencilState_Release(MFDepthStencilState *pDepthStencilState);

MF_API MFRasteriserState* MFRasteriserState_Create(MFRasteriserStateDesc *pDesc);
MF_API int MFRasteriserState_Release(MFRasteriserState *pRasteriserState);

MF_API MFStateBlock* MFStateBlock_Create(uint32 size);
MF_API MFStateBlock* MFStateBlock_CreateTemporary(uint32 size);
MF_API void MFStateBlock_Destroy(MFStateBlock *pStateBlock);
MF_API MFStateBlock* MFStateBlock_Clone(MFStateBlock *pSource);
MF_API void MFStateBlock_Copy(MFStateBlock *pSource, MFStateBlock *pDest);
MF_API MFStateBlock* MFStateBlock_Merge(MFStateBlock *pSource1, MFStateBlock *pSource2);
MF_API void MFStateBlock_Clear(MFStateBlock *pStateBlock);

MF_API size_t MFStateBlock_GetAllocatedBytes(MFStateBlock *pStateBlock);
MF_API size_t MFStateBlock_GetUsedBytes(MFStateBlock *pStateBlock);
MF_API size_t MFStateBlock_GetFreeBytes(MFStateBlock *pStateBlock);

MF_API const char* MFStateBlock_GetStateName(MFStateBlockConstantType ct, int constant);

MF_API bool MFStateBlock_SetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool state);
MF_API bool MFStateBlock_SetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, const MFVector &state);
MF_API bool MFStateBlock_SetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, const MFMatrix &state);
MF_API bool MFStateBlock_SetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture *pTexture);
MF_API bool MFStateBlock_SetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void *pState);
MF_API bool MFStateBlock_SetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, const void *pStateData, size_t dataSize);

inline bool MFStateBlock_SetAnimMatrices(MFStateBlock *pStateBlock, const MFStateConstant_AnimationMatrices &matrices)
{
	return MFStateBlock_SetMiscState(pStateBlock, MFSCMisc_AnimationMatrices, &matrices, sizeof(matrices));
}

inline bool MFStateBlock_SetMatrixBatch(MFStateBlock *pStateBlock, const MFStateConstant_MatrixBatch &batch)
{
	return MFStateBlock_SetMiscState(pStateBlock, MFSCMisc_MatrixBatch, &batch, sizeof(batch));
}

//MF_API void MFStateBlock_SetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, const MFLight *pLight);

MF_API bool MFStateBlock_GetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool *pState);
MF_API bool MFStateBlock_GetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, MFVector *pState);
MF_API bool MFStateBlock_GetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, MFMatrix *pState);
MF_API bool MFStateBlock_GetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture **ppTexture);
MF_API bool MFStateBlock_GetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void **ppState);
MF_API bool MFStateBlock_GetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, void **ppStateData);
//MF_API void MFStateBlock_GetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, MFLight **ppLight);

//MF_API void MFStateBlock_GetLightCounts(MFStateBlock *pStateBlock, int *pOmniLightCount, int *pSpotLightCount, int *pDirectionalLightCount);

MF_API void MFStateBlock_ClearBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant);
MF_API void MFStateBlock_ClearVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant);
MF_API void MFStateBlock_ClearMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant);
MF_API void MFStateBlock_ClearTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant);
MF_API void MFStateBlock_ClearRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState);
MF_API void MFStateBlock_ClearMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState);

#endif // _MFRENDERSTATE_H

/** @} */
