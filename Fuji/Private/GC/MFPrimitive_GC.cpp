#include "Fuji.h"
#include "Display_Internal.h"
#include "MFView.h"
#include "MFPrimitive.h"
#include "MFTexture.h"
#include "MFRenderer.h"
#include "MFMaterial.h"

#include <gx.h>

uint32 primType;
uint32 beginCount;
uint32 currentVert;

static uint8 gPTLookup[7] =
{
	GX_POINTS,
	GX_LINES,
	GX_LINESTRIP,
	GX_TRIANGLES,
	GX_TRIANGLESTRIP,
	GX_TRIANGLEFAN,
	GX_QUADS
};


void Primitive_InitModule()
{
	MFCALLSTACK;
}

void Primitive_DeinitModule()
{
	MFCALLSTACK;
}

void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));
	}

	MFRenderer_Begin();
}

void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	beginCount = vertexCount;
	currentVert = 0;

	GX_Begin(gPTLookup[primType], GX_VTXFMT0, beginCount);
}

void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

	GX_LoadPosMtxImm(mat, 0);
}

void MFSetColour(const MFVector &colour)
{
	MFSetColour(colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{
	GX_Color4u8((uint8)(r*255.0f), (uint8)(g*255.0f), (uint8)(b*255.0f), (uint8)(a*255.0f));
}

void MFSetColour(uint32 col)
{
	GX_Color1u32(col);
}

void MFSetTexCoord1(float u, float v)
{
	GX_TexCoord2f32(u, v);
}

void MFSetNormal(const MFVector &normal)
{
	MFSetNormal(normal.x, normal.y, normal.z);
}

void MFSetNormal(float x, float y, float z)
{
	GX_Normal3f32(x, y, z);
}

void MFSetPosition(const MFVector &pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	GX_Position3f32(x, y, z);
	++currentVert;
}

void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	GX_End()
}
