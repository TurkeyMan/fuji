#include "Fuji.h"
#include "MFTexture.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFRenderer_InitModulePlatformSpecific_##driver(); \
	void MFRenderer_DeinitModulePlatformSpecific_##driver(); \
	int MFRenderer_CreateDisplay_##driver(); \
	void MFRenderer_DestroyDisplay_##driver(); \
	void MFRenderer_ResetDisplay_##driver(); \
	void MFRenderer_BeginFrame_##driver(); \
	void MFRenderer_EndFrame_##driver(); \
	void MFRenderer_SetClearColour_##driver(float r, float g, float b, float a); \
	void MFRenderer_ClearScreen_##driver(uint32 flags); \
	void MFRenderer_GetViewport_##driver(MFRect *pRect); \
	void MFRenderer_SetViewport_##driver(MFRect *pRect); \
	void MFRenderer_ResetViewport_##driver(); \
	void MFRenderer_SetRenderTarget_##driver(MFTexture *pRenderTarget, MFTexture *pZTarget); \
	void MFRenderer_SetDeviceRenderTarget_##driver();

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFRenderer_InitModulePlatformSpecific_##driver, \
		MFRenderer_DeinitModulePlatformSpecific_##driver, \
		MFRenderer_CreateDisplay_##driver, \
		MFRenderer_DestroyDisplay_##driver, \
		MFRenderer_ResetDisplay_##driver, \
		MFRenderer_BeginFrame_##driver, \
		MFRenderer_EndFrame_##driver, \
		MFRenderer_SetClearColour_##driver, \
		MFRenderer_ClearScreen_##driver, \
		MFRenderer_GetViewport_##driver, \
		MFRenderer_SetViewport_##driver, \
		MFRenderer_ResetViewport_##driver, \
		MFRenderer_SetRenderTarget_##driver, \
		MFRenderer_SetDeviceRenderTarget_##driver \
	},

// declare the available plugins
#if defined(MF_RENDERPLUGIN_D3D9)
	DECLARE_PLUGIN_CALLBACKS(D3D9)
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
	void (*pBeginFrame)();
	void (*pEndFrame)();
	void (*pSetClearColour)(float r, float g, float b, float a);
	void (*pClearScreen)(uint32 flags);
	void (*pGetViewport)(MFRect *pRect);
	void (*pSetViewport)(MFRect *pRect);
	void (*pResetViewport)();
	void (*pSetRenderTarget)(MFTexture *pRenderTarget, MFTexture *pZTarget);
	void (*pSetDeviceRenderTarget)();
};

// create an array of actual callbacks to the various enabled plugins
MFRenderPluginCallbacks gRenderPlugins[] =
{
#if defined(MF_RENDERPLUGIN_D3D9)
	DEFINE_PLUGIN(D3D9)
#endif
#if defined(MF_RENDERPLUGIN_OPENGL)
	DEFINE_PLUGIN(OpenGL)
#endif
};

// the current active plugin
MFRenderPluginCallbacks *gpCurrentRenderPlugin = NULL;


/*** Function Wrappers ***/

void MFRenderer_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentRenderPlugin = &gRenderPlugins[0];

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

void MFRenderer_BeginFrame()
{
	gpCurrentRenderPlugin->pBeginFrame();
}

void MFRenderer_EndFrame()
{
	gpCurrentRenderPlugin->pEndFrame();
}

void MFRenderer_SetClearColour(float r, float g, float b, float a)
{
	gpCurrentRenderPlugin->pSetClearColour(r, g, b, a);
}

void MFRenderer_ClearScreen(uint32 flags)
{
	gpCurrentRenderPlugin->pClearScreen(flags);
}

void MFRenderer_GetViewport(MFRect *pRect)
{
	gpCurrentRenderPlugin->pGetViewport(pRect);
}

void MFRenderer_SetViewport(MFRect *pRect)
{
	gpCurrentRenderPlugin->pSetViewport(pRect);
}

void MFRenderer_ResetViewport()
{
	gpCurrentRenderPlugin->pResetViewport();
}

void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget)
{
	gpCurrentRenderPlugin->pSetRenderTarget(pRenderTarget, pZTarget);
}

void MFRenderer_SetDeviceRenderTarget()
{
	gpCurrentRenderPlugin->pSetDeviceRenderTarget();
}

#endif // MF_RENDERER
