#if !defined(_MFMAT_STANDARD_H)
#define _MFMAT_STANDARD_H

enum MFMat_Standard_Parameters
{
	MFMatStandard_Unknown = -1,

	MFMatStandard_Lit = 0,
	MFMatStandard_Prelit,
	MFMatStandard_DiffuseColour,
	MFMatStandard_AmbientColour,
	MFMatStandard_SpecularColour,
	MFMatStandard_SpecularPower,
	MFMatStandard_EmissiveColour,
	MFMatStandard_Mask,
	MFMatStandard_AlphaRef,
	MFMatStandard_CullMode,
	MFMatStandard_ZRead,
	MFMatStandard_ZWrite,
	MFMatStandard_Blend,
	MFMatStandard_Texture,
	MFMatStandard_TextureFlags,
	MFMatStandard_CelShading,
	MFMatStandard_Phong,
	MFMatStandard_Animated,
	MFMatStandard_Tile,
	MFMatStandard_TextureMatrix,

	MFMatStandard_Max,
	MFMatStandard_ForceInt = 0x7FFFFFFF
};

enum MFMat_Standard_Textures
{
	MFMatStandard_Tex_DifuseMap = 0,
	MFMatStandard_Tex_DiffuseMap2,
	MFMatStandard_Tex_NormalMap,
	MFMatStandard_Tex_DetailMap,
	MFMatStandard_Tex_EnvMap,
	MFMatStandard_Tex_LightMap,
	MFMatStandard_Tex_BumpMap,
	MFMatStandard_Tex_ReflectionMap,
	MFMatStandard_Tex_SpecularMap,

	MFMatStandard_Tex_Texture,

	MFMatStandard_Tex_Max
};

enum MFMat_Standard_TextureFlags
{
	MFMatStandard_TexFlag_AddressU = 0,
	MFMatStandard_TexFlag_AddressV,
	MFMatStandard_TexFlag_AddressW,
	MFMatStandard_TexFlag_MagFilter,
	MFMatStandard_TexFlag_MinFilter,
	MFMatStandard_TexFlag_MipFilter,
	MFMatStandard_TexFlag_BorderColour,

	MFMatStandard_TexFlags_Max
};

enum MFMat_Standard_TexAddress
{
	MFMatStandard_TexAddress_Wrap = 0,
	MFMatStandard_TexAddress_Mirror,
	MFMatStandard_TexAddress_Clamp,
	MFMatStandard_TexAddress_Border,
	MFMatStandard_TexAddress_MirrorOnce,

	MFMatStandard_TexAddress_Max
};

enum MFMat_Standard_TexFilter
{
	MFMatStandard_TexFilter_None = 0,
	MFMatStandard_TexFilter_Point,
	MFMatStandard_TexFilter_Linear,
	MFMatStandard_TexFilter_Anisotropic,

	MFMatStandard_TexFilter_Max
};

enum MFMat_Standard_BlendMode
{
	MFMatStandard_Blend_None = 0,
	MFMatStandard_Blend_Alpha,
	MFMatStandard_Blend_Additive,
	MFMatStandard_Blend_Subtractive,

	MFMatStandard_Blend_Max
};

enum MFMat_Standard_CullMode
{
	MFMatStandard_Cull_None = 0,
	MFMatStandard_Cull_CCW,
	MFMatStandard_Cull_CW,
	MFMatStandard_Cull_Default, // CCW

	MFMatStandard_Cull_Max
};

struct MFMat_Standard_AnimParams
{
	int hFrames, vFrames;
	float frameTime;
};

struct MFMat_Standard_TileParams
{
	int hFrames, vFrames;
	int hFrame, vFrame;
};

#endif
