#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9

#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"
#include "../MFRenderer_D3D9.h"
#include "Materials/MFMat_Effect.h"

static MFMaterial *pSetMaterial;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

extern IDirect3DDevice9 *pd3dDevice;

int MFMat_Effect_RegisterMaterial(MFMaterialType *pType)
{
	MFCALLSTACK;

	return 0;
}

void MFMat_Effect_UnregisterMaterial()
{
	MFCALLSTACK;

}

int MFMat_Effect_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	MFCALLSTACK;

//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

	return 0;
}

void MFMat_Effect_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Effect_Data));
	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;
	MFZeroMemory(pData, sizeof(MFMat_Effect_Data));
}

void MFMat_Effect_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
	MFCALLSTACK;

//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;
/*
	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->pTextures[a]);
	}
*/
	MFHeap_Free(pMaterial->pInstanceData);
}

void MFMat_Effect_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, const void *pValue)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;
}

uint32 MFMat_Effect_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;
	MFDebug_Assert(false, "Not Written");

	return 0;
}

int MFMat_Effect_GetNumParams()
{
	return 0;
}

MFMaterialParameterInfo* MFMat_Effect_GetParameterInfo(int parameterIndex)
{
/*
	MFDebug_Assert((uint32)parameterIndex < sizeof(parameterInformation)/sizeof(MFMaterialParameterInfo), MFStr("Invalid parameter id %d.", parameterIndex));

	return &parameterInformation[parameterIndex];
*/
	return NULL;
}

#endif // MF_RENDERER
