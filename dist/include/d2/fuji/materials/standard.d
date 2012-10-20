module fuji.materials.standard;

public import fuji.material;

enum MFMatStandardParameters
{
	Unknown = -1,

	Lit = 0,
	Prelit,
	DiffuseColour,
	AmbientColour,
	SpecularColour,
	SpecularPower,
	EmissiveColour,
	Mask,
	AlphaRef,
	CullMode,
	ZRead,
	ZWrite,
	Blend,
	Texture,
	TextureFlags,
	CelShading,
	Phong,
	Animated,
	Tile,
	TextureMatrix
}

enum MFMatStandardTextures
{
	DifuseMap,
	DiffuseMap2,
	NormalMap,
	DetailMap,
	EnvMap,
	LightMap,
	BumpMap,
	ReflectionMap,
	SpecularMap,

	Texture
}

enum MFMatStandardTextureFlags
{
	AddressU,
	AddressV,
	AddressW,
	MagFilter,
	MinFilter,
	MipFilter,
	BorderColour
}

enum MFMatStandardTexAddress
{
	Wrap,
	Mirror,
	Clamp,
	Border,
	MirrorOnce
}

enum MFMatStandardTexFilter
{
	None,
	Point,
	Linear,
	Anisotropic
}

enum MFMatStandardBlendMode
{
	None,
	Alpha,
	Additive,
	Subtractive
}

enum MFMatStandardCullMode
{
	None,
	CCW,
	CW,
	Default // CCW
}

struct MFMatStandardAnimParams
{
	int hFrames;
	int vFrames;
	float frameTime;
}

struct MFMatStandardTileParams
{
	int hFrames;
	int vFrames;
	int hFrame;
	int vFrame;
}

