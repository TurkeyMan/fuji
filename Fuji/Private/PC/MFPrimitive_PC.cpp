#include "Fuji.h"
#include "Display_Internal.h"
#include "MFView.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "MFPrimitive.h"
#include "MFTexture_Internal.h"
#include "MFRenderer.h"
#include "MFMaterial.h"

#include "Materials/MFMat_Standard.h"

#include <d3d9.h>

static const int primBufferSize = 1536;

static bool gRenderQuads = false;

struct LitVertex
{
	enum
	{
		FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1
	};

	struct LitPos
	{
		float x, y, z;
	} pos;

	struct LitNormal
	{
		float x, y, z;
	} normal;

	unsigned int colour;

	float u,v;
};

LitVertex primBuffer[primBufferSize];
LitVertex current;

uint32 primType;
uint32 beginCount;
uint32 currentVert;

extern IDirect3DDevice9 *pd3dDevice;

/*** functions ***/

void MFPrimitive_InitModule()
{
	MFCALLSTACK;

}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;

}

void MFPrimitive_DrawStats()
{

}

void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	primType = type & PT_PrimMask;

	if(primType == PT_QuadList)
	{
		primType = PT_TriList;
		gRenderQuads = true;
	}
	else
		gRenderQuads = false;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));
	}

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&MFMatrix::identity);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&MFView_GetViewToScreenMatrix());

	if(MFView_IsOrtho())
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&MFMatrix::identity);
	else
		pd3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&MFView_GetWorldToViewMatrix());

	MFRenderer_Begin();
}

void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	MFDebug_Assert(vertexCount > 0, "Invalid primitive count.");

	if(gRenderQuads)
		beginCount = vertexCount * 3;
	else
		beginCount = vertexCount;

	currentVert = 0;

	current.u = current.v = 0.0f;
	current.colour = 0xFFFFFFFF;
	current.normal.x = current.normal.z = 0.0f;
	current.normal.y = 1.0f;
}

void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

	pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);
}

void MFSetColour(const MFVector &colour)
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

void MFSetNormal(const MFVector &normal)
{
	MFSetNormal(normal.x, normal.y, normal.z);
}

void MFSetNormal(float x, float y, float z)
{
	current.normal.x = x;
	current.normal.y = y;
	current.normal.z = z;
}

void MFSetPosition(const MFVector &pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	current.pos.x = x;
	current.pos.y = y;
	current.pos.z = z;

	if(gRenderQuads && (currentVert & 1))
	{
		LitVertex &prev = primBuffer[currentVert - 1];

		primBuffer[currentVert + 0] = prev;
		primBuffer[currentVert + 1] = prev;
		primBuffer[currentVert + 2] = current;
		primBuffer[currentVert + 3] = current;
		primBuffer[currentVert + 4] = current;

		primBuffer[currentVert + 0].pos.x = current.pos.x;
		primBuffer[currentVert + 1].pos.y = current.pos.y;
		primBuffer[currentVert + 2].pos.x = prev.pos.x;
		primBuffer[currentVert + 3].pos.y = prev.pos.y;

		primBuffer[currentVert + 0].u = current.u;
		primBuffer[currentVert + 1].v = current.v;
		primBuffer[currentVert + 2].u = prev.u;
		primBuffer[currentVert + 3].v = prev.v;

		currentVert += 4;
	}
	else
		primBuffer[currentVert] = current;

	++currentVert;

	if(currentVert >= primBufferSize)
	{
		int newBeginCount = beginCount - currentVert;
		beginCount = currentVert;

		MFEnd();

		beginCount = newBeginCount;
		currentVert = 0;
	}
}

void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

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


int textureWidth, textureHeight;
float uScale, vScale;
float halfTexelU, halfTexelV;

void MFPrimitive_BeginBlitter(int numBlits)
{
	MFView_Push();

	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);
	MFView_SetOrtho(&rect);

	MFTexture *pTex;
	MFMaterial *pMat = MFMaterial_GetCurrent();
	MFMaterial_GetParamater(pMat, MFMatStandard_DifuseMap, 0, (uint32*)&pTex);
	textureWidth = pTex->pTemplateData->pSurfaces[0].width;
	textureHeight = pTex->pTemplateData->pSurfaces[0].height;

	uScale = 1.0f / (float)textureWidth;
	vScale = 1.0f / (float)textureHeight;
	halfTexelU = uScale * 0.5f;
	halfTexelV = vScale * 0.5f;

	MFPrimitive(PT_QuadList);
	MFBegin(numBlits * 2);
}

void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	MFSetTexCoord1((float)tx * uScale - halfTexelU, (float)ty * vScale - halfTexelV);
	MFSetPosition((float)x, (float)y, 0.0f);
	MFSetTexCoord1((float)(tx + tw) * uScale - halfTexelU, (float)(ty + th) * vScale - halfTexelV);
	MFSetPosition((float)(x + tw), (float)(y + th), 0.0f);
}

void MFPrimitive_EndBlitter()
{
	MFEnd();
	MFView_Pop();
}
