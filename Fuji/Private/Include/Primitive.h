#if !defined(_PRIMITIVE_H)
#define _PRIMITIVE_H

#include "System.h"
#include "Matrix.h"
#include "Vector3.h"
#include "Vector4.h"

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


void BeginPrimitive(uint32 primType, uint32 hint = NULL);
void PrimBegin(uint32 vertexCount);

void PrimSetMatrix(Matrix &mat);
void PrimSetColour(Vector4 &colour);
void PrimSetColour(float r, float g, float b, float a = 1.0f);
void PrimSetColour(uint32 col);
void PrimSetTexCoord1(float u, float v);
void PrimSetNormal(Vector3 &normal);
void PrimSetNormal(float x, float y, float z);
void PrimSetPosition(Vector3 &pos);
void PrimSetPosition(float x, float y, float z);

void PrimEnd();

#endif