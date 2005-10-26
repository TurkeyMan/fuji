#if !defined(_PRIMITIVE_H)
#define _PRIMITIVE_H

#include "MFSystem.h"
#include "MFVector.h"
#include "MFMatrix.h"

enum PrimType
{
	PT_PointList,
	PT_LineList,
	PT_LineStrip,
	PT_TriList,
	PT_TriStrip,
	PT_TriFan,
	PT_PrimMask = 0x07,

	PT_Prelit = 64,
	PT_Untextured = 128
};

void Primitive_InitModule();
void Primitive_DeinitModule();

void DrawMFPrimitiveStats();

void MFPrimitive(uint32 primType, uint32 hint = 0);
void MFBegin(uint32 vertexCount);

void MFSetMatrix(const MFMatrix &mat);
void MFSetColour(const MFVector &colour);
void MFSetColour(float r, float g, float b, float a = 1.0f);
void MFSetColour(uint32 col);
void MFSetTexCoord1(float u, float v);
void MFSetNormal(const MFVector &normal);
void MFSetNormal(float x, float y, float z);
void MFSetPosition(const MFVector &pos);
void MFSetPosition(float x, float y, float z);

void MFEnd();

#endif
