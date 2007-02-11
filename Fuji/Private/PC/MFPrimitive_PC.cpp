#include "Fuji.h"
#include "Display_Internal.h"
#include "MFView.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "MFPrimitive.h"
#include "MFTexture_Internal.h"
#include "MFRenderer.h"
#include "MFRenderer_PC.h"
#include "MFMaterial.h"

#include "Materials/MFMat_Standard.h"

#include <d3d9.h>

static const int primBufferSize = 1536;

static bool gRenderQuads = false;

struct LitVertex
{
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
static IDirect3DVertexDeclaration9 *pDecl;

/*** functions ***/

void MFPrimitive_InitModule()
{
	MFCALLSTACK;

	D3DVERTEXELEMENT9 decl[7];

	// write declaration
	decl[0].Stream = 0;
	decl[0].Offset = 0;
	decl[0].Type = D3DDECLTYPE_FLOAT3;
	decl[0].Method = D3DDECLMETHOD_DEFAULT;
	decl[0].Usage = D3DDECLUSAGE_POSITION;
	decl[0].UsageIndex = 0;

	decl[1].Stream = 0;
	decl[1].Offset = 12;
	decl[1].Type = D3DDECLTYPE_FLOAT3;
	decl[1].Method = D3DDECLMETHOD_DEFAULT;
	decl[1].Usage = D3DDECLUSAGE_NORMAL;
	decl[1].UsageIndex = 0;

	decl[2].Stream = 0;
	decl[2].Offset = 24;
	decl[2].Type = D3DDECLTYPE_D3DCOLOR;
	decl[2].Method = D3DDECLMETHOD_DEFAULT;
	decl[2].Usage = D3DDECLUSAGE_COLOR;
	decl[2].UsageIndex = 0;

	decl[3].Stream = 0;
	decl[3].Offset = 28;
	decl[3].Type = D3DDECLTYPE_FLOAT2;
	decl[3].Method = D3DDECLMETHOD_DEFAULT;
	decl[3].Usage = D3DDECLUSAGE_TEXCOORD;
	decl[3].UsageIndex = 0;

	D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
	decl[4] = endMacro;

	HRESULT hr = pd3dDevice->CreateVertexDeclaration(decl, &pDecl);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex declaration..");
}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;

	pDecl->Release();
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

	MFRenderer_SetMatrices(NULL, 0);
	MFRendererPC_SetNumWeights(0);
	MFRendererPC_SetWorldToScreenMatrix(MFView_GetWorldToScreenMatrix());
	MFRendererPC_SetModelColour(MFVector::white);

	pd3dDevice->SetVertexDeclaration(pDecl);

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

	MFMatrix temp;
	MFView_GetLocalToScreen(mat, &temp);
	MFRendererPC_SetWorldToScreenMatrix(temp);
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
	MFMaterial_GetParameter(pMat, MFMatStandard_DifuseMap, 0, &pTex);
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
