#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_NULL

#include "MFMaterial_Internal.h"

int MFMat_Effect_RegisterMaterial(MFMaterialType *pType)
{
	return 0;
}

void MFMat_Effect_UnregisterMaterial()
{
}

int MFMat_Effect_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	return 0;
}

void MFMat_Effect_CreateInstance(MFMaterial *pMaterial)
{
	pMaterial->pInstanceData = NULL;
}

void MFMat_Effect_DestroyInstance(MFMaterial *pMaterial)
{
}

void MFMat_Effect_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, size_t value)
{
}

uintp MFMat_Effect_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue)
{
	return 0;
}

int MFMat_Effect_GetNumParams()
{
	return 0;
}

MFMaterialParameterInfo* MFMat_Effect_GetParameterInfo(int parameterIndex)
{
	return NULL;
}

#endif
