#if !defined(_MATERIAL_H)
#define _MATERIAL_H

enum MaterialFlags
{
	// Material Flags
	MF_Lit					= 0x00000001,	// L
	MF_Additive				= 0x00000002,	// A
	MF_AlphaBlend			= 0x00000004,	// Al

	MF_Omni					= 0x00000008,	// O
	MF_Mask					= 0x00000010,	// M

	MF_DoubleSided			= 0x00000020,	// Ds

	// Vertex Flags
	MF_Weighted				= 0x00000700,	// W (Up to 8 bone influence)

	// Renderer Flags
	MF_PerPixelLighting		= 0x00010000,	// P
	MF_CelShading			= 0x00020000,	// C
	MF_BumpMap				= 0x00040000,	// B
	MF_NormalMap			= 0x00080000,	// N
	MF_DetailTexture		= 0x00100000,	// D
	MF_CubeEnvMap			= 0x00200000,	// Ec
	MF_SphereEnvMap			= 0x00400000,	// E
	MF_ScreenEnvMap			= 0x00800000,	// Es
	MF_DiffuseMap2			= 0x01000000,	// D2
	MF_LightMap				= 0x02000000,	// Lm
	MF_Untextured			= 0x04000000,	// U
};

class Material
{
	void Use();
	static void UseNone();

	Vec4	diffuse;
	Vec4	ambient;
	Vec4	specular;
	Vec4	illum;
	float	specularPow;

	uint32	maskColour;

	uint32 materialType;

	Texture *pTextures[8];
	uint32 textureCount;

	char name[32];

	uint32 diffuseIndex			: 2; // diffuse required to be one of the first 4 map's
	uint32 diffuse2Index		: 3;
	uint32 lightMapIndex		: 3;
	uint32 specularIndex		: 3;
	uint32 bumpMapIndex			: 3;
	uint32 detailMapIndex		: 3;
	uint32 envMapIndex			: 3;
	uint32 reflectionMapIndex	: 3;
	uint32 normalIndex			: 3;

	uint32 CubeMapIndex			: 3; // some what if's
	uint32 displacementIndex	: 3;

#if defined(_WINDOWS)
	IDirect3DVertexShader9	*pVertexShader;
	IDirect3DPixelShader9	*pPixelShader;
#endif

};

#endif
