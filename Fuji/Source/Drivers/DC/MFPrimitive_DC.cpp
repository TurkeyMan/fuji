#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_DC

#include "Display_Internal.h"
#include "View.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "Primitive.h"
#include "Texture.h"
#include "Renderer.h"
#include "MFMaterial.h"

uint32 primType;
uint32 beginCount;
uint32 currentVert;

MFInitStatus Primitive_InitModule()
{
	CALLSTACK;
}

void Primitive_DeinitModule()
{
	CALLSTACK;
}

MF_API void MFPrimitive(uint32 type, uint32 hint)
{
	CALLSTACK;

	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(Mat_White));
	}

	Renderer_Begin();
}

MF_API void MFBegin(uint32 vertexCount)
{
	CALLSTACK;

	beginCount = vertexCount;
	currentVert = 0;
}

MF_API void MFSetMatrix(const Matrix &mat)
{
	CALLSTACK;
}

MF_API void MFSetColour(float r, float g, float b, float a)
{
}

MF_API void MFSetTexCoord1(float u, float v)
{
}

MF_API void MFSetNormal(float x, float y, float z)
{
}

MF_API void MFSetPosition(float x, float y, float z)
{
	CALLSTACK;

	++currentVert;
}

MF_API void MFEnd()
{
	CALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");
}

#endif
