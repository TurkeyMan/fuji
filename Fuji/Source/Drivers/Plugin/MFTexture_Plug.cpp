#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFTexture.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFTexture_InitModulePlatformSpecific_##driver(); \
	void MFTexture_DeinitModulePlatformSpecific_##driver(); \
	void MFTexture_CreatePlatformSpecific_##driver(MFTexture *pTexture); \
	void MFTexture_DestroyPlatformSpecific_##driver(MFTexture *pTexture); \
	MF_API bool MFTexture_Update_##driver(MFTexture *pTexture, int element, int mipLevel, const void *pData, size_t lineStride, size_t sliceStride); \
	MF_API bool MFTexture_Map_##driver(MFTexture *pTexture, int element, int mipLevel, MFLockedTexture *pLock); \
	MF_API void MFTexture_Unmap_##driver(MFTexture *pTexture, int element, int mipLevel);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFTexture_InitModulePlatformSpecific_##driver, \
		MFTexture_DeinitModulePlatformSpecific_##driver, \
		MFTexture_CreatePlatformSpecific_##driver, \
		MFTexture_DestroyPlatformSpecific_##driver, \
		MFTexture_Update_##driver, \
		MFTexture_Map_##driver, \
		MFTexture_Unmap_##driver \
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
	void (*pCreatePlatformSpecific)(MFTexture *pTexture);
	void (*pDestroyPlatformSpecific)(MFTexture *pTexture);
	bool (*pUpdate)(MFTexture *pTexture, int element, int mipLevel, const void *pData, size_t lineStride, size_t sliceStride);
	bool (*pMap)(MFTexture *pTexture, int element, int mipLevel, MFLockedTexture *pLock);
	void (*pUnmap)(MFTexture *pTexture, int element, int mipLevel);
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

void MFTexture_CreatePlatformSpecific(MFTexture *pTexture)
{
	gpCurrentTexturePlugin->pCreatePlatformSpecific(pTexture);
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	gpCurrentTexturePlugin->pDestroyPlatformSpecific(pTexture);
}

MF_API bool MFTexture_Update(MFTexture *pTexture, int element, int mipLevel, const void *pData, size_t lineStride, size_t sliceStride)
{
	return gpCurrentTexturePlugin->pUpdate(pTexture, element, mipLevel, pData, lineStride, sliceStride);
}

MF_API bool MFTexture_Map(MFTexture *pTexture, int element, int mipLevel, MFLockedTexture *pLock)
{
	return gpCurrentTexturePlugin->pMap(pTexture, element, mipLevel, pLock);
}

MF_API void MFTexture_Unmap(MFTexture *pTexture, int element, int mipLevel)
{
	gpCurrentTexturePlugin->pUnmap(pTexture, element, mipLevel);
}

#endif // MF_RENDERER
