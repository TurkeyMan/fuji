#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFVertex_InitModulePlatformSpecific MFVertex_InitModulePlatformSpecific_OpenGL
	#define MFVertex_DeinitModulePlatformSpecific MFVertex_DeinitModulePlatformSpecific_OpenGL
	#define MFVertex_CreateVertexDeclarationPlatformSpecific MFVertex_CreateVertexDeclarationPlatformSpecific_OpenGL
	#define MFVertex_DestroyVertexDeclarationPlatformSpecific MFVertex_DestroyVertexDeclarationPlatformSpecific_OpenGL
	#define MFVertex_CreateVertexBufferPlatformSpecific MFVertex_CreateVertexBufferPlatformSpecific_OpenGL
	#define MFVertex_DestroyVertexBufferPlatformSpecific MFVertex_DestroyVertexBufferPlatformSpecific_OpenGL
	#define MFVertex_LockVertexBuffer MFVertex_LockVertexBuffer_OpenGL
	#define MFVertex_UnlockVertexBuffer MFVertex_UnlockVertexBuffer_OpenGL
	#define MFVertex_CreateIndexBufferPlatformSpecific MFVertex_CreateIndexBufferPlatformSpecific_OpenGL
	#define MFVertex_DestroyIndexBufferPlatformSpecific MFVertex_DestroyIndexBufferPlatformSpecific_OpenGL
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

bool MFVertex_CreateVertexDeclarationPlatformSpecific(MFVertexDeclaration *pDeclaration)
{
	return false;
}

void MFVertex_DestroyVertexDeclarationPlatformSpecific(MFVertexDeclaration *pDeclaration)
{

}

bool MFVertex_CreateVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer, void *pVertexBufferMemory)
{
	return false;
}

void MFVertex_DestroyVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer)
{

}

MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

bool MFVertex_CreateIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer, uint16 *pIndexBufferMemory)
{
	return false;
}

void MFVertex_DestroyIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer)
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
