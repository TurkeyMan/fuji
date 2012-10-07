#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFVertex_InitModulePlatformSpecific MFVertex_InitModulePlatformSpecific_OpenGL
	#define MFVertex_DeinitModulePlatformSpecific MFVertex_DeinitModulePlatformSpecific_OpenGL
	#define MFVertex_CreateVertexDeclaration MFVertex_CreateVertexDeclaration_OpenGL
	#define MFVertex_DestroyVertexDeclaration MFVertex_DestroyVertexDeclaration_OpenGL
	#define MFVertex_CreateVertexBuffer MFVertex_CreateVertexBuffer_OpenGL
	#define MFVertex_DestroyVertexBuffer MFVertex_DestroyVertexBuffer_OpenGL
	#define MFVertex_LockVertexBuffer MFVertex_LockVertexBuffer_OpenGL
	#define MFVertex_UnlockVertexBuffer MFVertex_UnlockVertexBuffer_OpenGL
	#define MFVertex_CreateIndexBuffer MFVertex_CreateIndexBuffer_OpenGL
	#define MFVertex_DestroyIndexBuffer MFVertex_DestroyIndexBuffer_OpenGL
	#define MFVertex_LockIndexBuffer MFVertex_LockIndexBuffer_OpenGL
	#define MFVertex_UnlockIndexBuffer MFVertex_UnlockIndexBuffer_OpenGL
	#define MFVertex_SetVertexDeclaration MFVertex_SetVertexDeclaration_OpenGL
	#define MFVertex_SetVertexStreamSource MFVertex_SetVertexStreamSource_OpenGL
	#define MFVertex_RenderVertices MFVertex_RenderVertices_OpenGL
	#define MFVertex_RenderIndexedVertices MFVertex_RenderIndexedVertices_OpenGL
#endif

#include "MFVector.h"
#include "MFVertex_Internal.h"

void MFVertex_InitModulePlatformSpecific()
{
}

void MFVertex_DeinitModulePlatformSpecific()
{
}

MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount)
{
	return NULL;
}

void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration *pDeclaration)
{

}

MF_API MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory)
{
	return NULL;
}

MF_API void MFVertex_DestroyVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

MF_API MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory)
{
	return NULL;
}

MF_API void MFVertex_DestroyIndexBuffer(MFIndexBuffer *pIndexBuffer)
{

}

MF_API void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices)
{

}

MF_API void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{

}

MF_API void MFVertex_SetVertexDeclaration(MFVertexDeclaration *pVertexDeclaration)
{

}

MF_API void MFVertex_SetVertexStreamSource(int stream, MFVertexBuffer *pVertexBuffer)
{

}

MF_API void MFVertex_RenderVertices(MFVertexPrimType primType, int firstVertex, int numVertices)
{

}

MF_API void MFVertex_RenderIndexedVertices(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer *pIndexBuffer)
{

}

#endif // MF_RENDERER
