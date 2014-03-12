#include "Fuji_Internal.h"
#include "MFSystem.h"

#include "MFRenderState.h"
#include "MFMaterial_Internal.h"
#include "MFTexture_Internal.h"
#include "Materials/MFMat_Effect.h"

void MFMat_Effect_Register()
{
	MFMaterialCallbacks matEffectCallbacks;

	matEffectCallbacks.pRegisterMaterial = MFMat_Effect_RegisterMaterial;
	matEffectCallbacks.pUnregisterMaterial = MFMat_Effect_UnregisterMaterial;
	matEffectCallbacks.pCreateInstance = MFMat_Effect_CreateInstance;
	matEffectCallbacks.pDestroyInstance = MFMat_Effect_DestroyInstance;
	matEffectCallbacks.pUpdate = MFMat_Effect_Update;
	matEffectCallbacks.pBuildStateBlock = MFMat_Effect_BuildStateBlock;
	matEffectCallbacks.pBegin = MFMat_Effect_Begin;
	matEffectCallbacks.pSetParameter = MFMat_Effect_SetParameter;
	matEffectCallbacks.pGetParameter = MFMat_Effect_GetParameter;
	matEffectCallbacks.pGetNumParams = MFMat_Effect_GetNumParams;
	matEffectCallbacks.pGetParameterInfo = MFMat_Effect_GetParameterInfo;

	MFMaterial_RegisterMaterialType("Effect", &matEffectCallbacks, sizeof(MFMat_Effect_Data));
}

void MFMat_Effect_CreateInstance(MFMaterial *pMaterial)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

	MFMat_Effect_CreateInstancePlatformSpecific(pMaterial);
}

void MFMat_Effect_DestroyInstance(MFMaterial *pMaterial)
{
	MFMat_Effect_DestroyInstancePlatformSpecific(pMaterial);

//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

}

void MFMat_Effect_Update(MFMaterial *pMaterial)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

}

void MFMat_Effect_BuildStateBlock(MFMaterial *pMaterial)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

	MFStateBlock_Clear(pMaterial->pMaterialState);

}

void MFMat_Effect_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, uintp value)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

}

uintp MFMat_Effect_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

	return 0;
}

int MFMat_Effect_GetNumParams()
{
//	return sizeof(parameterInformation)/sizeof(MFMaterialParameterInfo);

	return 0;
}

MFMaterialParameterInfo* MFMat_Effect_GetParameterInfo(int parameterIndex)
{
//	MFDebug_Assert((uint32)parameterIndex < sizeof(parameterInformation)/sizeof(MFMaterialParameterInfo), MFStr("Invalid parameter id %d.", parameterIndex));

//	return &parameterInformation[parameterIndex];

	return NULL;
}
