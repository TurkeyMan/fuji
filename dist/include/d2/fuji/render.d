module fuji.render;

public import fuji.fuji;
import fuji.matrix;
import fuji.texture;
import fuji.renderstate;
import fuji.vertex;
import fuji.material;
import fuji.model;
import fuji.rendertarget;

enum MFRenderClearFlags : int
{
	Colour		    = 1,
	ZBuffer		    = 2,
	Stencil		    = 4,

	None	    	= 0,
	All		    	= Colour | ZBuffer | Stencil,
	DepthStencil    = ZBuffer | Stencil,
};

extern (C) void MFRenderer_ClearScreen(MFRenderClearFlags flags = MFRenderClearFlags.All, ref const MFVector colour = MFVector.zero, float z = 1, int stencil = 0);

extern (C) void MFRenderer_SetViewport(MFRect* pRect);
extern (C) void MFRenderer_ResetViewport();

extern (C) MFTexture* MFRenderer_GetDeviceRenderTarget();
extern (C) MFTexture* MFRenderer_GetDeviceDepthStencil();
extern (C) void MFRenderer_SetRenderTarget(MFRenderTarget *pRenderTarget);
extern (C) void MFRenderer_SetDeviceRenderTarget();

extern (C) float MFRenderer_GetTexelCenterOffset();

/**
* Begin a render block.
* Configures the renderer to render geometry with the current settings.
* @return Remaining number of passes.
* @remarks MFRenderer_Begin() should be called in a while loop to perform multipass effects. The same geometry should be submitted for each renderer pass.
*/
extern (C) int MFRenderer_Begin(); // returns number of passes remaining..

/**
* Set the animation matrices.
* Sets the current set of animation matrices.
* @param pMatrices Pointer to an array of matrices to be used for animation.
* @param numMatrices The number of matrices that \a pMatrices points to.
* @return None.
*/
extern (C) void MFRenderer_SetMatrices(const(MFMatrix)* pMatrices, int numMatrices);

/**
* Set the current animation bone batch.
* Sets the current animation bone batch.
* @param pBatch Pointer to the matrix batch.
* @param numBonesInBatch The number of bones in the batch.
* @return None.
*/
extern (C) void MFRenderer_SetBatch(const(ushort)* pBatch, int numBonesInBatch);

extern (C) void* MFRenderer_AllocateCommandBufferMemory(size_t bytes, size_t alignment = 16);

extern (C) void* MFRenderer_AllocateScratchMemory(size_t bytes, size_t alignment = 16);

extern (C) void MFRenderer_GetMemoryStats(size_t *pCommandBuffer, size_t *pCommandBufferPeak, size_t *pScratch, size_t *pScratchPeak);

// new renderer interface...

enum MFRenderLayerSortMode
{
	MFRL_SM_Unknown = -1,

	MFRL_SM_Default = 0,
	MFRL_SM_FrontToBack,
	MFRL_SM_BackToFront,
	MFRL_SM_None,

	MFRL_SM_Max,
	MFRL_SM_ForceInt = 0x7FFFFFFF,
};


struct MFRenderer;

struct MFRenderLayer;

struct MFRenderLayerDescription
{
	const(char)* pName;
};

struct MFRenderLayerSet
{
	MFRenderLayer *pSolidLayer;
	MFRenderLayer *pAlphaLayer;
	MFRenderLayer *pZPrimeLayer;
};

extern (C) MFRenderer* MFRenderer_Create(MFRenderLayerDescription *pLayers, int numLayers, MFStateBlock *pGlobal, MFStateBlock *pOverride);
extern (C) void MFRenderer_Destroy(MFRenderer *pRenderer);

extern (C) MFStateBlock* MFRenderer_SetGlobalStateBlock(MFRenderer *pRenderer, const MFStateBlock *pGlobal);
extern (C) MFStateBlock* MFRenderer_SetOverrideStateBlock(MFRenderer *pRenderer, const MFStateBlock *pOverride);

extern (C) void MFRenderer_SetRenderLayerSet(MFRenderer *pRenderer, MFRenderLayerSet *pLayerSet);

extern (C) MFRenderLayer* MFRenderer_GetLayer(MFRenderer *pRenderer, int layer);
extern (C) MFRenderLayer* MFRenderer_GetDebugLayer(MFRenderer *pRenderer);

extern (C) void MFRenderer_CloneLayer(MFRenderer *pRenderer, int sourceLayer, int destLayer);

extern (C) void MFRenderer_BuildCommandBuffers(MFRenderer *pRenderer);
extern (C) void MFRenderer_Kick(MFRenderer *pRenderer);

extern (C) MFRenderer* MFRenderer_SetCurrent(MFRenderer *pRenderer);
extern (C) MFRenderer* MFRenderer_GetCurrent();

extern (C) void MFRenderer_AddMesh(MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
extern (C) void MFRenderer_AddModel(MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
extern (C) void MFRenderer_AddVertices(const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
extern (C) void MFRenderer_AddIndexedVertices(const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

extern (C) void MFRenderLayer_SetLayerStateBlock(MFRenderLayer *pLayer, const MFStateBlock *pState);
extern (C) void MFRenderLayer_SetLayerSortMode(MFRenderLayer *pLayer, MFRenderLayerSortMode sortMode);

extern (C) void MFRenderLayer_SetLayerRenderTarget(MFRenderLayer *pLayer, MFRenderTarget *pRenderTarget);
extern (C) void MFRenderLayer_SetLayerColourCapture(MFRenderLayer *pLayer, int targetIndex, MFTexture *pTexture);
extern (C) void MFRenderLayer_SetLayerDepthCapture(MFRenderLayer *pLayer, MFTexture *pTexture);

extern (C) void MFRenderLayer_SetClear(MFRenderLayer *pLayer, MFRenderClearFlags clearFlags, ref const MFVector colour = MFVector.zero, float z = 1, int stencil = 0);

extern (C) void MFRenderLayer_AddVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstVertex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
extern (C) void MFRenderLayer_AddIndexedVertices(MFRenderLayer *pLayer, const MFStateBlock *pMeshStateBlock, int firstIndex, int numVertices, MFPrimType primType, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
extern (C) void MFRenderLayer_AddMesh(MFRenderLayer *pLayer, MFMesh *pMesh, MFMaterial *pMaterial, const MFStateBlock *pEntity, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);
extern (C) void MFRenderLayer_AddModel(MFRenderLayerSet *pLayerSet, MFModel *pModel, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

extern (C) void MFRenderLayer_AddFence(MFRenderLayer *pLayer);


// helpers...
void MFRenderer_SetLayerStateBlock(MFRenderer *pRenderer, int layer, MFStateBlock *pState)
{
	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, layer);
	MFRenderLayer_SetLayerStateBlock(pLayer, pState);
}
