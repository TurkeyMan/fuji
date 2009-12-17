#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFTexture.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFTexture_InitModulePlatformSpecific_##driver(); \
	void MFTexture_DeinitModulePlatformSpecific_##driver(); \
	void MFTexture_CreatePlatformSpecific_##driver(MFTexture *pTexture, bool generateMipChain); \
	MFTexture* MFTexture_CreateRenderTarget_##driver(const char *pName, int width, int height); \
	int MFTexture_Destroy_##driver(MFTexture *pTexture);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFTexture_InitModulePlatformSpecific_##driver, \
		MFTexture_DeinitModulePlatformSpecific_##driver, \
		MFTexture_CreatePlatformSpecific_##driver, \
		MFTexture_CreateRenderTarget_##driver, \
		MFTexture_Destroy_##driver, \
	},

// declare the available plugins
#if defined(MF_RENDERPLUGIN_D3D9)
	DECLARE_PLUGIN_CALLBACKS(D3D9)
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
	MFTexture* (*pCreateRenderTarget)(const char *pName, int width, int height);
	int (*pDestroy)(MFTexture *pTexture);
};

// create an array of actual callbacks to the various enabled plugins
MFTexturePluginCallbacks gTexturePlugins[] =
{
#if defined(MF_RENDERPLUGIN_D3D9)
	DEFINE_PLUGIN(D3D9)
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
	gpCurrentTexturePlugin = &gTexturePlugins[0];

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

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	return gpCurrentTexturePlugin->pCreateRenderTarget(pName, width, height);
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	return gpCurrentTexturePlugin->pDestroy(pTexture);
}

#endif // MF_RENDERER
