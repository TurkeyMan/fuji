#include "Fuji.h"
#include "MFMaterial_Internal.h"

int MFMat_Effect_RegisterMaterial(void *pPlatformData)
{
	return 0;
}

void MFMat_Effect_UnregisterMaterial()
{
}

int MFMat_Effect_Begin(MFMaterial *pMaterial)
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

void MFMat_Effect_SetParameter(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 paramater)
{
}

uint32 MFMat_Effect_GetParameter(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 *pValue)
{
	return 0;
}

int MFMat_Effect_GetNumParams()
{
	return 0;
}

MFMaterialParamaterInfo* MFMat_Effect_GetParamaterInfo(int paramaterIndex)
{
	return NULL;
}
