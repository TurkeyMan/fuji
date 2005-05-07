#if !defined(_MATERIAL_INTERNAL_H)
#define _MATERIAL_INTERNAL_H

#include "Material.h"
#include "Vector4.h"
#include "Texture.h"
#include "Matrix.h"

// material functions
void Material_InitModule();
void Material_DeinitModule();

void Material_Update();

// material structure
struct Material
{
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


#endif