#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFPrimitive_InitModule_##driver(); \
	void MFPrimitive_DeinitModule_##driver(); \
	void MFPrimitive_DrawStats_##driver(); \
	void MFPrimitive_##driver(uint32 type, uint32 hint); \
	void MFBegin_##driver(uint32 vertexCount); \
	void MFSetMatrix_##driver(const MFMatrix &mat); \
	void MFSetColour_##driver(float r, float g, float b, float a); \
	void MFSetTexCoord1_##driver(float u, float v); \
	void MFSetNormal_##driver(float x, float y, float z); \
	void MFSetPosition_##driver(float x, float y, float z); \
	void MFEnd_##driver(); \
	void MFPrimitive_BeginBlitter_##driver(int numBlits); \
	void MFPrimitive_Blit_##driver(int x, int y, int tx, int ty, int tw, int th); \
	void MFPrimitive_EndBlitter_##driver();

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
		MFPrimitive_EndBlitter_##driver \
	},

// declare the available plugins
#if defined(MF_RENDERPLUGIN_D3D9)
	DECLARE_PLUGIN_CALLBACKS(D3D9)
#endif
#if defined(MF_RENDERPLUGIN_OPENGL)
	DECLARE_PLUGIN_CALLBACKS(OpenGL)
#endif

// list of plugins
struct MFPrimitivePluginCallbacks
{
	const char *pDriverName;
	void (*pInitModule)();
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
	void (*pEndBlitter)();
};

// create an array of actual callbacks to the various enabled plugins
MFPrimitivePluginCallbacks gPrimitivePlugins[] =
{
#if defined(MF_RENDERPLUGIN_D3D9)
	DEFINE_PLUGIN(D3D9)
#endif
#if defined(MF_RENDERPLUGIN_OPENGL)
	DEFINE_PLUGIN(OpenGL)
#endif
};

// the current active plugin
MFPrimitivePluginCallbacks *gpCurrentPrimitivePlugin = NULL;


/*** Function Wrappers ***/

void MFPrimitive_InitModule()
{
	// choose the plugin from init settings
	gpCurrentPrimitivePlugin = &gPrimitivePlugins[0];

	// init the plugin
	gpCurrentPrimitivePlugin->pInitModule();
}

void MFPrimitive_DeinitModule()
{
	gpCurrentPrimitivePlugin->pDeinitModule();
}

void MFPrimitive_DrawStats()
{
	gpCurrentPrimitivePlugin->pDrawStats();
}

void MFPrimitive(uint32 type, uint32 hint)
{
	gpCurrentPrimitivePlugin->pMFPrimitive(type, hint);
}

void MFBegin(uint32 vertexCount)
{
	gpCurrentPrimitivePlugin->pMFBegin(vertexCount);
}

void MFSetMatrix(const MFMatrix &mat)
{
	gpCurrentPrimitivePlugin->pMFSetMatrix(mat);
}

void MFSetColour(float r, float g, float b, float a)
{
	gpCurrentPrimitivePlugin->pMFSetColour(r, g, b, a);
}

void MFSetTexCoord1(float u, float v)
{
	gpCurrentPrimitivePlugin->pMFSetTexCoord1(u, v);
}

void MFSetNormal(float x, float y, float z)
{
	gpCurrentPrimitivePlugin->pMFSetNormal(x, y, z);
}

void MFSetPosition(float x, float y, float z)
{
	gpCurrentPrimitivePlugin->pMFSetPosition(x, y, z);
}

void MFEnd()
{
	gpCurrentPrimitivePlugin->pMFEnd();
}

void MFPrimitive_BeginBlitter(int numBlits)
{
	gpCurrentPrimitivePlugin->pBeginBlitter(numBlits);
}

void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	gpCurrentPrimitivePlugin->pBlit(x, y, tx, ty, tw, th);
}

void MFPrimitive_EndBlitter()
{
	gpCurrentPrimitivePlugin->pEndBlitter();
}

#endif // MF_RENDERER
