#include "Fuji.h"

#include "MFMaterial_Internal.h"
#include "Materials/Mat_Standard.h"

static ParamType lit[] = { ParamType_Bool };
static ParamType prelit[] = { ParamType_Bool };
static ParamType diffusecolour[] = { ParamType_Vector4 };
static ParamType ambientcolour[] = { ParamType_Vector4 };
static ParamType specularcolour[] = { ParamType_Vector4 };
static ParamType specularpower[] = { ParamType_Float };
static ParamType emissivecolour[] = { ParamType_Vector4 };
static ParamType mask[] = { ParamType_Bool };
static ParamType doublesided[] = { ParamType_Bool };
static ParamType backfacecull[] = { ParamType_Bool };
static ParamType additive[] = { ParamType_Bool };
static ParamType subtractive[] = { ParamType_Bool };
static ParamType alpha[] = { ParamType_Bool };
static ParamType blend[] = { ParamType_Int };
static ParamType texture[] = { ParamType_String };
static ParamType diffusemap[] = { ParamType_String };
static ParamType diffusemap2[] = { ParamType_String };
static ParamType normalmap[] = { ParamType_String };
static ParamType detailmap[] = { ParamType_String };
static ParamType envmap[] = { ParamType_String };
static ParamType lightmap[] = { ParamType_String };
static ParamType bumpmap[] = { ParamType_String };
static ParamType reflectionmap[] = { ParamType_String };
static ParamType specularmap[] = { ParamType_String };
static ParamType celshading[] = { ParamType_Unknown };
static ParamType phong[] = { ParamType_Unknown };
static ParamType animated[] = { ParamType_Int, ParamType_Int, ParamType_Float };

MFMaterialParamaterInfo paramaterInformation[] =
{
	{ "lit", lit, sizeof(lit)/sizeof(ParamType) },
	{ "prelit", prelit, sizeof(prelit)/sizeof(ParamType) },
	{ "diffusecolour", diffusecolour, sizeof(diffusecolour)/sizeof(ParamType) },
	{ "ambientcolour", ambientcolour, sizeof(ambientcolour)/sizeof(ParamType) },
	{ "specularcolour", specularcolour, sizeof(specularcolour)/sizeof(ParamType) },
	{ "specularpower", specularpower, sizeof(specularpower)/sizeof(ParamType) },
	{ "emissivecolour", emissivecolour, sizeof(emissivecolour)/sizeof(ParamType) },
	{ "mask", mask, sizeof(mask)/sizeof(ParamType) },
	{ "doublesided", doublesided, sizeof(doublesided)/sizeof(ParamType) },
	{ "backfacecull", backfacecull, sizeof(backfacecull)/sizeof(ParamType) },
	{ "additive", additive, sizeof(additive)/sizeof(ParamType) },
	{ "subtractive", subtractive, sizeof(subtractive)/sizeof(ParamType) },
	{ "alpha", alpha, sizeof(alpha)/sizeof(ParamType) },
	{ "blend", blend, sizeof(blend)/sizeof(ParamType) },
	{ "texture", texture, sizeof(texture)/sizeof(ParamType) },
	{ "diffusemap", diffusemap, sizeof(diffusemap)/sizeof(ParamType) },
	{ "diffusemap2", diffusemap2, sizeof(diffusemap2)/sizeof(ParamType) },
	{ "normalmap", normalmap, sizeof(normalmap)/sizeof(ParamType) },
	{ "detailmap", detailmap, sizeof(detailmap)/sizeof(ParamType) },
	{ "envmap", envmap, sizeof(envmap)/sizeof(ParamType) },
	{ "lightmap", lightmap, sizeof(lightmap)/sizeof(ParamType) },
	{ "bumpmap", bumpmap, sizeof(bumpmap)/sizeof(ParamType) },
	{ "reflectionmap", reflectionmap, sizeof(reflectionmap)/sizeof(ParamType) },
	{ "specularmap", specularmap, sizeof(specularmap)/sizeof(ParamType) },
	{ "celshading", celshading, sizeof(celshading)/sizeof(ParamType) },
	{ "phong", phong, sizeof(phong)/sizeof(ParamType) },
	{ "animated", animated, sizeof(animated)/sizeof(ParamType) }
};

void Mat_Standard_Register()
{
	MFMaterialCallbacks matStandardCallbacks;

	matStandardCallbacks.pRegisterMaterial = Mat_Standard_RegisterMaterial;
	matStandardCallbacks.pUnregisterMaterial = Mat_Standard_UnregisterMaterial;
	matStandardCallbacks.pCreateInstance = Mat_Standard_CreateInstance;
	matStandardCallbacks.pDestroyInstance = Mat_Standard_DestroyInstance;
	matStandardCallbacks.pBegin = Mat_Standard_Begin;
	matStandardCallbacks.pUpdate = Mat_Standard_Update;
	matStandardCallbacks.pSetParameter = Mat_Standard_SetParameter;
	matStandardCallbacks.pGetParameter = Mat_Standard_GetParameter;
	matStandardCallbacks.pGetNumParams = Mat_Standard_GetNumParams;
	matStandardCallbacks.pGetParamaterInfo = Mat_Standard_GetParamaterInfo;

	MFMaterial_RegisterMaterialType("Standard", &matStandardCallbacks);
}

void Mat_Standard_Update(MFMaterial *pMaterial)
{
	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	if(pData->materialType & MF_Animating)
	{
		pData->curTime += MFSystem_TimeDelta();

		while(pData->curTime >= pData->frameTime)
		{
			pData->curTime -= pData->frameTime;

			pData->curFrame++;
			pData->curFrame = pData->curFrame % (pData->uFrames*pData->vFrames);

			pData->textureMatrix.SetZAxis3(MakeVector((1.0f/(float)pData->uFrames) * (float)(pData->curFrame%pData->uFrames), (1.0f/(float)pData->vFrames) * (float)(pData->curFrame/pData->vFrames), 0.0f));
		}
	}
}

void Mat_Standard_SetParameter(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 paramater)
{
	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	switch(paramaterIndex)
	{
		case 0: // lit
			pData->materialType = (pData->materialType & ~MF_Lit) | (paramater ? MF_Lit : 0);
			break;
		case 1: // prelit
			pData->materialType = (pData->materialType & ~MF_Lit) | (!paramater ? MF_Lit : 0);
			break;
		case 2: // diffusecolour
			pData->diffuse = *(MFVector*)paramater;
			break;
		case 3: // ambientcolour
			pData->ambient = *(MFVector*)paramater;
			break;
		case 4: // specularcolour
			pData->specular = *(MFVector*)paramater;
			break;
		case 5: // specularpower
			pData->specularPow = *(float*)paramater;
			break;
		case 6: // emissivecolour
			pData->illum = *(MFVector*)paramater;
			break;
		case 7: // mask
			pData->materialType = (pData->materialType & ~MF_Mask) | (paramater ? MF_Mask : 0);
			break;
		case 8: // doublesided
			pData->materialType = (pData->materialType & ~MF_DoubleSided) | (paramater ? MF_DoubleSided : 0);
			break;
		case 9: // backfacecull
			pData->materialType = (pData->materialType & ~MF_DoubleSided) | (!paramater ? MF_DoubleSided : 0);
			break;
		case 10: // additive
			pData->materialType = (pData->materialType & ~MF_BlendMask) | (paramater ? MF_Additive : 0);
			break;
		case 11: // subtractive
			pData->materialType = (pData->materialType & ~MF_BlendMask) | (paramater ? MF_Subtractive : 0);
			break;
		case 12: // alpha
			pData->materialType = (pData->materialType & ~MF_BlendMask) | (paramater ? MF_AlphaBlend : 0);
			break;
		case 13: // blend
			pData->materialType = (pData->materialType & ~MF_BlendMask) | ((int)paramater << 1);
			break;
		case 14: // texture
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->textureCount++;
			break;
		case 15: // diffusemap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->diffuseMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 16: // diffusemap2
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->diffuseMap2Index = pData->textureCount;
			pData->textureCount++;
			break;
		case 17: // normalmap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->normalMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 18: // detailmap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->detailMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 19: // envmap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->envMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 20: // lightmap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->lightMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 21: // bumpmap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->bumpMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 22: // reflectionmap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->reflectionMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 23: // specularmap
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->specularMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case 24: // celshading
			pData->materialType |= MF_CelShading;
			break;
		case 25: // phong
			pData->materialType |= MF_LitPerPixel;
			break;
		case 26: // animated
		{
			pData->materialType |= MF_Animating;

			switch(argIndex)
			{
				case 0:	// uFrames
					pData->uFrames = (int)paramater;
					break;
				case 1:	// vFrames
					pData->vFrames = (int)paramater;
					break;
				case 2:	// frameTime
					pData->frameTime = (float&)paramater;
					break;
			}

			pData->textureMatrix.SetScale(MakeVector(1.0f/(float)pData->uFrames, 1.0f/(float)pData->vFrames, 1.0f));
			break;
		}
	}
}

uint32 Mat_Standard_GetParameter(MFMaterial *pMaterial, int paramaterIndex, int argIndex)
{
//	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;
	DBGASSERT(false, "Not Written");

	return 0;
}

int Mat_Standard_GetNumParams()
{
	return sizeof(paramaterInformation)/sizeof(MFMaterialParamaterInfo);
}

MFMaterialParamaterInfo* Mat_Standard_GetParamaterInfo(int paramaterIndex)
{
	DBGASSERT((uint32)paramaterIndex < sizeof(paramaterInformation)/sizeof(MFMaterialParamaterInfo), STR("Invalid paramater id %d.", paramaterIndex));

	return &paramaterInformation[paramaterIndex];
}

