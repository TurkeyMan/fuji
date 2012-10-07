#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PLUGIN

#include "MFVertex.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFVertex_InitModulePlatformSpecific_##driver(); \
	void MFVertex_DeinitModulePlatformSpecific_##driver(); \
	MF_API MFVertexDeclaration *MFVertex_CreateVertexDeclaration_##driver(MFVertexElement *pElementArray, int elementCount); \
	MF_API void MFVertex_DestroyVertexDeclaration_##driver(MFVertexDeclaration *pDeclaration); \
	MF_API MFVertexBuffer *MFVertex_CreateVertexBuffer_##driver(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory); \
	MF_API void MFVertex_DestroyVertexBuffer_##driver(MFVertexBuffer *pVertexBuffer); \
	MF_API void MFVertex_LockVertexBuffer_##driver(MFVertexBuffer *pVertexBuffer); \
	MF_API void MFVertex_UnlockVertexBuffer_##driver(MFVertexBuffer *pVertexBuffer); \
	MF_API MFIndexBuffer *MFVertex_CreateIndexBuffer_##driver(int numIndices, uint16 *pIndexBufferMemory); \
	MF_API void MFVertex_DestroyIndexBuffer_##driver(MFIndexBuffer *pIndexBuffer); \
	MF_API void MFVertex_LockIndexBuffer_##driver(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices); \
	MF_API void MFVertex_UnlockIndexBuffer_##driver(MFIndexBuffer *pIndexBuffer); \
	MF_API void MFVertex_SetVertexDeclaration_##driver(MFVertexDeclaration *pVertexDeclaration); \
	MF_API void MFVertex_SetVertexStreamSource_##driver(int stream, MFVertexBuffer *pVertexBuffer); \
	MF_API void MFVertex_RenderVertices_##driver(MFVertexPrimType primType, int firstVertex, int numVertices); \
	MF_API void MFVertex_RenderIndexedVertices_##driver(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer *pIndexBuffer);

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFVertex_InitModulePlatformSpecific_##driver, \
		MFVertex_DeinitModulePlatformSpecific_##driver, \
		MFVertex_CreateVertexDeclaration_##driver, \
		MFVertex_DestroyVertexDeclaration_##driver, \
		MFVertex_CreateVertexBuffer_##driver, \
		MFVertex_DestroyVertexBuffer_##driver, \
		MFVertex_LockVertexBuffer_##driver, \
		MFVertex_UnlockVertexBuffer_##driver, \
		MFVertex_CreateIndexBuffer_##driver, \
		MFVertex_DestroyIndexBuffer_##driver, \
		MFVertex_LockIndexBuffer_##driver, \
		MFVertex_UnlockIndexBuffer_##driver, \
		MFVertex_SetVertexDeclaration_##driver, \
		MFVertex_SetVertexStreamSource_##driver, \
		MFVertex_RenderVertices_##driver, \
		MFVertex_RenderIndexedVertices_##driver, \
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
struct MFVertexPluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
	MFVertexDeclaration *(*pCreateVertexDeclaration)(MFVertexElement *pElementArray, int elementCount);
	void (*pDestroyVertexDeclaration)(MFVertexDeclaration *pDeclaration);
	MFVertexBuffer *(*pCreateVertexBuffer)(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory);
	void (*pDestroyVertexBuffer)(MFVertexBuffer *pVertexBuffer);
	void (*pLockVertexBuffer)(MFVertexBuffer *pVertexBuffer);
	void (*pUnlockVertexBuffer)(MFVertexBuffer *pVertexBuffer);
	MFIndexBuffer *(*pCreateIndexBuffer)(int numIndices, uint16 *pIndexBufferMemory);
	void (*pDestroyIndexBuffer)(MFIndexBuffer *pIndexBuffer);
	void (*pLockIndexBuffer)(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices);
	void (*pUnlockIndexBuffer)(MFIndexBuffer *pIndexBuffer);
	void (*pSetVertexDeclaration)(MFVertexDeclaration *pVertexDeclaration);
	void (*pSetVertexStreamSource)(int stream, MFVertexBuffer *pVertexBuffer);
	void (*pRenderVertices)(MFVertexPrimType primType, int firstVertex, int numVertices);
	void (*pRenderIndexedVertices)(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer *pIndexBuffer);
};

// create an array of actual callbacks to the various enabled plugins
MFVertexPluginCallbacks gVertexPlugins[] =
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
MFVertexPluginCallbacks *gpCurrentVertexPlugin = NULL;


/*** Function Wrappers ***/

void MFVertex_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentVertexPlugin = &gVertexPlugins[gDefaults.plugin.renderPlugin];

	// init the plugin
	gpCurrentVertexPlugin->pInitModulePlatformSpecific();
}

void MFVertex_DeinitModulePlatformSpecific()
{
	gpCurrentVertexPlugin->pDeinitModulePlatformSpecific();
}

MF_API MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount)
{
	return gpCurrentVertexPlugin->pCreateVertexDeclaration(pElementArray, elementCount);
}

MF_API void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration *pDeclaration)
{
	gpCurrentVertexPlugin->pDestroyVertexDeclaration(pDeclaration);
}

MF_API MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory)
{
	return gpCurrentVertexPlugin->pCreateVertexBuffer(pVertexFormat, numVerts, type, pVertexBufferMemory);
}

MF_API void MFVertex_DestroyVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	gpCurrentVertexPlugin->pDestroyVertexBuffer(pVertexBuffer);
}

MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	gpCurrentVertexPlugin->pLockVertexBuffer(pVertexBuffer);
}

MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	gpCurrentVertexPlugin->pUnlockVertexBuffer(pVertexBuffer);
}

MF_API MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory)
{
	return gpCurrentVertexPlugin->pCreateIndexBuffer(numIndices, pIndexBufferMemory);
}

MF_API void MFVertex_DestroyIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	gpCurrentVertexPlugin->pDestroyIndexBuffer(pIndexBuffer);
}

MF_API void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices)
{
	gpCurrentVertexPlugin->pLockIndexBuffer(pIndexBuffer, ppIndices);
}

MF_API void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	gpCurrentVertexPlugin->pUnlockIndexBuffer(pIndexBuffer);
}

MF_API void MFVertex_SetVertexDeclaration(MFVertexDeclaration *pVertexDeclaration)
{
	gpCurrentVertexPlugin->pSetVertexDeclaration(pVertexDeclaration);
}

MF_API void MFVertex_SetVertexStreamSource(int stream, MFVertexBuffer *pVertexBuffer)
{
	gpCurrentVertexPlugin->pSetVertexStreamSource(stream, pVertexBuffer);
}

MF_API void MFVertex_RenderVertices(MFVertexPrimType primType, int firstVertex, int numVertices)
{
	gpCurrentVertexPlugin->pRenderVertices(primType, firstVertex, numVertices);
}

MF_API void MFVertex_RenderIndexedVertices(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer *pIndexBuffer)
{
	gpCurrentVertexPlugin->pRenderIndexedVertices(primType, numVertices, numIndices, pIndexBuffer);
}

#endif // MF_RENDERER
