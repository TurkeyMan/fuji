#include "Fuji.h"
#include "MFSystem.h"

#include "MFMaterial_Internal.h"
#include "Materials/MFMat_Effect.h"

void MFMat_Effect_Register()
{
	MFMaterialCallbacks matEffectCallbacks;

	matEffectCallbacks.pRegisterMaterial = MFMat_Effect_RegisterMaterial;
	matEffectCallbacks.pUnregisterMaterial = MFMat_Effect_UnregisterMaterial;
	matEffectCallbacks.pCreateInstance = MFMat_Effect_CreateInstance;
	matEffectCallbacks.pDestroyInstance = MFMat_Effect_DestroyInstance;
	matEffectCallbacks.pBegin = MFMat_Effect_Begin;
	matEffectCallbacks.pUpdate = MFMat_Effect_Update;
	matEffectCallbacks.pSetParameter = MFMat_Effect_SetParameter;
	matEffectCallbacks.pGetParameter = MFMat_Effect_GetParameter;
	matEffectCallbacks.pGetNumParams = MFMat_Effect_GetNumParams;
	matEffectCallbacks.pGetParameterInfo = MFMat_Effect_GetParameterInfo;

	MFMaterial_RegisterMaterialType("Effect", &matEffectCallbacks);
}

void MFMat_Effect_Update(MFMaterial *pMaterial)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;
}
