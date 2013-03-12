#if !defined(_MFMAT_STANDARD_INTERNAL_H)
#define _MFMAT_STANDARD_INTERNAL_H

#include "Materials/MFMat_Standard.h"
#include "MFRenderState.h"

enum MaterialFlags
{
	// Material Flags
	MF_Lit					= 0x00000001,	// L
	MF_AlphaBlend			= 0x00000002,	// Al
	MF_Additive				= 0x00000004,	// A
	MF_Subtractive			= 0x00000006,	// S
	MF_BlendMask			= 0x00000006,

	MF_Mask					= 0x00000020,	// M
	MF_CullMode				= 0x000000C0,
	MF_Animating			= 0x00000100,

	MF_NoZRead				= 0x00000200,
	MF_NoZWrite				= 0x00000400,

	// Renderer Flags
	MF_DisplacementMap		= 0x80000000,	// D

	MF_LitPerPixel			= 0x00010000,	// P
	MF_CelShading			= 0x00020000,	// C
	MF_DiffuseMap			= 0x00080000,	// D
	MF_DiffuseMap2			= 0x00100000,	// D2
	MF_SphereEnvMap			= 0x00200000,	// E
	MF_ScreenEnvMap			= 0x00400000,	// Es
	MF_SpecularMap			= 0x00800000,	// Sp
	MF_ReflectionMap		= 0x01000000,	// R
	MF_BumpMap				= 0x02000000,	// B
	MF_NormalMap			= 0x04000000,	// N
	MF_DetailTexture		= 0x08000000,	// Dt
	MF_LightMap				= 0x10000000,	// Lm
	MF_CubeEnvMap			= 0x20000000,	// Ec
};

struct MFMat_Standard_Data
{
	struct Texture
	{
		MFTexture  *pTexture;

		uint32		addressU	: 3;
		uint32		addressV	: 3;
		uint32		addressW	: 3;
		uint32		minFilter	: 3;
		uint32		magFilter	: 3;
		uint32		mipFilter	: 3;

		uint32		borderColour;
	};

	// Members
	MFMatrix	textureMatrix;

	MFVector	diffuse;
	MFVector	ambient;
	MFVector	specular;
	MFVector	illum;
	float		specularPow;

//	uint32		maskColour;
	float		alphaRef;

	uint32		materialType;

	Texture		textures[8];
	uint32		textureCount;

	int			uFrames, vFrames, curFrame;
	float		frameTime, curTime;

	uint32		diffuseMapIndex		: 2; // diffuse required to be one of the first 4 map's
	uint32		diffuseMap2Index	: 3;
	uint32		lightMapIndex		: 3;
	uint32		specularMapIndex	: 3;
	uint32		bumpMapIndex		: 3;
	uint32		detailMapIndex		: 3;
	uint32		envMapIndex			: 3;
	uint32		reflectionMapIndex	: 3;
	uint32		normalMapIndex		: 3;
	// glossMap?

	uint32		cubeMapIndex			: 3; // some what if's
	uint32		displacementMapIndex	: 3;

	uint16		opaque	: 1;
	uint16		flags	: 15;

	MFRasteriserState *pRasteriserState;
	MFDepthStencilState *pDepthStencilState;
	MFBlendState *pBlendState;
	MFSamplerState *pSamplerState[8];
};

int       MFMat_Standard_RegisterMaterial(void *pPlatformData);
void      MFMat_Standard_UnregisterMaterial();
void      MFMat_Standard_CreateInstance(MFMaterial *pMaterial);
void      MFMat_Standard_DestroyInstance(MFMaterial *pMaterial);
void      MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial);
void      MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial);
void      MFMat_Standard_Update(MFMaterial *pMaterial);
void      MFMat_Standard_BuildStateBlock(MFMaterial *pMaterial);
int       MFMat_Standard_Begin(MFMaterial *pMaterial);
void      MFMat_Standard_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, uintp value);
uintp     MFMat_Standard_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue);
int       MFMat_Standard_GetNumParams();
MFMaterialParameterInfo* MFMat_Standard_GetParameterInfo(int parameterIndex);

#endif
