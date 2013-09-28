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

/**
 * Matrix render states.
 * Render state constant matrices.
 */
enum MFStateConstant_Matrix
{
	MFSCM_Unknown = -1,				/**< Unknown matrix. */

	MFSCM_World = 0,				/**< World matrix. */
	MFSCM_Camera,					/**< Camera matrix. */
	MFSCM_Projection,				/**< Projection matrix. */
	MFSCM_ShadowMap,				/**< Shadow map projection matrix. */
	MFSCM_Fuji0,					/**< Fuji reserved matrix 0. */
	MFSCM_Fuji1,					/**< Fuji reserved matrix 1. */
	MFSCM_User0,					/**< User matrix 0. */
	MFSCM_User1,					/**< User matrix 1. */
	MFSCM_User2,					/**< User matrix 2. */
	MFSCM_User3,					/**< User matrix 3. */
	MFSCM_UV0,						/**< Texture UV matrix 0. */
	MFSCM_UV1,						/**< Texture UV matrix 1. */
	MFSCM_UV2,						/**< Texture UV matrix 2. */
	MFSCM_UV3,						/**< Texture UV matrix 3. */
	MFSCM_View,						/**< Calculated View matrix; inverse Camera matrix. This matrix state may not be set. */
	MFSCM_WorldView,				/**< Calculated World*View matrix. This matrix state may not be set. */
	MFSCM_ViewProjection,			/**< Calculated View*Projection matrix. This matrix state may not be set. */
	MFSCM_WorldViewProjection,		/**< Calculated World*View*Projection matrix. This matrix state may not be set. */
	MFSCM_InverseWorld,				/**< Calculated inverse World matrix. This matrix state may not be set. */
	MFSCM_InverseViewProjection,	/**< Calculated inverse View*Projection matrix. This matrix state may not be set. */

	MFSCM_Max,						/**< Maximum matrix state. */

	MFSCM_FujiMatrixCount = MFSCM_User0 - MFSCM_Fuji0,	/**< Number of Fuji reserved matrices. */
	MFSCM_UserMatrixCount = MFSCM_UV0 - MFSCM_User0,	/**< Number of user matrices. */
	MFSCM_UVMatrixCount = MFSCM_View - MFSCM_UV0,		/**< Number of texture UV matrices. */

	MFSCM_ManagedStart = MFSCM_World,
	MFSCM_NumManaged = MFSCM_ShadowMap - MFSCM_ManagedStart,

	MFSCM_DerivedStart = MFSCM_View,
	MFSCM_NumDerived = MFSCM_Max - MFSCM_DerivedStart,

	MFSCM_NumSetable = MFSCM_DerivedStart,

	MFSCM_ForceInt = 0x7FFFFFFF		/**< Force MFStateConstant_Matrix to an int type. */
};

/**
 * @fn MFStateConstant_Matrix MFSCM_Fuji(int index)
 * Get MFSCM_Fuji# constant by index.
 * @param index Index.
 * @return The specified MFSCM_Fuji# render state.
 * @see MFStateConstant_Matrix
 */
#define MFSCM_Fuji(i) ((MFStateConstant_Matrix)(MFSCM_Fuji0 + i))

/**
 * @fn MFStateConstant_Matrix MFSCM_User(int index)
 * Get MFSCM_User# constant by index.
 * @param index Index.
 * @return The specified MFSCM_User# render state.
 * @see MFStateConstant_Matrix
 */
#define MFSCM_User(i) ((MFStateConstant_Matrix)(MFSCM_User0 + i))

/**
 * @fn MFStateConstant_Matrix MFSCM_UV(int index)
 * Get MFSCM_UV# constant by index.
 * @param index Index.
 * @return The specified MFSCM_UV# render state.
 * @see MFStateConstant_Matrix
 */
#define MFSCM_UV(i) ((MFStateConstant_Matrix)(MFSCM_UV0 + i))

/**
 * Vector render states.
 * Render state constant vectors.
 */
enum MFStateConstant_Vector
{
	MFSCV_Unknown = -1,				/**< Unknown vector. */

	MFSCV_Time = 0,					/**< x = game time in seconds, y = tile delta, z = 1/y, w = reserved. */
	MFSCV_FogColour,				/**< Fog colour. */
	MFSCV_FogParams1,				/**< Fog parameters 1. ???. */
	MFSCV_FogParams2,				/**< Fog parameters 2. ???. */
	MFSCV_RenderState,				/**< x = alpha ref, ...? */
	MFSCV_MaterialDiffuseColour,	/**< Material diffuse modulation colour. */
	MFSCV_DiffuseColour,			/**< Geometry diffuse modulation colour. */
	MFSCV_AmbientColour,			/**< Ambient colour. */

	MFSCV_Fuji0,					/**< Fuji reserved matrix 0. */
	MFSCV_Fuji1,					/**< Fuji reserved matrix 1. */
	MFSCV_Fuji2,					/**< Fuji reserved matrix 2. */
	MFSCV_Fuji3,					/**< Fuji reserved matrix 3. */
	MFSCV_Fuji4,					/**< Fuji reserved matrix 4. */
	MFSCV_Fuji5,					/**< Fuji reserved matrix 5. */
	MFSCV_Fuji6,					/**< Fuji reserved matrix 6. */

	MFSCV_LightCounts,				/**< x = directional count, y = omni count, z = spot count, w = reserved */

	MFSCV_User0,					/**< User vector 0. */
	MFSCV_User1,					/**< User vector 1. */
	MFSCV_User2,					/**< User vector 2. */
	MFSCV_User3,					/**< User vector 3. */
	MFSCV_User4,					/**< User vector 4. */
	MFSCV_User5,					/**< User vector 5. */
	MFSCV_User6,					/**< User vector 6. */
	MFSCV_User7,					/**< User vector 7. */
	MFSCV_User8,					/**< User vector 8. */
	MFSCV_User9,					/**< User vector 9. */
	MFSCV_User10,					/**< User vector 10. */
	MFSCV_User11,					/**< User vector 11. */
	MFSCV_User12,					/**< User vector 12. */
	MFSCV_User13,					/**< User vector 13. */
	MFSCV_User14,					/**< User vector 14. */
	MFSCV_User15,					/**< User vector 15. */

	MFSCV_Max,						/**< Maximum vector state. */

	MFSCV_FujiVectorCount = MFSCV_LightCounts - MFSCV_Fuji0,	/**< Number of Fuji reserved vectors. */
	MFSCV_UserVectorCount = MFSCV_Max - MFSCV_User0,			/**< Number of user vectors. */

	MFSCV_ForceInt = 0x7FFFFFFF		/**< Force MFStateConstant_Vector to an int type. */
};

/**
 * @fn MFStateConstant_Vector MFSCV_Fuji(int index)
 * Get MFSCV_Fuji# constant by index.
 * @param index Index.
 * @return The specified MFSCV_Fuji# render state.
 * @see MFStateConstant_Vector
 */
#define MFSCV_Fuji(i) ((MFStateConstant_Vector)(MFSCV_Fuji0 + i))

/**
 * @fn MFStateConstant_Vector MFSCV_User(int index)
 * Get MFSCV_User# constant by index.
 * @param index Index.
 * @return The specified MFSCV_User# render state.
 * @see MFStateConstant_Vector
 */
#define MFSCV_User(i) ((MFStateConstant_Vector)(MFSCV_User0 + i))

/**
 * Texture render states.
 * Render state constant textures.
 */
enum MFStateConstant_Texture
{
	MFSCT_Unknown = -1,				/**< Unknown texture. */

	MFSCT_Diffuse = 0,				/**< Diffuse texture map. */
	MFSCT_NormalMap,				/**< Normal map. */
	MFSCT_SpecularMap,				/**< Specular map. */
	MFSCT_DetailMap,				/**< Detail map. */
	MFSCT_OpacityMap,				/**< Opacity map. */
	MFSCT_EnvironmentMap,			/**< Environment map. */
	MFSCT_SpecularPowerMap,			/**< Specular power map. */
	MFSCT_EmissiveMap,				/**< Emissive map. */
	MFSCT_LightMap,					/**< Light map. */
	MFSCT_ShadowBuffer,				/**< Shadow map. */
	MFSCT_Projection,				/**< Projected texture map. */
	MFSCT_User0,					/**< User texture 0. */
	MFSCT_User1,					/**< User texture 1. */
	MFSCT_User2,					/**< User texture 2. */
	MFSCT_User3,					/**< User texture 3. */
	MFSCT_User4,					/**< User texture 4. */
	MFSCT_Vertex0,					/**< Vertex texture 0. */
	MFSCT_Vertex1,					/**< Vertex texture 1. */
	MFSCT_Vertex2,					/**< Vertex texture 2. */

	MFSCT_Max,						/**< Maximum texture state. */

	MFSCV_FujiTextureCount = 0,								/**< Number of Fuji reserved textures. */
	MFSCV_UserTextureCount = MFSCT_Vertex0 - MFSCT_User0,	/**< Number of user textures. */
	MFSCV_VertexTextureCount = MFSCT_Max - MFSCT_Vertex0,	/**< Number of vertex textures. */

	MFSCT_ForceInt = 0x7FFFFFFF		/**< Force MFStateConstant_Texture to an int type. */
};

/**
 * @fn MFStateConstant_Texture MFSCT_Fuji(int index)
 * Get MFSCT_Fuji# constant by index.
 * @param index Index.
 * @return The specified MFSCT_Fuji# render state.
 * @see MFStateConstant_Texture
 */
#define MFSCT_Fuji(i) ((MFStateConstant_Texture)(MFSCT_Max + i))

/**
 * @fn MFStateConstant_Texture MFSCT_User(int index)
 * Get MFSCT_User# constant by index.
 * @param index Index.
 * @return The specified MFSCT_User# render state.
 * @see MFStateConstant_Texture
 */
#define MFSCT_User(i) ((MFStateConstant_Texture)(MFSCT_User0 + i))

/**
 * @fn MFStateConstant_Texture MFSCT_Vertex(int index)
 * Get MFSCT_Vertex# constant by index.
 * @param index Index.
 * @return The specified MFSCT_Vertex# render state.
 * @see MFStateConstant_Texture
 */
#define MFSCT_Vertex(i) ((MFStateConstant_Texture)(MFSCT_Vertex0 + i))

/**
 * Bool render states.
 * Render state constant booleans.
 */
enum MFStateConstant_Bool
{
	MFSCB_Unknown = -1,				/**< Unknown bool. */

	MFSCB_Animated = 0,				/**< Geometry has animation? */
	MFSCB_ZPrime,					/**< Is z-prime pass? */
	MFSCB_ShadowGeneration,			/**< Is shadow generation pass? */
	MFSCB_ShadowReceiving,			/**< Is geometry shadow receiving? */
	MFSCB_Opaque,					/**< Is diffuse texture+colour opaque? */
	MFSCB_AlphaTest,				/**< Is alpha testing enabled? */
	MFSCB_Fuji0,					/**< Fuji reserved bool 0. */
	MFSCB_Fuji1,					/**< Fuji reserved bool 1. */
	MFSCB_Fuji2,					/**< Fuji reserved bool 2. */
	MFSCB_User0,					/**< User bool 0. */
	MFSCB_User1,					/**< User bool 1. */
	MFSCB_User2,					/**< User bool 2. */
	MFSCB_User3,					/**< User bool 3. */

	MFSCB_DiffuseSet,				/**< Diffuse texture set? */
	MFSCB_NormalMapSet,				/**< Normal map texture set? */
	MFSCB_SpecularMapSet,			/**< Specular map texture set? */
	MFSCB_DetailMapSet,				/**< Detail map texture set? */
	MFSCB_OpacityMapSet,			/**< Opacity map texture set? */
	MFSCB_EnvironmentMapSet,		/**< Environment map texture set? */
	MFSCB_SpecularPowerMapSet,		/**< Specular power map texture set? */
	MFSCB_EmissiveMapSet,			/**< Emissive map texture set? */
	MFSCB_LightMapSet,				/**< Light map texture set? */
	MFSCB_ShadowBufferSet,			/**< Shadow map texture set? */
	MFSCB_ProjectorSet,				/**< Projected texture set? */
	MFSCB_UserTex0Set,				/**< User texture 0 set? */
	MFSCB_UserTex1Set,				/**< User texture 1 set? */
	MFSCB_UserTex2Set,				/**< User texture 2 set? */
	MFSCB_UserTex3Set,				/**< User texture 3 set? */
	MFSCB_UserTex4Set,				/**< User texture 4 set? */
	MFSCB_VertexTex0Set,			/**< Vertex texture 0 set? */
	MFSCB_VertexTex1Set,			/**< Vertex texture 1 set? */
	MFSCB_VertexTex2Set,			/**< Vertex texture 2 set? */

	MFSCB_Max,						/**< Maximum boolean state. */

	MFSCB_FujiBoolCount = MFSCB_User0 - MFSCB_Fuji0,		/**< Number of Fuji reserved bools. */
	MFSCB_UserBoolCount = MFSCB_DiffuseSet - MFSCB_User0,	/**< Number of user bools. */

	MFSCB_ForceInt = 0x7FFFFFFF		/**< Force MFStateConstant_Bool to an int type. */
};

/**
 * @fn MFStateConstant_Bool MFSCB_Fuji(int index)
 * Get MFSCB_Fuji# constant by index.
 * @param index Index.
 * @return The specified MFSCB_Fuji# render state.
 * @see MFStateConstant_Bool
 */
#define MFSCB_Fuji(i) ((MFStateConstant_Bool)(MFSCB_Fuji0 + i))

/**
 * @fn MFStateConstant_Bool MFSCB_User(int index)
 * Get MFSCB_User# constant by index.
 * @param index Index.
 * @return The specified MFSCB_User# render state.
 * @see MFStateConstant_Bool
 */
#define MFSCB_User(i) ((MFStateConstant_Bool)(MFSCB_User0 + i))

/**
 * @fn MFStateConstant_Bool MFSCB_TexSet(MFStateConstant_Texture type)
 * Get MFSCB_XXXXSet constant by type.
 * @param type Texture type.
 * @return The specified MFSCB_TexSet# render state.
 * @see MFStateConstant_Bool
 */
#define MFSCB_TexSet(i) ((MFStateConstant_Bool)(MFSCB_DiffuseSet + i))

/**
 * Render states.
 * Render state constants.
 */
enum MFStateConstant_RenderState
{
	MFSCRS_Unknown = -1,					/**< Unknown render state. */

	MFSCRS_BlendState = 0,					/**< Blend state. */
	MFSCRS_DepthStencilState,				/**< Depth/stencil state. */
	MFSCRS_RasteriserState,					/**< Rasteriser state. */

	MFSCRS_DiffuseSamplerState,				/**< Diffuse texture sampler. */
	MFSCRS_NormalMapSamplerState,			/**< Normal map sampler. */
	MFSCRS_SpecularMapSamplerState,			/**< Specular map sampler. */
	MFSCRS_DetailMapSamplerState,			/**< Detail map sampler. */
	MFSCRS_OpacityMapSamplerState,			/**< Opacity map sampler. */
	MFSCRS_EnvironmentMapSamplerState,		/**< Environment map sampler. */
	MFSCRS_SpecularPowerMapSamplerState,	/**< Specular power map sampler. */
	MFSCRS_EmissiveMapSamplerState,			/**< Emissive map sampler. */
	MFSCRS_LightMapSamplerState,			/**< Light map sampler. */
	MFSCRS_ShadowBufferSamplerState,		/**< Shadow map sampler. */
	MFSCRS_ProjectorSamplerState,			/**< Projected texture sampler. */
	MFSCRS_UserTex0SamplerState,			/**< User texture 0 sampler. */
	MFSCRS_UserTex1SamplerState,			/**< User texture 1 sampler. */
	MFSCRS_UserTex2SamplerState,			/**< User texture 2 sampler. */
	MFSCRS_UserTex3SamplerState,			/**< User texture 3 sampler. */
	MFSCRS_UserTex4SamplerState,			/**< User texture 4 sampler. */
	MFSCRS_VertexTex0SamplerState,			/**< Vertex texture 0 sampler. */
	MFSCRS_VertexTex1SamplerState,			/**< Vertex texture 1 sampler. */
	MFSCRS_VertexTex2SamplerState,			/**< Vertex texture 2 sampler. */

	MFSCRS_VertexDeclaration,				/**< Vertex declaration. */
	MFSCRS_VertexBuffer0,					/**< Vertex buffer 0. */
	MFSCRS_VertexBuffer1,					/**< Vertex buffer 1. */
	MFSCRS_VertexBuffer2,					/**< Vertex buffer 2. */
	MFSCRS_VertexBuffer3,					/**< Vertex buffer 3. */
	MFSCRS_VertexBuffer4,					/**< Vertex buffer 4. */
	MFSCRS_VertexBuffer5,					/**< Vertex buffer 5. */
	MFSCRS_VertexBuffer6,					/**< Vertex buffer 6. */
	MFSCRS_VertexBuffer7,					/**< Vertex buffer 7. */
	MFSCRS_IndexBuffer,						/**< Index buffer. */

	MFSCRS_Max,								/**< Maximum render state. */

	MFSCRS_VertexBufferCount = MFSCRS_VertexBuffer0 - MFSCRS_IndexBuffer,	/**< Number of vertex buffers. */

	MFSCRS_ForceInt = 0x7FFFFFFF			/**< Force MFStateConstant_RenderState to an int type. */
};

/**
 * @fn MFStateConstant_RenderState MFSCRS_SamplerState(MFStateConstant_Texture type)
 * Get MFSCRS_XXXXSamplerState constant by type.
 * @param type Texture type.
 * @return The specified MFSCRS_SamplerState# render state.
 * @see MFStateConstant_RenderState
 */
#define MFSCRS_SamplerState(tex) ((MFStateConstant_RenderState)(MFSCRS_DiffuseSamplerState + tex))

/**
 * @fn MFStateConstant_RenderState MFSCRS_VertexBuffer(int index)
 * Get MFSCRS_VertexBuffer# constant by index.
 * @param index Index.
 * @return The specified MFSCRS_VertexBuffer# render state.
 * @see MFStateConstant_RenderState
 */
#define MFSCRS_VertexBuffer(i) ((MFStateConstant_RenderState)(MFSCRS_VertexBuffer0 + i))

/**
 * Miscellaneous render states.
 * Miscellaneous render state constant.
 */
enum MFStateConstant_Miscellaneous
{
	MFSCMisc_Unknown = -1,				/**< Unknown state. */

	MFSCMisc_AnimationMatrices,			/**< Animation matrices. Value of \a MFStateConstant_AnimationMatrices. */
	MFSCMisc_MatrixBatch,				/**< Animation matrix batch. Value of \a MFStateConstant_MatrixBatch. */
	MFSCMisc_Light0,					/**< Light 0 description. */
	MFSCMisc_Light1,					/**< Light 1 description. */
	MFSCMisc_Light2,					/**< Light 2 description. */
	MFSCMisc_Light3,					/**< Light 3 description. */
	MFSCMisc_Light4,					/**< Light 4 description. */
	MFSCMisc_Light5,					/**< Light 5 description. */
	MFSCMisc_Light6,					/**< Light 6 description. */
	MFSCMisc_Light7,					/**< Light 7 description. */
	MFSCMisc_Light8,					/**< Light 8 description. */
	MFSCMisc_Light9,					/**< Light 9 description. */
	MFSCMisc_Light10,					/**< Light 10 description. */
	MFSCMisc_Light11,					/**< Light 11 description. */
	MFSCMisc_Light12,					/**< Light 12 description. */
	MFSCMisc_Light13,					/**< Light 13 description. */
	MFSCMisc_Light14,					/**< Light 14 description. */
	MFSCMisc_Light15,					/**< Light 15 description. */
	MFSCMisc_GPUEvent,					/**< GPU event description. */

	MFSCMisc_Max,						/**< Maximum miscellaneous state. */

	MFSCMisc_LightCount = MFSCMisc_GPUEvent - MFSCMisc_Light0,	/**< Number of lights. */

	MFSCMisc_ForceInt = 0x7FFFFFFF		/**< Force MFStateConstant_Miscellaneous to an int type. */
};

/**
 * @fn MFStateConstant_Miscellaneous MFSCMisc_Light(int index)
 * Get MFSCMisc_Light# constant by index.
 * @param index Index.
 * @return The specified MFSCMisc_Light# render state.
 * @see MFStateConstant_Miscellaneous
 */
#define MFSCMisc_Light(i) ((MFStateConstant_Miscellaneous)(MFSCMisc_Light0 + i))

/**
 * Animation matrices.
 * Render state constant animation matrices.
 */
struct MFStateConstant_AnimationMatrices
{
	MFMatrix *pMatrices;				/**< Pointer to the animation matrices. */
	size_t numMatrices;					/**< Number of animation matrices. */
};

/**
 * Animation matrix batch.
 * Render state constant animation matrix batch.
 */
struct MFStateConstant_MatrixBatch
{
	uint16 *pIndices;					/**< Pointer to the batch matrix indices. */
	uint32 numMatrices;					/**< Number of matrices in the batch. */
};

/**
 * Blend operation.
 * Frame buffer blend operation.
 */
enum MFBlendOp
{
	MFBlendOp_Unknown = -1,				/**< Unknown blend op. */

	MFBlendOp_Add = 0,					/**< Add source 1 and source 2. */
	MFBlendOp_Subtract,					/**< Subtract source 1 from source 2. */
	MFBlendOp_RevSubtract,				/**< Subtract source 2 from source 1. */
	MFBlendOp_Min,						/**< Find the minimum of source 1 and source 2. */
	MFBlendOp_Max,						/**< Find the maximum of source 1 and source 2. */

	MFBlendOp_BlendOpCount,				/**< Number of blend operations. */
	MFBlendOp_ForceInt = 0x7FFFFFFF		/**< Force MFBlendOp to an int type. */
};

/**
 * Blend arguments.
 * Frame buffer blending arguments.
 */
enum MFBlendArg
{
	MFBlendArg_Unknown = -1,			/**< Unknown blend argument. */

	MFBlendArg_Zero = 0,				/**< The data source is the colour black (0, 0, 0, 0). No pre-blend operation. */
	MFBlendArg_One,						/**< The data source is the colour white (1, 1, 1, 1). No pre-blend operation. */
	MFBlendArg_SrcColour,				/**< The data source is the incoming colour data. No pre-blend operation. */
	MFBlendArg_InvSrcColour,			/**< The data source is the incoming colour data. The pre-blend operation inverts the data, generating 1 - RGB. */
	MFBlendArg_SrcAlpha,				/**< The data source is the incoming alpha data. No pre-blend operation. */
	MFBlendArg_InvSrcAlpha,				/**< The data source is the incoming alpha data. The pre-blend operation inverts the data, generating 1 - A. */
	MFBlendArg_DestColour,				/**< The data source is colour data from the rendertarget. No pre-blend operation. */
	MFBlendArg_InvDestColour,			/**< The data source is colour data from the rendertarget. The pre-blend operation inverts the data, generating 1 - RGB. */
	MFBlendArg_DestAlpha,				/**< The data source is alpha data from the rendertarget. No pre-blend operation. */
	MFBlendArg_InvDestAlpha,			/**< The data source is alpha data from the rendertarget. The pre-blend operation inverts the data, generating 1 - A. */
	MFBlendArg_SrcAlphaSaturate,		/**< The data source is the incoming alpha data. The pre-blend operation clamps the data to 1 or less.  */

	MFBlendArg_BlendFactor,				/**< Constant colour blend factor. No pre-blend operation. */
	MFBlendArg_InvBlendFactor,			/**< Constant colour blend factor. The pre-blend operation inverts the blend factor, generating 1 - blend_factor. */
	MFBlendArg_Src1Colour,				/**< The data sources are both incoming colour data. There is no pre-blend operation. This options supports dual-source colour blending. */
	MFBlendArg_InvSrc1Colour,			/**< The data sources are both incoming colour data. The pre-blend operation inverts the data, generating 1 - RGB. This options supports dual-source colour blending. */
	MFBlendArg_Src1Alpha,				/**< The data sources are incoming alpha data. There is no pre-blend operation. This options supports dual-source colour blending. */
	MFBlendArg_InvSrc1Alpha,			/**< The data sources are incoming alpha data. The pre-blend operation inverts the data, generating 1 - A. This options supports dual-source colour blending. */

	MFBlendArg_Max,						/**< Maximum blend arg. */
	MFBlendArg_ForceInt = 0x7FFFFFFF	/**< Force MFBlendArg to an int type. */
};

/**
 * Colour write flags.
 * Frame buffer colour write flags. These flags can be combined with a bitwise 'or'.
 */
enum MFColourWriteEnable
{
	MFColourWriteEnable_None = 0,		/**< Disable colour writing. */

	MFColourWriteEnable_Red = 1,		/**< Allow data to be written to the red component. */
	MFColourWriteEnable_Green = 2,		/**< Allow data to be written to the green component. */
	MFColourWriteEnable_Blue = 4,		/**< Allow data to be written to the blue component. */
	MFColourWriteEnable_Alpha = 8,		/**< Allow data to be written to the alpha component. */

	MFColourWriteEnable_All = (MFColourWriteEnable_Red | MFColourWriteEnable_Green | MFColourWriteEnable_Blue | MFColourWriteEnable_Alpha), /**< Allow data to be written to all components. */
	MFColourWriteEnable_RGB = (MFColourWriteEnable_Red | MFColourWriteEnable_Green | MFColourWriteEnable_Blue), 							/**< Allow data to be written to the red, green, and blue components. */
};

/**
 * Texture filter mode.
 * Texture filtering modes.
 */
enum MFTexFilter
{
	MFTexFilter_Unknown = -1,			/**< Unknown texture filter type. */

	MFTexFilter_None,					/**< No filtering. */
	MFTexFilter_Point,					/**< Use nearest point sampling. */
	MFTexFilter_Linear,					/**< Use linear interpolation. */
	MFTexFilter_Anisotropic,			/**< Use anisotropic interpolation. */
//	MFTexFilter_Text_1Bit,				/**< Filtering for 1-bit text. */

	MFTexFilter_Max,					/**< Maximum texture filter. */
	MFTexFilter_ForceInt = 0x7FFFFFFF	/**< Force MFTexFilter to an int type. */
};

/**
 * Texture address mode.
 * Texture addressing modes.
 */
enum MFTexAddressMode
{
	MFTexAddressMode_Unknown = -1,			/**< Unknown texture address mode. */

	MFTexAddressMode_Wrap = 0,				/**< Tile the texture at every (u,v) integer junction. For example, for u values between 0 and 3, the texture is repeated three times. */
	MFTexAddressMode_Mirror,				/**< Flip the texture at every (u,v) integer junction. For u values between 0 and 1, for example, the texture is addressed normally; between 1 and 2, the texture is flipped (mirrored); between 2 and 3, the texture is normal again; and so on.  */
	MFTexAddressMode_Clamp,					/**< Texture coordinates outside the range [0.0, 1.0] are set to the texture colour at 0.0 or 1.0, respectively. */
	MFTexAddressMode_Border,				/**< Texture coordinates outside the range [0.0, 1.0] are set to the border colour specified in the active \a MFSamplerState or shader code. */
	MFTexAddressMode_MirrorOnce,			/**< Similar to \a MFTexAddressMode_Mirror and \a MFTexAddressMode_Clamp. Takes the absolute value of the texture coordinate (thus, mirroring around 0), and then clamps to the maximum value. */

	MFTexAddressMode_Max,					/**< Maximum address mode. */
	MFTexAddressMode_ForceInt = 0x7FFFFFFF	/**< Force MFTexAddressMode to an int type. */
};

/**
 * Comparison function.
 * Comparison functions for alpha and stencil testing.
 */
enum MFComparisonFunc
{
	MFComparisonFunc_Unknown = -1,			/**< Unknown comparison function. */

	MFComparisonFunc_Never = 0,				/**< Never pass the comparison. */
	MFComparisonFunc_Less,					/**< If the source data is less than the destination data, the comparison passes. */
	MFComparisonFunc_Equal,					/**< If the source data is equal to the destination data, the comparison passes. */
	MFComparisonFunc_LessEqual,				/**< If the source data is less than or equal to the destination data, the comparison passes. */
	MFComparisonFunc_Greater,				/**< If the source data is greater than the destination data, the comparison passes. */
	MFComparisonFunc_NotEqual,				/**< If the source data is not equal to the destination data, the comparison passes. */
	MFComparisonFunc_GreaterEqual,			/**< If the source data is greater than or equal to the destination data, the comparison passes. */
	MFComparisonFunc_Always,				/**< Always pass the comparison. */

	MFComparisonFunc_Max,					/**< Maximum comparison function. */
	MFComparisonFunc_ForceInt = 0x7FFFFFFF	/**< Force MFComparisonFunc to an int type. */
};

/**
 * Depth write mask.
 * Depth write mask.
 */
enum MFDepthWriteMask
{
	MFDepthWriteMask_Unknown = -1,			/**< Unknown depth write mask. */

	MFDepthWriteMask_Zero = 0,				/**< Turn off writes to the depth/stencil buffer. */
	MFDepthWriteMask_All = 1,				/**< Turn on writes to the depth/stencil buffer. */

	MFDepthWriteMask_Max,					/**< Maximum depth write mask. */
	MFDepthWriteMask_ForceInt = 0x7FFFFFFF	/**< Force MFDepthWriteMask to an int type. */
};

/**
 * Stencil operation.
 * Stencil buffer write operations.
 */
enum MFStencilOp
{
	MFStencilOp_Unknown = -1,			/**< Unknown stencil operation. */

	MFStencilOp_Keep = 0,				/**< Keep the existing stencil data. */
	MFStencilOp_Zero,					/**< Set the stencil data to 0. */
	MFStencilOp_Replace,				/**< Set the stencil data to the reference value. */
	MFStencilOp_IncrementClamp,			/**< Increment the stencil value by 1, and clamp the result. */
	MFStencilOp_DecrementClamp,			/**< Decrement the stencil value by 1, and clamp the result. */
	MFStencilOp_Invert,					/**< Invert the stencil data. */
	MFStencilOp_Increment,				/**< Increment the stencil value by 1, and wrap the result if necessary. */
	MFStencilOp_Decrement,				/**< Increment the stencil value by 1, and wrap the result if necessary. */

	MFStencilOp_Max,					/**< Maximum stencil op. */
	MFStencilOp_ForceInt = 0x7FFFFFFF	/**< Force MFStencilOp to an int type. */
};

/**
 * Fill mode.
 * Rasteriser fill mode.
 */
enum MFFillMode
{
	MFFillMode_Unknown = -1,			/**< Unknown fill mode. */

	MFFillMode_Solid = 0,				/**< Render solid primitives. */
	MFFillMode_Wireframe,				/**< Render wireframe. */
	MFFillMode_Point,					/**< Render points. */

	MFFillMode_Max,						/**< Maximum fill mode. */
	MFFillMode_ForceInt = 0x7FFFFFFF	/**< Force MFFillMode to an int type. */
};

/**
 * Cull mode.
 * Back face culling mode.
 */
enum MFCullMode
{
	MFCullMode_Unknown = -1,			/**< Unknown cull mode. */

	MFCullMode_None = 0,				/**< Always draw all triangles. */
	MFCullMode_CCW,						/**< Do not draw counter-clockwise triangles. */
	MFCullMode_CW,						/**< Do not draw clockwise triangles. */

	MFCullMode_Max,						/**< Maximum cull mode. */
	MFCullMode_ForceInt = 0x7FFFFFFF	/**< Force MFCullMode to an int type. */
};

/**
 * State block constant type.
 * MFStateBlock constant type.
 */
enum MFStateBlockConstantType
{
	MFSB_CT_Bool = 0,					/**< Boolean constant. */
	MFSB_CT_Vector,						/**< Vector constant. */
	MFSB_CT_Matrix,						/**< Matrix constant. */
	MFSB_CT_Texture,					/**< Texture constatnt. */
	MFSB_CT_RenderState,				/**< Render state constant. */
	MFSB_CT_Misc,						/**< Miscellaneous constant. */

	MFSB_CT_Unknown = 7,				/**< Unknown state block constant type. */

	MFSB_CT_TypeCount,					/**< Number of state block constant types. */
	MFSB_CT_ForceInt = 0x7FFFFFFF		/**< Force MFStateBlockConstantType to an int type. */
};

/**
 * @struct MFBlendState
 * Represents a frame buffer blend state.
 */
struct MFBlendState;

/**
 * Blend state description.
 * Describes a frame buffer blend state.
 */
struct MFBlendStateDesc
{
	/**
	 * Render target blend state.
	 * Describes a frame buffer blend state for a single render target.
	 */
	struct RenderTargetBlendDesc
	{
		MFBlendOp blendOp;				/**< This blend operation defines how to combine the RGB data sources. */
		MFBlendArg srcBlend;			/**< This blend option specifies the first RGB data source and includes an optional pre-blend operation. */
		MFBlendArg destBlend;			/**< This blend option specifies the second RGB data source and includes an optional pre-blend operation. */
		MFBlendOp blendOpAlpha;			/**< This blend operation defines how to combine the alpha data sources. */
		MFBlendArg srcBlendAlpha;		/**< This blend option specifies the first alpha data source and includes an optional pre-blend operation. Blend options that end in 'Colour' are not allowed. */
		MFBlendArg destBlendAlpha;		/**< This blend option specifies the second alpha data source and includes an optional pre-blend operation. Blend options that end in 'Colour' are not allowed. */
		uint8 writeMask;				/**< A write mask created by combining fields in \a MFColourWriteEnable. */
		bool bEnable;					/**< Enable (or disable) blending. */

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
	bool bIndependentBlendEnable;			/**< Enables independent blending to simultaneous render targets. If false, only the \a renderTarget[0] members are used. \a renderTarget[1..7] are ignored. */
	RenderTargetBlendDesc renderTarget[8];	/**< Array of RenderTargetBlendDesc; corresponding to the eight rendertargets that can be set to the output-merger stage at one time. */

	MFBlendStateDesc()
	{
		bAlphaToCoverageEnable = false;
		bIndependentBlendEnable = false;
	}
};

/**
 * @struct MFSamplerState
 * Represents a texture sampler state.
 */
struct MFSamplerState;

/**
 * Texture sampler state description.
 * Describes a texture sampler state.
 */
struct MFSamplerStateDesc
{
	MFTexFilter minFilter;				/**< Filtering method to use for minification. */
	MFTexFilter magFilter;				/**< Filtering method to use for magnification. */
	MFTexFilter mipFilter;				/**< Mipmap filter to use during minification. */
	MFTexAddressMode addressU;			/**< Method to use for resolving a u texture coordinate that is outside the 0 to 1 range. */
	MFTexAddressMode addressV;			/**< Method to use for resolving a v texture coordinate that is outside the 0 to 1 range. */
	MFTexAddressMode addressW;			/**< Method to use for resolving a w texture coordinate that is outside the 0 to 1 range. */
	bool bEnableComparison;				/**< Enable texture sample comparison mode. */
	MFComparisonFunc comparisonFunc;	/**< A function that compares sampled data against existing sampled data. */
	uint32 maxAnisotropy;				/**< Clamping value used if \a MFTexFilter_Anisotropic filtering is specified. Valid values are between 1 and 16. */
	float mipLODBias;					/**< Offset from the calculated mipmap level. For example, if the renderer calculates that a texture should be sampled at mipmap level 3 and \a mipLODBias is 2, then the texture will be sampled at mipmap level 5. */
	float minLOD;						/**< Lower end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level and any higher level is less detailed. */
	float maxLOD;						/**< Upper end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level and any higher level is less detailed. This value must be greater than or equal to \a minLOD. To have no upper limit on LOD set this to a large value such as FLT_MAX. */
	MFVector borderColour;				/**< Border color to use if \a MFTexAddressMode_Border is specified for \a addressU, \a addressV, or \a addressW. */

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

/**
 * @struct MFDepthStencilState
 * Represents a depth/stencil state.
 */
struct MFDepthStencilState;

/**
 * Depth/stencil state description.
 * Describes depth/stencil operations.
 */
struct MFDepthStencilStateDesc
{
	/**
	 * Stencil operation description.
	 * Describes a stencil buffer operation.
	 */
	struct StencilOpDesc
	{
		MFStencilOp stencilFailOp;		/**< The stencil operation to perform when stencil testing fails. */
		MFStencilOp stencilDepthFailOp;	/**< The stencil operation to perform when stencil testing passes and depth testing fails. */
		MFStencilOp stencilPassOp;		/**< The stencil operation to perform when stencil testing and depth testing both pass. */
		MFComparisonFunc stencilFunc;	/**< A function that compares stencil data against existing stencil data. */

		StencilOpDesc()
		{
			stencilFailOp = MFStencilOp_Keep;
			stencilDepthFailOp = MFStencilOp_Keep;
			stencilPassOp = MFStencilOp_Keep;
			stencilFunc = MFComparisonFunc_Always;
		}
	};

	bool bDepthEnable;					/**< Enable depth testing. */
	MFComparisonFunc depthFunc;			/**< A function that compares depth data against existing depth data. */
	MFDepthWriteMask depthWriteMask;	/**< Identify a portion of the depth-stencil buffer that can be modified by depth data. */
	bool bStencilEnable;				/**< Enable stencil testing. */
	uint8 stencilReadMask;				/**< Identify a portion of the depth-stencil buffer for reading stencil data. */
	uint8 stencilWriteMask;				/**< Identify a portion of the depth-stencil buffer for writing stencil data. */
	StencilOpDesc frontFace;			/**< Identify how to use the results of the depth test and the stencil test for pixels whose surface normal is facing towards the camera. */
	StencilOpDesc backFace;				/**< Identify how to use the results of the depth test and the stencil test for pixels whose surface normal is facing away from the camera. */

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

/**
 * @struct MFRasteriserState
 * Represents a rasteriser state.
 */
struct MFRasteriserState;

/**
 * Rasteriser state description.
 * Describes a primitive rasteriser state.
 */
struct MFRasteriserStateDesc
{
	MFFillMode fillMode;			/**< Determines the fill mode to use when rendering. */
	MFCullMode cullMode;			/**< Indicates triangles facing the specified direction are not drawn. */
	int depthBias;					/**< Depth value added to each pixel. */
	float depthBiasClamp;			/**< Maximum depth bias of a pixel. */
	float slopeScaledDepthBias;		/**< Scalar on a given pixel's slope. */
	bool bDepthClipEnable;			/**< Enable clipping based on distance. */
	bool bScissorEnable;			/**< Enable scissor-rectangle culling. All pixels ouside an active scissor rectangle are culled. */
	bool bMultisampleEnable;		/**< Enable multisample antialiasing. */
	bool bAntialiasedLineEnable;	/**< Enable line antialiasing; only applies if doing line drawing and multisampleEnable is false. */

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

/**
 * @struct MFStateBlock
 * Represents a rendering state-block.
 */
struct MFStateBlock;

/**
 * Create a blend state object.
 * Creates a frame buffer blend state object.
 * @param pDesc Pointer to a blend state description.
 * @return The created blend state object.
 * @see MFBlendState_Release()
 */
MF_API MFBlendState* MFBlendState_Create(MFBlendStateDesc *pDesc);

/**
 * Release a blend state object.
 * Releases a reference to a frame buffer blend state object.
 * @param pBlendState Blend state to release.
 * @return The new reference count of the blend state object.
 * @see MFBlendState_Create()
 */
MF_API int MFBlendState_Release(MFBlendState *pBlendState);

/**
 * Create a texture sampler state object.
 * Creates a texture sampler state object.
 * @param pDesc Pointer to a sampler state description.
 * @return The created sampler state object.
 * @see MFSamplerState_Release()
 */
MF_API MFSamplerState* MFSamplerState_Create(MFSamplerStateDesc *pDesc);

/**
 * Release a texture sampler state object.
 * Releases a reference to a texture sampler state object.
 * @param pSamplerState Sampler state to release.
 * @return The new reference count of the sampler state object.
 * @see MFSamplerState_Create()
 */
MF_API int MFSamplerState_Release(MFSamplerState *pSamplerState);

/**
 * Create a depth/stencil state object.
 * Creates a depth/stencil state object.
 * @param pDesc Pointer to a depth/stencil state description.
 * @return The created depth/stencil state object.
 * @see MFDepthStencilState_Release()
 */
MF_API MFDepthStencilState* MFDepthStencilState_Create(MFDepthStencilStateDesc *pDesc);

/**
 * Release a depth/stencil state object.
 * Releases a reference to a depth/stencil state object.
 * @param pDepthStencilState Depth/stencil state to release.
 * @return The new reference count of the depth/stencil state object.
 * @see MFDepthStencilState_Create()
 */
MF_API int MFDepthStencilState_Release(MFDepthStencilState *pDepthStencilState);

/**
 * Create a rasteriser state object.
 * Creates a rasteriser state object.
 * @param pDesc Pointer to a rasteriser state description.
 * @return The created rasteriser state object.
 * @see MFRasteriserState_Release()
 */
MF_API MFRasteriserState* MFRasteriserState_Create(MFRasteriserStateDesc *pDesc);

/**
 * Release a rasteriser state object.
 * Releases a reference to a rasteriser state object.
 * @param pRasteriserState Rasteriser state to release.
 * @return The new reference count of the rasteriser state object.
 * @see MFRasteriserState_Create()
 */
MF_API int MFRasteriserState_Release(MFRasteriserState *pRasteriserState);

/**
 * Create a state block.
 * Creates a new state block.
 * @param size Minimum size for the state block.
 * @return A new state block.
 */
MF_API MFStateBlock* MFStateBlock_Create(uint32 size);

/**
 * Create a temporary state block.
 * Creates a temporary state block that will be destroyed at the end of the frame.
 * @param size Minimum size for the state block.
 * @return A new temporary state block.
 */
MF_API MFStateBlock* MFStateBlock_CreateTemporary(uint32 size);

/**
 * Destroy a state block.
 * Destroys a state block.
 * @param pStateBlock State block to destroy.
 * @return None.
 * @see MFStateBlock_Create()
 */
MF_API void MFStateBlock_Destroy(MFStateBlock *pStateBlock);

/**
 * Clone a state block.
 * Clone an existing state block into a new state block.
 * @param pSource State block to clone.
 * @return A clone of \a pSource.
 */
MF_API MFStateBlock* MFStateBlock_Clone(MFStateBlock *pSource);

/**
 * Copy a state block.
 * Copies the contents of a state block to another. The destination state block data is overwritten.
 * @param pSource State block to copy.
 * @param pDest Target state block.
 * @return None.
 */
MF_API void MFStateBlock_Copy(MFStateBlock *pSource, MFStateBlock *pDest);

/**
 * Merge 2 state blocks.
 * Merge the contents of 2 state blocks.
 * @param pSource1 The first state block.
 * @param pSource2 The second state block.
 * @return A new state block containing the states from both source state blocks.
 * @remarks If a state is set in both source state blocks, the new state block will receive the value from \a pSource1.
 */
MF_API MFStateBlock* MFStateBlock_Merge(MFStateBlock *pSource1, MFStateBlock *pSource2);

/**
 * Clear a state block.
 * Clear the contents of a state block.
 * @param pStateBlock The state block to clear.
 * @return None.
 */
MF_API void MFStateBlock_Clear(MFStateBlock *pStateBlock);

/**
 * Get the allocation size of a state block.
 * Gets the total allocation size of a state block.
 * @param pStateBlock A state block.
 * @return The number of bytes allocated to the state block.
 */
MF_API size_t MFStateBlock_GetAllocatedBytes(MFStateBlock *pStateBlock);

/**
 * Get the size of a state block.
 * Gets the number of bytes used by a state block.
 * @param pStateBlock A state block.
 * @return The number of bytes current used by the state block.
 */
MF_API size_t MFStateBlock_GetUsedBytes(MFStateBlock *pStateBlock);

/**
 * Get the number of available bytes in a state block.
 * Gets the number of bytes available in a state block.
 * @param pStateBlock A state block.
 * @return The number of free bytes.
 */
MF_API size_t MFStateBlock_GetFreeBytes(MFStateBlock *pStateBlock);

/**
 * Get a render state name.
 * Gets the name of a render state.
 * @param constantType Type of the render state constant.
 * @param constant The render state constant id.
 * @return The name of the render state.
 */
MF_API const char* MFStateBlock_GetRenderStateName(MFStateBlockConstantType constantType, int constant);

MF_API int MFStateBlock_GetNumRenderStates(MFStateBlockConstantType constantType);

/**
 * Set a bool render state.
 * Sets a boolean render state.
 * @param pStateBlock A state block.
 * @param constant The render state constant to set.
 * @param state The value of the bool.
 * @return \a true if the state was set successfully.
 */
MF_API bool MFStateBlock_SetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool state);

/**
 * Set a vector render state.
 * Sets a vector render state.
 * @param pStateBlock A state block.
 * @param constant The render state constant to set.
 * @param state The value of the vector.
 * @return \a true if the state was set successfully.
 */
MF_API bool MFStateBlock_SetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, const MFVector &state);

/**
 * Set a matrix render state.
 * Sets a matrix render state.
 * @param pStateBlock A state block.
 * @param constant The render state constant to set.
 * @param state The value of the matrix.
 * @return \a true if the state was set successfully.
 */
MF_API bool MFStateBlock_SetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, const MFMatrix &state);

/**
 * Set a texture render state.
 * Sets a texture render state.
 * @param pStateBlock A state block.
 * @param constant The render state constant to set.
 * @param pTexture The texture to set.
 * @return \a true if the state was set successfully.
 */
MF_API bool MFStateBlock_SetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture *pTexture);

/**
 * Set a render state.
 * Sets a render state.
 * @param pStateBlock A state block.
 * @param renderState The render state constant to set.
 * @param pState Pointer to a render state object.
 * @return \a true if the state was set successfully.
 */
MF_API bool MFStateBlock_SetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void *pState);

/**
 * Set a miscellaneous render state.
 * Sets a miscellaneous render state.
 * @param pStateBlock A state block.
 * @param miscState The render state constant to set.
 * @param pStateData Pointer to render state data to write to the state block.
 * @param dataSize Size of the render state data.
 * @return \a true if the state was set successfully.
 */
MF_API bool MFStateBlock_SetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, const void *pStateData, size_t dataSize);

/**
 * Set the animation matrices render state.
 * Sets the animation matrices render state.
 * @param pStateBlock A state block.
 * @param matrices A set of animation matrices.
 * @return \a true if the state was set successfully.
 */
inline bool MFStateBlock_SetAnimMatrices(MFStateBlock *pStateBlock, const MFStateConstant_AnimationMatrices &matrices)
{
	return MFStateBlock_SetMiscState(pStateBlock, MFSCMisc_AnimationMatrices, &matrices, sizeof(matrices));
}

/**
 * Set the animation matrix batch render state.
 * Sets the animation matrix batch render state.
 * @param pStateBlock A state block.
 * @param batch An animation matrix batch.
 * @return \a true if the state was set successfully.
 */
inline bool MFStateBlock_SetMatrixBatch(MFStateBlock *pStateBlock, const MFStateConstant_MatrixBatch &batch)
{
	return MFStateBlock_SetMiscState(pStateBlock, MFSCMisc_MatrixBatch, &batch, sizeof(batch));
}

//MF_API void MFStateBlock_SetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, const MFLight *pLight);

/**
 * Get a bool render state.
 * Gets a boolean render state.
 * @param pStateBlock A state block.
 * @param constant The render state constant to get.
 * @param pState Pointer to a bool that receives the render state.
 * @return \a true if the state was read successfully.
 */
MF_API bool MFStateBlock_GetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool *pState);

/**
 * Get a vector render state.
 * Gets a vector render state.
 * @param pStateBlock A state block.
 * @param constant The render state constant to get.
 * @param pState Pointer to an \a MFVector that receives the render state.
 * @return \a true if the state was read successfully.
 */
MF_API bool MFStateBlock_GetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, MFVector *pState);

/**
 * Get a matrix render state.
 * Gets a matrix render state.
 * @param pStateBlock A state block.
 * @param constant The render state constant to get.
 * @param pState Pointer to an \a MFMatrix that receives the render state.
 * @return \a true if the state was read successfully.
 */
MF_API bool MFStateBlock_GetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, MFMatrix *pState);

/**
 * Get a texture render state.
 * Gets a texture render state.
 * @param pStateBlock A state block.
 * @param constant The texture to get.
 * @param ppTexture Pointer to an \a MFTexture* that receives the texture.
 * @return \a true if the state was read successfully.
 */
MF_API bool MFStateBlock_GetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture **ppTexture);

/**
 * Get a render state.
 * Gets a render state.
 * @param pStateBlock A state block.
 * @param renderState The render state constant to get.
 * @param ppState Pointer to a \a void* that receives the render state.
 * @return \a true if the state was read successfully.
 */
MF_API bool MFStateBlock_GetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void **ppState);

/**
 * Get a miscellaneous render state.
 * Gets a miscellaneous render state.
 * @param pStateBlock A state block.
 * @param miscState The render state constant to get.
 * @param ppStateData Pointer to a \a void* that receives the render state.
 * @return \a true if the state was read successfully.
 */
MF_API bool MFStateBlock_GetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, void **ppStateData);

//MF_API void MFStateBlock_GetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, MFLight **ppLight);

//MF_API void MFStateBlock_GetLightCounts(MFStateBlock *pStateBlock, int *pOmniLightCount, int *pSpotLightCount, int *pDirectionalLightCount);

/**
 * Clear a bool render state.
 * Clears a boolean render state.
 * @param pStateBlock A state block.
 * @param constant The bool to clear.
 * @return None.
 */
MF_API void MFStateBlock_ClearBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant);

/**
 * Clear a vector render state.
 * Clears a vector render state.
 * @param pStateBlock A state block.
 * @param constant The vector to clear.
 * @return None.
 */
MF_API void MFStateBlock_ClearVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant);

/**
 * Clear a matrix render state.
 * Clears a matrix render state.
 * @param pStateBlock A state block.
 * @param constant The matrix to clear.
 * @return None.
 */
MF_API void MFStateBlock_ClearMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant);

/**
 * Clear a texture render state.
 * Clears a texture render state.
 * @param pStateBlock A state block.
 * @param constant The texture to clear.
 * @return None.
 */
MF_API void MFStateBlock_ClearTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant);

/**
 * Clear a render state.
 * Clears a render state.
 * @param pStateBlock A state block.
 * @param renderState The render state to clear.
 * @return None.
 */
MF_API void MFStateBlock_ClearRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState);

/**
 * Clear a miscellaneous render state.
 * Clears a miscellaneous render state.
 * @param pStateBlock A state block.
 * @param miscState The render state to clear.
 * @return None.
 */
MF_API void MFStateBlock_ClearMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState);

#if 0
	// These are here to keep doxygen happy.
	MFStateConstant_Matrix MFSCM_Fuji(int index);
	MFStateConstant_Matrix MFSCM_User(int index);
	MFStateConstant_Matrix MFSCM_UV(int index);
	MFStateConstant_Vector MFSCV_Fuji(int index);
	MFStateConstant_Vector MFSCV_User(int index);
	MFStateConstant_Texture MFSCT_Fuji(int index);
	MFStateConstant_Texture MFSCT_User(int index);
	MFStateConstant_Texture MFSCT_Vertex(int index);
	MFStateConstant_Bool MFSCB_Fuji(int index);
	MFStateConstant_Bool MFSCB_User(int index);
	MFStateConstant_Bool MFSCB_TexSet(MFStateConstant_Texture type);
	MFStateConstant_RenderState MFSCRS_SamplerState(MFStateConstant_Texture type);
	MFStateConstant_RenderState MFSCRS_VertexBuffer(int index);
	MFStateConstant_Miscellaneous MFSCMisc_Light(int index);
#endif

#endif // _MFRENDERSTATE_H

/** @} */
