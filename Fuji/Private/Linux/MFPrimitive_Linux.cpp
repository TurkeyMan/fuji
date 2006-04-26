#include "Fuji.h"
#include "MFPrimitive.h"

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
	primType = type & PT_PrimMask;
}

void MFBegin(uint32 vertexCount)
{
	beginCount = vertexCount;

	glBegin(gPrimTypes[primType]);
}

void MFSetMatrix(const MFMatrix &mat)
{
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
	// TODO: work out raw colour byte order
}

void MFSetTexCoord1(float u, float v)
{
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
	glVertex3f(x, y, z);

	if(primType == PT_QuadList && (currentVert & 1))
	{
		// TODO: add top right and bottom lft verts
	}

	++currentVert;
}

void MFEnd()
{
	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	glEnd();
}
