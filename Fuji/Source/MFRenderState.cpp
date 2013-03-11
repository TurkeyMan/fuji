#include "Fuji.h"
#include "MFRenderState_Internal.h"

#include "MFModule.h"
#include "MFOpenHashTable.h"

MFInitStatus MFRenderState_InitModule()
{
	return MFAIC_Succeeded;
}

void MFRenderState_DeinitModule()
{

}

MF_API MFBlendState* MFBlendState_Create(MFBlendStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFBlendStateDesc));

	MFBlendState *pBS = (MFBlendState*)MFResource_FindResource(hash);
	if(!pBS)
	{
		pBS = (MFBlendState*)MFHeap_Alloc(sizeof(MFBlendState));
		pBS->type = MFRT_BlendState;
		pBS->hash = hash;
		pBS->refCount = 0;

		pBS->stateDesc = *pDesc;

		MFResource_AddResource(pBS);

		//... CreateInternal?
	}

	++pBS->refCount;

	return pBS;
}

MF_API void MFBlendState_Destroy(MFBlendState *pBlendState)
{
	if(--pBlendState->refCount == 0)
	{
		//... DestroyInternal?

		MFResource_RemoveResource(pBlendState);
		MFHeap_Free(pBlendState);
	}
}

MF_API MFSamplerState* MFSamplerState_Create(MFSamplerStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFSamplerStateDesc));

	MFSamplerState *pSS = (MFSamplerState*)MFResource_FindResource(hash);
	if(!pSS)
	{
		pSS = (MFSamplerState*)MFHeap_Alloc(sizeof(MFSamplerState));
		pSS->type = MFRT_SamplerState;
		pSS->hash = hash;
		pSS->refCount = 0;

		pSS->stateDesc = *pDesc;

		MFResource_AddResource(pSS);

		//... CreateInternal?
	}

	++pSS->refCount;

	return pSS;
}

void MFSamplerState_Destroy(MFSamplerState *pSamplerState)
{
	if(--pSamplerState->refCount == 0)
	{
		//... DestroyInternal?

		MFResource_RemoveResource(pSamplerState);
		MFHeap_Free(pSamplerState);
	}
}

MF_API MFDepthStencilState* MFDepthStencilState_Create(MFDepthStencilStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFDepthStencilStateDesc));

	MFDepthStencilState *pDSS = (MFDepthStencilState*)MFResource_FindResource(hash);
	if(!pDSS)
	{
		pDSS = (MFDepthStencilState*)MFHeap_Alloc(sizeof(MFDepthStencilState));
		pDSS->type = MFRT_DepthStencilState;
		pDSS->hash = hash;
		pDSS->refCount = 0;

		pDSS->stateDesc = *pDesc;

		MFResource_AddResource(pDSS);

		//... CreateInternal?
	}

	++pDSS->refCount;

	return pDSS;
}

MF_API void MFDepthStencilState_Destroy(MFDepthStencilState *pDepthStencilState)
{
	if(--pDepthStencilState->refCount == 0)
	{
		//... DestroyInternal?

		MFResource_RemoveResource(pDepthStencilState);
		MFHeap_Free(pDepthStencilState);
	}
}

MF_API MFRasteriserState* MFRasteriserState_Create(MFRasteriserStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFRasteriserStateDesc));

	MFRasteriserState *pRS = (MFRasteriserState*)MFResource_FindResource(hash);
	if(!pRS)
	{
		pRS = (MFRasteriserState*)MFHeap_Alloc(sizeof(MFRasteriserState));
		pRS->type = MFRT_RasteriserState;
		pRS->hash = hash;
		pRS->refCount = 0;

		pRS->stateDesc = *pDesc;

		MFResource_AddResource(pRS);

		//... CreateInternal?
	}

	++pRS->refCount;

	return pRS;
}

MF_API void MFRasteriserState_Destroy(MFRasteriserState *pRasteriserState)
{
	if(--pRasteriserState->refCount == 0)
	{
		//... DestroyInternal?

		MFResource_RemoveResource(pRasteriserState);
		MFHeap_Free(pRasteriserState);
	}
}

MF_API MFStateBlock* MFStateBlock_Create(uint32 size)
{
	MFDebug_Assert(size >= 64 && MFUtil_NextPowerOf2(size) == size, "Invalid size. Must be a power of 2, and >= 64 bytes");

	MFStateBlock *pSB = (MFStateBlock*)MFHeap_Alloc(size);

	// calculate the size
	int shift = 0;
	for(uint32 s = size >> 6; !(s & 1); s >>= 1, ++shift) {}

	pSB->allocated = shift;
	pSB->used = 0;
	pSB->numStateChanges = 0;
	pSB->boolsSet = 0;
	pSB->dirLightCount = 0;
	pSB->spotLightCount = 0;
	pSB->omniLightCount = 0;

	return pSB;
}

MF_API MFStateBlock* MFStateBlock_CreateTemporary(uint32 size)
{
	MFDebug_Assert(false, "TODO!");

	// allocate from per-frame temp mem
	return NULL;
}

MF_API void MFStateBlock_Destroy(MFStateBlock *pStateBlock)
{
	MFHeap_Free(pStateBlock);
}

MF_API MFStateBlock* MFStateBlock_Clone(MFStateBlock *pSource)
{
	MFDebug_Assert(false, "TODO!");
	return NULL;
}

MF_API void MFStateBlock_Copy(MFStateBlock *pSource, MFStateBlock *pDest)
{
	MFDebug_Assert(pSource->allocated <= pDest->allocated, "pDest is smaller than pSource!");
	uint8 oldSize = pDest->allocated;
	MFCopyMemory(pDest, pSource, MFStateBlock::MINIMUM_SIZE << pSource->allocated);
	pDest->allocated = oldSize;
}

MF_API MFStateBlock* MFStateBlock_Merge(MFStateBlock *pSource1, MFStateBlock *pSource2)
{
	MFDebug_Assert(false, "TODO!");
	return NULL;
}

MF_API void MFStateBlock_Reset(MFStateBlock *pStateBlock)
{
	pStateBlock->used = 0;
	pStateBlock->numStateChanges = 0;
	pStateBlock->boolsSet = 0;
	pStateBlock->dirLightCount = 0;
	pStateBlock->spotLightCount = 0;
	pStateBlock->omniLightCount = 0;
}

MF_API size_t MFStateBlock_GetAllocatedBytes(MFStateBlock *pStateBlock)
{
	return pStateBlock->GetSize();
}

MF_API size_t MFStateBlock_GetUsedBytes(MFStateBlock *pStateBlock)
{
	return pStateBlock->GetUsed();
}

MF_API size_t MFStateBlock_GetFreeBytes(MFStateBlock *pStateBlock)
{
	return pStateBlock->GetFree();
}

MF_API bool MFStateBlock_SetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool state)
{
	uint32 bit = 1 << constant;
	pStateBlock->bools = (pStateBlock->bools & ~bit) | (state ? bit : 0);
	pStateBlock->boolsSet |= bit;
	return true;
}

MF_API bool MFStateBlock_SetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, MFVector &state)
{
	return false;
}

MF_API bool MFStateBlock_SetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, const MFMatrix &state)
{
	return false;
}

MF_API bool MFStateBlock_SetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture *pTexture)
{
	return false;
}

MF_API bool MFStateBlock_SetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void *pState)
{
	return false;
}

MF_API bool MFStateBlock_SetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, const void *pStateData, int dataSize)
{
	return false;
}

//MF_API void MFStateBlock_SetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, const MKLight *pLight)

MF_API bool MFStateBlock_GetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool *pState)
{
	uint32 bit = 1 << constant;
	bool isSet = !!(pStateBlock->boolsSet & bit);
	if(pState)
		*pState = isSet && (pStateBlock->bools & bit);
	return isSet;
}

MF_API bool MFStateBlock_GetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, MFVector *pState)
{
	return false;
}

MF_API bool MFStateBlock_GetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, MFMatrix *pState)
{
	return false;
}

MF_API bool MFStateBlock_GetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture **ppTexture)
{
	return false;
}

MF_API bool MFStateBlock_GetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void **ppState)
{
	return false;
}

MF_API bool MFStateBlock_GetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, void **ppStateData)
{
	return false;
}

//MF_API void MFStateBlock_GetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, MKLight **ppLight)

//MF_API void MFStateBlock_GetLightCounts(MFStateBlock *pStateBlock, int *pOmniLightCount, int *pSpotLightCount, int *pDirectionalLightCount)

MF_API void MFStateBlock_ClearBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant)
{
	pStateBlock->boolsSet &= ~(1 << constant);
}

MF_API void MFStateBlock_ClearVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant)
{

}

MF_API void MFStateBlock_ClearMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant)
{

}

MF_API void MFStateBlock_ClearTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant)
{

}

MF_API void MFStateBlock_ClearRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState)
{

}

MF_API void MFStateBlock_ClearMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState)
{

}
