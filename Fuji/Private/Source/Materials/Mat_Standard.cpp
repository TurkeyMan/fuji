#include "Fuji.h"

#include "MFMaterial_Internal.h"
#include "Materials/Mat_Standard.h"

static MFParamType lit[] = { MFParamType_Bool };
static MFParamType prelit[] = { MFParamType_Bool };
static MFParamType diffusecolour[] = { MFParamType_Vector4 };
static MFParamType ambientcolour[] = { MFParamType_Vector4 };
static MFParamType specularcolour[] = { MFParamType_Vector4 };
static MFParamType specularpower[] = { MFParamType_Float };
static MFParamType emissivecolour[] = { MFParamType_Vector4 };
static MFParamType mask[] = { MFParamType_Bool };
static MFParamType doublesided[] = { MFParamType_Bool };
static MFParamType backfacecull[] = { MFParamType_Bool };
static MFParamType additive[] = { MFParamType_Bool };
static MFParamType subtractive[] = { MFParamType_Bool };
static MFParamType alpha[] = { MFParamType_Bool };
static MFParamType blend[] = { MFParamType_Int };
static MFParamType texture[] = { MFParamType_String };
static MFParamType diffusemap[] = { MFParamType_String };
static MFParamType diffusemap2[] = { MFParamType_String };
static MFParamType normalmap[] = { MFParamType_String };
static MFParamType detailmap[] = { MFParamType_String };
static MFParamType envmap[] = { MFParamType_String };
static MFParamType lightmap[] = { MFParamType_String };
static MFParamType bumpmap[] = { MFParamType_String };
static MFParamType reflectionmap[] = { MFParamType_String };
static MFParamType specularmap[] = { MFParamType_String };
static MFParamType celshading[] = { MFParamType_Unknown };
static MFParamType phong[] = { MFParamType_Unknown };
static MFParamType animated[] = { MFParamType_Int, MFParamType_Int, MFParamType_Float };

MFMaterialParamaterInfo paramaterInformation[] =
{
	{ "lit", lit, sizeof(lit)/sizeof(MFParamType) },
	{ "prelit", prelit, sizeof(prelit)/sizeof(MFParamType) },
	{ "diffusecolour", diffusecolour, sizeof(diffusecolour)/sizeof(MFParamType) },
	{ "ambientcolour", ambientcolour, sizeof(ambientcolour)/sizeof(MFParamType) },
	{ "specularcolour", specularcolour, sizeof(specularcolour)/sizeof(MFParamType) },
	{ "specularpower", specularpower, sizeof(specularpower)/sizeof(MFParamType) },
	{ "emissivecolour", emissivecolour, sizeof(emissivecolour)/sizeof(MFParamType) },
	{ "mask", mask, sizeof(mask)/sizeof(MFParamType) },
	{ "doublesided", doublesided, sizeof(doublesided)/sizeof(MFParamType) },
	{ "backfacecull", backfacecull, sizeof(backfacecull)/sizeof(MFParamType) },
	{ "additive", additive, sizeof(additive)/sizeof(MFParamType) },
	{ "subtractive", subtractive, sizeof(subtractive)/sizeof(MFParamType) },
	{ "alpha", alpha, sizeof(alpha)/sizeof(MFParamType) },
	{ "blend", blend, sizeof(blend)/sizeof(MFParamType) },
	{ "texture", texture, sizeof(texture)/sizeof(MFParamType) },
	{ "diffusemap", diffusemap, sizeof(diffusemap)/sizeof(MFParamType) },
	{ "diffusemap2", diffusemap2, sizeof(diffusemap2)/sizeof(MFParamType) },
	{ "normalmap", normalmap, sizeof(normalmap)/sizeof(MFParamType) },
	{ "detailmap", detailmap, sizeof(detailmap)/sizeof(MFParamType) },
	{ "envmap", envmap, sizeof(envmap)/sizeof(MFParamType) },
	{ "lightmap", lightmap, sizeof(lightmap)/sizeof(MFParamType) },
	{ "bumpmap", bumpmap, sizeof(bumpmap)/sizeof(MFParamType) },
	{ "reflectionmap", reflectionmap, sizeof(reflectionmap)/sizeof(MFParamType) },
	{ "specularmap", specularmap, sizeof(specularmap)/sizeof(MFParamType) },
	{ "celshading", celshading, sizeof(celshading)/sizeof(MFParamType) },
	{ "phong", phong, sizeof(phong)/sizeof(MFParamType) },
	{ "animated", animated, sizeof(animated)/sizeof(MFParamType) }
};

void MFMat_Standard_Register()
{
	MFMaterialCallbacks matStandardCallbacks;

	matStandardCallbacks.pRegisterMaterial = MFMat_Standard_RegisterMaterial;
	matStandardCallbacks.pUnregisterMaterial = MFMat_Standard_UnregisterMaterial;
	matStandardCallbacks.pCreateInstance = MFMat_Standard_CreateInstance;
	matStandardCallbacks.pDestroyInstance = MFMat_Standard_DestroyInstance;
	matStandardCallbacks.pBegin = MFMat_Standard_Begin;
	matStandardCallbacks.pUpdate = MFMat_Standard_Update;
	matStandardCallbacks.pSetParameter = MFMat_Standard_SetParameter;
	matStandardCallbacks.pGetParameter = MFMat_Standard_GetParameter;
	matStandardCallbacks.pGetNumParams = MFMat_Standard_GetNumParams;
	matStandardCallbacks.pGetParamaterInfo = MFMat_Standard_GetParamaterInfo;

	MFMaterial_RegisterMaterialType("Standard", &matStandardCallbacks);
}

void MFMat_Standard_Update(MFMaterial *pMaterial)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

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

void MFMat_Standard_SetParameter(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 paramater)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	switch(paramaterIndex)
	{
		case MFMatStandard_Lit:
			pData->materialType = (pData->materialType & ~MF_Lit) | (paramater ? MF_Lit : 0);
			break;
		case MFMatStandard_Prelit:
			pData->materialType = (pData->materialType & ~MF_Lit) | (!paramater ? MF_Lit : 0);
			break;
		case MFMatStandard_DiffuseColour:
			pData->diffuse = *(MFVector*)paramater;
			break;
		case MFMatStandard_AmbientColour:
			pData->ambient = *(MFVector*)paramater;
			break;
		case MFMatStandard_SpecularColour:
			pData->specular = *(MFVector*)paramater;
			break;
		case MFMatStandard_SpecularPower:
			pData->specularPow = *(float*)paramater;
			break;
		case MFMatStandard_EmissiveColour:
			pData->illum = *(MFVector*)paramater;
			break;
		case MFMatStandard_Mask:
			pData->materialType = (pData->materialType & ~MF_Mask) | (paramater ? MF_Mask : 0);
			break;
		case MFMatStandard_DoubleSided:
			pData->materialType = (pData->materialType & ~MF_DoubleSided) | (paramater ? MF_DoubleSided : 0);
			break;
		case MFMatStandard_BackfaceCull:
			pData->materialType = (pData->materialType & ~MF_DoubleSided) | (!paramater ? MF_DoubleSided : 0);
			break;
		case MFMatStandard_Additive:
			pData->materialType = (pData->materialType & ~MF_BlendMask) | (paramater ? MF_Additive : 0);
			break;
		case MFMatStandard_Subtractive:
			pData->materialType = (pData->materialType & ~MF_BlendMask) | (paramater ? MF_Subtractive : 0);
			break;
		case MFMatStandard_Alpha:
			pData->materialType = (pData->materialType & ~MF_BlendMask) | (paramater ? MF_AlphaBlend : 0);
			break;
		case MFMatStandard_Blend:
			pData->materialType = (pData->materialType & ~MF_BlendMask) | ((int)paramater << 1);
			break;
		case MFMatStandard_Texture:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->textureCount++;
			break;
		case MFMatStandard_DifuseMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->diffuseMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_DiffuseMap2:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->diffuseMap2Index = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_NormalMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->normalMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_DetailMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->detailMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_EnvMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->envMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_LightMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->lightMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_BumpMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->bumpMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_ReflectionMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->reflectionMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_SpecularMap:
			pData->pTextures[pData->textureCount] = MFTexture_Create((const char *)paramater);
			pData->specularMapIndex = pData->textureCount;
			pData->textureCount++;
			break;
		case MFMatStandard_CelShading:
			pData->materialType |= MF_CelShading;
			break;
		case MFMatStandard_Phong:
			pData->materialType |= MF_LitPerPixel;
			break;
		case MFMatStandard_Animated:
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

uint32 MFMat_Standard_GetParameter(MFMaterial *pMaterial, int paramaterIndex, int argIndex)
{
//	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;
	MFDebug_Assert(false, "Not Written");

	return 0;
}

int MFMat_Standard_GetNumParams()
{
	return sizeof(paramaterInformation)/sizeof(MFMaterialParamaterInfo);
}

MFMaterialParamaterInfo* MFMat_Standard_GetParamaterInfo(int paramaterIndex)
{
	MFDebug_Assert((uint32)paramaterIndex < sizeof(paramaterInformation)/sizeof(MFMaterialParamaterInfo), MFStr("Invalid paramater id %d.", paramaterIndex));

	return &paramaterInformation[paramaterIndex];
}

