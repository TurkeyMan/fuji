#include "Fuji_Internal.h"

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
	#define MFPrimitive_StretchBlit MFPrimitive_StretchBlit_D3D9
	#define MFPrimitive_EndBlitter MFPrimitive_EndBlitter_D3D9
#endif

#include "MFRenderer.h"
#include "MFRenderState.h"
#include "MFRenderer_D3D9.h"
#include "MFVertex.h"
#include "MFPrimitive.h"
#include "MFMaterial.h"
#include "MFView.h"
#include "MFDisplay.h"

#include "MFTexture_Internal.h"
#include "Materials/MFMat_Standard.h"

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

static MFVertexDeclaration *pDecl;
static MFStateBlock *pIdentity;

static MFMaterial *pMaterial;

static MFMesh currentPrim;
static MFStateBlock *pEntity;
static MFVertexBuffer *pVB;
static LitVertex *pLocked;

static LitVertex current;
static int currentVert;

static bool gImmitateQuads = false;

/*** functions ***/

MFInitStatus MFPrimitive_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFVertexElement elements[4];

	// write declaration
	elements[0].stream = 0;
	elements[0].type = MFVET_Position;
	elements[0].index = 0;
	elements[0].componentCount = 3;
	elements[0].format = MFVDF_Float3;

	elements[1].stream = 0;
	elements[1].type = MFVET_Normal;
	elements[1].index = 0;
	elements[1].componentCount = 3;
	elements[1].format = MFVDF_Float3;

	elements[2].stream = 0;
	elements[2].type = MFVET_Colour;
	elements[2].index = 0;
	elements[2].componentCount = 4;
	elements[2].format = MFVDF_UByte4N_BGRA;

	elements[3].stream = 0;
	elements[3].type = MFVET_TexCoord;
	elements[3].index = 0;
	elements[3].componentCount = 2;
	elements[3].format = MFVDF_Float2;

	pDecl = MFVertex_CreateVertexDeclaration(elements, 4);
	MFDebug_Assert(pDecl, "Failed to create vertex declaration..");

	pIdentity = MFStateBlock_Create(128);
	MFStateBlock_SetMatrix(pIdentity, MFSCM_World, MFMatrix::identity);

	return MFIS_Succeeded;
}

void MFPrimitive_DeinitModule()
{
	MFStateBlock_Destroy(pIdentity);

	MFVertex_ReleaseVertexDeclaration(pDecl);
}

void MFPrimitive_DrawStats()
{

}

MF_API void MFPrimitive(uint32 type, uint32 hint)
{
	currentPrim.primType = (MFPrimType)(type & PT_PrimMask);
	currentPrim.pMeshState = NULL;
	currentPrim.indexOffset = 0;
	currentPrim.vertexOffset = 0;
	currentPrim.numIndices = 0;
	pEntity = pIdentity;

	if(type & PT_Untextured)
		pMaterial = MFMaterial_GetStockMaterial(MFMat_White);
	else
		pMaterial = MFMaterial_GetCurrent();

	if(currentPrim.primType == MFPT_QuadList)
	{
		// D3D9 doesn't support rendering quads...
		currentPrim.primType = MFPT_TriangleList;
		gImmitateQuads = true;
	}
	else
		gImmitateQuads = false;
}

MF_API void MFBegin(uint32 vertexCount)
{
	MFDebug_Assert(vertexCount > 0, "Invalid primitive count.");

	currentPrim.numVertices = vertexCount;
	if(gImmitateQuads)
		currentPrim.numVertices *= 3;

	// create an appropriate vertex buffer
	pVB = MFVertex_CreateVertexBuffer(pDecl, currentPrim.numVertices, MFVBType_Scratch);

	currentPrim.pMeshState = MFStateBlock_CreateTemporary(64);
	MFStateBlock_SetRenderState(currentPrim.pMeshState, MFSCRS_VertexDeclaration, pDecl);
	MFStateBlock_SetRenderState(currentPrim.pMeshState, MFSCRS_VertexBuffer0, pVB);

	MFVertex_LockVertexBuffer(pVB, (void**)&pLocked);

	currentVert = 0;

	current.u = current.v = 0.0f;
	current.colour = 0xFFFFFFFF;
	current.normal.x = current.normal.z = 0.0f;
	current.normal.y = 1.0f;
}

MF_API void MFSetMatrix(const MFMatrix &mat)
{
	if(pEntity == pIdentity)
		pEntity = MFStateBlock_CreateTemporary(128);
	MFStateBlock_SetMatrix(pEntity, MFSCM_World, mat);
}

MF_API void MFSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(r*255.0f))<<16 | ((uint32)(g*255.0f))<<8 | (uint32)(b*255.0f) | ((uint32)(a*255.0f))<<24;
}

MF_API void MFSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
}

MF_API void MFSetNormal(float x, float y, float z)
{
	current.normal.x = x;
	current.normal.y = y;
	current.normal.z = z;
}

MF_API void MFSetPosition(float x, float y, float z)
{
	current.pos.x = x;
	current.pos.y = y;
	current.pos.z = z;

	if(gImmitateQuads && (currentVert & 1))
	{
		LitVertex &prev = pLocked[currentVert - 1];

		pLocked[currentVert + 0] = prev;
		pLocked[currentVert + 1] = prev;
		pLocked[currentVert + 2] = current;
		pLocked[currentVert + 3] = current;
		pLocked[currentVert + 4] = current;

		pLocked[currentVert + 0].pos.x = current.pos.x;
		pLocked[currentVert + 1].pos.y = current.pos.y;
		pLocked[currentVert + 2].pos.x = prev.pos.x;
		pLocked[currentVert + 3].pos.y = prev.pos.y;

		pLocked[currentVert + 0].u = current.u;
		pLocked[currentVert + 1].v = current.v;
		pLocked[currentVert + 2].u = prev.u;
		pLocked[currentVert + 3].v = prev.v;

		currentVert += 4;
	}
	else
		pLocked[currentVert] = current;

	++currentVert;
}

MF_API void MFEnd()
{
	MFVertex_UnlockVertexBuffer(pVB);

	MFDebug_Assert(currentVert == currentPrim.numVertices, "Incorrect number of vertices.");

	MFRenderer_AddMesh(&currentPrim, pMaterial, pEntity, NULL, MFView_GetViewState());
}


static int textureWidth, textureHeight;
static float uScale, vScale;
static float halfTexelU, halfTexelV;

MF_API void MFPrimitive_BeginBlitter(int numBlits)
{
	MFView_Push();

	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);
	MFView_SetOrtho(&rect);

	MFMaterial *pMat = MFMaterial_GetCurrent();
	MFTexture *pTex = MFMaterial_GetParameterT(pMat, MFMatStandard_Texture, MFMatStandard_Tex_DifuseMap);
	textureWidth = pTex->pTemplateData->pSurfaces[0].width;
	textureHeight = pTex->pTemplateData->pSurfaces[0].height;

	uScale = 1.0f / (float)textureWidth;
	vScale = 1.0f / (float)textureHeight;
	halfTexelU = uScale * 0.5f;
	halfTexelV = vScale * 0.5f;

	MFPrimitive(PT_QuadList);
	MFBegin(numBlits * 2);
}

MF_API void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	MFPrimitive_StretchBlit(x, y, tw, th, tx, ty, tw, th);
}

MF_API void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
	MFSetTexCoord1((float)tx * uScale - halfTexelU, (float)ty * vScale - halfTexelV);
	MFSetPosition((float)x, (float)y, 0.0f);
	MFSetTexCoord1((float)(tx + tw) * uScale - halfTexelU, (float)(ty + th) * vScale - halfTexelV);
	MFSetPosition((float)(x + w), (float)(y + h), 0.0f);
}

MF_API void MFPrimitive_EndBlitter()
{
	MFEnd();
	MFView_Pop();
}

#endif // MF_RENDERER
