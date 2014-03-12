#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFShader.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFShader_InitModulePlatformSpecific_##driver(); \
	void MFShader_DeinitModulePlatformSpecific_##driver(); \
	bool MFShader_CreatePlatformSpecific_##driver(MFShader *pShader); \
	void MFShader_DestroyPlatformSpecific_##driver(MFShader *pShader);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFShader_InitModulePlatformSpecific_##driver, \
		MFShader_DeinitModulePlatformSpecific_##driver, \
		MFShader_CreatePlatformSpecific_##driver, \
		MFShader_DestroyPlatformSpecific_##driver \
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
struct MFShaderPluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	bool (*pCreatePlatformSpecific)(MFShader *pShader);
	void (*pDestroyPlatformSpecific)(MFShader *pShader);
};

// create an array of actual callbacks to the various enabled plugins
MFShaderPluginCallbacks gShaderPlugins[] =
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
MFShaderPluginCallbacks *gpCurrentShaderPlugin = NULL;


/*** Function Wrappers ***/

void MFShader_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentShaderPlugin = &gShaderPlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentShaderPlugin->pInitModulePlatformSpecific();
}

void MFShader_DeinitModulePlatformSpecific()
{
	gpCurrentShaderPlugin->pDeinitModulePlatformSpecific();
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader)
{
	return gpCurrentShaderPlugin->pCreatePlatformSpecific(pShader);
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
	gpCurrentShaderPlugin->pDestroyPlatformSpecific(pShader);
}

#endif // MF_RENDERER
