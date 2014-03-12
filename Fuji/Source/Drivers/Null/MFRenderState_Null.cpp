#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_NULL

#include "MFRenderState_Internal.h"

void MFRenderState_InitModulePlatformSpecific()
{
}

void MFRenderState_DeinitModulePlatformSpecific()
{
}

bool MFBlendState_CreatePlatformSpecific(MFBlendState *pBS)
{
	return true;
}

void MFBlendState_DestroyPlatformSpecific(MFBlendState *pBS)
{
}

bool MFSamplerState_CreatePlatformSpecific(MFSamplerState *pSS)
{
	return true;
}

void MFSamplerState_DestroyPlatformSpecific(MFSamplerState *pSS)
{
}

bool MFDepthStencilState_CreatePlatformSpecific(MFDepthStencilState *pDSS)
{
	return true;
}

void MFDepthStencilState_DestroyPlatformSpecific(MFDepthStencilState *pDSS)
{
}

bool MFRasteriserState_CreatePlatformSpecific(MFRasteriserState *pRS)
{
	return true;
}

void MFRasteriserState_DestroyPlatformSpecific(MFRasteriserState *pRS)
{
}

#endif
