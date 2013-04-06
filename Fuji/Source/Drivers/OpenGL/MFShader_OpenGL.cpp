#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFShader_InitModulePlatformSpecific MFShader_InitModulePlatformSpecific_OpenGL
	#define MFShader_DeinitModulePlatformSpecific MFShader_DeinitModulePlatformSpecific_OpenGL
	#define MFShader_CreatePlatformSpecific MFShader_CreatePlatformSpecific_OpenGL
	#define MFShader_DestroyPlatformSpecific MFShader_DestroyPlatformSpecific_OpenGL
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
