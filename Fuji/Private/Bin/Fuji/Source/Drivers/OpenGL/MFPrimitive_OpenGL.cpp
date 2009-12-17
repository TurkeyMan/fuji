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
	#define MFPrimitive_EndBlitter MFPrimitive_EndBlitter_OpenGL
#endif


#include "MFPrimitive.h"
#include "MFMaterial.h"
#include "MFView.h"
#include "MFTexture_Internal.h"
#include "MFRenderer.h"
#include "Display.h"
#include "Materials/MFMat_Standard.h"

#if defined(MF_LINUX) || defined(MF_OSX)
	#include <GL/glx.h>
#endif

#if defined(MF_WINDOWS)
	#include <windows.h>
	#include <gl/gl.h>
#endif

struct Vert
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	uint32 colour;
};

static Vert prevVert;
static Vert curVert;

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
	GL_QUADS
};

void MFPrimitive_InitModule()
{
	MFCALLSTACK;
}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;
}

void MFPrimitive_DrawStats()
{
}

void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));
	}

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat *)&MFView_GetViewToScreenMatrix());

	glMatrixMode(GL_MODELVIEW);
	if(MFView_IsOrtho())
		glLoadMatrixf((GLfloat *)&MFMatrix::identity);
	else
		glLoadMatrixf((GLfloat *)&MFView_GetWorldToViewMatrix());

	glDepthRange(0.0f, 1.0f);

	MFRenderer_Begin();
}

void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	MFDebug_Assert(vertexCount > 0, "Invalid primitive count.");

	beginCount = vertexCount;
	currentVert = 0;

	MFZeroMemory(&curVert, sizeof(curVert));
	curVert.colour = 0xFFFFFFFF;
	curVert.ny = 1.0f;

	glBegin(gPrimTypes[primType]);
}

void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

	MFMatrix localToView;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat *)MFView_GetLocalToView(mat, &localToView));
}

void MFSetColour(float r, float g, float b, float a)
{
	curVert.colour = ((uint32)(r*255.0f) << 0) | ((uint32)(g*255.0f) << 8) | ((uint32)(b*255.0f) << 16) | ((uint32)(a*255.0f) << 24);
}

void MFSetTexCoord1(float u, float v)
{
	curVert.u = u;
	curVert.v = v;
}

void MFSetNormal(float x, float y, float z)
{
	curVert.nx = x;
	curVert.ny = y;
	curVert.nz = z;
}

void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	bool bIsQuads = primType == PT_QuadList;
	bool bInsertVerts = bIsQuads && (currentVert & 1);
	uint32 col;

	if(bInsertVerts)
	{
		// add top right vertex
		col = prevVert.colour;

		glColor4ub((uint8)(col & 0xFF), (uint8)((col >> 8) & 0xFF), (uint8)((col >> 16) & 0xFF), (uint8)((col >> 24) & 0xFF));
		glTexCoord2f(curVert.u, prevVert.v);
		glVertex3f(x, prevVert.y, prevVert.z);
	}

	col = curVert.colour;

	glColor4ub((uint8)(col & 0xFF), (uint8)((col >> 8) & 0xFF), (uint8)((col >> 16) & 0xFF), (uint8)((col >> 24) & 0xFF));
	glTexCoord2f(curVert.u, curVert.v);
	glVertex3f(x, y, z);

	if(bInsertVerts)
	{
		// add the bottom left vertex
		glColor4ub((uint8)(col & 0xFF), (uint8)((col >> 8) & 0xFF), (uint8)((col >> 16) & 0xFF), (uint8)((col >> 24) & 0xFF));
		glTexCoord2f(prevVert.u, curVert.v);
		glVertex3f(prevVert.x, y, z);
	}
	else if(bIsQuads)
	{
		curVert.x = x;
		curVert.y = y;
		curVert.z = z;
		prevVert = curVert;
	}

	++currentVert;
}

void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	glEnd();
}


static int textureWidth, textureHeight;
static float uScale, vScale;
static float halfTexelU, halfTexelV;

void MFPrimitive_BeginBlitter(int numBlits)
{
	MFView_Push();

	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);
	MFView_SetOrtho(&rect);

	MFTexture *pTex;
	MFMaterial *pMat = MFMaterial_GetCurrent();
	MFMaterial_GetParameter(pMat, MFMatStandard_DifuseMap, 0, &pTex);
	textureWidth = pTex->pTemplateData->pSurfaces[0].width;
	textureHeight = pTex->pTemplateData->pSurfaces[0].height;

	uScale = 1.0f / (float)textureWidth;
	vScale = 1.0f / (float)textureHeight;
	halfTexelU = uScale * 0.5f;
	halfTexelV = vScale * 0.5f;

	MFPrimitive(PT_QuadList);
	MFBegin(numBlits * 2);
}

void MFPrimitive_Blit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
	MFSetTexCoord1((float)tx * uScale - halfTexelU, (float)ty * vScale - halfTexelV);
	MFSetPosition((float)x, (float)y, 0.0f);
	MFSetTexCoord1((float)(tx + tw) * uScale - halfTexelU, (float)(ty + th) * vScale - halfTexelV);
	MFSetPosition((float)(x + w), (float)(y + h), 0.0f);
}

void MFPrimitive_EndBlitter()
{
	MFEnd();
	MFView_Pop();
}

#endif
