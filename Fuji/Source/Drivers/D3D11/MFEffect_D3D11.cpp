#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFEffect_InitModulePlatformSpecific MFEffect_InitModulePlatformSpecific_D3D11
	#define MFEffect_DeinitModulePlatformSpecific MFEffect_DeinitModulePlatformSpecific_D3D11
	#define MFEffect_CreatePlatformSpecific MFEffect_CreatePlatformSpecific_D3D11
	#define MFEffect_DestroyPlatformSpecific MFEffect_DestroyPlatformSpecific_D3D11
#endif

#include "MFEffect_Internal.h"


void MFEffect_InitModulePlatformSpecific()
{
}

void MFEffect_DeinitModulePlatformSpecific()
{
}

bool MFEffect_CreatePlatformSpecific(MFEffect *pEffect)
{
	return true;
}

void MFEffect_DestroyPlatformSpecific(MFEffect *pEffect)
{
}

#endif // MF_RENDERER
