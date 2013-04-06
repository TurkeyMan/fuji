#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFRenderState_InitModulePlatformSpecific MFRenderState_InitModulePlatformSpecific_OpenGL
	#define MFRenderState_DeinitModulePlatformSpecific MFRenderState_DeinitModulePlatformSpecific_OpenGL
	#define MFBlendState_CreatePlatformSpecific MFBlendState_CreatePlatformSpecific_OpenGL
	#define MFBlendState_DestroyPlatformSpecific MFBlendState_DestroyPlatformSpecific_OpenGL
	#define MFSamplerState_CreatePlatformSpecific MFSamplerState_CreatePlatformSpecific_OpenGL
	#define MFSamplerState_DestroyPlatformSpecific MFSamplerState_DestroyPlatformSpecific_OpenGL
	#define MFDepthStencilState_CreatePlatformSpecific MFDepthStencilState_CreatePlatformSpecific_OpenGL
	#define MFDepthStencilState_DestroyPlatformSpecific MFDepthStencilState_DestroyPlatformSpecific_OpenGL
	#define MFRasteriserState_CreatePlatformSpecific MFRasteriserState_CreatePlatformSpecific_OpenGL
	#define MFRasteriserState_DestroyPlatformSpecific MFRasteriserState_DestroyPlatformSpecific_OpenGL
#endif

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

#endif // MF_RENDERER
