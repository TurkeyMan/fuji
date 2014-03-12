#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFPrimitive.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	MFInitStatus MFPrimitive_InitModule_##driver(int moduleId, bool bPerformInitialisation); \
	void MFPrimitive_DeinitModule_##driver(); \
	void MFPrimitive_DrawStats_##driver(); \
	MF_API void MFPrimitive_##driver(uint32 type, uint32 hint); \
	MF_API void MFBegin_##driver(uint32 vertexCount); \
	MF_API void MFSetMatrix_##driver(const MFMatrix &mat); \
	MF_API void MFSetColour_##driver(float r, float g, float b, float a); \
	MF_API void MFSetTexCoord1_##driver(float u, float v); \
	MF_API void MFSetNormal_##driver(float x, float y, float z); \
	MF_API void MFSetPosition_##driver(float x, float y, float z); \
	MF_API void MFEnd_##driver(); \
	MF_API void MFPrimitive_BeginBlitter_##driver(int numBlits); \
	MF_API void MFPrimitive_Blit_##driver(int x, int y, int tx, int ty, int tw, int th); \
	MF_API void MFPrimitive_StretchBlit_##driver(int x, int y, int w, int h, int tx, int ty, int tw, int th); \
	MF_API void MFPrimitive_EndBlitter_##driver();

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFPrimitive_InitModule_##driver, \
		MFPrimitive_DeinitModule_##driver, \
		MFPrimitive_DrawStats_##driver, \
		MFPrimitive_##driver, \
		MFBegin_##driver, \
		MFSetMatrix_##driver, \
		MFSetColour_##driver, \
		MFSetTexCoord1_##driver, \
		MFSetNormal_##driver, \
		MFSetPosition_##driver, \
		MFEnd_##driver, \
		MFPrimitive_BeginBlitter_##driver, \
		MFPrimitive_Blit_##driver, \
		MFPrimitive_StretchBlit_##driver, \
		MFPrimitive_EndBlitter_##driver \
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
struct MFPrimitivePluginCallbacks
{
	const char *pDriverName;
	MFInitStatus (*pInitModule)(int moduleId, bool bPerformInitialisation);
	void (*pDeinitModule)();
	void (*pDrawStats)();
	void (*pMFPrimitive)(uint32 type, uint32 hint);
	void (*pMFBegin)(uint32 vertexCount);
	void (*pMFSetMatrix)(const MFMatrix &mat);
	void (*pMFSetColour)(float r, float g, float b, float a);
	void (*pMFSetTexCoord1)(float u, float v);
	void (*pMFSetNormal)(float x, float y, float z);
	void (*pMFSetPosition)(float x, float y, float z);
	void (*pMFEnd)();
	void (*pBeginBlitter)(int numBlits);
	void (*pBlit)(int x, int y, int tx, int ty, int tw, int th);
	void (*pStretchBlit)(int x, int y, int w, int h, int tx, int ty, int tw, int th);
	void (*pEndBlitter)();
};

// create an array of actual callbacks to the various enabled plugins
MFPrimitivePluginCallbacks gPrimitivePlugins[] =
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
MFPrimitivePluginCallbacks *gpCurrentPrimitivePlugin = NULL;


/*** Function Wrappers ***/

MFInitStatus MFPrimitive_InitModule(int moduleId, bool bPerformInitialisation)
{
	// choose the plugin from init settings
	gpCurrentPrimitivePlugin = &gPrimitivePlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentPrimitivePlugin->pInitModule(moduleId, bPerformInitialisation);

	return MFIS_Succeeded;
}

void MFPrimitive_DeinitModule()
{
	gpCurrentPrimitivePlugin->pDeinitModule();
}

void MFPrimitive_DrawStats()
{
	gpCurrentPrimitivePlugin->pDrawStats();
}

MF_API void MFPrimitive(uint32 type, uint32 hint)
{
	gpCurrentPrimitivePlugin->pMFPrimitive(type, hint);
}

MF_API void MFBegin(uint32 vertexCount)
{
	gpCurrentPrimitivePlugin->pMFBegin(vertexCount);
}

MF_API void MFSetMatrix(const MFMatrix &mat)
{
	gpCurrentPrimitivePlugin->pMFSetMatrix(mat);
}

MF_API void MFSetColour(float r, float g, float b, float a)
{
	gpCurrentPrimitivePlugin->pMFSetColour(r, g, b, a);
}

MF_API void MFSetTexCoord1(float u, float v)
{
	gpCurrentPrimitivePlugin->pMFSetTexCoord1(u, v);
}

MF_API void MFSetNormal(float x, float y, float z)
{
	gpCurrentPrimitivePlugin->pMFSetNormal(x, y, z);
}

MF_API void MFSetPosition(float x, float y, float z)
{
	gpCurrentPrimitivePlugin->pMFSetPosition(x, y, z);
}

MF_API void MFEnd()
{
	gpCurrentPrimitivePlugin->pMFEnd();
}

MF_API void MFPrimitive_BeginBlitter(int numBlits)
{
	gpCurrentPrimitivePlugin->pBeginBlitter(numBlits);
}

MF_API void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	gpCurrentPrimitivePlugin->pBlit(x, y, tx, ty, tw, th);
}

MF_API void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
	gpCurrentPrimitivePlugin->pStretchBlit(x, y, w, h, tx, ty, tw, th);
}

MF_API void MFPrimitive_EndBlitter()
{
	gpCurrentPrimitivePlugin->pEndBlitter();
}

#endif // MF_RENDERER
