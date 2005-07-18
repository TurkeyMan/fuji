#include "Common.h"
#include "Display_Internal.h"
#include "View.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Primitive.h"
#include "Texture.h"
#include "Renderer.h"
#include "MFMaterial.h"

#include <pspdisplay.h>
#include <pspgu.h>

#define NUM_VERTS 2048
struct Vertex
{
	float u, v;
	unsigned int colour;
	float x,y,z;
};

struct Vertex __attribute__((aligned(16))) primBuffer[NUM_VERTS];
Vertex current;

uint32 primType = 0;
uint32 beginCount = 0;
uint32 startVert = 0;
uint32 currentVert = 0;

void Primitive_InitModule()
{
	CALLSTACK;
}

void Primitive_DeinitModule()
{
	CALLSTACK;
}

void MFPrimitive(uint32 type, uint32 hint)
{
	CALLSTACK;

	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(Mat_White));
	}

	sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&Matrix::identity);
	sceGuSetMatrix(GU_PROJECTION, (ScePspFMatrix4*)&View_GetViewToScreenMatrix());

	if(View_IsOrtho())
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&Matrix::identity);
	else
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&View_GetWorldToViewMatrix());

	Renderer_Begin();
}

void MFBegin(uint32 vertexCount)
{
	CALLSTACK;

	beginCount = vertexCount;
	startVert = currentVert;
//	currentVert = 0;
}

void MFSetMatrix(const Matrix &mat)
{
	CALLSTACK;
	sceGuSetMatrix(2, (ScePspFMatrix4*)&mat);
}

void MFSetColour(const Vector4 &colour)
{
	MFSetColour(colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(a*255.0f) << 24) |
					 ((uint32)(b*255.0f) << 16) |
					 ((uint32)(g*255.0f) << 8) |
					  (uint32)(r*255.0f);
}

void MFSetColour(uint32 col)
{
	current.colour = col;
}

void MFSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
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
	CALLSTACK;

	current.x = x;
	current.y = y;
	current.z = z;

	primBuffer[currentVert] = current;
	++currentVert;
	DBGASSERT(currentVert < NUM_VERTS, STR("Exceeded primitive vertex cache %d", NUM_VERTS));
}

void MFEnd()
{
	CALLSTACK;

	DBGASSERT(currentVert - startVert == beginCount, "Incorrect number of vertices.");

	int pt = 0;

	switch(primType)
	{
		case PT_PointList:
			pt = GU_POINTS;
			break;
		case PT_LineList:
			pt = GU_LINES;
			break;
		case PT_LineStrip:
			pt = GU_LINE_STRIP;
			break;
		case PT_TriList:
			pt = GU_TRIANGLES;
			break;
		case PT_TriStrip:
			pt = GU_TRIANGLE_STRIP;
			break;
		case PT_TriFan:
			pt = GU_TRIANGLE_FAN;
			break;
		default:
			DBGASSERT(false, "Unknown Primitive Type..");
			break;
	}

	sceKernelDcacheWritebackAll();
	sceGuDrawArray(pt, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, beginCount, 0, &primBuffer[startVert]);
}
