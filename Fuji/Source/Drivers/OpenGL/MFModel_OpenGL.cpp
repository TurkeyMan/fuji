#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFModel_InitModulePlatformSpecific MFModel_InitModulePlatformSpecific_OpenGL
	#define MFModel_DeinitModulePlatformSpecific MFModel_DeinitModulePlatformSpecific_OpenGL
	#define MFModel_Draw MFModel_Draw_OpenGL
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

#define USE_VBOS

extern bool gbUseVBOs;

/*** Functions ****/

void MFModel_InitModulePlatformSpecific()
{
}

void MFModel_DeinitModulePlatformSpecific()
{
}

MF_API void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

#if !defined(MF_OPENGL_SUPPORT_SHADERS)
	// GLES2.0
	MFDebug_Assert(false, "GLES 1.0 no longer supported!");
#endif

	MFModelDataChunk *pChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pSubobjects[a].pMeshChunks + b;

				MFMaterial_SetMaterial(pMC->pMaterial);

				MFRenderer_Begin();

				MFRenderer_OpenGL_SetMatrix(MFOGL_MatrixType_Projection, MFView_GetViewToScreenMatrix());

				if(MFView_IsOrtho())
					MFRenderer_OpenGL_SetMatrix(MFOGL_MatrixType_WorldView, pModel->worldMatrix);
				else
				{
					MFMatrix localToView;
					MFView_GetLocalToView(pModel->worldMatrix, &localToView);
					MFRenderer_OpenGL_SetMatrix(MFOGL_MatrixType_WorldView, localToView);
				}
/*
				// find vertex attrib locations
				GLuint program = MFRenderer_OpenGL_GetCurrentProgram();
				GLint pos = glGetAttribLocation(program, "vPos");
				GLint normal = glGetAttribLocation(program, "vNormal");
				GLint colour = glGetAttribLocation(program, "vColour");
				GLint uv0 = glGetAttribLocation(program, "vUV0");

				MeshChunkOpenGLRuntimeData &runtimeData = (MeshChunkOpenGLRuntimeData&)pMC->runtimeData;

				// just use conventional vertex arrays
				for(int e=0; e<pMC->elementCount; ++e)
				{
#if defined(USE_VBOS)
					if(gbUseVBOs)
						glBindBuffer(GL_ARRAY_BUFFER, runtimeData.streams[pMC->pElements[e].stream]);
#endif

					for(int b=0; b<pStream->numVertexElements; ++b)
					{
						// if we're using vertex buffer objects, the the pointer is just an offset into the buffer
						char *pDataPointer = gbUseVBOs ? (char*)(uintp)pStream->pElements[b].offset : (char*)pMC->ppVertexStreams[a] + pStream->pElements[b].offset;

						switch(pStream->pElements[b].usage)
						{
							case MFVET_Position:
								if(pos != -1)
								{
									glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, pStream->streamStride, pDataPointer);
									glEnableVertexAttribArray(pos);
								}
								break;
							case MFVET_Normal:
								if(normal != -1)
								{
									glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, pStream->streamStride, pDataPointer);
									glEnableVertexAttribArray(normal);
								}
								break;
							case MFVET_Colour:
								if(colour != -1)
								{
									glVertexAttribPointer(colour, 4, GL_UNSIGNED_BYTE, GL_TRUE, pStream->streamStride, pDataPointer);
									glEnableVertexAttribArray(colour);
								}
								break;
							case MFVET_TexCoord:
								if(uv0 != -1)
								{
									glActiveTexture(GL_TEXTURE0);
									glVertexAttribPointer(uv0, 2, GL_FLOAT, GL_TRUE, pStream->streamStride, pDataPointer);
									glEnableVertexAttribArray(uv0);
								}
								break;
						}
					}
				}

				if(gbUseVBOs)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, runtimeData.streams[7]);
					glDrawElements(GL_TRIANGLES, pMC->numIndices, GL_UNSIGNED_SHORT, NULL);
				}
				else
					glDrawElements(GL_TRIANGLES, pMC->numIndices, GL_UNSIGNED_SHORT, pMC->pIndexData);

				if(pos != -1)
					glDisableVertexAttribArray(pos);
				if(normal != -1)
					glDisableVertexAttribArray(normal);
				if(colour != -1)
					glDisableVertexAttribArray(colour);
				if(uv0 != -1)
					glDisableVertexAttribArray(uv0);
*/
			}
		}
	}
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
/*
#if defined(USE_VBOS)
	if(gbUseVBOs)
	{
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
	}
#endif
*/
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;
/*
#if defined(USE_VBOS)
	if(gbUseVBOs)
	{
		MeshChunkOpenGLRuntimeData &runtimeData = (MeshChunkOpenGLRuntimeData&)pMC->runtimeData;

		// destroy the vertex buffers
		for(int a=0; a<pMC->pVertexFormat->numVertexStreams && a < 7; ++a)
			glDeleteBuffers(1, &runtimeData.streams[a]);

		// destroy the index buffer
		glDeleteBuffers(1, &runtimeData.streams[7]);
	}
#endif
*/

	MFMaterial_Destroy(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMeshChunk, pBase, load);
}

#endif
