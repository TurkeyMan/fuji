#pragma once
#if !defined(_MFRENDERER_INTERNAL_H)
#define _MFRENDERER_INTERNAL_H

#include "MFRenderer.h"
#include "MFRenderState.h"
#include "MFArray.h"
#include "MFShader.h"

struct MFDisplay;
struct MFDisplaySettings;

// init/deinit
MFInitStatus MFRenderer_InitModule(int moduleId, bool bPerformInitialisation);
void MFRenderer_DeinitModule();
bool MFRenderer_BeginFrame();
void MFRenderer_EndFrame();

void MFRenderer_InitModulePlatformSpecific();
void MFRenderer_DeinitModulePlatformSpecific();
bool MFRenderer_BeginFramePlatformSpecific();
void MFRenderer_EndFramePlatformSpecific();
void MFRenderer_SetRenderTargetPlatformSpecific(MFRenderTarget *pRenderTarget);

int MFRenderer_CreateDisplay(MFDisplay *pDisplay);
void MFRenderer_DestroyDisplay(MFDisplay *pDisplay);
bool MFRenderer_ResetDisplay(MFDisplay *pDisplay, const MFDisplaySettings *pSettings);

void MFRenderer_LostFocus(MFDisplay *pDisplay);
void MFRenderer_GainedFocus(MFDisplay *pDisplay);

typedef int (*MFRenderSortFunction)(const void*, const void*);

struct MFEffectTechnique;

enum MFStateBlockType
{
	MFSBT_Global = 0,
	MFSBT_View,
	MFSBT_Entity,
	MFSBT_Material,
	MFSBT_Geometry,
	MFSBT_MaterialOverride,
	MFSBT_Layer,
	MFSBT_Override,

	MFSBT_Max,
	MFSBT_ForceInt = 0x7FFFFFFF,
};

MFALIGN_BEGIN(16)
struct MFRenderElement // 42 bytes atm... compress state block handles?
{
	// sorting (cost):
	//   dx9  - shaders, shader constants, render targets, decl, sampler state, vert/index buffer, textures, render states, drawprim
	//        - pShader, pView, pEntity, pMaterial ???
	//   dx11 - textures...
	//        - pMaterial, pShader, pView, pEntity ???

//	MFRenderElementData *pData;

	MFMaterial *pMaterial;
//	MFRenderTechnique *pRenderTechnique;

	const MFStateBlock *pViewState;
	const MFStateBlock *pGeometryState;
	const MFStateBlock *pEntityState;
	const MFStateBlock *pMaterialState;
	const MFStateBlock *pMaterialOverrideState;

	uint64 vertexBufferOffset : 20;
	uint64 indexBufferOffset : 20;
	uint64 vertexCount : 20;
	uint64 primType : 3;
	uint64 renderIndexed : 1;

//	MFRenderInstance *pInstances;
	uint8 numInstances;

	uint8 animBatch;

	uint8 type;
	uint8 primarySortKey;
	uint16 zSort;

	uint16 unused;
#if !defined(MF_64BIT)
	uint16 padding[2];
#endif

//	MFRenderElementDebug *pDbg; // name/model/event/etc...
}
MFALIGN_END(16);

struct MFRenderLayer
{
	MFVector clearColour;
	float clearZ;
	int clearStencil;
	MFRenderClearFlags clearFlags;

	MFRenderTarget *pRenderTarget;

	MFRenderLayerSortMode sortMode;
	const MFStateBlock *pLayer;

	MFArray<MFRenderElement> elements;

	int primaryKey;

	const char *pName;
};

struct MFRenderer
{
	// layers
	MFRenderLayer *pLayers;
	int numLayers;

	const MFStateBlock *pGlobal;
	const MFStateBlock *pOverride;
};

struct MFRendererState
{
	__forceinline bool isSet(int type, int constant)
	{
		return !!(rsSet[type] & MFBIT(constant));
	}

	__forceinline uint32 getBool(int constant)
	{
		return rsSet[MFSB_CT_Bool] & bools & MFBIT(constant);
	}

	__forceinline uint32 boolChanged(int constant)
	{
		return (boolsSet ^ (rsSet[MFSB_CT_Bool] & bools)) & MFBIT(constant);
	}

	MFMatrix *getDerivedMatrix(MFStateConstant_Matrix matrix);

	MFMatrix derivedMatrices[MFSCM_NumDerived];
	uint32 derivedMatrixDirty;

	uint32 bools, boolsSet;

	const MFStateBlock *pStateBlocks[MFSBT_Max];

	MFMatrix *pMatrixStates[MFSCM_Max];
	MFMatrix *pMatrixStatesSet[MFSCM_Max];
	MFVector *pVectorStates[MFSCV_Max];
	MFVector *pVectorStatesSet[MFSCV_Max];
	MFTexture *pTextures[MFSCT_Max];
	MFTexture *pTexturesSet[MFSCT_Max];
	void *pRenderStates[MFSCRS_Max];
	void *pRenderStatesSet[MFSCRS_Max];

	MFStateConstant_AnimationMatrices animation;
	MFStateConstant_MatrixBatch matrixBatch;

	MFRect *pViewport;
	MFRect *pViewportSet;

	uint32 rsSet[MFSB_CT_TypeCount];
	uint32 rsMask[MFSB_CT_TypeCount];

	MFEffectTechnique *pTechniqueSet;
	MFShader *pShadersSet[MFST_Max];
};

// internal functions
void MFRendererInternal_SortElements(MFRenderLayer &layer);


#endif
