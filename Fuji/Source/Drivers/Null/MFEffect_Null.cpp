#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_NULL

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
