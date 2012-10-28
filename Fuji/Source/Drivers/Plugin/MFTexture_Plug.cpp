#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFTexture.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFTexture_InitModulePlatformSpecific_##driver(); \
	void MFTexture_DeinitModulePlatformSpecific_##driver(); \
	void MFTexture_CreatePlatformSpecific_##driver(MFTexture *pTexture, bool generateMipChain); \
	MF_API MFTexture* MFTexture_CreateRenderTarget_##driver(const char *pName, int width, int height, MFImageFormat targetFormat); \
	void MFTexture_DestroyPlatformSpecific_##driver(MFTexture *pTexture);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFTexture_InitModulePlatformSpecific_##driver, \
		MFTexture_DeinitModulePlatformSpecific_##driver, \
		MFTexture_CreatePlatformSpecific_##driver, \
		MFTexture_CreateRenderTarget_##driver, \
		MFTexture_DestroyPlatformSpecific_##driver, \
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
struct MFTexturePluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	void (*pCreatePlatformSpecific)(MFTexture *pTexture, bool generateMipChain);
	MFTexture* (*pCreateRenderTarget)(const char *pName, int width, int height, MFImageFormat targetFormat);
	void (*pDestroyPlatformSpecific)(MFTexture *pTexture);
};

// create an array of actual callbacks to the various enabled plugins
MFTexturePluginCallbacks gTexturePlugins[] =
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
MFTexturePluginCallbacks *gpCurrentTexturePlugin = NULL;


/*** Function Wrappers ***/

void MFTexture_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentTexturePlugin = &gTexturePlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentTexturePlugin->pInitModulePlatformSpecific();
}

void MFTexture_DeinitModulePlatformSpecific()
{
	gpCurrentTexturePlugin->pDeinitModulePlatformSpecific();
}

void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	gpCurrentTexturePlugin->pCreatePlatformSpecific(pTexture, generateMipChain);
}

MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFImageFormat targetFormat)
{
	return gpCurrentTexturePlugin->pCreateRenderTarget(pName, width, height, targetFormat);
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	gpCurrentTexturePlugin->pDestroyPlatformSpecific(pTexture);
}

#endif // MF_RENDERER
