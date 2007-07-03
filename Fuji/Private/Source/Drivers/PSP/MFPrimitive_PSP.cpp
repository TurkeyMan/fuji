#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PSP

#include "MFHeap.h"
#include "Display_Internal.h"
#include "MFView.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "MFPrimitive.h"
#include "MFTexture.h"
#include "MFRenderer.h"
#include "MFMaterial.h"
#include "MFFont.h"
#include "DebugMenu_Internal.h"

#include <pspdisplay.h>
#include <pspgu.h>

#define NUM_VERTS (4096*4)
struct ImmediateVertex
{
	float u, v;
	unsigned int colour;
	float x,y,z;
};

struct ImmediateVertex __attribute__((aligned(16))) primBuffer[NUM_VERTS];
ImmediateVertex current;

ImmediateVertex *pPrimBuffer = NULL;

uint32 primType = 0;
uint32 beginCount = 0;
uint32 startVert = 0;
uint32 currentVert = 0;

const char *pPrimStrings[] = { "Disabled", "Enabled", NULL };
MenuItemIntString showPrimitiveStats(pPrimStrings, 0);

void MFPrimitive_InitModule()
{
	MFCALLSTACK;

	DebugMenu_AddItem("Show Primitive Stats", "Fuji Options", &showPrimitiveStats, NULL, NULL);

	pPrimBuffer = (ImmediateVertex*)MFHeap_GetUncachedPointer(primBuffer);
}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;
}

void MFPrimitive_DrawStats()
{
	if(showPrimitiveStats)
	{
		MFMaterial_SetMaterial(NULL);

		MFPrimitive(PT_TriStrip);
		MFBegin(4);
		MFSetColour(0,0,0,0.8f);
		MFSetPosition(70, 252, 0);
		MFSetPosition(70, 20, 0);
		MFSetPosition(410, 252, 0);
		MFSetPosition(410, 20, 0);
		MFEnd();

		MFFont_DrawTextf(MFFont_GetDebugFont(), 90.0f, 40.0f, 20.0f, MakeVector(1,1,0,1), "NumVerts: %d", currentVert);
	}
}

void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));
	}

	sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&MFMatrix::identity);
	sceGuSetMatrix(GU_PROJECTION, (ScePspFMatrix4*)&MFView_GetViewToScreenMatrix());

	if(MFView_IsOrtho())
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&MFMatrix::identity);
	else
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&MFView_GetWorldToViewMatrix());

	MFRenderer_Begin();
}

void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	beginCount = vertexCount;
	startVert = currentVert;

	current.colour = 0xFFFFFFFF;
	current.u = 0.0f;
	current.v = 0.0f;

	MFDebug_Assert(startVert+vertexCount < NUM_VERTS, MFStr("Exceeded primitive vertex cache %d", NUM_VERTS));
}

void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;
	sceGuSetMatrix(2, (ScePspFMatrix4*)&mat);
}

void MFSetColour(const MFVector &colour)
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

void MFSetNormal(const MFVector &normal)
{
	MFSetNormal(normal.x, normal.y, normal.z);
}

void MFSetNormal(float x, float y, float z)
{
}

void MFSetPosition(const MFVector &pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	current.x = x;
	current.y = y;
	current.z = z;

	pPrimBuffer[currentVert] = current;
	++currentVert;
}

void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert - startVert == beginCount, "Incorrect number of vertices.");

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
		case PT_QuadList:
			pt = GU_SPRITES;
			break;
		default:
			MFDebug_Assert(false, "Unknown Primitive Type..");
			break;
	}

//	Heap_FlushDCache();
	sceGuDrawArray(pt, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, beginCount, 0, &pPrimBuffer[startVert]);
}

#endif
