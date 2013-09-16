#include "Fuji.h"
#include "MFRenderer_Internal.h"
#include "MFRenderState_Internal.h"
#include "MFVertex_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFModel.h"
#include "MFView.h"
#include "MFHeap.h"
#include "MFSystem.h"

extern MFMaterial *pCurrentMaterial;

const MFMatrix *pAnimMats = NULL;
int gNumAnimMats = 0;

const uint16 *pCurrentBatch = NULL;
int gNumBonesInBatch = 0;

static MFRenderer *gpCurrentRenderer = NULL;
static MFRenderLayerSet gCurrentLayers;

static char *pScratchMemory = NULL;
static size_t gScratchMemorySize = 0;
static size_t gScratchMemoryOffset = 0;
static size_t gScratchMemoryMark = 0;
static size_t gScratchMemoryPeak = 0;

MFInitStatus MFRenderer_InitModule()
{
	gScratchMemorySize = gDefaults.render.renderHeapSize;

	int old = MFHeap_SetAllocAlignment(128); // 4k maybe? texture page?
	pScratchMemory = (char*)MFHeap_Alloc(gScratchMemorySize);
	MFHeap_SetAllocAlignment(old);

	MFRenderer_InitModulePlatformSpecific();

	return MFIS_Succeeded;
}

void MFRenderer_DeinitModule()
{
	MFRenderer_DeinitModulePlatformSpecific();

	MFHeap_Free(pScratchMemory);
}

bool MFRenderer_BeginFrame()
{
	return MFRenderer_BeginFramePlatformSpecific();
}

void MFRenderer_EndFrame()
{
	MFRenderer_EndFramePlatformSpecific();

	// free scratch buffers
	MFVertex_EndFrame();

	// calculate the high water mark for the scratch memory
	size_t size = gScratchMemoryOffset < gScratchMemoryMark ? gScratchMemorySize - gScratchMemoryMark + gScratchMemoryOffset : gScratchMemoryOffset - gScratchMemoryMark;
	gScratchMemoryPeak = MFMax(gScratchMemoryPeak, size);

	// move the render memory marker to the current offset
	gScratchMemoryMark = gScratchMemoryOffset;
}

MF_API int MFRenderer_Begin()
{
	MFCALLSTACK;

	MFDebug_Assert(pCurrentMaterial, "No material set!");
	return pCurrentMaterial->pType->materialCallbacks.pBegin(pCurrentMaterial, *(MFRendererState*)NULL);
}

MF_API void MFRenderer_SetDeviceRenderTarget()
{
	MFRenderer_SetRenderTarget(MFRenderer_GetDeviceRenderTarget(), MFRenderer_GetDeviceDepthStencil());
}

MF_API void MFRenderer_SetMatrices(const MFMatrix *pMatrices, int numMatrices)
{
	pAnimMats = pMatrices;
	gNumAnimMats = numMatrices;
}

MF_API void MFRenderer_SetBatch(const uint16 *pBatch, int numBonesInBatch)
{
	pCurrentBatch = pBatch;
	gNumBonesInBatch = numBonesInBatch;
}

MF_API void* MFRenderer_AllocateCommandBufferMemory(size_t bytes, size_t alignment)
{
	return NULL;
}

MF_API void* MFRenderer_AllocateScratchMemory(size_t bytes, size_t alignment)
{
	MFDebug_Assert(bytes <= gScratchMemorySize, "Allocation is larger than render memory heap!");

	size_t end = gScratchMemoryOffset < gScratchMemoryMark ? gScratchMemoryMark-1 : gScratchMemorySize;

	while(end - MFALIGN(gScratchMemoryOffset, alignment) < bytes)
	{
		if(gScratchMemoryOffset >= gScratchMemoryMark)
		{
			gScratchMemoryOffset = 0;
			end = gScratchMemoryMark-1;
			continue;
		}

		MFDebug_Assert(false, "Out of render memory!");
		return NULL;
	}

	void *pMem = pScratchMemory + MFALIGN(gScratchMemoryOffset, alignment);
	gScratchMemoryOffset = MFALIGN(gScratchMemoryOffset, alignment) + bytes;
	return pMem;
}

MF_API void MFRenderer_GetMemoryStats(size_t *pCommandBuffer, size_t *pCommandBufferPeak, size_t *pScratch, size_t *pScratchPeak)
{
	// TODO: return details...
	if(pCommandBuffer)
		*pCommandBuffer = 0;
	if(pCommandBufferPeak)
		*pCommandBufferPeak = 0;
	if(pScratch)
		*pScratch = gScratchMemoryOffset < gScratchMemoryMark ? gScratchMemorySize - gScratchMemoryMark + gScratchMemoryOffset : gScratchMemoryOffset - gScratchMemoryMark;
	if(pScratchPeak)
		*pScratchPeak = gScratchMemoryPeak;
}

//////////////////

MF_API MFRenderer* MFRenderer_Create(MFRenderLayerDescription *pLayers, int numLayers, MFStateBlock *pGlobal, MFStateBlock *pOverride)
{
#if !defined(MF_RETAIL)
	// add a layer for the system/debug stuff
	numLayers += 1;
#endif

	MFRenderer *pRenderer = (MFRenderer*)MFHeap_AllocAndZero(sizeof(MFRenderer) + sizeof(MFRenderLayer)*numLayers);

	pRenderer->pLayers = (MFRenderLayer*)&pRenderer[1];
	pRenderer->numLayers = numLayers;
	pRenderer->pGlobal = pGlobal;
	pRenderer->pOverride = pOverride;

	MFTexture *pRenderTarget = MFRenderer_GetDeviceRenderTarget();
	MFTexture *pDepthStencil = MFRenderer_GetDeviceDepthStencil();

	for(int a=0; a<numLayers; ++a)
	{
#if !defined(MF_RETAIL)
		if(a == numLayers - 1)
		{
			pRenderer->pLayers[a].pName = "Debug";
		}
#endif
		pRenderer->pLayers[a].pName = pLayers[a].pName;
		pRenderer->pLayers[a].pRenderTarget[0] = pRenderTarget;
		pRenderer->pLayers[a].pDepthStencil = pDepthStencil;
	}

	return pRenderer;
}

MF_API void MFRenderer_Destroy(MFRenderer *pRenderer)
{
	MFHeap_Free(pRenderer);
}

MF_API MFRenderer* MFRenderer_SetCurrent(MFRenderer *pRenderer)
{
	MFRenderer *pOld = gpCurrentRenderer;
	gpCurrentRenderer = pRenderer;
	return pOld;
}

MF_API MFRenderer* MFRenderer_GetCurrent()
{
	return gpCurrentRenderer;
}

MF_API MFRenderLayer* MFRenderer_GetLayer(MFRenderer *pRenderer, int layer)
{
	return &pRenderer->pLayers[layer];
}

MF_API MFRenderLayer* MFRenderer_GetDebugLayer(MFRenderer *pRenderer)
{
#if !defined(MF_RETAIL)
	return &pRenderer->pLayers[pRenderer->numLayers - 1];
#else
	return NULL;
#endif
}

MF_API void MFRenderer_SetRenderLayerSet(MFRenderer *pRenderer, MFRenderLayerSet *pLayerSet)
{
	gCurrentLayers = *pLayerSet;
}

MF_API void MFRenderer_AddVertices(const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	// TODO: choose appropriate layer from layer set...

	MFRenderLayer_AddVertices(gCurrentLayers.pSolidLayer, pMeshStateBlock, firstVertex, numVertices, primType, pMaterial, pEntity, pMaterialOverride, pView);
}

MF_API void MFRenderer_AddIndexedVertices(const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	// TODO: choose appropriate layer from layer set...

	MFRenderLayer_AddIndexedVertices(gCurrentLayers.pSolidLayer, pMeshStateBlock, firstIndex, numVertices, primType, pMaterial, pEntity, pMaterialOverride, pView);
}

MF_API void MFRenderer_AddMesh(MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	// TODO: choose appropriate layer from layer set...

	MFRenderLayer_AddMesh(gCurrentLayers.pSolidLayer, pMesh, pMaterial, pEntity, pMaterialOverride, pView);
}

MF_API void MFRenderer_AddModel(MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	MFRenderLayer_AddModel(&gCurrentLayers, pModel, pMaterialOverride, pView);
}

MFMatrix *MFRendererState::getDerivedMatrix(MFStateConstant_Matrix matrix)
{
	if(derivedMatrixDirty & MFBIT(matrix))
	{
		switch(matrix)
		{
			case MFSCM_View:
				pMatrixStatesSet[matrix]->Inverse(*pMatrixStates[MFSCM_Camera]);
				break;
			case MFSCM_WorldView:
				pMatrixStatesSet[matrix]->Multiply(*pMatrixStates[MFSCM_World], *getDerivedMatrix(MFSCM_View));
				break;
			case MFSCM_ViewProjection:
				pMatrixStatesSet[matrix]->Multiply4x4(*getDerivedMatrix(MFSCM_View), *pMatrixStates[MFSCM_Projection]);
				break;
			case MFSCM_WorldViewProjection:
				pMatrixStatesSet[matrix]->Multiply4x4(*pMatrixStates[MFSCM_World], *getDerivedMatrix(MFSCM_ViewProjection));
				break;
			case MFSCM_InverseWorld:
				pMatrixStatesSet[matrix]->Inverse(*pMatrixStates[MFSCM_World]);
				break;
			case MFSCM_InverseViewProjection:
				pMatrixStatesSet[matrix]->Inverse(*getDerivedMatrix(MFSCM_ViewProjection));
				break;
			default:
				MFDebug_Assert(false, "Not a derived matrix!");
		}

		derivedMatrixDirty ^= MFBIT(matrix);
	}

	return pMatrixStatesSet[matrix];
}

static void MFRenderer_ApplyRenderStates(MFRendererState &state, const MFStateBlock **ppStateBlocks, int numStateBlocks)
{
	// prime 'set' flags from mask
	MFCopyMemory(state.rsSet, state.rsMask, sizeof(state.rsSet));

	// for each stateblock in order of precedence
	for(int sb = numStateBlocks-1; sb >= 0; --sb)
	{
		const MFStateBlock *pSB = ppStateBlocks[sb];
		if(!pSB)
			continue;

		// get a mask of the bools that have not been set
		uint32 boolsToSet = pSB->boolsSet & ~state.rsSet[MFSB_CT_Bool];

		// mark bools as set
		state.rsSet[MFSB_CT_Bool] |= boolsToSet;

		// set the bool states
		state.bools = (state.bools & ~boolsToSet) | (pSB->bools & boolsToSet);

		// set render states
		const MFStateBlock::MFStateBlockStateChange *pStateChanges = pSB->GetStateChanges();
		for(int s = 0; s < pSB->numStateChanges; ++s)
		{
			const MFStateBlock::MFStateBlockStateChange &sc = pStateChanges[s];

			// if the state was cleared, of already set, skip it...
			if(!sc.stateSet || state.isSet(sc.constantType, sc.constant))
				continue;

			// mark as set
			state.rsSet[sc.constantType] |= MFBIT(sc.constant);

			// set the state
			const void *pData = pSB->GetStateData(sc.offset*4);

			switch(sc.constantType)
			{
				case MFSB_CT_Vector:
					if(state.pVectorStates[sc.constant] != (MFVector*)pData)
						state.pVectorStates[sc.constant] = (MFVector*)pData;
					break;
				case MFSB_CT_Matrix:
					if(state.pMatrixStates[sc.constant] != (MFMatrix*)pData)
					{
						state.pMatrixStates[sc.constant] = (MFMatrix*)pData;

						// ** HACK ** - Managed matrices need to dirty the derived matrices!
						if(sc.constant >= MFSCM_ManagedStart && sc.constant < MFSCM_ManagedStart + MFSCM_NumManaged)
						{
							switch(sc.constant)
							{
								case MFSCM_World:
									state.derivedMatrixDirty |= MFBIT(MFSCM_WorldView) | MFBIT(MFSCM_WorldViewProjection) | MFBIT(MFSCM_InverseWorld);
									state.pMatrixStates[MFSCM_WorldView] = NULL;
									state.pMatrixStates[MFSCM_WorldViewProjection] = NULL;
									state.pMatrixStates[MFSCM_InverseWorld] = NULL;
									break;
								case MFSCM_Camera:
									state.derivedMatrixDirty |= MFBIT(MFSCM_View) | MFBIT(MFSCM_WorldView) | MFBIT(MFSCM_ViewProjection) | MFBIT(MFSCM_WorldViewProjection) | MFBIT(MFSCM_InverseViewProjection);
									state.pMatrixStates[MFSCM_View] = NULL;
									state.pMatrixStates[MFSCM_WorldView] = NULL;
									state.pMatrixStates[MFSCM_ViewProjection] = NULL;
									state.pMatrixStates[MFSCM_WorldViewProjection] = NULL;
									state.pMatrixStates[MFSCM_InverseViewProjection] = NULL;
									break;
								case MFSCM_Projection:
									state.derivedMatrixDirty |= MFBIT(MFSCM_ViewProjection) | MFBIT(MFSCM_WorldViewProjection) | MFBIT(MFSCM_InverseViewProjection);
									state.pMatrixStates[MFSCM_ViewProjection] = NULL;
									state.pMatrixStates[MFSCM_WorldViewProjection] = NULL;
									state.pMatrixStates[MFSCM_InverseViewProjection] = NULL;
									break;
								default:
									MFUNREACHABLE;
									break;
							}
						}
					}
					break;
				case MFSB_CT_Texture:
					if(state.pTextures[sc.constant] != *(MFTexture**)pData)
						state.pTextures[sc.constant] = *(MFTexture**)pData;
					break;
				case MFSB_CT_RenderState:
					if(state.pRenderStates[sc.constant] != *(void**)pData)
						state.pRenderStates[sc.constant] = *(void**)pData;
					break;
				case MFSB_CT_Misc:
					switch(sc.constant)
					{
						case MFSCMisc_AnimationMatrices:
							state.animation = *(MFStateConstant_AnimationMatrices*)pData;
							break;
						case MFSCMisc_MatrixBatch:
							state.matrixBatch = *(MFStateConstant_MatrixBatch*)pData;
							break;
						default:
							MFUNREACHABLE;
							break;
					}
					break;
				default:
					continue;
			}
		}
	}
}

static void MissingStates(MFStateBlockConstantType type, uint32 missing)
{
	static const char * const sStateType[MFSB_CT_TypeCount] =
	{
		"Bool",
		"Vector",
		"Matrix",
		"Texture",
		"RenderState",
		"Misc",
		"",
		"Unknown"
	};

	MFString states;
	for(int a=0; a<32; ++a)
	{
		if(missing & (1<<a))
		{
			if(!states.IsNull())
				states += ", ";
			states += MFStateBlock_GetStateName(type, a);
		}
	}

	MFDebug_Assert(missing == 0, MFStr("Missing %s states: %s", sStateType[type], states.CStr()));
}

static void MFRenderer_CheckRequirements(MFRendererState &state, MFRenderElement &element)
{
	uint32 required[MFSB_CT_TypeCount] =
	{
		0, //MFSB_CT_Bool = 0,
		0, //MFSB_CT_Vector,
		0, //MFSB_CT_Matrix,
		0, //MFSB_CT_Texture,
		MFBIT(MFSCRS_VertexDeclaration) | MFBIT(MFSCRS_BlendState) | MFBIT(MFSCRS_DepthStencilState) | MFBIT(MFSCRS_RasteriserState),
		0, //MFSB_CT_Misc,
		0,
	};

	// add required vertex streams
	MFVertexDeclaration *pDecl = (MFVertexDeclaration*)state.pRenderStates[MFSCRS_VertexDeclaration];
	required[MFSB_CT_RenderState] |= pDecl->streamsUsed << MFSCRS_VertexBuffer0;

	// if we need an index buffer
	if(element.renderIndexed)
		required[MFSB_CT_RenderState] |= MFBIT(MFSCRS_IndexBuffer);

	// add misc requirements
	if(state.getBool(MFSCB_Animated))
		required[MFSB_CT_Misc] |= MFBIT(MFSCMisc_AnimationMatrices) | MFBIT(MFSCMisc_MatrixBatch);
	if(state.getBool(MFSCB_AlphaTest))
		required[MFSB_CT_Vector] |= MFBIT(MFSCV_RenderState);

	// add material requirements
	//...
	// HACK: hardcode the matrices for now...
	required[MFSB_CT_Matrix] |= MFBIT(MFSCM_World) | MFBIT(MFSCM_Camera) | MFBIT(MFSCM_Projection);

	// complain about missing states
	for(int a = 0; a < MFSB_CT_TypeCount; ++a)
	{
		uint32 missing = required[a] & ~state.rsSet[a];
		if(missing != 0)
			MissingStates((MFStateBlockConstantType)a, missing);
	}
}

static void MFRenderer_RenderElements(MFRendererState &state, MFRenderElement *pElements, int numElements)
{
	// set and mask the Layer + Override states
	MFRenderer_ApplyRenderStates(state, state.pStateBlocks + MFSBT_Layer, MFSBT_Max - MFSBT_Layer);
	MFCopyMemory(state.rsMask, state.rsSet, sizeof(state.rsMask));

	// for each render element
	for(int e=0; e<numElements; ++e)
	{
		MFRenderElement &element = pElements[e];

		// collect state blocks
		state.pStateBlocks[MFSBT_View] = element.pViewState;
		state.pStateBlocks[MFSBT_Entity] = element.pEntityState;
		state.pStateBlocks[MFSBT_Material] = element.pMaterialState;
		state.pStateBlocks[MFSBT_Geometry] = element.pGeometryState;
		state.pStateBlocks[MFSBT_MaterialOverride] = element.pMaterialOverrideState;

		// gather render states
		MFRenderer_ApplyRenderStates(state, state.pStateBlocks, MFSBT_Layer);

		// select the technique here? or do it inside RenderElement()?
		//...
		// selection criteria:
		//  - animation
		//  - lights
		//  - constants present
		//  - values of constants?

#if defined(MF_DEBUG)
		// check that all required states are set
		MFRenderer_CheckRequirements(state, element);
#endif

		// apply render state
		element.pMaterial->pType->materialCallbacks.pBegin(element.pMaterial, state);

		// update the bools 'set' state
		state.boolsSet = state.bools & state.rsSet[MFSB_CT_Bool];

		// set geometry buffers
		MFVertexDeclaration *pDecl = (MFVertexDeclaration*)state.pRenderStates[MFSCRS_VertexDeclaration];
		if(state.pRenderStatesSet[MFSCRS_VertexDeclaration] != pDecl)
		{
			state.pRenderStatesSet[MFSCRS_VertexDeclaration] = pDecl;
			MFVertex_SetVertexDeclaration(pDecl);
		}

		for(int vb = 0; vb < 8; ++vb)
		{
			if(pDecl->streamsUsed & MFBIT(vb))
			{
				MFVertexBuffer *pVB = (MFVertexBuffer*)state.pRenderStates[MFSCRS_VertexBuffer(vb)];
				if(state.pRenderStatesSet[MFSCRS_VertexBuffer(vb)] != pVB)
				{
					state.pRenderStatesSet[MFSCRS_VertexBuffer(vb)] = pVB;
					MFVertex_SetVertexStreamSource(vb, pVB);
				}
			}
		}

		// render
		if(element.renderIndexed)
		{
			if(state.pRenderStatesSet[MFSCRS_IndexBuffer] != state.pRenderStates[MFSCRS_IndexBuffer])
			{
				MFIndexBuffer *pIB = (MFIndexBuffer*)state.pRenderStates[MFSCRS_IndexBuffer];
				state.pRenderStatesSet[MFSCRS_IndexBuffer] = pIB;
				MFVertex_SetIndexBuffer(pIB);
			}

			MFVertexBuffer *pVB = (MFVertexBuffer*)state.pRenderStates[MFSCRS_VertexBuffer0];
			int numVertices = pVB->numVerts - (int)element.vertexBufferOffset;

			MFVertex_RenderIndexedVertices((MFPrimType)element.primType, element.vertexBufferOffset, element.indexBufferOffset, numVertices, element.vertexCount);
		}
		else
		{
			MFVertex_RenderVertices((MFPrimType)element.primType, element.vertexBufferOffset, element.vertexCount);
		}
	}
}

MF_API void MFRenderer_BuildCommandBuffers(MFRenderer *pRenderer)
{
	for(int l = 0; l < pRenderer->numLayers; ++l)
	{
		MFRenderLayer &layer = pRenderer->pLayers[l];

		int numElements = (int)layer.elements.size();

		// configure render target
		if(numElements > 0 || layer.clearFlags != MFRCF_None)
			MFRenderer_SetRenderTarget(layer.pRenderTarget[0], layer.pDepthStencil);

		// clear render target
		MFRenderClearFlags cf = layer.pDepthStencil ? layer.clearFlags : (MFRenderClearFlags)(layer.clearFlags & ~MFRCF_DepthStencil);
		if(layer.clearFlags != MFRCF_None)
			MFRenderer_ClearScreen(cf, layer.clearColour, layer.clearZ, layer.clearStencil);

		if(numElements == 0)
			continue;

		// sort elements
		if(layer.sortMode != MFRL_SM_None)
			MFRendererInternal_SortElements(layer);

		// init renderer state
		MFRendererState state;
		MFZeroMemory(&state, sizeof(state));
		for(int m = 0; m < MFSCM_NumDerived; ++m)
			state.pMatrixStatesSet[MFSCM_DerivedStart + m] = &state.derivedMatrices[m];

		state.pStateBlocks[MFSBT_Global] = pRenderer->pGlobal;
		state.pStateBlocks[MFSBT_Layer] = layer.pLayer;
		state.pStateBlocks[MFSBT_Override] = pRenderer->pOverride;

		// render elements
		MFRenderer_RenderElements(state, layer.elements.getPointer(), numElements);

		// clear the layer
		layer.elements.clear();
	}
}

MF_API void MFRenderer_Kick(MFRenderer *pRenderer)
{
	//...
}

MF_API const MFStateBlock* MFRenderer_SetGlobalStateBlock(MFRenderer *pRenderer, const MFStateBlock *pGlobal)
{
	const MFStateBlock *pOld = pRenderer->pGlobal;
	pRenderer->pGlobal = pGlobal;
	return pOld;
}

MF_API const MFStateBlock* MFRenderer_SetOverrideStateBlock(MFRenderer *pRenderer, const MFStateBlock *pOverride)
{
	const MFStateBlock *pOld = pRenderer->pOverride;
	pRenderer->pOverride = pOverride;
	return pOld;
}

MF_API void MFRenderer_CloneLayer(MFRenderer *pRenderer, int sourceLayer, int destLayer)
{
}

MF_API void MFRenderLayer_SetLayerStateBlock(MFRenderLayer *pLayer, const MFStateBlock *pState)
{
	pLayer->pLayer = pState;
}

MF_API void MFRenderLayer_SetLayerSortMode(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode)
{
	pLayer->sortMode = sortMode;
}

MF_API void MFRenderLayer_SetLayerRenderTarget(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture)
{
	pLayer->pRenderTarget[targetIndex] = pTexture;
}

MF_API void MFRenderLayer_SetLayerDepthTarget(MFRenderLayer *pLayer, MFTexture *pTexture)
{
	pLayer->pDepthStencil = pTexture;
}

MF_API void MFRenderLayer_SetLayerColorCapture(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture)
{

}

MF_API void MFRenderLayer_SetLayerDepthCapture(MFRenderLayer *pLayer, MFTexture *pTexture)
{

}

MF_API void MFRenderLayer_SetClear(MFRenderLayer *pLayer, MFRenderClearFlags clearFlags, const MFVector &colour, float z, int stencil)
{
	pLayer->clearFlags = clearFlags;
	pLayer->clearColour = colour;
	pLayer->clearZ = z;
	pLayer->clearStencil = stencil;
}

MF_API void MFRenderLayer_AddVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	MFRenderElement &e = pLayer->elements.push();

	e.type = 0; // TODO: geometry?
	e.primarySortKey = (uint8)pLayer->primaryKey;
	e.zSort = 0;

//	e.pShaderTechnique = NULL;
	e.pMaterial = pMaterial;

	e.pViewState = pView;
	e.pGeometryState = pMeshStateBlock;
	e.pEntityState = pEntity;
	e.pMaterialState = MFMaterial_GetMaterialStateBlock(pMaterial);
	e.pMaterialOverrideState = pMaterialOverride;

	e.vertexBufferOffset = firstVertex;
	e.indexBufferOffset = 0;
	e.vertexCount = numVertices;
	e.primType = primType;
	e.renderIndexed = 0;

//	e.pInstances = NULL;
//	e.numInstances = 0;
//	e.animBatch = 0;
}

MF_API void MFRenderLayer_AddIndexedVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	MFRenderElement &e = pLayer->elements.push();

	e.type = 0; // TODO: geometry?
	e.primarySortKey = (uint8)pLayer->primaryKey;
	e.zSort = 0;

//	e.pShaderTechnique = NULL;
	e.pMaterial = pMaterial;

	e.pViewState = pView;
	e.pGeometryState = pMeshStateBlock;
	e.pEntityState = pEntity;
	e.pMaterialState = MFMaterial_GetMaterialStateBlock(pMaterial);
	e.pMaterialOverrideState = pMaterialOverride;

	e.vertexBufferOffset = 0;
	e.indexBufferOffset = firstIndex;
	e.vertexCount = numVertices;
	e.primType = primType;
	e.renderIndexed = 1;

//	e.pInstances = NULL;
//	e.numInstances = 0;
//	e.animBatch = 0;
}

MF_API void MFRenderLayer_AddMesh(MFRenderLayer *pLayer, MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	if(pMesh->numIndices)
		MFRenderLayer_AddIndexedVertices(pLayer, pMesh->pMeshState, pMesh->indexOffset, pMesh->numIndices, pMesh->primType, pMaterial, pEntity, pMaterialOverride, pView);
	else
		MFRenderLayer_AddVertices(pLayer, pMesh->pMeshState, pMesh->vertexOffset, pMesh->numVertices, pMesh->primType, pMaterial, pEntity, pMaterialOverride, pView);
}

MF_API void MFRenderLayer_AddModel(MFRenderLayerSet *pLayerSet, MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView)
{
	MFModel_SubmitGeometry(pModel, pLayerSet, pMaterialOverride, pView);
}

MF_API void MFRenderLayer_AddFence(MFRenderLayer *pLayer)
{
	++pLayer->primaryKey;
}
