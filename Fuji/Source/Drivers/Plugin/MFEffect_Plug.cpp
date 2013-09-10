#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFEffect.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFEffect_InitModulePlatformSpecific_##driver(); \
	void MFEffect_DeinitModulePlatformSpecific_##driver(); \
	bool MFEffect_CreatePlatformSpecific_##driver(MFEffect *pEffect); \
	void MFEffect_DestroyPlatformSpecific_##driver(MFEffect *pEffect);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFEffect_InitModulePlatformSpecific_##driver, \
		MFEffect_DeinitModulePlatformSpecific_##driver, \
		MFEffect_CreatePlatformSpecific_##driver, \
		MFEffect_DestroyPlatformSpecific_##driver \
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
struct MFEffectPluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	bool (*pCreatePlatformSpecific)(MFEffect *pEffect);
	void (*pDestroyPlatformSpecific)(MFEffect *pEffect);
};

// create an array of actual callbacks to the various enabled plugins
MFEffectPluginCallbacks gShaderPlugins[] =
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
MFEffectPluginCallbacks *gpCurrentShaderPlugin = NULL;


/*** Function Wrappers ***/

void MFEffect_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentShaderPlugin = &gShaderPlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentShaderPlugin->pInitModulePlatformSpecific();
}

void MFEffect_DeinitModulePlatformSpecific()
{
	gpCurrentShaderPlugin->pDeinitModulePlatformSpecific();
}

bool MFEffect_CreatePlatformSpecific(MFEffect *pEffect)
{
	return gpCurrentShaderPlugin->pCreatePlatformSpecific(pEffect);
}

void MFEffect_DestroyPlatformSpecific(MFEffect *pEffect)
{
	gpCurrentShaderPlugin->pDestroyPlatformSpecific(pEffect);
}

#endif // MF_RENDERER
