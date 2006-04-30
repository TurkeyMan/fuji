#include "Fuji.h"
#include "MFPrimitive.h"
#include "MFMaterial.h"
#include "MFView.h"
#include "MFTexture_Internal.h"
#include "MFRenderer.h"
#include "Display.h"
#include "Materials/MFMat_Standard.h"

#include <GL/glx.h>

struct Vert
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	uint32 colour;
};

Vert prevVert;
Vert curVert;

uint32 beginCount;
uint32 currentVert;

uint32 primType;

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

	glBegin(gPrimTypes[primType]);
}

void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

	MFMatrix localToView;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat *)MFView_GetLocalToView(mat, &localToView));
}

void MFSetColour(const MFVector &colour)
{
	glColor4f(colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void MFSetColour(uint32 col)
{
	glColor4ub((uint8)(col & 0xFF), (uint8)((col >> 8) & 0xFF), (uint8)((col >> 16) & 0xFF), (uint8)((col >> 24) & 0xFF));
}

void MFSetTexCoord1(float u, float v)
{
	if(primType == PT_QuadList)
	{
		curVert.u = u;
		curVert.v = v;
	}

	glTexCoord2f(u, v);
}

void MFSetNormal(const MFVector &normal)
{
	glNormal3f(normal.x, normal.y, normal.z);
}

void MFSetNormal(float x, float y, float z)
{
	glNormal3f(x, y, z);
}

void MFSetPosition(const MFVector &pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	if(primType == PT_QuadList)
	{
		if(currentVert & 1)
		{
			// TODO: add top right and bottom left verts
			glTexCoord2f(curVert.u, prevVert.v);
			glVertex3f(x, prevVert.y, prevVert.z);
			glTexCoord2f(curVert.u, curVert.v);
			glVertex3f(x, y, z);
			glTexCoord2f(prevVert.u, curVert.v);
			glVertex3f(prevVert.x, y, z);
		}
		else
		{
			glVertex3f(x, y, z);

			curVert.x = x;
			curVert.y = y;
			curVert.z = z;

			prevVert = curVert;
		}
	}
	else
	{
		glVertex3f(x, y, z);
	}

	++currentVert;
}

void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	glEnd();
}


int textureWidth, textureHeight;
float uScale, vScale;
float halfTexelU, halfTexelV;

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
	MFSetTexCoord1((float)tx * uScale - halfTexelU, (float)ty * vScale - halfTexelV);
	MFSetPosition((float)x, (float)y, 0.0f);
	MFSetTexCoord1((float)(tx + tw) * uScale - halfTexelU, (float)(ty + th) * vScale - halfTexelV);
	MFSetPosition((float)(x + tw), (float)(y + th), 0.0f);
}

void MFPrimitive_EndBlitter()
{
	MFEnd();
	MFView_Pop();
}
