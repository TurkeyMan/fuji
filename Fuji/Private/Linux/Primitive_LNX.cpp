#include "Common.h"
#include "Primitive.h"

// FIXME

uint32 primType;
uint32 vertexCount;
uint32 beginCount;

void Primitive_InitModule()
{
}

void Primitive_DeinitModule()
{
}

void MFPrimitive(uint32 type, uint32 hint)
{
	primType = type & PT_PrimMask;
	vertexCount = 0;
}

void MFBegin(uint32 vertexCount)
{
	beginCount = vertexCount;

	switch(primType)
	{
	case PT_PointList:
		glBegin(GL_POINTS);
		break;
	case PT_LineList:
		glBegin(GL_LINES);
		break;
	case PT_LineStrip:
		glBegin(GL_LINE_STRIP);
		break;
	case PT_TriList:
		glBegin(GL_TRIANGLES);
		break;
	case PT_TriStrip:
		glBegin(GL_TRIANGLE_STRIP);
		break;
	case PT_TriFan:
		glBegin(GL_TRIANGLE_FAN);
		break;
	default:
		DBGASSERT(false, "Unsupported PrimType");
	};
}

void MFSetMatrix(const Matrix &mat)
{
}

void MFSetColour(const Vector4 &colour)
{
	glColor4f(colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void MFSetColour(uint32 col)
{
	glColor4ub((col&0xFF0000) >> 16, (col&0xFF00) >> 8, col&0xFF, (col&0xFF000000) >> 24);
}

void MFSetTexCoord1(float u, float v)
{
	glTexCoord2f(u, v);
}

void MFSetNormal(const Vector3 &normal)
{
	glNormal3f(normal.x, normal.y, normal.z);
}

void MFSetPosition(const Vector3 &pos)
{
	glVertex3f(pos.x, pos.y, pos.z);

	++vertexCount;
}

void MFSetPosition(float x, float y, float z)
{
	glVertex3f(x, y, z);

	++vertexCount;
}

void MFEnd()
{
	glEnd();

	DBGASSERT(beginCount == vertexCount, "Incorrect number of vertices in MFPrimitive block");
}
