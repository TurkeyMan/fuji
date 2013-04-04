#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFPrimitive_InitModule MFPrimitive_InitModule_OpenGL
	#define MFPrimitive_DeinitModule MFPrimitive_DeinitModule_OpenGL
	#define MFPrimitive_DrawStats MFPrimitive_DrawStats_OpenGL
	#define MFPrimitive MFPrimitive_OpenGL
	#define MFBegin MFBegin_OpenGL
	#define MFSetMatrix MFSetMatrix_OpenGL
	#define MFSetColour MFSetColour_OpenGL
	#define MFSetTexCoord1 MFSetTexCoord1_OpenGL
	#define MFSetNormal MFSetNormal_OpenGL
	#define MFSetPosition MFSetPosition_OpenGL
	#define MFEnd MFEnd_OpenGL
	#define MFPrimitive_BeginBlitter MFPrimitive_BeginBlitter_OpenGL
	#define MFPrimitive_Blit MFPrimitive_Blit_OpenGL
	#define MFPrimitive_StretchBlit MFPrimitive_StretchBlit_OpenGL
	#define MFPrimitive_EndBlitter MFPrimitive_EndBlitter_OpenGL
#endif

#include "MFPrimitive.h"
#include "MFMaterial.h"
#include "MFView.h"
#include "MFTexture_Internal.h"
#include "MFRenderer.h"
#include "MFDisplay.h"
#include "Materials/MFMat_Standard.h"

#include "MFOpenGL.h"

static const int primBufferSize = 1536;

#if defined(MF_OPENGL_ES)
static bool gImmitateQuads = false;
#endif

struct Vert
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	uint32 colour;
};

static Vert primBuffer[primBufferSize];
static Vert current;

static uint32 beginCount;
static uint32 currentVert;

static uint32 primType;

static int gPrimTypes[7] =
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

MFInitStatus MFPrimitive_InitModule()
{
	MFCALLSTACK;

	return MFAIC_Succeeded;
}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;
}

void MFPrimitive_DrawStats()
{
}

MF_API void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	primType = type & PT_PrimMask;

#if defined(MF_OPENGL_ES)
	if(primType == PT_QuadList)
	{
		primType = PT_TriList;
		gImmitateQuads = true;
	}
	else
		gImmitateQuads = false;
#endif

	if(type & PT_Untextured)
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));

	glDepthRange(0.0f, 1.0f);

	MFRenderer_Begin();

	MFRenderer_OpenGL_SetMatrix(MFOGL_ShaderType_Projection, MFView_GetViewToScreenMatrix());

	if(MFView_IsOrtho())
		MFRenderer_OpenGL_SetMatrix(MFOGL_ShaderType_WorldView, MFMatrix::identity);
	else
		MFRenderer_OpenGL_SetMatrix(MFOGL_ShaderType_WorldView, MFView_GetWorldToViewMatrix());
}

MF_API void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	MFDebug_Assert(vertexCount > 0, "Invalid primitive count.");

#if defined(MF_OPENGL_ES)
	if(gImmitateQuads)
		beginCount = vertexCount * 3;
	else
#endif
	beginCount = (primType == PT_QuadList) ? vertexCount*2 : vertexCount;

	currentVert = 0;

	current.u = current.v = 0.0f;
	current.colour = 0xFFFFFFFF;
	current.nx = current.nz = 0.0f;
	current.ny = 1.0f;
}

MF_API void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

	MFMatrix localToView;
	MFView_GetLocalToView(mat, &localToView);
	MFRenderer_OpenGL_SetMatrix(MFOGL_ShaderType_WorldView, localToView);
}

MF_API void MFSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(r*255.0f) << 0) | ((uint32)(g*255.0f) << 8) | ((uint32)(b*255.0f) << 16) | ((uint32)(a*255.0f) << 24);
}

MF_API void MFSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
}

MF_API void MFSetNormal(float x, float y, float z)
{
	current.nx = x;
	current.ny = y;
	current.nz = z;
}

MF_API void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	current.x = x;
	current.y = y;
	current.z = z;

#if defined(MF_OPENGL_ES)
	if(gImmitateQuads && (currentVert & 1))
	{
		Vert &prev = primBuffer[currentVert - 1];

		primBuffer[currentVert + 0] = prev;
		primBuffer[currentVert + 1] = prev;
		primBuffer[currentVert + 2] = current;
		primBuffer[currentVert + 3] = current;
		primBuffer[currentVert + 4] = current;

		primBuffer[currentVert + 0].x = current.x;
		primBuffer[currentVert + 1].y = current.y;
		primBuffer[currentVert + 2].x = prev.x;
		primBuffer[currentVert + 3].y = prev.y;

		primBuffer[currentVert + 0].u = current.u;
		primBuffer[currentVert + 1].v = current.v;
		primBuffer[currentVert + 2].u = prev.u;
		primBuffer[currentVert + 3].v = prev.v;

		currentVert += 4;
	}
#else
	if(primType == PT_QuadList && (currentVert & 1))
	{
		Vert &prev = primBuffer[currentVert - 1];

		primBuffer[currentVert + 0] = prev;
		primBuffer[currentVert + 1] = current;
		primBuffer[currentVert + 2] = current;

		primBuffer[currentVert + 0].x = current.x;
		primBuffer[currentVert + 2].x = prev.x;

		primBuffer[currentVert + 0].u = current.u;
		primBuffer[currentVert + 2].u = prev.u;

		currentVert += 2;
	}
#endif
	else
		primBuffer[currentVert] = current;

	++currentVert;

	if(currentVert >= primBufferSize)
	{
		int newBeginCount = beginCount - currentVert;
		beginCount = currentVert;

		MFEnd();

		beginCount = newBeginCount;
		currentVert = 0;
	}
}

MF_API void MFEnd()
{
	MFCALLSTACK;
	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	if(beginCount)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);

#if defined(MF_OPENGL_SUPPORT_SHADERS)
		if(MFOpenGL_UseShaders())
		{
			GLuint program = MFRenderer_OpenGL_GetCurrentProgram();
			GLint pos = glGetAttribLocation(program, "vPos");
			GLint normal = glGetAttribLocation(program, "vNormal");
			GLint colour = glGetAttribLocation(program, "vColour");
			GLint uv0 = glGetAttribLocation(program, "vUV0");
			GLint uv1 = glGetAttribLocation(program, "vUV1");

			// gles2 uses some new functions...
			if(pos != -1)
			{
				glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), &primBuffer[0].x);
				glEnableVertexAttribArray(pos);
			}

			if(normal != -1)
			{
				glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), &primBuffer[0].nx);
				glEnableVertexAttribArray(normal);
			}

			if(colour != -1)
			{
				glVertexAttribPointer(colour, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vert), &primBuffer[0].colour);
				glEnableVertexAttribArray(colour);
			}

			if(uv0 != -1)
			{
				glActiveTexture(GL_TEXTURE0);

				glVertexAttribPointer(uv0, 2, GL_FLOAT, GL_TRUE, sizeof(Vert), &primBuffer[0].u);
				glEnableVertexAttribArray(uv0);
			}

			if(uv1 != -1)
			{
				glActiveTexture(GL_TEXTURE1);

				glVertexAttribPointer(uv1, 2, GL_FLOAT, GL_TRUE, sizeof(Vert), &primBuffer[0].u);
				glEnableVertexAttribArray(uv1);
			}

			glDrawArrays(gPrimTypes[primType], 0, beginCount);

			if(pos != -1)
				glDisableVertexAttribArray(pos);
			if(normal != -1)
				glDisableVertexAttribArray(normal);
			if(colour != -1)
				glDisableVertexAttribArray(colour);
			if(uv0 != -1)
				glDisableVertexAttribArray(uv0);
			if(uv1 != -1)
				glDisableVertexAttribArray(uv1);
		}
		else
#endif
		{
#if !defined(MF_OPENGL_ES) || MF_OPENGL_ES_VER < 2
			glVertexPointer(3, GL_FLOAT, sizeof(Vert), &primBuffer[0].x);
			glEnableClientState(GL_VERTEX_ARRAY);

			glNormalPointer(GL_FLOAT, sizeof(Vert), &primBuffer[0].nx);
			glEnableClientState(GL_NORMAL_ARRAY);

			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vert), &primBuffer[0].colour);
			glEnableClientState(GL_COLOR_ARRAY);

			glClientActiveTexture(GL_TEXTURE0);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vert), &primBuffer[0].u);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glClientActiveTexture(GL_TEXTURE1);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vert), &primBuffer[0].u);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glDrawArrays(gPrimTypes[primType], 0, beginCount);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);

			glClientActiveTexture(GL_TEXTURE0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glClientActiveTexture(GL_TEXTURE1);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
		}
	}

	MFCheckForOpenGLError(true);
}


static int textureWidth, textureHeight;
static float uScale, vScale;

MF_API void MFPrimitive_BeginBlitter(int numBlits)
{
	MFView_Push();

	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);
	MFView_SetOrtho(&rect);

	MFMaterial *pMat = MFMaterial_GetCurrent();

	MFTexture *pTex = MFMaterial_GetParameterT(pMat, MFMatStandard_Texture, MFMatStandard_Tex_DifuseMap);
	textureWidth = pTex->pTemplateData->pSurfaces[0].width;
	textureHeight = pTex->pTemplateData->pSurfaces[0].height;

	MFMatrix matrix;
	MFMaterial_GetParameterM(pMat, MFMatStandard_TextureMatrix, 0, &matrix);

	uScale = 1.0f / (float)textureWidth / matrix.GetXAxis().Magnitude3();
	vScale = 1.0f / (float)textureHeight / matrix.GetYAxis().Magnitude3();

	MFPrimitive(PT_QuadList);
	MFBegin(numBlits * 2);
}

MF_API void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	MFPrimitive_StretchBlit(x, y, tw, th, tx, ty, tw, th);
}

MF_API void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
	MFSetTexCoord1((float)tx * uScale, (float)ty * vScale);
	MFSetPosition((float)x, (float)y, 0.0f);
	MFSetTexCoord1((float)(tx + tw) * uScale, (float)(ty + th) * vScale);
	MFSetPosition((float)(x + w), (float)(y + h), 0.0f);
}

MF_API void MFPrimitive_EndBlitter()
{
	MFEnd();
	MFView_Pop();
}

#endif
