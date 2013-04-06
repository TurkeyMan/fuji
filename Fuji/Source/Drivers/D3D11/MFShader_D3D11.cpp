#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFShader_InitModulePlatformSpecific MFShader_InitModulePlatformSpecific_D3D11
	#define MFShader_DeinitModulePlatformSpecific MFShader_DeinitModulePlatformSpecific_D3D11
	#define MFShader_CreatePlatformSpecific MFShader_CreatePlatformSpecific_D3D11
	#define MFShader_DestroyPlatformSpecific MFShader_DestroyPlatformSpecific_D3D11
#endif

#include "MFShader_Internal.h"


void MFShader_InitModulePlatformSpecific()
{
}

void MFShader_DeinitModulePlatformSpecific()
{
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader)
{
	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
}

#endif // MF_RENDERER
