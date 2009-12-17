#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_XBOX

#include "Display_Internal.h"
#include "MFView.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "MFPrimitive.h"
#include "MFTexture.h"
#include "MFRenderer.h"
#include "MFMaterial.h"

uint32 primType;
uint32 beginCount;
uint32 currentVert;

extern IDirect3DDevice8 *pd3dDevice;

MFVector prevPos;
MFVector prevUV;

/*** functions ***/

void MFPrimitive_InitModule()
{

}

void MFPrimitive_DeinitModule()
{

}

void MFPrimitive_DrawStats()
{

}

void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));
	}

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&MFMatrix::identity);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&MFView_GetViewToScreenMatrix());

	if(MFView_IsOrtho())
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&MFMatrix::identity);
	else
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&MFView_GetWorldToViewMatrix());

	MFRenderer_Begin();
}

void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	beginCount = vertexCount;
	currentVert = 0;

	pd3dDevice->SetVertexShader(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1);

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
		case PT_QuadList:
			pd3dDevice->Begin(D3DPT_QUADLIST);
			break;
	}

	pd3dDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xFFFFFFFF);
	pd3dDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, 0, 0);
	pd3dDevice->SetVertexData4f(D3DVSDE_NORMAL, 0, 1, 0, 0);
}

void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);
}

void MFSetColour(const MFVector &colour)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, colour.x, colour.y, colour.z, colour.w);
}

void MFSetColour(float r, float g, float b, float a)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, a);
}

void MFSetColour(uint32 col)
{
	pd3dDevice->SetVertexData4ub(D3DVSDE_DIFFUSE, (col>>16)&0xFF, (col>>8)&0xFF, col&0xFF, (col>>24)&0xFF);
}

void MFSetTexCoord1(float u, float v)
{
	if(primType == PT_QuadList)
		prevUV.Set(prevUV.z, prevUV.w, u, v);
	else
		pd3dDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, u, v);
}

void MFSetNormal(const MFVector &normal)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_NORMAL, normal.x, normal.y, normal.z, 0.0f);
}

void MFSetNormal(float x, float y, float z)
{
	pd3dDevice->SetVertexData4f(D3DVSDE_NORMAL, x, y, z, 0.0f);
}

void MFSetPosition(const MFVector &pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

void MFSetPosition(float x, float y, float z)
{
	if(primType == PT_QuadList)
	{
		if(currentVert & 1)
		{
			// if we're rendering quads, we need to insert the top-right and botom-left verts
			pd3dDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, prevUV.z, prevUV.y);
			pd3dDevice->SetVertexData4f(D3DVSDE_VERTEX, x, prevPos.y, prevPos.z, 1.0f);
			pd3dDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, prevUV.z, prevUV.w);
			pd3dDevice->SetVertexData4f(D3DVSDE_VERTEX, x, y, z, 1.0f);
			pd3dDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, prevUV.x, prevUV.w);
			pd3dDevice->SetVertexData4f(D3DVSDE_VERTEX, prevPos.x, y, z, 1.0f);
		}
		else
		{
			prevPos.Set(x, y, z);
			pd3dDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, prevUV.z, prevUV.w);
			pd3dDevice->SetVertexData4f(D3DVSDE_VERTEX, x, y, z, 1.0f);
		}
	}
	else
		pd3dDevice->SetVertexData4f(D3DVSDE_VERTEX, x, y, z, 1.0f);

	++currentVert;
}

void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	pd3dDevice->End();
}

#endif
