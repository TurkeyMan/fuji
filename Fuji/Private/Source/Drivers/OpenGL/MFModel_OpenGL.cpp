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

#define NO_VBOS

// VBO Extension Definitions, From glext.h
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

#if defined(MF_WINDOWS)
	#define GETPROCADDRESS wglGetProcAddress
#elif defined(MF_LINUX) || defined(MF_OSX)
	extern "C" void (*glXGetProcAddressARB(const char *))();
	#define GETPROCADDRESS glXGetProcAddressARB
#endif

// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC glGenBuffers = NULL;			// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBuffer = NULL;			// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferData = NULL;			// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffers = NULL;	// VBO Deletion Procedure

bool gbUseVBOs = false;


/*** Functions ****/

bool IsExtensionSupported(const char *extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	// Extension names should not have spaces.
	where = (GLubyte *) strchr(extension, ' ');
	if(where || *extension == '\0')
		return false;
	extensions = glGetString(GL_EXTENSIONS);

    // It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. Don't be fooled by sub-strings, etc.
	start = extensions;
	for(;;)
	{
		where = (GLubyte *)strstr((const char *)start, extension);
		if(!where)
			break;
		terminator = where + strlen(extension);
		if(where == start || *(where - 1) == ' ')
			if(*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}
	return false;
}

void MFModel_InitModulePlatformSpecific()
{
#if !defined(NO_VBOS)
	// check if VBO extension is present
	const GLubyte *version;
	GLboolean glVersion15 = GL_FALSE;

    // Make sure required functionality is available!
	version = glGetString(GL_VERSION);
	if((version[0] == '1') && (version[1] == '.') && (version[2] >= '5') && (version[2] <= '9'))
	{
		glVersion15 = GL_TRUE;
	}

	if(!glVersion15 && !IsExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		MFDebug_Warn(1, "Neither OpenGL 1.5 nor GL_ARB_vertex_buffer_object extension is available!");
	}
	else
	{
		// Load the function pointers
		if(glVersion15)
		{
			glBindBuffer = (PFNGLBINDBUFFERARBPROC)GETPROCADDRESS("glBindBuffer");
			glBufferData = (PFNGLBUFFERDATAARBPROC)GETPROCADDRESS("glBufferData");
//			glBufferSubData = GETPROCADDRESS("glBufferSubData");
			glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)GETPROCADDRESS("glDeleteBuffers");
			glGenBuffers = (PFNGLGENBUFFERSARBPROC)GETPROCADDRESS("glGenBuffers");
//			glMapBuffer = GETPROCADDRESS("glMapBuffer");
//			glUnmapBuffer = GETPROCADDRESS("glUnmapBuffer");
		}
		else
		{
			glBindBuffer = (PFNGLBINDBUFFERARBPROC)GETPROCADDRESS("glBindBufferARB");
			glBufferData = (PFNGLBUFFERDATAARBPROC)GETPROCADDRESS("glBufferDataARB");
//			glBufferSubData = GETPROCADDRESS("glBufferSubDataARB");
			glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)GETPROCADDRESS("glDeleteBuffersARB");
			glGenBuffers = (PFNGLGENBUFFERSARBPROC)GETPROCADDRESS("glGenBuffersARB");
//			glMapBuffer = GETPROCADDRESS("glMapBufferARB");
//			glUnmapBuffer = GETPROCADDRESS("glUnmapBufferARB");
		}

		if(glBindBuffer && glBufferData && glDeleteBuffers && glGenBuffers)
		{
			gbUseVBOs = true;
		}
		else
		{
			MFDebug_Warn(1, "MFModel: OpenGL Vertex Buffer Object extension not loaded, performance may suffer.");
		}
	}
#endif
}

void MFModel_DeinitModulePlatformSpecific()
{
}

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

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

#if !defined(NO_VBOS)
				if(gbUseVBOs)
				{
					// user video memory buffers
					glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->vertBuffer);
					glVertexPointer(3, GL_FLOAT, 0, NULL);
					glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->normalBuffer);
					glNormalPointer(GL_FLOAT, 0, NULL);
					glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->colourBuffer);
					glColorPointer(4, GL_UNSIGNED_BYTE, 0, NULL);
					glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->uvBuffer);
					glTexCoordPointer(2, GL_FLOAT, 0, NULL);
				}
				else
#endif
				{
					// just use conventional vertex arrays
					for(int a=0; a<pMC->pVertexFormat->numVertexStreams; ++a)
					{
						MFVertexStream *pStream = &pMC->pVertexFormat->pStreams[a];

						for(int b=0; b<pStream->numVertexElements; ++b)
						{
							switch(pStream->pElements[b].usage)
							{
								case MFVET_Position:
									glVertexPointer(3, GL_FLOAT, pStream->streamStride, (char*)pMC->ppVertexStreams[a] + pStream->pElements[b].offset);
									glEnableClientState(GL_VERTEX_ARRAY);
									break;
								case MFVET_Normal:
									glNormalPointer(GL_FLOAT, pStream->streamStride, (char*)pMC->ppVertexStreams[a] + pStream->pElements[b].offset);
									glEnableClientState(GL_NORMAL_ARRAY);
									break;
								case MFVET_Colour:
									glColorPointer(4, GL_UNSIGNED_BYTE, pStream->streamStride, (char*)pMC->ppVertexStreams[a] + pStream->pElements[b].offset);
									glEnableClientState(GL_COLOR_ARRAY);
									break;
								case MFVET_TexCoord:
									glTexCoordPointer(2, GL_FLOAT, pStream->streamStride, (char*)pMC->ppVertexStreams[a] + pStream->pElements[b].offset);
									glEnableClientState(GL_TEXTURE_COORD_ARRAY);
									break;
							}
						}
					}
				}

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

#if !defined(NO_VBOS)
	if(gbUseVBOs)
	{
		glGenBuffers(1, &pMC->vertBuffer);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->vertBuffer);
		glBufferData(GL_ARRAY_BUFFER_ARB, pMC->vertexDataSize, pMC->pVertexData, GL_STATIC_DRAW_ARB);

		glGenBuffers(1, &pMC->normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->normalBuffer);
		glBufferData(GL_ARRAY_BUFFER_ARB, pMC->normalDataSize, pMC->pNormalData, GL_STATIC_DRAW_ARB);

		glGenBuffers(1, &pMC->colourBuffer);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->colourBuffer);
		glBufferData(GL_ARRAY_BUFFER_ARB, pMC->colourDataSize, pMC->pColourData, GL_STATIC_DRAW_ARB);

		glGenBuffers(1, &pMC->uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, pMC->uvBuffer);
		glBufferData(GL_ARRAY_BUFFER_ARB, pMC->uvDataSize, pMC->pUVData, GL_STATIC_DRAW_ARB);
	}
#endif
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

#if !defined(NO_VBOS)
	if(gbUseVBOs)
	{
		glDeleteBuffers(1, &pMC->vertBuffer);
		glDeleteBuffers(1, &pMC->normalBuffer);
		glDeleteBuffers(1, &pMC->colourBuffer);
		glDeleteBuffers(1, &pMC->uvBuffer);
	}
#endif

	MFMaterial_Destroy(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMeshChunk, pBase, load);
}

#endif
