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

/*** Globals ****/

#define USE_VBOS

bool gbUseVBOs = false;

/*** Functions ****/

void MFModel_InitModulePlatformSpecific()
{
#if defined(USE_VBOS)
	if(glBindBuffer && glBufferData && glDeleteBuffers && glGenBuffers)
		gbUseVBOs = true;
	else
		MFDebug_Warn(1, "MFModel: OpenGL Vertex Buffer Object extension not loaded, performance may suffer.");
#endif
}

void MFModel_DeinitModulePlatformSpecific()
{
}

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

	MFMatrix localToView;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)&MFView_GetViewToScreenMatrix());

	glMatrixMode(GL_MODELVIEW);
	if(MFView_IsOrtho())
		glLoadMatrixf((GLfloat*)&pModel->worldMatrix);
	else
		glLoadMatrixf((GLfloat*)MFView_GetLocalToView(pModel->worldMatrix, &localToView));

	MFMaterial *pMatOverride = (MFMaterial*)MFRenderer_GetRenderStateOverride(MFRS_MaterialOverride);

	if(pMatOverride)
		MFMaterial_SetMaterial(pMatOverride);

	MFModelDataChunk *pChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)&pSubobjects[a].pMeshChunks[b];

				if(!pMatOverride)
					MFMaterial_SetMaterial(pMC->pMaterial);

				MFRenderer_Begin();

				// just use conventional vertex arrays
				for(int a=0; a<pMC->pVertexFormat->numVertexStreams; ++a)
				{
					MFVertexStream *pStream = &pMC->pVertexFormat->pStreams[a];

#if defined(USE_VBOS)
					if(gbUseVBOs)
						glBindBuffer(GL_ARRAY_BUFFER, pMC->userData[a]);
#endif

					for(int b=0; b<pStream->numVertexElements; ++b)
					{
						// if we're using vertex buffer objects, the the pointer is just an offset into the buffer
						char *pDataPointer = gbUseVBOs ? (char*)pStream->pElements[b].offset : (char*)pMC->ppVertexStreams[a] + pStream->pElements[b].offset;

						switch(pStream->pElements[b].usage)
						{
							case MFVET_Position:
								glVertexPointer(3, GL_FLOAT, pStream->streamStride, pDataPointer);
								glEnableClientState(GL_VERTEX_ARRAY);
								break;
							case MFVET_Normal:
								glNormalPointer(GL_FLOAT, pStream->streamStride, pDataPointer);
								glEnableClientState(GL_NORMAL_ARRAY);
								break;
							case MFVET_Colour:
								glColorPointer(4, GL_UNSIGNED_BYTE, pStream->streamStride, pDataPointer);
								glEnableClientState(GL_COLOR_ARRAY);
								break;
							case MFVET_TexCoord:
								glTexCoordPointer(2, GL_FLOAT, pStream->streamStride, pDataPointer);
								glEnableClientState(GL_TEXTURE_COORD_ARRAY);
								break;
						}
					}
				}

				if(gbUseVBOs)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMC->userData[7]);
					glDrawElements(GL_TRIANGLES, pMC->numIndices, GL_UNSIGNED_SHORT, NULL);
				}
				else
					glDrawElements(GL_TRIANGLES, pMC->numIndices, GL_UNSIGNED_SHORT, pMC->pIndexData);

				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
	}
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);

#if defined(USE_VBOS)
	if(gbUseVBOs)
	{
		MFDebug_Assert(pMC->pVertexFormat->numVertexStreams <= 7, "OpenGL rendering currently supports a maximum of 7 vertex data streams.");

		// bind the index buffer
		glGenBuffers(1, &pMC->userData[7]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMC->userData[7]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16)*pMC->numVertices, pMC->pIndexData, GL_STATIC_DRAW);

		// bind the vertex buffers
		for(int a=0; a<pMC->pVertexFormat->numVertexStreams; ++a)
		{
			MFVertexStream *pStream = &pMC->pVertexFormat->pStreams[a];

			glGenBuffers(1, &pMC->userData[a]);
			glBindBuffer(GL_ARRAY_BUFFER, pMC->userData[a]);
			glBufferData(GL_ARRAY_BUFFER, pStream->streamStride*pMC->numVertices, pMC->ppVertexStreams[a], GL_STATIC_DRAW);
		}
	}
#endif
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

#if defined(USE_VBOS)
	if(gbUseVBOs)
	{
		// destroy the vertex buffers
		for(int a=0; a<pMC->pVertexFormat->numVertexStreams && a < 7; ++a)
			glDeleteBuffers(1, &pMC->userData[a]);

		// destroy the index buffer
		glDeleteBuffers(1, &pMC->userData[7]);
	}
#endif

	MFMaterial_Destroy(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMeshChunk, pBase, load);
}

#endif
