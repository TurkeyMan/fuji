#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFPrimitive_InitModule MFPrimitive_InitModule_OpenGL
	#define MFPrimitive_DeinitModule MFPrimitive_DeinitModule_OpenGL
	#define MFPrimitive_DrawStats MFPrimitive_DrawStats_OpenGL
	#define MFPrimitive MFPrimitive_OpenGL
	#define MFBegin MFBegin_OpenGL
	#define MFSetMatrix MFSetMatrix_OpenGL
	#define MFSetColour MFSetColour_OpenGL
	#define MFSetTexCoord1 MFSetTexCoord1_OpenGL
	#define MFSetNormal MFSetNormal_OpenGL
	#define MFSetPosition MFSetPosition_OpenGL
	#define MFEnd MFEnd_OpenGL
	#define MFPrimitive_BeginBlitter MFPrimitive_BeginBlitter_OpenGL
	#define MFPrimitive_Blit MFPrimitive_Blit_OpenGL
	#define MFPrimitive_StretchBlit MFPrimitive_StretchBlit_OpenGL
	#define MFPrimitive_EndBlitter MFPrimitive_EndBlitter_OpenGL
#endif

#include "MFPrimitive.h"
#include "MFView.h"
#include "MFTexture_Internal.h"
#include "MFRenderer.h"
#include "MFDisplay.h"
#include "Materials/MFMat_Standard.h"
#include "MFRenderer.h"
#include "MFRenderState.h"
#include "MFMaterial.h"
#include "MFVertex.h"

#include "MFOpenGL.h"

static const int primBufferSize = 1536;

struct Vert
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	uint32 colour;
};

static MFVertexDeclaration *pDecl;
static MFStateBlock *pIdentity;

static MFMaterial *pMaterial;

static MFMesh currentPrim;
static MFStateBlock *pEntity;
static MFVertexBuffer *pVB;
static Vert *pLocked;

static Vert current;
static int currentVert;

#if defined(MF_OPENGL_ES)
static bool gImmitateQuads = false;
#endif


MFInitStatus MFPrimitive_InitModule()
{
	MFVertexElement elements[4];

	// write declaration
	elements[0].stream = 0;
	elements[0].type = MFVET_Position;
	elements[0].index = 0;
	elements[0].componentCount = 3;
	elements[0].format = MFVDF_Float3;

	elements[1].stream = 0;
	elements[1].type = MFVET_TexCoord;
	elements[1].index = 0;
	elements[1].componentCount = 2;
	elements[1].format = MFVDF_Float2;

	elements[2].stream = 0;
	elements[2].type = MFVET_Normal;
	elements[2].index = 0;
	elements[2].componentCount = 3;
	elements[2].format = MFVDF_Float3;

	elements[3].stream = 0;
	elements[3].type = MFVET_Colour;
	elements[3].index = 0;
	elements[3].componentCount = 4;
	elements[3].format = MFVDF_UByte4N_RGBA;

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

#if defined(MF_OPENGL_ES)
	if(currentPrim.primType == MFPT_QuadList)
	{
		currentPrim.primType = MFPT_TriangleList;
		gImmitateQuads = true;
	}
	else
		gImmitateQuads = false;
#endif
}

MF_API void MFBegin(uint32 vertexCount)
{
	MFDebug_Assert(vertexCount > 0, "Invalid primitive count.");

	currentPrim.numVertices = (currentPrim.primType == MFPT_QuadList) ? vertexCount*2 : vertexCount;
#if defined(MF_OPENGL_ES)
	if(gImmitateQuads)
		currentPrim.numVertices = vertexCount * 3;
#endif

	// create an appropriate vertex buffer
	pVB = MFVertex_CreateVertexBuffer(pDecl, currentPrim.numVertices, MFVBType_Scratch);

	currentPrim.pMeshState = MFStateBlock_CreateTemporary(64);
	MFStateBlock_SetRenderState(currentPrim.pMeshState, MFSCRS_VertexDeclaration, pDecl);
	MFStateBlock_SetRenderState(currentPrim.pMeshState, MFSCRS_VertexBuffer0, pVB);

	MFVertex_LockVertexBuffer(pVB, (void**)&pLocked);

	currentVert = 0;

	current.u = current.v = 0.0f;
	current.colour = 0xFFFFFFFF;
	current.nx = current.nz = 0.0f;
	current.ny = 1.0f;
}

MF_API void MFSetMatrix(const MFMatrix &mat)
{
	if(pEntity == pIdentity)
		pEntity = MFStateBlock_CreateTemporary(128);
	MFStateBlock_SetMatrix(pEntity, MFSCM_World, mat);
}

MF_API void MFSetColour(float r, float g, float b, float a)
{
	current.colour = ((uint32)(r*255.0f) << 0) | ((uint32)(g*255.0f) << 8) | ((uint32)(b*255.0f) << 16) | ((uint32)(a*255.0f) << 24);
}

MF_API void MFSetTexCoord1(float u, float v)
{
	current.u = u;
	current.v = v;
}

MF_API void MFSetNormal(float x, float y, float z)
{
	current.nx = x;
	current.ny = y;
	current.nz = z;
}

MF_API void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	current.x = x;
	current.y = y;
	current.z = z;

#if defined(MF_OPENGL_ES)
	if(gImmitateQuads && (currentVert & 1))
	{
		Vert &prev = pLocked[currentVert - 1];

		pLocked[currentVert + 0] = prev;
		pLocked[currentVert + 1] = prev;
		pLocked[currentVert + 2] = current;
		pLocked[currentVert + 3] = current;
		pLocked[currentVert + 4] = current;

		pLocked[currentVert + 0].x = current.x;
		pLocked[currentVert + 1].y = current.y;
		pLocked[currentVert + 2].x = prev.x;
		pLocked[currentVert + 3].y = prev.y;

		pLocked[currentVert + 0].u = current.u;
		pLocked[currentVert + 1].v = current.v;
		pLocked[currentVert + 2].u = prev.u;
		pLocked[currentVert + 3].v = prev.v;

		currentVert += 4;
	}
#else
	if(currentPrim.primType == MFPT_QuadList && (currentVert & 1))
	{
		Vert &prev = pLocked[currentVert - 1];

		pLocked[currentVert + 0] = prev;
		pLocked[currentVert + 1] = current;
		pLocked[currentVert + 2] = current;

		pLocked[currentVert + 0].x = current.x;
		pLocked[currentVert + 2].x = prev.x;

		pLocked[currentVert + 0].u = current.u;
		pLocked[currentVert + 2].u = prev.u;

		currentVert += 2;
	}
#endif
	else
		pLocked[currentVert] = current;

	++currentVert;
}

MF_API void MFEnd()
{
	MFVertex_UnlockVertexBuffer(pVB);

	MFDebug_Assert(currentVert == currentPrim.numVertices, "Incorrect number of vertices.");

	MFRenderer_AddMesh(&currentPrim, pMaterial, pEntity, NULL, MFView_GetViewState());

/*
	// *** pre-shader render code! ***

	MFCALLSTACK;
	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	if(beginCount)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glVertexPointer(3, GL_FLOAT, sizeof(Vert), &primBuffer[0].x);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(Vert), &primBuffer[0].nx);
		glEnableClientState(GL_NORMAL_ARRAY);

		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vert), &primBuffer[0].colour);
		glEnableClientState(GL_COLOR_ARRAY);

		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vert), &primBuffer[0].u);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vert), &primBuffer[0].u);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glDrawArrays(gPrimTypes[primType], 0, beginCount);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
*/
}


static int textureWidth, textureHeight;
static float uScale, vScale;

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

	MFMatrix matrix;
	MFMaterial_GetParameterM(pMat, MFMatStandard_TextureMatrix, 0, &matrix);

	uScale = 1.0f / (float)textureWidth / matrix.GetXAxis().Magnitude3();
	vScale = 1.0f / (float)textureHeight / matrix.GetYAxis().Magnitude3();

	MFPrimitive(PT_QuadList);
	MFBegin(numBlits * 2);
}

MF_API void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	MFPrimitive_StretchBlit(x, y, tw, th, tx, ty, tw, th);
}

MF_API void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
	MFSetTexCoord1((float)tx * uScale, (float)ty * vScale);
	MFSetPosition((float)x, (float)y, 0.0f);
	MFSetTexCoord1((float)(tx + tw) * uScale, (float)(ty + th) * vScale);
	MFSetPosition((float)(x + w), (float)(y + h), 0.0f);
}

MF_API void MFPrimitive_EndBlitter()
{
	MFEnd();
	MFView_Pop();
}

#endif
