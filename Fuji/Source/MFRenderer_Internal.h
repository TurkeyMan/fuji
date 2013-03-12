#pragma once
#if !defined(_MFRENDERER_INTERNAL_H)
#define _MFRENDERER_INTERNAL_H

#include "MFRenderer.h"
#include "MFArray.h"

// init/deinit
MFInitStatus MFRenderer_InitModule();
void MFRenderer_DeinitModule();

void MFRenderer_InitModulePlatformSpecific();
void MFRenderer_DeinitModulePlatformSpecific();

bool MFRenderer_BeginFrame();
void MFRenderer_EndFrame();

int MFRenderer_CreateDisplay();
void MFRenderer_DestroyDisplay();
void MFRenderer_ResetDisplay();
bool MFRenderer_SetDisplayMode(int width, int height, bool bFullscreen);

MFALIGN_BEGIN(16)
struct MFRenderElement // 42 bytes atm... compress state block handles?
{
	// sorting (cost):
	//   dx9  - shaders, shader constants, render targets, decl, sampler state, vert/index buffer, textures, render states, drawprim
	//        - pShader, pView, pEntity, pMaterial ???
	//   dx11 - textures...
	//        - pMaterial, pShader, pView, pEntity ???

//	MFRenderElementData *pData;
	uint8 type;
	uint8 primarySortKey;
	uint16 zSort;
//	MFShaderTechnique *pShaderTechnique;

	MFStateBlock *pViewState;
	MFStateBlock *pGeometryState;
	MFStateBlock *pEntityState;
	MFStateBlock *pMaterialState;
	MFStateBlock *pMaterialOverrideState;

	uint64 vertexBufferOffset : 20;
	uint64 indexBufferOffset : 20;
	uint64 vertexCount : 20;
	uint64 primType : 3;
	uint64 renderIndexed : 1;

//	MFRenderInstance *pInstances;
	uint8 numInstances;

	uint8 animBatch;

	uint16 unused[7];

//	MFRenderElementDebug *pDbg; // name/model/event/etc...
}
MFALIGN_END(16);

struct MFRenderLayer
{
	// render target config

	MFRenderLayerSortMode sortMode;
	MFStateBlock *pLayer;

	MFArray<MFRenderElement> elements;

	int primaryKey;
};

struct MFRenderer
{
	// layers
	MFRenderLayer *pLayers;
	int numLayers;

	MFStateBlock *pGlobal;
	MFStateBlock *pOverride;
};

#endif
