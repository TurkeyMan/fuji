#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFEffect_InitModulePlatformSpecific MFEffect_InitModulePlatformSpecific_OpenGL
	#define MFEffect_DeinitModulePlatformSpecific MFEffect_DeinitModulePlatformSpecific_OpenGL
	#define MFEffect_CreatePlatformSpecific MFEffect_CreatePlatformSpecific_OpenGL
	#define MFEffect_DestroyPlatformSpecific MFEffect_DestroyPlatformSpecific_OpenGL
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
