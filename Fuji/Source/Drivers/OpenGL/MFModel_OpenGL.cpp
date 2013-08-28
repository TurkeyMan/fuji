#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFModel_InitModulePlatformSpecific MFModel_InitModulePlatformSpecific_OpenGL
	#define MFModel_DeinitModulePlatformSpecific MFModel_DeinitModulePlatformSpecific_OpenGL
	#define MFModel_CreateMeshChunk MFModel_CreateMeshChunk_OpenGL
	#define MFModel_DestroyMeshChunk MFModel_DestroyMeshChunk_OpenGL
	#define MFModel_FixUpMeshChunk MFModel_FixUpMeshChunk_OpenGL
#endif

#include "MFMesh_Internal.h"
#include "MFModel_Internal.h"
#include "MFView.h"
#include "MFRenderer.h"

#include "MFOpenGL.h"
#include <string.h>

struct MeshChunkOpenGLRuntimeData
{
	GLuint streams[8];
};

/*** Globals ****/

/*** Functions ****/

void MFModel_InitModulePlatformSpecific()
{
}

void MFModel_DeinitModulePlatformSpecific()
{
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
/*
	MFDebug_Assert(sizeof(MeshChunkOpenGLRuntimeData) <= sizeof(pMC->runtimeData), "MeshChunkOpenGLRuntimeData is larger than runtimeData!");
	MeshChunkOpenGLRuntimeData &runtimeData = (MeshChunkOpenGLRuntimeData&)pMC->runtimeData;

	MFDebug_Assert(pMC->pVertexFormat->numVertexStreams <= 7, "OpenGL rendering currently supports a maximum of 7 vertex data streams.");

	// bind the index buffer
	glGenBuffers(1, &runtimeData.streams[7]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, runtimeData.streams[7]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16)*pMC->numIndices, pMC->pIndexData, GL_STATIC_DRAW);

	// bind the vertex buffers
	for(int a=0; a<pMC->pVertexFormat->numVertexStreams; ++a)
	{
		MFMeshVertexStream *pStream = &pMC->pVertexFormat->pStreams[a];

		glGenBuffers(1, &runtimeData.streams[a]);
		glBindBuffer(GL_ARRAY_BUFFER, runtimeData.streams[a]);
		glBufferData(GL_ARRAY_BUFFER, pStream->streamStride*pMC->numVertices, pMC->ppVertexStreams[a], GL_STATIC_DRAW);
	}
*/
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;
/*
	MeshChunkOpenGLRuntimeData &runtimeData = (MeshChunkOpenGLRuntimeData&)pMC->runtimeData;

	// destroy the vertex buffers
	for(int a=0; a<pMC->pVertexFormat->numVertexStreams && a < 7; ++a)
		glDeleteBuffers(1, &runtimeData.streams[a]);

	// destroy the index buffer
	glDeleteBuffers(1, &runtimeData.streams[7]);
*/

	MFMaterial_Release(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMeshChunk, pBase, load);
}

#endif
