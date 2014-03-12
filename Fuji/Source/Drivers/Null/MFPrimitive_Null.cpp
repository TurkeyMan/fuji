#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_NULL

#include "MFPrimitive.h"

uint32 beginCount;
uint32 currentVert;

MFInitStatus MFPrimitive_InitModule(int moduleId, bool bPerformInitialisation)
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
}

MF_API void MFSetMatrix(const MFMatrix &mat)
{
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
	++currentVert;
}

MF_API void MFEnd()
{
	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");
}

MF_API void MFPrimitive_BeginBlitter(int numBlits)
{
}

MF_API void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
}

MF_API void MFPrimitive_BlitRect(int x, int y, const MFRect &uvs)
{
}

MF_API void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
}

MF_API void MFPrimitive_StretchBlitRect(int x, int y, int w, int h, const MFRect &uvs)
{
}

MF_API void MFPrimitive_EndBlitter()
{
}

#endif
