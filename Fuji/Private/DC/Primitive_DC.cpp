#include "Common.h"
#include "Display.h"
#include "View.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Primitive.h"
#include "Texture.h"
#include "Renderer.h"
#include "Material.h"

uint32 primType;
uint32 beginCount;
uint32 currentVert;

void Primitive_InitModule()
{

}

void Primitive_DeinitModule()
{

}

void MFPrimitive(uint32 type, uint32 hint)
{
	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		Texture::UseNone();
	}

}

void MFBegin(uint32 vertexCount)
{
	beginCount = vertexCount;
	currentVert = 0;

}

void MFSetMatrix(const Matrix &mat)
{

}

void MFSetColour(const Vector4 &colour)
{
	MFSetColour(colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{

}

void MFSetColour(uint32 col)
{

}

void MFSetTexCoord1(float u, float v)
{

}

void MFSetNormal(const Vector3 &normal)
{
	MFSetNormal(normal.x, normal.y, normal.z);
}

void MFSetNormal(float x, float y, float z)
{

}

void MFSetPosition(const Vector3 &pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

void MFSetPosition(float x, float y, float z)
{

}

void MFEnd()
{
	DBGASSERT(currentVert == beginCount, "Incorrect number of vertices.");

}
