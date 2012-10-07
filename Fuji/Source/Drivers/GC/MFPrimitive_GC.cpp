#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_GC

#include "Display_Internal.h"
#include "MFView.h"
#include "MFPrimitive.h"
#include "MFTexture.h"
#include "MFRenderer.h"
#include "MFMaterial.h"

#include <ogc/gx.h>

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

MFInitStatus MFPrimitive_InitModule()
{
}

void MFPrimitive_DeinitModule()
{
}

void MFPrimitive_DrawStats()
{

}

MF_API void MFPrimitive(uint32 type, uint32 hint)
{
}

MF_API void MFBegin(uint32 vertexCount)
{
	beginCount = vertexCount;
	currentVert = 0;

	GX_Begin(gPTLookup[primType], GX_VTXFMT0, beginCount);
}

MF_API void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

//	GX_LoadPosMtxImm(mat, 0);
}

MF_API void MFSetColour(float r, float g, float b, float a)
{
	GX_Color4u8((uint8)(r*255.0f), (uint8)(g*255.0f), (uint8)(b*255.0f), (uint8)(a*255.0f));
}

MF_API void MFSetTexCoord1(float u, float v)
{
	GX_TexCoord2f32(u, v);
}

MF_API void MFSetNormal(float x, float y, float z)
{
	GX_Normal3f32(x, y, z);
}

MF_API void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	GX_Position3f32(x, y, z);
	++currentVert;
}

MF_API void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	GX_End();
}

#endif
