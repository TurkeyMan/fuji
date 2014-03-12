#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFMat_Effect_RegisterMaterial MFMat_Effect_RegisterMaterial_D3D11
	#define MFMat_Effect_UnregisterMaterial MFMat_Effect_UnregisterMaterial_D3D11
	#define MFMat_Effect_Begin MFMat_Effect_Begin_D3D11
	#define MFMat_Effect_CreateInstancePlatformSpecific MFMat_Effect_CreateInstancePlatformSpecific_D3D11
	#define MFMat_Effect_DestroyInstancePlatformSpecific MFMat_Effect_DestroyInstancePlatformSpecific_D3D11
#endif

#include "MFRenderState_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "../MFRenderer_D3D11.h"
#include "Materials/MFMat_Effect.h"
#include "MFEffect_Internal.h"

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;

int MFMat_Effect_RegisterMaterial(MFMaterialType *pType)
{
	// create default shaders
	//...

	return 0;
}

void MFMat_Effect_UnregisterMaterial()
{
	MFCALLSTACK;

}

int MFMat_Effect_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

	return 0;
}

void MFMat_Effect_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

void MFMat_Effect_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
}


#endif // MF_RENDERER
