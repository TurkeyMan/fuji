#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFModel.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFModel_InitModulePlatformSpecific_##driver(); \
	void MFModel_DeinitModulePlatformSpecific_##driver(); \
	MF_API void MFModel_Draw_##driver(MFModel *pModel); \
	void MFModel_CreateMeshChunk_##driver(MFMeshChunk *pMeshChunk); \
	void MFModel_DestroyMeshChunk_##driver(MFMeshChunk *pMeshChunk); \
	void MFModel_FixUpMeshChunk_##driver(MFMeshChunk *pMC, void *pBase, bool load);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFModel_InitModulePlatformSpecific_##driver, \
		MFModel_DeinitModulePlatformSpecific_##driver, \
		MFModel_Draw_##driver, \
		MFModel_CreateMeshChunk_##driver, \
		MFModel_DestroyMeshChunk_##driver, \
		MFModel_FixUpMeshChunk_##driver, \
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
struct MFModelPluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	void (*pDraw)(MFModel *pModel);
	void (*pCreateMeshChunk)(MFMeshChunk *pMeshChunk);
	void (*pDestroyMeshChunk)(MFMeshChunk *pMeshChunk);
	void (*pFixUpMeshChunk)(MFMeshChunk *pMC, void *pBase, bool load);
};

// create an array of actual callbacks to the various enabled plugins
MFModelPluginCallbacks gModelPlugins[] =
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
MFModelPluginCallbacks *gpCurrentModelPlugin = NULL;


/*** Function Wrappers ***/

void MFModel_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentModelPlugin = &gModelPlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentModelPlugin->pInitModulePlatformSpecific();
}

void MFModel_DeinitModulePlatformSpecific()
{
	gpCurrentModelPlugin->pDeinitModulePlatformSpecific();
}

MF_API void MFModel_Draw(MFModel *pModel)
{
	gpCurrentModelPlugin->pDraw(pModel);
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	gpCurrentModelPlugin->pCreateMeshChunk(pMeshChunk);
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	gpCurrentModelPlugin->pDestroyMeshChunk(pMeshChunk);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	gpCurrentModelPlugin->pFixUpMeshChunk(pMC, pBase, load);
}

#endif // MF_RENDERER
