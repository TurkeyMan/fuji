#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_NULL

#include "MFShader_Internal.h"

void MFRenderState_InitModulePlatformSpecific()
{
}

void MFRenderState_DeinitModulePlatformSpecific()
{
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader, MFShaderMacro *pMacros, const char *pSource, const char *pFilename, int line)
{
	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
}

#endif
