#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFRenderTarget_InitModulePlatformSpecific MFRenderTarget_InitModulePlatformSpecific_D3D11
	#define MFRenderTarget_DeinitModulePlatformSpecific MFRenderTarget_DeinitModulePlatformSpecific_D3D11
	#define MFRenderTarget_CreatePlatformSpecific MFRenderTarget_CreatePlatformSpecific_D3D11
	#define MFRenderTarget_DestroyPlatformSpecific MFRenderTarget_DestroyPlatformSpecific_D3D11
#endif


/**** Defines ****/

/**** Includes ****/

#include "MFRenderTarget_Internal.h"


/**** Globals ****/

/**** Functions ****/

bool MFRenderTarget_InitModulePlatformSpecific()
{
	return true;
}

void MFRenderTarget_DeinitModulePlatformSpecific()
{
}

bool MFRenderTarget_CreatePlatformSpecific(MFRenderTarget *pRenderTarget)
{
	return true;
}

void MFRenderTarget_DestroyPlatformSpecific(MFRenderTarget *pRenderTarget)
{
}

#endif // MF_RENDERER
