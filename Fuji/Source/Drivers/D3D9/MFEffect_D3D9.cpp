#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFEffect_InitModulePlatformSpecific MFEffect_InitModulePlatformSpecific_D3D9
	#define MFEffect_DeinitModulePlatformSpecific MFEffect_DeinitModulePlatformSpecific_D3D9
	#define MFEffect_PlatformDataSize MFEffect_PlatformDataSize_D3D9
	#define MFEffect_CreatePlatformSpecific MFEffect_CreatePlatformSpecific_D3D9
	#define MFEffect_DestroyPlatformSpecific MFEffect_DestroyPlatformSpecific_D3D9
#endif

#include "MFEffect_Internal.h"

//extern IDirect3DDevice9 *pd3dDevice;


void MFEffect_InitModulePlatformSpecific()
{
}

void MFEffect_DeinitModulePlatformSpecific()
{
}

size_t MFEffect_PlatformDataSize()
{
	return 0;
}

bool MFEffect_CreatePlatformSpecific(MFEffect *pEffect)
{
	return true;
}

void MFEffect_DestroyPlatformSpecific(MFEffect *pEffect)
{
}

#endif // MF_RENDERER
