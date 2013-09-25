#include "Fuji.h"
#include "MFTexture.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFSystem_Internal.h"
#include "MFRenderer.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFRenderer_InitModulePlatformSpecific_##driver(); \
	void MFRenderer_DeinitModulePlatformSpecific_##driver(); \
	int MFRenderer_CreateDisplay_##driver(); \
	void MFRenderer_DestroyDisplay_##driver(); \
	void MFRenderer_ResetDisplay_##driver(); \
	bool MFRenderer_SetDisplayMode_##driver(int width, int height, bool bFullscreen); \
	bool MFRenderer_BeginFramePlatformSpecific_##driver(); \
	void MFRenderer_EndFramePlatformSpecific_##driver(); \
	MF_API void MFRenderer_ClearScreen_##driver(MFRenderClearFlags flags, const MFVector &colour, float z, int stencil); \
	MF_API void MFRenderer_SetViewport_##driver(MFRect *pRect); \
	MF_API void MFRenderer_ResetViewport_##driver(); \
	MF_API MFRenderTarget* MFRenderer_GetDeviceRenderTarget_##driver(); \
	MF_API void MFRenderer_SetRenderTarget_##driver(MFRenderTarget *pRenderTarget); \
	MF_API float MFRenderer_GetTexelCenterOffset_##driver(); \
	void MFRendererInternal_SortElements_##driver(MFRenderLayer &layer);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFRenderer_InitModulePlatformSpecific_##driver, \
		MFRenderer_DeinitModulePlatformSpecific_##driver, \
		MFRenderer_CreateDisplay_##driver, \
		MFRenderer_DestroyDisplay_##driver, \
		MFRenderer_ResetDisplay_##driver, \
		MFRenderer_SetDisplayMode_##driver, \
		MFRenderer_BeginFramePlatformSpecific_##driver, \
		MFRenderer_EndFramePlatformSpecific_##driver, \
		MFRenderer_ClearScreen_##driver, \
		MFRenderer_SetViewport_##driver, \
		MFRenderer_ResetViewport_##driver, \
		MFRenderer_GetDeviceRenderTarget_##driver, \
		MFRenderer_SetRenderTarget_##driver, \
		MFRenderer_GetTexelCenterOffset_##driver, \
		MFRendererInternal_SortElements_##driver \
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
struct MFRenderPluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	int (*pCreateDisplay)();
	void (*pDestroyDisplay)();
	void (*pResetDisplay)();
	bool (*pSetDisplayMode)(int width, int height, bool bFullscreen);
	bool (*pBeginFramePlatformSpecific)();
	void (*pEndFramePlatformSpecific)();
	void (*pClearScreen)(MFRenderClearFlags flags, const MFVector &colour, float z, int stencil);
	void (*pSetViewport)(MFRect *pRect);
	void (*pResetViewport)();
	MFRenderTarget* (*pGetDeviceRenderTarget)();
	void (*pSetRenderTarget)(MFRenderTarget *pRenderTarget);
	float (*pGetTexelCenterOffset)();
	void (*pSortElements)(MFRenderLayer &layer);
};

// create an array of actual callbacks to the various enabled plugins
MFRenderPluginCallbacks gRenderPlugins[] =
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
MFRenderPluginCallbacks *gpCurrentRenderPlugin = NULL;


/*** Function Wrappers ***/

int MFRenderer_GetCurrentRendererPlugin()
{
	if(!MFString_CaseCmp("D3D9", gpCurrentRenderPlugin->pDriverName))
		return MF_DRIVER_D3D9;
	else if(!MFString_CaseCmp("D3D11", gpCurrentRenderPlugin->pDriverName))
		return MF_DRIVER_D3D11;
	else if(!MFString_CaseCmp("OpenGL", gpCurrentRenderPlugin->pDriverName))
		return MF_DRIVER_OPENGL;
	return -1;
}

void MFRenderer_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentRenderPlugin = &gRenderPlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentRenderPlugin->pInitModulePlatformSpecific();
}

void MFRenderer_DeinitModulePlatformSpecific()
{
	gpCurrentRenderPlugin->pDeinitModulePlatformSpecific();
}

int MFRenderer_CreateDisplay()
{
	return gpCurrentRenderPlugin->pCreateDisplay();
}

void MFRenderer_DestroyDisplay()
{
	gpCurrentRenderPlugin->pDestroyDisplay();
}

void MFRenderer_ResetDisplay()
{
	gpCurrentRenderPlugin->pResetDisplay();
}

bool MFRenderer_SetDisplayMode(int width, int height, bool bFullscreen)
{
	return gpCurrentRenderPlugin->pSetDisplayMode(width, height, bFullscreen);
}

bool MFRenderer_BeginFramePlatformSpecific()
{
	return gpCurrentRenderPlugin->pBeginFramePlatformSpecific();
}

void MFRenderer_EndFramePlatformSpecific()
{
	gpCurrentRenderPlugin->pEndFramePlatformSpecific();
}

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags, const MFVector &colour, float z, int stencil)
{
	gpCurrentRenderPlugin->pClearScreen(flags, colour, z, stencil);
}

MF_API void MFRenderer_SetViewport(MFRect *pRect)
{
	gpCurrentRenderPlugin->pSetViewport(pRect);
}

MF_API void MFRenderer_ResetViewport()
{
	gpCurrentRenderPlugin->pResetViewport();
}

MF_API MFRenderTarget* MFRenderer_GetDeviceRenderTarget()
{
	return gpCurrentRenderPlugin->pGetDeviceRenderTarget();
}

MF_API void MFRenderer_SetRenderTarget(MFRenderTarget *pRenderTarget)
{
	gpCurrentRenderPlugin->pSetRenderTarget(pRenderTarget);
}

MF_API float MFRenderer_GetTexelCenterOffset()
{
	return gpCurrentRenderPlugin->pGetTexelCenterOffset();
}

void MFRendererInternal_SortElements(MFRenderLayer &layer)
{
	return gpCurrentRenderPlugin->pSortElements(layer);
}

#endif // MF_RENDERER
