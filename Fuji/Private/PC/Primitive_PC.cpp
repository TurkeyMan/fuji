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

LitVertex primBuffer[1024];
LitVertex current;

uint32 primType;
uint32 beginCount;
uint32 currentVert;

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

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&Matrix::identity);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&View_GetViewToScreenMatrix());

	if(View_IsOrtho())
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&Matrix::identity);
	else
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&View_GetWorldToViewMatrix());

	Renderer_Begin();
}

void MFBegin(uint32 vertexCount)
{
	CALLSTACK;

	beginCount = vertexCount;
	currentVert = 0;

	current.u = current.v = 0.0f;
	current.colour = 0xFFFFFFFF;
	current.normal.x = current.normal.z = 0.0f;
	current.normal.y = 1.0f;
}

void MFSetMatrix(const Matrix &mat)
{
	CALLSTACK;

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);
}

void MFSetColour(const Vector4 &colour)
{
	MFSetColour(colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(r*255.0f))<<16 | ((uint32)(g*255.0f))<<8 | (uint32)(b*255.0f) | ((uint32)(a*255.0f))<<24;
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
	current.normal.x = x;
	current.normal.y = y;
	current.normal.z = z;
}

void MFSetPosition(const Vector3 &pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

void MFSetPosition(float x, float y, float z)
{
	CALLSTACK;

	current.pos.x = x;
	current.pos.y = y;
	current.pos.z = z;

	primBuffer[currentVert] = current;
	++currentVert;
}

void MFEnd()
{
	CALLSTACK;

	DBGASSERT(currentVert == beginCount, "Incorrect number of vertices.");

	pd3dDevice->SetFVF(LitVertex::FVF);

	switch(primType)
	{
		case PT_PointList:
			pd3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST, beginCount, primBuffer, sizeof(LitVertex));
			break;
		case PT_LineList:
			pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, beginCount/2, primBuffer, sizeof(LitVertex));
			break;
		case PT_LineStrip:
			pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, beginCount-1, primBuffer, sizeof(LitVertex));
			break;
		case PT_TriList:
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, beginCount/3, primBuffer, sizeof(LitVertex));
			break;
		case PT_TriStrip:
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, beginCount-2, primBuffer, sizeof(LitVertex));
			break;
		case PT_TriFan:
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, beginCount-2, primBuffer, sizeof(LitVertex));
			break;
	}
}
