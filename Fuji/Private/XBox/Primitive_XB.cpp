#include "Common.h"
#include "Display_Internal.h"
#include "Texture_Internal.h"
#include "Material_Internal.h"
#include "View_Internal.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Primitive.h"
#include "Renderer.h"

LitVertex primBuffer[1024];
LitVertex current;

uint32 primType;
uint32 beginCount;
uint32 currentVert;

void Primitive_InitModule()
{

}

void Primitive_DeinitModule()
{

}

void MFPrimitive(uint32 type, uint32 hint)
{
	uint32 rendererFlags = 0;
	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		rendererFlags |= RT_Untextured;
	}

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&Matrix::identity);
	pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&View_GetWorldToViewMatrix());
	pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&View_GetViewToScreenMatrix());

	Renderer_SetRenderer(rendererFlags, 0, RS_MFPrimitive);
}

void MFBegin(uint32 vertexCount)
{
	beginCount = vertexCount;
	currentVert = 0;

	pd3dDevice->SetVertexShader(LitVertex::FVF);

	switch(primType)
	{
		case PT_PointList:
			pd3dDevice->Begin(D3DPT_POINTLIST);
			break;
		case PT_LineList:
			pd3dDevice->Begin(D3DPT_LINELIST);
			break;
		case PT_LineStrip:
			pd3dDevice->Begin(D3DPT_LINESTRIP);
			break;
		case PT_TriList:
			pd3dDevice->Begin(D3DPT_TRIANGLELIST);
			break;
		case PT_TriStrip:
			pd3dDevice->Begin(D3DPT_TRIANGLESTRIP);
			break;
		case PT_TriFan:
			pd3dDevice->Begin(D3DPT_TRIANGLEFAN);
			break;
	}
}

void MFSetMatrix(const Matrix &mat)
{
	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);
}

void MFSetColour(const Vector4 &colour)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, a);
}

void MFSetColour(uint32 col)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, float((col>>16)&0xFF) / 255.0f, float((col>>8)&0xFF) / 255.0f, float(col&0xFF) / 255.0f, float((col>>24)&0xFF) / 255.0f);
}

void MFSetTexCoord1(float u, float v)
{
	pd3dDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, u, v);
}

void MFSetNormal(const Vector3 &normal)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, normal.x, normal.y, normal.z, 0.0f);
}

void MFSetNormal(float x, float y, float z)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, x, y, z, 0.0f);
}

void MFSetPosition(const Vector3 &pos)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, pos.x, pos.y, pos.z, 0.0f);
	++currentVert;
}

void MFSetPosition(float x, float y, float z)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, x, y, z, 0.0f);
	++currentVert;
}

void MFEnd()
{
	DBGASSERT(currentVert == beginCount, "Incorrect number of vertices.");

	pd3dDevice->End();
}
