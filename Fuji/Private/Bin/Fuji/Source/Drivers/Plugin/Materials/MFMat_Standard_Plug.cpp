#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFMaterial.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	int MFMat_Standard_RegisterMaterial_##driver(void *pPlatformData); \
	void MFMat_Standard_UnregisterMaterial_##driver(); \
	int MFMat_Standard_Begin_##driver(MFMaterial *pMaterial); \
	void MFMat_Standard_CreateInstance_##driver(MFMaterial *pMaterial); \
	void MFMat_Standard_DestroyInstance_##driver(MFMaterial *pMaterial);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFMat_Standard_RegisterMaterial_##driver, \
		MFMat_Standard_UnregisterMaterial_##driver, \
		MFMat_Standard_Begin_##driver, \
		MFMat_Standard_CreateInstance_##driver, \
		MFMat_Standard_DestroyInstance_##driver \
	},

// declare the available plugins
#if defined(MF_RENDERPLUGIN_D3D9)
	DECLARE_PLUGIN_CALLBACKS(D3D9)
#endif
#if defined(MF_RENDERPLUGIN_OPENGL)
	DECLARE_PLUGIN_CALLBACKS(OpenGL)
#endif

// list of plugins
struct MFMat_StandardPluginCallbacks
{
	const char *pDriverName;
	int (*pRegisterMaterial)(void *pPlatformData);
	void (*pUnregisterMaterial)();
	int (*pBegin)(MFMaterial *pMaterial);
	void (*pCreateInstance)(MFMaterial *pMaterial);
	void (*pDestroyInstance)(MFMaterial *pMaterial);
};

// create an array of actual callbacks to the various enabled plugins
MFMat_StandardPluginCallbacks gMatStandardPlugins[] =
{
#if defined(MF_RENDERPLUGIN_D3D9)
	DEFINE_PLUGIN(D3D9)
#endif
#if defined(MF_RENDERPLUGIN_OPENGL)
	DEFINE_PLUGIN(OpenGL)
#endif
};

// the current active plugin
MFMat_StandardPluginCallbacks *gpCurrentMatStandardPlugin = NULL;


/*** Function Wrappers ***/

int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	// choose the plugin from init settings
	gpCurrentMatStandardPlugin = &gMatStandardPlugins[0];

	return gpCurrentMatStandardPlugin->pRegisterMaterial(pPlatformData);
}

void MFMat_Standard_UnregisterMaterial()
{
	gpCurrentMatStandardPlugin->pUnregisterMaterial();
}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	return gpCurrentMatStandardPlugin->pBegin(pMaterial);
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	gpCurrentMatStandardPlugin->pCreateInstance(pMaterial);
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	gpCurrentMatStandardPlugin->pDestroyInstance(pMaterial);
}

#endif // MF_RENDERER
