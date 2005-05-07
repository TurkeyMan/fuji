#if !defined(_MATERIAL_H)
#define _MATERIAL_H

#include "IniFile.h"

struct Material;

// interface functions
int			Material_AddDefinitionsFile(const char *pName, const char *pFilename);
int			Material_AddDefinitionsFileInMemory(const char *pName, const char *pBuffer);
int			Material_AddDefinitionsIniFile(const char *pName, IniFile *pMatDefs);
void		Material_RemoveDefinitions(const char *pName);

Material*	Material_Create(const char *pName);
int			Material_Destroy(Material *pMaterial);
Material*	Material_Find(const char *pName);

Material*	Material_GetCurrent();

void		Material_Use(Material *pMaterial);
void		Material_UseWhite();

// some enums
enum RederTypeFlags
{
	// Vertex Flags
	RT_Animated				= 0x00000007,	// (Up to 8 bone influence)
	RT_Tween				= 0x00000008,

	RT_Omni					= 0x00000010,

	RT_Untextured			= 0x00000020
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

#endif
