#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFVertex_InitModulePlatformSpecific MFVertex_InitModulePlatformSpecific_OpenGL
	#define MFVertex_DeinitModulePlatformSpecific MFVertex_DeinitModulePlatformSpecific_OpenGL
	#define MFVertex_ChoooseVertexDataTypePlatformSpecific MFVertex_ChoooseVertexDataTypePlatformSpecific_OpenGL
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
	#define MFVertex_SetIndexBuffer MFVertex_SetIndexBuffer_OpenGL
	#define MFVertex_RenderVertices MFVertex_RenderVertices_OpenGL
	#define MFVertex_RenderIndexedVertices MFVertex_RenderIndexedVertices_OpenGL
#endif

#include "MFVector.h"
#include "MFVertex_Internal.h"
#include "MFEffect_Internal.h"
#include "MFHeap.h"

#include "MFOpenGL.h"

struct MFVertex_RenderBuffers
{
	const MFVertexDeclaration *pVertexDeclaration;
	const MFVertexBuffer *pVertexBuffer[16];
	const MFIndexBuffer *pIndexBuffer;
};

/*** Globals ****/

static MFVertex_RenderBuffers gRenderBuffers;

static int gPrimTypes[MFPT_Max] =
{
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
#if !defined(MF_OPENGL_ES)
	GL_QUADS // OpenGLES does't support quad rendering, so we'll have to simulate with triangles
#endif
};

char gAttribNames[MFVET_Max][16] =
{
	"vPos\0\0",
	"vNormal\0\0",
	"vColour\0\0",
	"vUV\0\0",
	"vBiNormal\0\0",
	"vTangent\0\0",
	"vIndices\0\0",
	"vWeights\0\0"
};

uint8 gAttribNameLen[MFVET_Max] =
{
	4,
	7,
	7,
	3,
	9,
	8,
	8,
	8
};

struct MFVertexDataFormatGL
{
	GLint components;
	GLenum type;
	GLboolean normalise;
} gDataFormat[] =
{
	{ 0,		0,								0 },
	{ 4,		GL_FLOAT,						GL_FALSE }, // MFVDF_Float4,
	{ 3,		GL_FLOAT,						GL_FALSE }, // MFVDF_Float3,
	{ 2,		GL_FLOAT,						GL_FALSE }, // MFVDF_Float2,
	{ 1,		GL_FLOAT,						GL_FALSE }, // MFVDF_Float1,
	{ 4,		GL_UNSIGNED_BYTE,				GL_FALSE }, // MFVDF_UByte4_RGBA,
	{ 4,		GL_UNSIGNED_BYTE,				GL_TRUE },  // MFVDF_UByte4N_RGBA,
	{ GL_BGRA,	GL_UNSIGNED_BYTE,				GL_TRUE },  // MFVDF_UByte4N_BGRA,
	{ 4,		GL_SHORT,						GL_FALSE }, // MFVDF_SShort4,
	{ 2,		GL_SHORT,						GL_FALSE }, // MFVDF_SShort2,
	{ 4,		GL_SHORT,						GL_TRUE },  // MFVDF_SShort4N,
	{ 2,		GL_SHORT,						GL_TRUE },  // MFVDF_SShort2N,
	{ 4,		GL_UNSIGNED_SHORT,				GL_FALSE }, // MFVDF_UShort4,
	{ 2,		GL_UNSIGNED_SHORT,				GL_FALSE }, // MFVDF_UShort2,
	{ 4,		GL_UNSIGNED_SHORT,				GL_TRUE },  // MFVDF_UShort4N,
	{ 2,		GL_UNSIGNED_SHORT,				GL_TRUE },  // MFVDF_UShort2N,
	{ 4,		GL_HALF_FLOAT,					GL_FALSE }, // MFVDF_Float16_4,
	{ 2,		GL_HALF_FLOAT,					GL_FALSE }, // MFVDF_Float16_2,
	{ 4,		GL_UNSIGNED_INT_2_10_10_10_REV,	GL_FALSE }, // MFVDF_UDec3,
	{ 4,		GL_INT_2_10_10_10_REV,			GL_TRUE }   // MFVDF_Dec3N,
};


/*** Functions ****/

void MFVertex_InitModulePlatformSpecific()
{
}

void MFVertex_DeinitModulePlatformSpecific()
{
}

MFVertexDataFormat MFVertex_ChoooseVertexDataTypePlatformSpecific(MFVertexElementType elementType, int components)
{
	const MFVertexDataFormat floatComponents[5] = { MFVDF_Unknown, MFVDF_Float1, MFVDF_Float2, MFVDF_Float3, MFVDF_Float4 };
	switch(elementType)
	{
		case MFVET_Colour:
		case MFVET_Weights:
			return MFVDF_UByte4N_BGRA;	// Does OpenGL have this same limitation?
		case MFVET_Indices:
			return MFVDF_UByte4_RGBA;
		default:
			break;
	}
	// everything else is a float for now...
	return floatComponents[components];
}

bool MFVertex_CreateVertexDeclarationPlatformSpecific(MFVertexDeclaration *pDeclaration)
{
	return true;
}

void MFVertex_DestroyVertexDeclarationPlatformSpecific(MFVertexDeclaration *pDeclaration)
{

}

bool MFVertex_CreateVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer, void *pVertexBufferMemory)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	if(pVertexBufferMemory)
		glBufferData(GL_ARRAY_BUFFER, pVertexBuffer->numVerts * pVertexBuffer->pVertexDeclatation->pElementData[0].stride, pVertexBufferMemory, GL_STATIC_DRAW);

	MFCheckForOpenGLError(true);

	pVertexBuffer->pPlatformData = (void*)(size_t)buffer;

	return true;
}

void MFVertex_DestroyVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer)
{
	GLuint buffer = (GLuint)(size_t)pVertexBuffer->pPlatformData;
	glDeleteBuffers(1, &buffer);
}

MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer, void **ppVertices)
{
	MFDebug_Assert(!pVertexBuffer->bLocked, "Vertex buffer already locked!");

	pVertexBuffer->pLocked = MFHeap_Alloc(pVertexBuffer->numVerts*pVertexBuffer->pVertexDeclatation->pElementData[0].stride, MFHeap_GetHeap(MFHT_ActiveTemporary));

	if(ppVertices)
		*ppVertices = pVertexBuffer->pLocked;

	pVertexBuffer->bLocked = true;
}

MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	MFDebug_Assert(pVertexBuffer->bLocked, "Vertex buffer not locked!");

	GLuint buffer = (GLuint)(size_t)pVertexBuffer->pPlatformData;
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, pVertexBuffer->numVerts * pVertexBuffer->pVertexDeclatation->pElementData[0].stride, pVertexBuffer->pLocked, GL_STATIC_DRAW);

	MFCheckForOpenGLError(true);

	MFHeap_Free(pVertexBuffer->pLocked);

	pVertexBuffer->pLocked = NULL;
	pVertexBuffer->bLocked = false;
}

bool MFVertex_CreateIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer, uint16 *pIndexBufferMemory)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

	if(pIndexBufferMemory)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16)*pIndexBuffer->numIndices, pIndexBufferMemory, GL_STATIC_DRAW);

	MFCheckForOpenGLError(true);

	pIndexBuffer->pPlatformData = (void*)(size_t)buffer;

	return true;
}

void MFVertex_DestroyIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer)
{
	GLuint buffer = (GLuint)(size_t)pIndexBuffer->pPlatformData;
	glDeleteBuffers(1, &buffer);
}

MF_API void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices)
{
	MFDebug_Assert(!pIndexBuffer->bLocked, "Vertex buffer already locked!");

	pIndexBuffer->pLocked = MFHeap_Alloc(sizeof(uint16)*pIndexBuffer->numIndices, MFHeap_GetHeap(MFHT_ActiveTemporary));

	if(ppIndices)
		*ppIndices = (uint16*)pIndexBuffer->pLocked;

	pIndexBuffer->bLocked = true;
}

MF_API void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	MFDebug_Assert(pIndexBuffer->bLocked, "Vertex buffer not locked!");

	GLuint buffer = (GLuint)(size_t)pIndexBuffer->pPlatformData;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16)*pIndexBuffer->numIndices, pIndexBuffer->pLocked, GL_STATIC_DRAW);

	MFCheckForOpenGLError(true);

	MFHeap_Free(pIndexBuffer->pLocked);

	pIndexBuffer->pLocked = NULL;
	pIndexBuffer->bLocked = false;
}

MF_API void MFVertex_SetVertexDeclaration(const MFVertexDeclaration *pVertexDeclaration)
{
	gRenderBuffers.pVertexDeclaration = pVertexDeclaration;
}

MF_API void MFVertex_SetVertexStreamSource(int stream, const MFVertexBuffer *pVertexBuffer)
{
	gRenderBuffers.pVertexBuffer[stream] = pVertexBuffer;
}

MF_API void MFVertex_SetIndexBuffer(const MFIndexBuffer *pIndexBuffer)
{
	gRenderBuffers.pIndexBuffer = pIndexBuffer;
}

MF_API void MFVertex_RenderVertices(MFEffectTechnique *pTechnique, MFPrimType primType, int firstVertex, int numVertices)
{
	MFVertexElement *pElements = gRenderBuffers.pVertexDeclaration->pElements;
	MFVertexElementData *pElementData = gRenderBuffers.pVertexDeclaration->pElementData;
	MFEffectData_OpenGL &techniqueData = *(MFEffectData_OpenGL*)pTechnique->pPlatformData;

	// bind the vertex streams...
	GLint attribs[16];
	int stream = -1;

	for(int a=0; a<gRenderBuffers.pVertexDeclaration->numElements; ++a)
	{
		int type = pElements[a].type;

		if(pElements[a].index == 0)
			attribs[a] = glGetAttribLocation(techniqueData.program, gAttribNames[type]);
		if(pElements[a].index > 0 || attribs[a] == -1)
		{
			gAttribNames[type][gAttribNameLen[type]] = (char)('0' + pElements[a].index);
			attribs[a] = glGetAttribLocation(techniqueData.program, gAttribNames[type]);
			gAttribNames[type][gAttribNameLen[type]] = 0;
		}

		if(attribs[a] == -1)
			continue;

		MFVertexDataFormatGL &f = gDataFormat[pElements[a].format];

		if(pElements[a].stream != stream)
		{
			stream = pElements[a].stream;
			glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(size_t)gRenderBuffers.pVertexBuffer[stream]->pPlatformData);
		}

		glVertexAttribPointer(attribs[a], f.components, f.type, f.normalise, pElementData[a].stride, (GLvoid*)(size_t)pElementData[a].offset);

		glEnableVertexAttribArray(attribs[a]);
	}

	glDrawArrays(gPrimTypes[primType], firstVertex, numVertices);

	// unbind the streams
	for(int a=0; a<gRenderBuffers.pVertexDeclaration->numElements; ++a)
	{
		if(attribs[a] != -1)
			glDisableVertexAttribArray(attribs[a]);
	}

	MFCheckForOpenGLError(true);
}

MF_API void MFVertex_RenderIndexedVertices(MFEffectTechnique *pTechnique, MFPrimType primType, int vertexOffset, int indexOffset, int numVertices, int numIndices)
{
	// TODO: how to we set VB/IB offsets in OpenGL?

	MFVertexElement *pElements = gRenderBuffers.pVertexDeclaration->pElements;
	MFVertexElementData *pElementData = gRenderBuffers.pVertexDeclaration->pElementData;
	MFEffectData_OpenGL &techniqueData = *(MFEffectData_OpenGL*)pTechnique->pPlatformData;

	// bind the vertex streams...
	GLint attribs[16];
	int stream = -1;

	for(int a=0; a<gRenderBuffers.pVertexDeclaration->numElements; ++a)
	{
		int type = pElements[a].type;

		// TODO: we shouldn't be looking up attribute locations at runtime!
		// pre-lookup to an array in techniqueData
		if(pElements[a].index == 0)
			attribs[a] = glGetAttribLocation(techniqueData.program, gAttribNames[type]);
		if(pElements[a].index > 0 || attribs[a] == -1)
		{
			gAttribNames[type][gAttribNameLen[type]] = (char)('0' + pElements[a].index);
			attribs[a] = glGetAttribLocation(techniqueData.program, gAttribNames[type]);
			gAttribNames[type][gAttribNameLen[type]] = 0;
		}

		if(attribs[a] == -1)
			continue;

		MFVertexDataFormatGL &f = gDataFormat[pElements[a].format];

		if(pElements[a].stream != stream)
		{
			stream = pElements[a].stream;
			glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(size_t)gRenderBuffers.pVertexBuffer[stream]->pPlatformData);
		}

		glVertexAttribPointer(attribs[a], f.components, f.type, f.normalise, pElementData[a].stride, (GLvoid*)(size_t)pElementData[a].offset);

		glEnableVertexAttribArray(attribs[a]);
	}

	// bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)(size_t)gRenderBuffers.pIndexBuffer->pPlatformData);
	glDrawElements(gPrimTypes[primType], numIndices, GL_UNSIGNED_SHORT, NULL);

	// unbind the streams
	for(int a=0; a<gRenderBuffers.pVertexDeclaration->numElements; ++a)
	{
		if(attribs[a] != -1)
			glDisableVertexAttribArray(attribs[a]);
	}

	MFCheckForOpenGLError(true);
}

#endif // MF_RENDERER
