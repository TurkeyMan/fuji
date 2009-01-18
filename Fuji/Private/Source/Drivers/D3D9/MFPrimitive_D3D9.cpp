#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFPrimitive_InitModule MFPrimitive_InitModule_D3D9
	#define MFPrimitive_DeinitModule MFPrimitive_DeinitModule_D3D9
	#define MFPrimitive_DrawStats MFPrimitive_DrawStats_D3D9
	#define MFPrimitive MFPrimitive_D3D9
	#define MFBegin MFBegin_D3D9
	#define MFSetMatrix MFSetMatrix_D3D9
	#define MFSetColour MFSetColour_D3D9
	#define MFSetTexCoord1 MFSetTexCoord1_D3D9
	#define MFSetNormal MFSetNormal_D3D9
	#define MFSetPosition MFSetPosition_D3D9
	#define MFEnd MFEnd_D3D9
	#define MFPrimitive_BeginBlitter MFPrimitive_BeginBlitter_D3D9
	#define MFPrimitive_Blit MFPrimitive_Blit_D3D9
	#define MFPrimitive_EndBlitter MFPrimitive_EndBlitter_D3D9
#endif

#include "Display_Internal.h"
#include "MFView.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "MFPrimitive.h"
#include "MFTexture_Internal.h"
#include "MFRenderer.h"
#include "MFRenderer_D3D9.h"
#include "MFMaterial.h"
#include "MFVertex.h"

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

static LitVertex primBuffer[primBufferSize];
static LitVertex current;

static uint32 primType;
static uint32 beginCount;
static uint32 currentVert;

extern IDirect3DDevice9 *pd3dDevice;
static MFVertexDeclaration *pDecl;

/*** functions ***/

void MFPrimitive_InitModule()
{
	MFCALLSTACK;

	MFVertexElement elements[4];

	// write declaration
	elements[0].stream = 0;
	elements[0].elementType = MFVE_Position;
	elements[0].elementIndex = 0;
	elements[0].componentCount = 3;

	elements[1].stream = 0;
	elements[1].elementType = MFVE_Normal;
	elements[1].elementIndex = 0;
	elements[1].componentCount = 3;

	elements[2].stream = 0;
	elements[2].elementType = MFVE_Colour;
	elements[2].elementIndex = 0;
	elements[2].componentCount = 4;

	elements[3].stream = 0;
	elements[3].elementType = MFVE_TexCoord;
	elements[3].elementIndex = 0;
	elements[3].componentCount = 2;

	pDecl = MFVertex_CreateVertexDeclaration(elements, 4);
	MFDebug_Assert(pDecl, "Failed to create vertex declaration..");
}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;

	MFVertex_DestroyVertexDeclaration(pDecl);
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

	MFMaterial *pMatOverride = (MFMaterial*)MFRenderer_GetRenderStateOverride(MFRS_MaterialOverride);
	if(pMatOverride)
		MFMaterial_SetMaterial(pMatOverride);
	else if(type & PT_Untextured)
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));

	MFRenderer_SetMatrices(NULL, 0);
	MFRendererPC_SetNumWeights(0);
	MFRendererPC_SetWorldToScreenMatrix(MFView_GetWorldToScreenMatrix());
	MFRendererPC_SetModelColour(MFVector::white);

	MFVertex_SetVertexDeclaration(pDecl);

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

void MFSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(r*255.0f))<<16 | ((uint32)(g*255.0f))<<8 | (uint32)(b*255.0f) | ((uint32)(a*255.0f))<<24;
}

void MFSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
}

void MFSetNormal(float x, float y, float z)
{
	current.normal.x = x;
	current.normal.y = y;
	current.normal.z = z;
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


static int textureWidth, textureHeight;
static float uScale, vScale;
static float halfTexelU, halfTexelV;

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

#endif // MF_RENDERER
