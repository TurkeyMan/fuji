#if !defined(_MATERIAL_H)
#define _MATERIAL_H

#include "Vector4.h"
#include "Texture.h"
#include "IniFile.h"
#include "Matrix.h"

enum RederTypeFlags
{
	// Material Flags
	RT_Lit					= 0x00000001,
	RT_AlphaBlend			= 0x00000002,
	RT_Additive				= 0x00000004,
	RT_Subtractive			= 0x00000006,
	RT_BlendMask			= 0x00000006,
	MF_Specular				= 0x00000008,

	RT_Omni					= 0x00000070,
	RT_Mask					= 0x00000080,

	RT_DoubleSided			= 0x00000100,

	// Vertex Flags
	RT_Animated				= 0x00000700,	// (Up to 8 bone influence)
	RT_Tween				= 0x00000800,

	// Renderer Flags
	RT_PerPixelLighting		= 0x00010000,
	RT_CelShading			= 0x00020000,
	RT_BumpMap				= 0x00040000,
	RT_NormalMap			= 0x00080000,
	RT_DetailTexture		= 0x00100000,
	RT_CubeEnvMap			= 0x00200000,
	RT_SphereEnvMap			= 0x00400000,
	RT_ScreenEnvMap			= 0x00800000,
	RT_DiffuseMap2			= 0x01000000,
	RT_LightMap				= 0x02000000,
};

enum MaterialFlags
{
	// Material Flags
	MF_Lit					= 0x00000001,	// L
	MF_AlphaBlend			= 0x00000002,	// Al
	MF_Additive				= 0x00000004,	// A
	MF_Subtractive			= 0x00000006,	// S
	MF_BlendMask			= 0x00000006,

	MF_Mask					= 0x00000020,	// M
	MF_DoubleSided			= 0x00000040,	// Ds
	MF_CullMode				= 0x000000C0,	// Ds
	MF_Animating			= 0x00000100,

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

class Material
{
public:
	// Static Methods
	static Material* CreateDefault();
	static Material* Create(const char *pName);
	static void CreateMaterialFromDefinition(Material *pMat, const char *pDefinition);
	inline static void UseNone() { pNone->Use(); }
	inline static Material* GetCurrent() { return &current; }
	static Material* Find(const char *pName);

	// Static Members
	static Material current;
	static Material *pNone;

	static IniFile materialDefinitions;

	// Methods
	void Release();
	void Use();

	void Update();

	char* GetIDString();

	// Members
	Vector4	diffuse;
	Vector4	ambient;
	Vector4	specular;
	Vector4	illum;
	float	specularPow;

	uint32	maskColour;

	uint32	materialType;

	Texture *pTextures[8];
	uint32	textureCount;

	Matrix	textureMatrix;
	int uFrames, vFrames, curFrame;
	float frameTime, curTime;

	char name[32];

	uint16	opaque	: 1;
	uint16	flags	: 15;

	uint16	refCount;

	uint32 diffuseMapIndex		: 2; // diffuse required to be one of the first 4 map's
	uint32 diffuseMap2Index		: 3;
	uint32 lightMapIndex		: 3;
	uint32 specularMapIndex		: 3;
	uint32 bumpMapIndex			: 3;
	uint32 detailMapIndex		: 3;
	uint32 envMapIndex			: 3;
	uint32 reflectionMapIndex	: 3;
	uint32 normalMapIndex		: 3;
	// glossMap?

	uint32 cubeMapIndex			: 3; // some what if's
	uint32 displacementMapIndex	: 3;
};

void Material_InitModule();
void Material_DeinitModule();

void Material_Update();

#endif
