#include "Common.h"
#include "Display.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Primitive.h"

#define PRIM_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX0)
struct PrimVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	DWORD colour;
	float u,v;
};

PrimVertex primBuffer[1024];
PrimVertex current;

uint32 primType;
uint32 beginCount;
uint32 currentVert;

void Primitive_InitModule()
{

}

void Primitive_DeinitModule()
{

}

void BeginPrimitive(uint32 type, uint32 hint)
{
	primType = type;
}

void PrimBegin(uint32 vertexCount)
{
	beginCount = vertexCount;
	currentVert = 0;

	current.u = current.v = 0.0f;
	current.colour = 0xFFFFFFFF;
	current.normal.x = current.normal.z = 0.0f;
	current.normal.y = 1.0f;
}

void PrimSetMatrix(Matrix &mat)
{
	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);
}

void PrimSetColour(Vector4 &colour)
{
	PrimSetColour(colour.x, colour.y, colour.z, colour.w);
}

void PrimSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(r*255.0f))<<16 | ((uint32)(g*255.0f))<<8 | (uint32)(b*255.0f) | ((uint32)(a*255.0f))<<24;
}

void PrimSetColour(uint32 col)
{
	current.colour = col;
}

void PrimSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
}

void PrimSetNormal(Vector3 &normal)
{
	PrimSetNormal(normal.x, normal.y, normal.z);
}

void PrimSetNormal(float x, float y, float z)
{
	current.normal.x = x;
	current.normal.y = y;
	current.normal.z = z;
}

void PrimSetPosition(Vector3 &pos)
{
	PrimSetPosition(pos.x, pos.y, pos.z);
}

void PrimSetPosition(float x, float y, float z)
{
	current.pos.x = x;
	current.pos.y = y;
	current.pos.z = z;

	primBuffer[currentVert] = current;
	++currentVert;
}

void PrimEnd()
{
	DBGASSERT(currentVert == beginCount, "Incorrect number of vertices.");

	pd3dDevice->SetVertexShader(PRIM_FVF);

	switch(primType)
	{
		case PT_PointList:
			pd3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST, beginCount, primBuffer, sizeof(PrimVertex));
			break;
		case PT_LineList:
			pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, beginCount-1, primBuffer, sizeof(PrimVertex));
			break;
		case PT_LineStrip:
			pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, beginCount/2, primBuffer, sizeof(PrimVertex));
			break;
		case PT_TriList:
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, beginCount/3, primBuffer, sizeof(PrimVertex));
			break;
		case PT_TriStrip:
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, beginCount-2, primBuffer, sizeof(PrimVertex));
			break;
		case PT_TriFan:
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, beginCount-2, primBuffer, sizeof(PrimVertex));
			break;
	}
}
