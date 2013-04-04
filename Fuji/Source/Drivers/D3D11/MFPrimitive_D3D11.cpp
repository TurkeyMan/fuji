#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFPrimitive_InitModule MFPrimitive_InitModule_D3D11
	#define MFPrimitive_DeinitModule MFPrimitive_DeinitModule_D3D11
	#define MFPrimitive_DrawStats MFPrimitive_DrawStats_D3D11
	#define MFPrimitive MFPrimitive_D3D11
	#define MFBegin MFBegin_D3D11
	#define MFSetMatrix MFSetMatrix_D3D11
	#define MFSetColour MFSetColour_D3D11
	#define MFSetTexCoord1 MFSetTexCoord1_D3D11
	#define MFSetNormal MFSetNormal_D3D11
	#define MFSetPosition MFSetPosition_D3D11
	#define MFEnd MFEnd_D3D11
	#define MFPrimitive_BeginBlitter MFPrimitive_BeginBlitter_D3D11
	#define MFPrimitive_Blit MFPrimitive_Blit_D3D11
	#define MFPrimitive_StretchBlit MFPrimitive_StretchBlit_D3D11
	#define MFPrimitive_EndBlitter MFPrimitive_EndBlitter_D3D11
#endif

#include "MFPrimitive.h"
#include "MFVertex.h"
#include "MFMaterial.h"
#include "Materials/MFMat_Standard.h"
#include "MFRenderer.h"
#include "MFView.h"
#include "MFDisplay.h"
#include "MFTexture.h"
#include "MFRenderer_D3D11.h"

#include "MFVertex_Internal.h"


static const int primBufferSize = 1536;

static bool gRenderQuads = false;

struct LitVertexD3D11
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

static LitVertexD3D11 primBuffer[primBufferSize];
static LitVertexD3D11 current;

static PrimType primType;
static uint32 beginCount;
static uint32 currentVert;

static MFVertexDeclaration *pDecl;
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;

static MFVertexBuffer *pVertexBuffer = NULL;

static int32 phase = 0;

//---------------------------------------------------------------------------------------------------------------------
MFInitStatus MFPrimitive_InitModule()
{
	MFCALLSTACK;
	
	MFVertexElement elements[4];

	// write declaration
	elements[0].stream = 0;
	elements[0].type = MFVET_Position;
	elements[0].index = 0;
	elements[0].componentCount = 3;

	elements[1].stream = 0;
	elements[1].type = MFVET_Normal;
	elements[1].index = 0;
	elements[1].componentCount = 3;

	elements[2].stream = 0;
	elements[2].type = MFVET_Colour;
	elements[2].index = 0;
	elements[2].componentCount = 4;

	elements[3].stream = 0;
	elements[3].type = MFVET_TexCoord;
	elements[3].index = 0;
	elements[3].componentCount = 2;

	pDecl = MFVertex_CreateVertexDeclaration(elements, 4);
	MFDebug_Assert(pDecl, "Failed to create vertex declaration..");

	pVertexBuffer = MFVertex_CreateVertexBuffer(pDecl, primBufferSize, MFVBType_Dynamic);

	MFRenderer_D3D11_SetDebugName((ID3D11Buffer*)pVertexBuffer->pPlatformData, "MFPrimative vertex buffer");

	return MFAIC_Succeeded;
}
//---------------------------------------------------------------------------------------------------------------------
void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;

	MFVertex_DestroyVertexBuffer(pVertexBuffer); pVertexBuffer = NULL;

	MFVertex_DestroyVertexDeclaration(pDecl);
}
//---------------------------------------------------------------------------------------------------------------------
void MFPrimitive_DrawStats()
{

}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	MFDebug_Assert(phase == 0, "not in order 0");

	primType = static_cast<PrimType>(type & PT_PrimMask);

	if (primType == PT_QuadList)
	{
		primType = PT_TriList;
		gRenderQuads = true;
	}
	else
	{
		gRenderQuads = false;
	}

	if(type & PT_Untextured)
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));

	MFRenderer_SetMatrices(NULL, 0);
	// SJS ????
	//MFRendererPC_SetNumWeights(0);
	
	MFRenderer_D3D11_SetWorldToScreenMatrix(MFView_GetWorldToScreenMatrix());

	//MFRendererPC_SetModelColour(MFVector::white);

	MFVertex_SetVertexDeclaration(pDecl);

	MFRenderer_Begin();

	phase = 1;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	MFDebug_Assert(phase == 1, "not in order 1");

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

	phase = 2;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;

	MFMatrix temp;
	MFView_GetLocalToScreen(mat, &temp);
	MFRenderer_D3D11_SetWorldToScreenMatrix(temp);
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(r*255.0f))<<16 | ((uint32)(g*255.0f))<<8 | (uint32)(b*255.0f) | ((uint32)(a*255.0f))<<24;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFSetNormal(float x, float y, float z)
{
	current.normal.x = x;
	current.normal.y = y;
	current.normal.z = z;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	current.pos.x = x;
	current.pos.y = y;
	current.pos.z = z;

	if(gRenderQuads && (currentVert & 1))
	{
		LitVertexD3D11 &prev = primBuffer[currentVert - 1];

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
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(phase == 2, "not in order 2");

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	void *pBuffer;
	MFVertex_LockVertexBuffer(pVertexBuffer, &pBuffer);
	MFCopyMemory(pBuffer, primBuffer, beginCount * sizeof(LitVertexD3D11));
	MFVertex_UnlockVertexBuffer(pVertexBuffer);

	MFVertex_SetVertexStreamSource(0, pVertexBuffer);
	
	switch(primType)
	{
	case PT_PointList:
		MFVertex_RenderVertices(MFPT_Points, 0, beginCount);
		break;
	case PT_LineList:
		MFVertex_RenderVertices(MFPT_LineList, 0, beginCount);
		break;
	case PT_LineStrip:
		MFVertex_RenderVertices(MFPT_LineStrip, 0, beginCount);
		break;
	case PT_TriList:
		MFVertex_RenderVertices(MFPT_TriangleList, 0, beginCount);
		break;
	case PT_TriStrip:
		MFVertex_RenderVertices(MFPT_TriangleStrip, 0, beginCount);
		break;
	case PT_TriFan:
		MFVertex_RenderVertices(MFPT_TriangleFan, 0, beginCount);
		break;
	}

	phase = 0;
}
//---------------------------------------------------------------------------------------------------------------------
static int textureWidth, textureHeight;
static float uScale, vScale;
static float halfTexelU, halfTexelV;
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFPrimitive_BeginBlitter(int numBlits)
{
	MFView_Push();

	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);
	MFView_SetOrtho(&rect);

	MFMaterial *pMat = MFMaterial_GetCurrent();
	MFTexture *pTex = MFMaterial_GetParameterT(pMat, MFMatStandard_Texture, MFMatStandard_Tex_DifuseMap);
	MFTexture_GetTextureDimensions(pTex, &textureWidth, &textureHeight);

	uScale = 1.0f / (float)textureWidth;
	vScale = 1.0f / (float)textureHeight;
	halfTexelU = uScale * 0.5f;
	halfTexelV = vScale * 0.5f;

	MFPrimitive(PT_QuadList);
	MFBegin(numBlits * 2);
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	MFPrimitive_StretchBlit(x, y, tw, th, tx, ty, tw, th);
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
	MFSetTexCoord1((float)tx * uScale - halfTexelU, (float)ty * vScale - halfTexelV);
	MFSetPosition((float)x, (float)y, 0.0f);
	MFSetTexCoord1((float)(tx + tw) * uScale - halfTexelU, (float)(ty + th) * vScale - halfTexelV);
	MFSetPosition((float)(x + w), (float)(y + h), 0.0f);
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFPrimitive_EndBlitter()
{
	MFEnd();
	MFView_Pop();
}
//---------------------------------------------------------------------------------------------------------------------

#endif // MF_RENDERER
