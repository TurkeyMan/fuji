#include "Fuji.h"
#include "MFPrimitive.h"

uint32 beginCount;
uint32 currentVert;

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
}

void MFBegin(uint32 vertexCount)
{
	beginCount = vertexCount;
}

void MFSetMatrix(const MFMatrix &mat)
{
}

void MFSetColour(const MFVector &colour)
{
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

void MFSetNormal(const MFVector &normal)
{
}

void MFSetNormal(float x, float y, float z)
{
}

void MFSetPosition(const MFVector &pos)
{
	++currentVert;
}

void MFSetPosition(float x, float y, float z)
{
	++currentVert;
}

void MFEnd()
{
	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");
}
