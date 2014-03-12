#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFMaterial.h"
#include "MFRenderer_Internal.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	int MFMat_Standard_RegisterMaterial_##driver(MFMaterialType *pType); \
	void MFMat_Standard_UnregisterMaterial_##driver(); \
	int MFMat_Standard_Begin_##driver(MFMaterial *pMaterial, MFRendererState &state); \
	void MFMat_Standard_CreateInstancePlatformSpecific_##driver(MFMaterial *pMaterial); \
	void MFMat_Standard_DestroyInstancePlatformSpecific_##driver(MFMaterial *pMaterial);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFMat_Standard_RegisterMaterial_##driver, \
		MFMat_Standard_UnregisterMaterial_##driver, \
		MFMat_Standard_Begin_##driver, \
		MFMat_Standard_CreateInstancePlatformSpecific_##driver, \
		MFMat_Standard_DestroyInstancePlatformSpecific_##driver \
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
struct MFMat_StandardPluginCallbacks
{
	const char *pDriverName;
	int (*pRegisterMaterial)(MFMaterialType *pType);
	void (*pUnregisterMaterial)();
	int (*pBegin)(MFMaterial *pMaterial, MFRendererState &state);
	void (*pCreateInstancePlatformSpecific)(MFMaterial *pMaterial);
	void (*pDestroyInstancePlatformSpecific)(MFMaterial *pMaterial);
};

// create an array of actual callbacks to the various enabled plugins
MFMat_StandardPluginCallbacks gMatStandardPlugins[] =
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
MFMat_StandardPluginCallbacks *gpCurrentMatStandardPlugin = NULL;


/*** Function Wrappers ***/

int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
	// choose the plugin from init settings
	gpCurrentMatStandardPlugin = &gMatStandardPlugins[gDefaults.plugin.renderPlugin];

	return gpCurrentMatStandardPlugin->pRegisterMaterial(pType);
}

void MFMat_Standard_UnregisterMaterial()
{
	gpCurrentMatStandardPlugin->pUnregisterMaterial();
}

int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	return gpCurrentMatStandardPlugin->pBegin(pMaterial, state);
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
	gpCurrentMatStandardPlugin->pCreateInstancePlatformSpecific(pMaterial);
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
	gpCurrentMatStandardPlugin->pDestroyInstancePlatformSpecific(pMaterial);
}

#endif // MF_RENDERER
