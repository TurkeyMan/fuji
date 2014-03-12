#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFRenderState.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFRenderState_InitModulePlatformSpecific_##driver(); \
	void MFRenderState_DeinitModulePlatformSpecific_##driver(); \
	bool MFBlendState_CreatePlatformSpecific_##driver(MFBlendState *pBS); \
	void MFBlendState_DestroyPlatformSpecific_##driver(MFBlendState *pBS); \
	bool MFSamplerState_CreatePlatformSpecific_##driver(MFSamplerState *pSS); \
	void MFSamplerState_DestroyPlatformSpecific_##driver(MFSamplerState *pSS); \
	bool MFDepthStencilState_CreatePlatformSpecific_##driver(MFDepthStencilState *pDSS); \
	void MFDepthStencilState_DestroyPlatformSpecific_##driver(MFDepthStencilState *pDSS); \
	bool MFRasteriserState_CreatePlatformSpecific_##driver(MFRasteriserState *pRS); \
	void MFRasteriserState_DestroyPlatformSpecific_##driver(MFRasteriserState *pRS);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFRenderState_InitModulePlatformSpecific_##driver, \
		MFRenderState_DeinitModulePlatformSpecific_##driver, \
		MFBlendState_CreatePlatformSpecific_##driver, \
		MFBlendState_DestroyPlatformSpecific_##driver, \
		MFSamplerState_CreatePlatformSpecific_##driver, \
		MFSamplerState_DestroyPlatformSpecific_##driver, \
		MFDepthStencilState_CreatePlatformSpecific_##driver, \
		MFDepthStencilState_DestroyPlatformSpecific_##driver, \
		MFRasteriserState_CreatePlatformSpecific_##driver, \
		MFRasteriserState_DestroyPlatformSpecific_##driver, \
	},

// declare the available plugins
#if defined(MF_RENDERPLUGIN_D3D9)
	DECLARE_PLUGIN_CALLBACKS(D3D9)
#endif
#if defined(MF_RENDERPLUGIN_D3D11)
	DECLARE_PLUGIN_CALLBACKS(D3D11)
#endif
#if defined(MF_RENDERPLUGIN_OPENGL)
	DECLARE_PLUGIN_CALLBACKS(OpenGL)
#endif

// list of plugins
struct MFRenderStatePluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	bool (*pBlendState_CreatePlatformSpecific)(MFBlendState *pBS);
	void (*pBlendState_DestroyPlatformSpecific)(MFBlendState *pBS);
	bool (*pSamplerState_CreatePlatformSpecific)(MFSamplerState *pSS);
	void (*pSamplerState_DestroyPlatformSpecific)(MFSamplerState *pSS);
	bool (*pDepthStencilState_CreatePlatformSpecific)(MFDepthStencilState *pDSS);
	void (*pDepthStencilState_DestroyPlatformSpecific)(MFDepthStencilState *pDSS);
	bool (*pRasteriserState_CreatePlatformSpecific)(MFRasteriserState *pRS);
	void (*pRasteriserState_DestroyPlatformSpecific)(MFRasteriserState *pRS);
};

// create an array of actual callbacks to the various enabled plugins
MFRenderStatePluginCallbacks gRenderStatePlugins[] =
{
#if defined(MF_RENDERPLUGIN_D3D9)
	DEFINE_PLUGIN(D3D9)
#endif
#if defined(MF_RENDERPLUGIN_D3D11)
	DEFINE_PLUGIN(D3D11)
#endif
#if defined(MF_RENDERPLUGIN_OPENGL)
	DEFINE_PLUGIN(OpenGL)
#endif
};

// the current active plugin
MFRenderStatePluginCallbacks *gpCurrentRenderStatePlugin = NULL;


/*** Function Wrappers ***/

void MFRenderState_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentRenderStatePlugin = &gRenderStatePlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentRenderStatePlugin->pInitModulePlatformSpecific();
}

void MFRenderState_DeinitModulePlatformSpecific()
{
	gpCurrentRenderStatePlugin->pDeinitModulePlatformSpecific();
}

bool MFBlendState_CreatePlatformSpecific(MFBlendState *pBS)
{
	return gpCurrentRenderStatePlugin->pBlendState_CreatePlatformSpecific(pBS);
}

void MFBlendState_DestroyPlatformSpecific(MFBlendState *pBS)
{
	gpCurrentRenderStatePlugin->pBlendState_DestroyPlatformSpecific(pBS);
}

bool MFSamplerState_CreatePlatformSpecific(MFSamplerState *pSS)
{
	return gpCurrentRenderStatePlugin->pSamplerState_CreatePlatformSpecific(pSS);
}

void MFSamplerState_DestroyPlatformSpecific(MFSamplerState *pSS)
{
	gpCurrentRenderStatePlugin->pSamplerState_DestroyPlatformSpecific(pSS);
}

bool MFDepthStencilState_CreatePlatformSpecific(MFDepthStencilState *pDSS)
{
	return gpCurrentRenderStatePlugin->pDepthStencilState_CreatePlatformSpecific(pDSS);
}

void MFDepthStencilState_DestroyPlatformSpecific(MFDepthStencilState *pDSS)
{
	gpCurrentRenderStatePlugin->pDepthStencilState_DestroyPlatformSpecific(pDSS);
}

bool MFRasteriserState_CreatePlatformSpecific(MFRasteriserState *pRS)
{
	return gpCurrentRenderStatePlugin->pRasteriserState_CreatePlatformSpecific(pRS);
}

void MFRasteriserState_DestroyPlatformSpecific(MFRasteriserState *pRS)
{
	gpCurrentRenderStatePlugin->pRasteriserState_DestroyPlatformSpecific(pRS);
}

#endif // MF_RENDERER
