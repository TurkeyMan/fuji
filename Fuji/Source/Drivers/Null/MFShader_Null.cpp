#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_NULL

#include "MFShader_Internal.h"

void MFRenderState_InitModulePlatformSpecific()
{
}

void MFRenderState_DeinitModulePlatformSpecific()
{
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader)
{
	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
}

#endif
