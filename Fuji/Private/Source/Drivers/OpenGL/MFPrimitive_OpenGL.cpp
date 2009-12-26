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
#include "Display.h"
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

#if defined(MF_OPENGL_ES)
	if(primType == PT_QuadList)
	{
		primType = PT_TriList;
		gImmitateQuads = true;
	}
	else
		gImmitateQuads = false;
#endif

	MFMaterial *pMatOverride = (MFMaterial*)MFRenderer_GetRenderStateOverride(MFRS_MaterialOverride);
	if(pMatOverride)
		MFMaterial_SetMaterial(pMatOverride);
	else if(type & PT_Untextured)
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));

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

#if defined(MF_OPENGL_ES)
	if(gImmitateQuads)
		beginCount = vertexCount * 3;
	else
#endif
	beginCount = vertexCount;

	currentVert = 0;

	current.u = current.v = 0.0f;
	current.colour = 0xFFFFFFFF;
	current.nx = current.nz = 0.0f;
	current.ny = 1.0f;
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
	current.colour = ((uint32)(r*255.0f) << 0) | ((uint32)(g*255.0f) << 8) | ((uint32)(b*255.0f) << 16) | ((uint32)(a*255.0f) << 24);
}

void MFSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
}

void MFSetNormal(float x, float y, float z)
{
	current.nx = x;
	current.ny = y;
	current.nz = z;
}

void MFSetPosition(float x, float y, float z)
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

void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	int numVerts = 0;
	switch(primType)
	{
		case PT_PointList:	numVerts = beginCount;		break;
		case PT_LineList:	numVerts = beginCount/2;	break;
		case PT_LineStrip:	numVerts = beginCount-1;	break;
		case PT_TriList:	numVerts = beginCount/3;	break;
		case PT_TriStrip:	numVerts = beginCount-2;	break;
		case PT_TriFan:		numVerts = beginCount-2;	break;
	}

	if(numVerts)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glVertexPointer(3, GL_FLOAT, sizeof(Vert), &primBuffer[0].x);
		glNormalPointer(GL_FLOAT, sizeof(Vert), &primBuffer[0].nx);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vert), &primBuffer[0].colour);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vert), &primBuffer[0].u);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glDrawArrays(gPrimTypes[primType], 0, numVerts);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
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

void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	MFPrimitive_StretchBlit(x, y, tw, th, tx, ty, tw, th);
}

void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
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
