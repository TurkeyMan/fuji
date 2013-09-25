#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFRenderTarget.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	bool MFRenderTarget_InitModulePlatformSpecific_##driver(); \
	void MFRenderTarget_DeinitModulePlatformSpecific_##driver(); \
	bool MFRenderTarget_CreatePlatformSpecific_##driver(MFRenderTarget *pTexture); \
	void MFRenderTarget_DestroyPlatformSpecific_##driver(MFRenderTarget *pTexture);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFRenderTarget_InitModulePlatformSpecific_##driver, \
		MFRenderTarget_DeinitModulePlatformSpecific_##driver, \
		MFRenderTarget_CreatePlatformSpecific_##driver, \
		MFRenderTarget_DestroyPlatformSpecific_##driver, \
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
struct MFRenderTargetPluginCallbacks
{
	const char *pDriverName;
	bool (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	bool (*pCreatePlatformSpecific)(MFRenderTarget *pRenderTarget);
	void (*pDestroyPlatformSpecific)(MFRenderTarget *pRenderTarget);
};

// create an array of actual callbacks to the various enabled plugins
MFRenderTargetPluginCallbacks gRenderTargetPlugins[] =
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
MFRenderTargetPluginCallbacks *gpCurrentRenderTargetPlugin = NULL;


/*** Function Wrappers ***/

bool MFRenderTarget_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentRenderTargetPlugin = &gRenderTargetPlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	return gpCurrentRenderTargetPlugin->pInitModulePlatformSpecific();
}

void MFRenderTarget_DeinitModulePlatformSpecific()
{
	gpCurrentRenderTargetPlugin->pDeinitModulePlatformSpecific();
}

bool MFRenderTarget_CreatePlatformSpecific(MFRenderTarget *pRenderTarget)
{
	return gpCurrentRenderTargetPlugin->pCreatePlatformSpecific(pRenderTarget);
}

void MFRenderTarget_DestroyPlatformSpecific(MFRenderTarget *pRenderTarget)
{
	gpCurrentRenderTargetPlugin->pDestroyPlatformSpecific(pRenderTarget);
}

#endif // MF_RENDERER
