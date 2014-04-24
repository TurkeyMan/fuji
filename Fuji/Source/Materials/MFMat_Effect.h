#pragma once
#if !defined(_MFMAT_EFFECT_H)
#define _MFMAT_EFFECT_H

#include "MFEffect.h"

struct MFMat_Effect_Data
{
	MFEffect *pEffect;
};

int       MFMat_Effect_RegisterMaterial(MFMaterialType *pType);
void      MFMat_Effect_UnregisterMaterial();
void      MFMat_Effect_CreateInstance(MFMaterial *pMaterial);
void      MFMat_Effect_DestroyInstance(MFMaterial *pMaterial);
void      MFMat_Effect_CreateInstancePlatformSpecific(MFMaterial *pMaterial);
void      MFMat_Effect_DestroyInstancePlatformSpecific(MFMaterial *pMaterial);
void      MFMat_Effect_Update(MFMaterial *pMaterial);
void      MFMat_Effect_BuildStateBlock(MFMaterial *pMaterial);
int       MFMat_Effect_Begin(MFMaterial *pMaterial, MFRendererState &state);
void      MFMat_Effect_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, size_t value);
uintp     MFMat_Effect_GetParameter(const MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue);
int       MFMat_Effect_GetNumParams();
const MFMaterialParameterInfo* MFMat_Effect_GetParameterInfo(int parameterIndex);

#endif // _MFMAT_EFFECT_H
