#include "Fuji.h"
#include "MFRenderState_Internal.h"
#include "MFRenderer.h"
#include "MFHeap.h"
#include "MFModule.h"


static void MFBlendState_Destroy(MFResource *pRes)
{
	MFBlendState *pBlendState = (MFBlendState*)pRes;
	MFBlendState_DestroyPlatformSpecific(pBlendState);
	MFHeap_Free(pBlendState);
}

static void MFSamplerState_Destroy(MFResource *pRes)
{
	MFSamplerState *pSamplerState = (MFSamplerState*)pRes;
	MFSamplerState_DestroyPlatformSpecific(pSamplerState);
	MFHeap_Free(pSamplerState);
}

static void MFDepthStencilState_Destroy(MFResource *pRes)
{
	MFDepthStencilState *pDepthStencilState = (MFDepthStencilState*)pRes;
	MFDepthStencilState_DestroyPlatformSpecific(pDepthStencilState);
	MFHeap_Free(pDepthStencilState);
}

static void MFRasteriserState_Destroy(MFResource *pRes)
{
	MFRasteriserState *pRasteriserState = (MFRasteriserState*)pRes;
	MFRasteriserState_DestroyPlatformSpecific(pRasteriserState);
	MFHeap_Free(pRasteriserState);
}

MFInitStatus MFRenderState_InitModule()
{
	MFRT_BlendState = MFResource_Register("MFBlendState", &MFBlendState_Destroy);
	MFRT_SamplerState = MFResource_Register("MFSamplerState", &MFSamplerState_Destroy);
	MFRT_DepthStencilState = MFResource_Register("MFDepthStencilState", &MFDepthStencilState_Destroy);
	MFRT_RasteriserState = MFResource_Register("MFRasteriserState", &MFRasteriserState_Destroy);

	MFRenderState_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFRenderState_DeinitModule()
{
	MFRenderState_DeinitModulePlatformSpecific();
}

MF_API MFBlendState* MFBlendState_Create(MFBlendStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFBlendStateDesc));

	MFBlendState *pBS = (MFBlendState*)MFResource_Find(hash);
	if(!pBS)
	{
		pBS = (MFBlendState*)MFHeap_AllocAndZero(sizeof(MFBlendState));
		pBS->stateDesc = *pDesc;

		MFBlendState_CreatePlatformSpecific(pBS);

		MFResource_AddResource(pBS, MFRT_BlendState, hash);
	}
	return pBS;
}

MF_API int MFBlendState_Release(MFBlendState *pBlendState)
{
	return MFResource_Release(pBlendState);
}

MF_API MFSamplerState* MFSamplerState_Create(MFSamplerStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFSamplerStateDesc));

	MFSamplerState *pSS = (MFSamplerState*)MFResource_Find(hash);
	if(!pSS)
	{
		pSS = (MFSamplerState*)MFHeap_AllocAndZero(sizeof(MFSamplerState));
		pSS->stateDesc = *pDesc;

		MFSamplerState_CreatePlatformSpecific(pSS);

		MFResource_AddResource(pSS, MFRT_SamplerState, hash);
	}
	return pSS;
}

MF_API int MFSamplerState_Release(MFSamplerState *pSamplerState)
{
	return MFResource_Release(pSamplerState);
}

MF_API MFDepthStencilState* MFDepthStencilState_Create(MFDepthStencilStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFDepthStencilStateDesc));

	MFDepthStencilState *pDSS = (MFDepthStencilState*)MFResource_Find(hash);
	if(!pDSS)
	{
		pDSS = (MFDepthStencilState*)MFHeap_AllocAndZero(sizeof(MFDepthStencilState));
		pDSS->stateDesc = *pDesc;

		MFDepthStencilState_CreatePlatformSpecific(pDSS);

		MFResource_AddResource(pDSS, MFRT_DepthStencilState, hash);
	}
	return pDSS;
}

MF_API int MFDepthStencilState_Release(MFDepthStencilState *pDepthStencilState)
{
	return MFResource_Release(pDepthStencilState);
}

MF_API MFRasteriserState* MFRasteriserState_Create(MFRasteriserStateDesc *pDesc)
{
	uint32 hash = MFUtil_HashBuffer(pDesc, sizeof(MFRasteriserStateDesc));

	MFRasteriserState *pRS = (MFRasteriserState*)MFResource_Find(hash);
	if(!pRS)
	{
		pRS = (MFRasteriserState*)MFHeap_AllocAndZero(sizeof(MFRasteriserState));
		pRS->stateDesc = *pDesc;

		MFRasteriserState_CreatePlatformSpecific(pRS);

		MFResource_AddResource(pRS, MFRT_RasteriserState, hash);
	}
	return pRS;
}

MF_API int MFRasteriserState_Release(MFRasteriserState *pRasteriserState)
{
	return MFResource_Release(pRasteriserState);
}

MF_API MFStateBlock* MFStateBlock_Create(uint32 size)
{
	MFDebug_Assert(size >= MFStateBlock::MINIMUM_SIZE && MFUtil_NextPowerOf2(size) == size, "Invalid size. Must be a power of 2, and >= MFStateBlock::MINIMUM_SIZE bytes");

	MFStateBlock *pSB = (MFStateBlock*)MFHeap_AllocAndZero(size);

	// calculate the size
	int shift = 0;
	for(uint32 s = size >> 6; !(s & 1); s >>= 1, ++shift) {}

	pSB->allocated = shift;

	return pSB;
}

MF_API MFStateBlock* MFStateBlock_CreateTemporary(uint32 size)
{
	MFDebug_Assert(size >= MFStateBlock::MINIMUM_SIZE && MFUtil_NextPowerOf2(size) == size, "Invalid size. Must be a power of 2, and >= MFStateBlock::MINIMUM_SIZE bytes");

	MFStateBlock *pSB = (MFStateBlock*)MFRenderer_AllocateScratchMemory(size);
	MFZeroMemory(pSB, size);

	// calculate the size
	int shift = 0;
	for(uint32 s = size >> 6; !(s & 1); s >>= 1, ++shift) {}

	pSB->allocated = shift;

	return pSB;
}

MF_API void MFStateBlock_Destroy(MFStateBlock *pStateBlock)
{
	MFHeap_Free(pStateBlock);
}

MF_API MFStateBlock* MFStateBlock_Clone(MFStateBlock *pSource)
{
	// find size for copy
	size_t size = MFUtil_NextPowerOf2(pSource->GetUsed());
	MFStateBlock *pNew = (MFStateBlock*)MFHeap_Alloc(size);

	// copy state block
	MFCopyMemory(pNew, pSource, pSource->GetUsed());

	// calculate the size
	int shift = 0;
	for(size_t s = size >> 6; !(s & 1); s >>= 1, ++shift) {}

	pNew->allocated = shift;
	return pNew;
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

MF_API void MFStateBlock_Clear(MFStateBlock *pStateBlock)
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

static MFStateBlock::MFStateBlockStateChange* FindState(MFStateBlock *pStateBlock, uint32 type, uint32 constant, bool bFindEmpty = false, int *pLastWord = NULL)
{
	MFStateBlock::MFStateBlockStateChange *pStates = pStateBlock->GetStateChanges();
	MFStateBlock::MFStateBlockStateChange *pVacant = NULL;

	for(int a = 0; a < pStateBlock->numStateChanges; ++a)
	{
		if(pStates[a].stateSet != 0 && pStates[a].constantType == type && pStates[a].constant == constant)
			return &pStates[a];
		if(bFindEmpty)
		{
			if(!pVacant && pStates[a].stateSet == 0 && pStates[a].constantType == type)
				pVacant = &pStates[a];
			if(pLastWord && pStates[a].constantSize == MFStateBlock::MFSB_CS_Word)
				*pLastWord = pStates[a].offset;
		}
	}

	return pVacant;
}

static MFStateBlock::MFStateBlockStateChange* AllocState(MFStateBlock *pStateBlock, int numVectors)
{
	MFStateBlock::MFStateBlockStateChange *pStates = pStateBlock->GetStateChanges();

	bool bGrowStateList = (pStateBlock->numStateChanges & 3) == 0;

	size_t memNeeded = numVectors*16 + (bGrowStateList ? 16 : 0);
	if(pStateBlock->GetFree() < memNeeded)
		return NULL;

	if(bGrowStateList)
	{
		// TODO: use SIMD regs to do this...
#if defined(MF_64BIT)
		uint64 *pData = (uint64*)pStateBlock->GetStateData();
		for(int a = pStateBlock->used; a > 0; --a)
		{
			pData[a*2    ] = pData[(a-1)*2    ];
			pData[a*2 + 1] = pData[(a-1)*2 + 1];
		}
#else
		uint32 *pData = (uint32*)pStateBlock->GetStateData();
		for(int a = pStateBlock->used; a > 0; --a)
		{
			pData[a*4    ] = pData[(a-1)*4    ];
			pData[a*4 + 1] = pData[(a-1)*4 + 1];
			pData[a*4 + 2] = pData[(a-1)*4 + 2];
			pData[a*4 + 3] = pData[(a-1)*4 + 3];
		}
#endif
	}

	MFStateBlock::MFStateBlockStateChange *pState = &pStates[pStateBlock->numStateChanges++];
	pState->offset = pStateBlock->used * 4;
	pStateBlock->used += (uint16)numVectors;
	return pState;
}

MF_API bool MFStateBlock_SetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool state)
{
	uint32 bit = 1 << constant;
	pStateBlock->bools = (pStateBlock->bools & ~bit) | (state ? bit : 0);
	pStateBlock->boolsSet |= bit;
	return true;
}

MF_API bool MFStateBlock_SetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, const MFVector &state)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Vector, constant, true);
	if(!pSC)
	{
		pSC = AllocState(pStateBlock, 1);

		MFDebug_Assert(pSC, "Stateblock is full!");
		if(!pSC)
			return false;

		pSC->constantType = MFSB_CT_Vector;
		pSC->constantSize = MFStateBlock::MFSB_CS_Vector;
	}

	pSC->constant = constant;
	pSC->stateSet = 1;

	MFVector *pVector = (MFVector*)pStateBlock->GetStateData(pSC->offset * 4);
	*pVector = state;

	return true;
}

MF_API bool MFStateBlock_SetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, const MFMatrix &state)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Matrix, constant, true);
	if(!pSC)
	{
		pSC = AllocState(pStateBlock, 4);

		MFDebug_Assert(pSC, "Stateblock is full!");
		if(!pSC)
			return false;

		pSC->constantType = MFSB_CT_Matrix;
		pSC->constantSize = MFStateBlock::MFSB_CS_Matrix;
	}

	pSC->constant = constant;
	pSC->stateSet = 1;

	MFMatrix *pMat = (MFMatrix*)pStateBlock->GetStateData(pSC->offset * 4);
	*pMat = state;

	return true;
}

MF_API bool MFStateBlock_SetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture *pTexture)
{
	int lastWord = -1;
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Texture, constant, true, &lastWord);
	if(!pSC)
	{
		bool bCanSqueeze = lastWord >= 0 && (lastWord & 3) != 3;

		pSC = AllocState(pStateBlock, bCanSqueeze ? 0 : 1);

		MFDebug_Assert(pSC, "Stateblock is full!");
		if(!pSC)
			return false;

		if(bCanSqueeze)
			pSC->offset = lastWord + 1;
	}

	pSC->constantType = MFSB_CT_Texture;
	pSC->constantSize = MFStateBlock::MFSB_CS_Word;
	pSC->constant = constant;
	pSC->stateSet = 1;

	MFTexture **ppTex = (MFTexture**)pStateBlock->GetStateData(pSC->offset * 4);
	*ppTex = pTexture;

	MFStateBlock_SetBool(pStateBlock, MFSCB_TexSet(constant), true);

	return true;
}

MF_API bool MFStateBlock_SetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void *pState)
{
	int lastWord = -1;
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_RenderState, renderState, true, &lastWord);
	if(!pSC)
	{
		bool bCanSqueeze = lastWord >= 0 && (lastWord & 3) != 3;

		pSC = AllocState(pStateBlock, bCanSqueeze ? 0 : 1);

		MFDebug_Assert(pSC, "Stateblock is full!");
		if(!pSC)
			return false;

		if(bCanSqueeze)
			pSC->offset = lastWord + 1;
	}

	pSC->constantType = MFSB_CT_RenderState;
	pSC->constantSize = MFStateBlock::MFSB_CS_Word;
	pSC->constant = renderState;
	pSC->stateSet = 1;

	void **ppState = (void**)pStateBlock->GetStateData(pSC->offset * 4);
	*ppState = pState;

	return true;
}

MF_API bool MFStateBlock_SetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, const void *pStateData, int dataSize)
{
	MFDebug_Assert(false, "TODO!");
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
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Vector, constant);
	if(!pSC)
		return false;
	if(pState)
		*pState = *(MFVector*)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, MFMatrix *pState)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Matrix, constant);
	if(!pSC)
		return false;
	if(pState)
		*pState = *(MFMatrix*)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture **ppTexture)
{
	if(!ppTexture)
		return MFStateBlock_GetBool(pStateBlock, MFSCB_TexSet(constant), NULL);

	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Texture, constant);
	if(!pSC)
		return false;
	*ppTexture = *(MFTexture**)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void **ppState)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_RenderState, renderState);
	if(!pSC)
		return false;
	if(ppState)
		*ppState = *(void**)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, void **ppStateData)
{
	MFDebug_Assert(false, "TODO!");
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
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Vector, constant);
	if(pSC)
		pSC->stateSet = 0;
}

MF_API void MFStateBlock_ClearMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Matrix, constant);
	if(pSC)
		pSC->stateSet = 0;
}

MF_API void MFStateBlock_ClearTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Texture, constant);
	if(pSC)
		pSC->stateSet = 0;

	MFStateBlock_ClearBool(pStateBlock, MFSCB_TexSet(constant));
}

MF_API void MFStateBlock_ClearRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_RenderState, renderState);
	if(pSC)
		pSC->stateSet = 0;
}

MF_API void MFStateBlock_ClearMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState)
{
	MFStateBlock::MFStateBlockStateChange *pSC = FindState(pStateBlock, MFSB_CT_Misc, miscState);
	if(pSC)
		pSC->stateSet = 0;
}
