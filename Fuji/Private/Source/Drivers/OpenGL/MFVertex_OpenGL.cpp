#include "Fuji.h"
#include "MFVector.h"
#include "MFVertex_Internal.h"

#if MF_RENDERER == MF_DRIVER_OPENGL

MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount)
{

}

MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type)
{

}

void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexData)
{

}

void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 *pIndices)
{

}

void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{

}

void MFVertex_SetVertexStreamSource(int stream, MFVertexBuffer *pVertexBuffer)
{

}

void MFVertex_RenderVertices(int numVertices)
{

}

void MFVertex_RenderIndexedVertices(int numIndices, MFIndexBuffer *pIndexBuffer)
{

}

#endif // MF_RENDERER
