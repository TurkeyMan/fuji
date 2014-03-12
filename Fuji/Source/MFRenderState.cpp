#include "Fuji_Internal.h"
#include "MFRenderState_Internal.h"
#include "MFRenderer.h"
#include "MFHeap.h"
#include "MFModule.h"
#include "Util.h"

static const char * const gpRenderStateNames_Matrix[] =
{
	"mWorld",
	"mCamera",
	"mProjection",
	"mShadowMap",
	"mFuji0",
	"mFuji1",
	"mUser0",
	"mUser1",
	"mUser2",
	"mUser3",
	"mUV0",
	"mUV1",
	"mUV2",
	"mUV3",
	"mView",
	"mWorldView",
	"mViewProjection",
	"mWorldViewProjection",
	"mInverseWorld",
	"mInverseViewProjection"
};
static const char * const gpRenderStateNames_Vector[] =
{
	"vTime",
	"vFogColour",
	"vFogParams1",
	"vFogParams2",
	"vRenderState",
	"vMaterialDiffuseColour",
	"vDiffuseColour",
	"vAmbientColour",
	"vFuji0", "vFuji1", "vFuji2", "vFuji3", "vFuji4", "vFuji5", "vFuji6",
	"vLightCounts",
	"vUser0", "vUser1", "vUser2", "vUser3", "vUser4", "vUser5", "vUser6", "vUser7",
	"vUser8", "vUser9", "vUser10","vUser11", "vUser12", "vUser13", "vUser14", "vUser15"
};
static const char * const gpRenderStateNames_Texture[] =
{
	"Diffuse",
	"NormalMap",
	"SpecularMap",
	"DetailMap",
	"OpacityMap",
	"EnvironmentMap",
	"SpecularPowerMap",
	"EmissiveMap",
	"LightMap",
	"ShadowBuffer",
	"Projector",
	"User0",
	"User1",
	"User2",
	"User3",
	"User4",
	"Vertex0",
	"Vertex1",
	"Vertex2"
};
static const char * const gpRenderStateNames_Bool[] =
{
	"bAnimated",
	"bZPrime",
	"bShadowGeneration",
	"bShadowReceiving",
	"bOpaque",
	"bAlphaTest",
	"bFuji0",
	"bFuji1",
	"bFuji2",
	"bUser0",
	"bUser1",
	"bUser2",
	"bUser3",
	"bDiffuseSet",
	"bNormalMapSet",
	"bSpecularMapSet",
	"bDetailMapSet",
	"bOpacityMapSet",
	"bEnvironmentMapSet",
	"bSpecularPowerMapSet",
	"bEmissiveMapSet",
	"bLightMapSet",
	"bShadowBufferSet",
	"bProjectionSet",
	"bUserTex0Set",
	"bUserTex1Set",
	"bUserTex2Set",
	"bUserTex3Set",
	"bUserTex4Set",
	"bVertexTex0Set",
	"bVertexTex1Set",
	"bVertexTex2Set"
};
static const char * const gpRenderStateNames_RenderState[] =
{
	"BlendState",
	"DepthStencilState",
	"RasteriserState",
	"sDiffuseSampler",
	"sNormalSampler",
	"sSpecularSampler",
	"sDetailSampler",
	"sOpacitySampler",
	"sEnvironmentSampler",
	"sSpecularPowerSampler",
	"sEmissiveSampler",
	"sLightSampler",
	"sShadowBufferSampler",
	"sProjectionSampler",
	"sUser0Sampler",
	"sUser1Sampler",
	"sUser2Sampler",
	"sUser3Sampler",
	"sUser4Sampler",
	"sVertex0Sampler",
	"sVertex1Sampler",
	"sVertex2Sampler",
	"VertexDeclaration",
	"VertexBuffer0",
	"VertexBuffer1",
	"VertexBuffer2",
	"VertexBuffer3",
	"VertexBuffer4",
	"VertexBuffer5",
	"VertexBuffer6",
	"VertexBuffer7",
	"IndexBuffer"
};
static const char * const gpRenderStateNames_Misc[] =
{
	"mAnimationMatrices",
	"MatrixBatch",
	"Light0",
	"Light1",
	"Light2",
	"Light3",
	"Light4",
	"Light5",
	"Light6",
	"Light7",
	"Light8",
	"Light9",
	"Light10",
	"Light11",
	"Light12",
	"Light13",
	"Light14",
	"Light15",
	"Light16",
	"GPUEvent"
};

static const char * const *gppRenderStateNames[MFSB_CT_TypeCount] =
{
	gpRenderStateNames_Bool,
	gpRenderStateNames_Vector,
	gpRenderStateNames_Matrix,
	gpRenderStateNames_RenderState,
	gpRenderStateNames_Texture,
	gpRenderStateNames_Misc,
	NULL,
	NULL
};

static const uint8 gConstantCount[MFSB_CT_TypeCount] =
{
	MFSCB_Max,
	MFSCV_Max,
	MFSCM_Max,
	MFSCRS_Max,
	MFSCT_Max,
	MFSCM_Max,
	0,
	0
};

MF_API const char* MFStateBlock_GetRenderStateName(MFStateBlockConstantType constantType, int constant)
{
	return gppRenderStateNames[constantType][constant];
}

MF_API int MFStateBlock_GetNumRenderStates(MFStateBlockConstantType constantType)
{
	return gConstantCount[constantType];
}

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

MFInitStatus MFRenderState_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFRT_BlendState = MFResource_Register("MFBlendState", &MFBlendState_Destroy);
	MFRT_SamplerState = MFResource_Register("MFSamplerState", &MFSamplerState_Destroy);
	MFRT_DepthStencilState = MFResource_Register("MFDepthStencilState", &MFDepthStencilState_Destroy);
	MFRT_RasteriserState = MFResource_Register("MFRasteriserState", &MFRasteriserState_Destroy);

	MFRenderState_InitModulePlatformSpecific();

	return MFIS_Succeeded;
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
	MFDebug_Assert(pDesc->mipFilter != MFTexFilter_Anisotropic, "Anisotropic filtering is not a valid mip filter!");

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

MF_API MFStateBlock* MFStateBlock_Create(size_t size)
{
	MFDebug_Assert(size >= MFStateBlock::MINIMUM_SIZE && MFUtil_NextPowerOf2(size) == size, "Invalid size. Must be a power of 2, and >= MFStateBlock::MINIMUM_SIZE bytes");

	MFStateBlock *pSB = (MFStateBlock*)MFHeap_AllocAndZero(size);

	// calculate the size
	int shift = 0;
	for(size_t s = size >> 6; !(s & 1); s >>= 1, ++shift) {}

	pSB->allocated = shift;

	return pSB;
}

MF_API MFStateBlock* MFStateBlock_CreateTemporary(size_t size)
{
	MFDebug_Assert(size >= MFStateBlock::MINIMUM_SIZE && MFUtil_NextPowerOf2(size) == size, "Invalid size. Must be a power of 2, and >= MFStateBlock::MINIMUM_SIZE bytes");

	MFStateBlock *pSB = (MFStateBlock*)MFRenderer_AllocateScratchMemory(size);
	MFZeroMemory(pSB, size);

	// calculate the size
	int shift = 0;
	for(size_t s = size >> 6; !(s & 1); s >>= 1, ++shift) {}

	pSB->allocated = shift;

	return pSB;
}

MF_API MFStateBlock* MFStateBlock_CreateDefault(size_t size)
{
	MFStateBlock *pDefaults = MFStateBlock_Create(size < 512 ? 512 : size);

	MFStateBlock_SetMatrix(pDefaults, MFSCM_World, MFMatrix::identity);
	MFStateBlock_SetMatrix(pDefaults, MFSCM_Camera, MFMatrix::identity);
	MFStateBlock_SetMatrix(pDefaults, MFSCM_UV0, MFMatrix::identity);
	MFStateBlock_SetMatrix(pDefaults, MFSCM_UV1, MFMatrix::identity);
	MFStateBlock_SetMatrix(pDefaults, MFSCM_UV2, MFMatrix::identity);
	MFStateBlock_SetMatrix(pDefaults, MFSCM_UV3, MFMatrix::identity);

	MFStateBlock_SetVector(pDefaults, MFSCV_MaterialDiffuseColour, MFVector::one);
	MFStateBlock_SetVector(pDefaults, MFSCV_DiffuseColour, MFVector::one);
	MFStateBlock_SetVector(pDefaults, MFSCV_AmbientColour, MFVector::one);

	return pDefaults;
}

MF_API void MFStateBlock_Destroy(MFStateBlock *pStateBlock)
{
	MFHeap_Free(pStateBlock);
}

MF_API MFStateBlock* MFStateBlock_Clone(const MFStateBlock *pSource)
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

MF_API void MFStateBlock_Copy(const MFStateBlock *pSource, MFStateBlock *pDest)
{
	size_t sourceSize = pSource->GetUsed();
	MFDebug_Assert(sourceSize <= pDest->GetSize(), "pDest is too small!");

	uint8 oldSize = pDest->allocated;
	MFCopyMemory(pDest, pSource, sourceSize);
	pDest->allocated = oldSize;
}

MF_API MFStateBlock* MFStateBlock_Merge(const MFStateBlock *pSource1, const MFStateBlock *pSource2)
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

const MFStateBlock::MFStateBlockStateChange* MFStateBlock::FindState(uint32 type, uint32 constant) const
{
	const MFStateBlock::MFStateBlockStateChange *pStates = GetStateChanges();
	for(int a = 0; a < numStateChanges; ++a)
	{
		if(pStates[a].constant == constant && pStates[a].constantType == type && pStates[a].stateSet != 0)
			return &pStates[a];
	}
	return NULL;
}

MFStateBlock::MFStateBlockStateChange* MFStateBlock::AllocState(uint32 type, uint32 constant, size_t bytes)
{
	bytes = MFALIGN(bytes, 4);
	bool bAlign16 = (bytes & 0xC) == 0;

	MFStateBlock::MFStateBlockStateChange *pStates = GetStateChanges();

	MFStateBlock::MFStateBlockStateChange *pState = NULL;
	MFStateBlock::MFStateBlockStateChange *pLastSmall = NULL;
	size_t bestSize = 0x10000;

	// linear scan of the state list; clear existing state, find space to overwrite
	for(int a = 0; a < numStateChanges; ++a)
	{
		// if the state already exists, we'll invalidate it
		// if the state is the same size, we'll overwrite it with new data in the next paragraph
		if(pStates[a].constant == constant && pStates[a].constantType == type)
			pStates[a].stateSet = 0;

		if(pStates[a].stateSet == 0)
		{
			size_t stateSize = pStates[a].StateSize();
			if(stateSize >= bytes && stateSize < bestSize)
			{
				pState = pStates + a;
				bestSize = stateSize;

				// NOTE: if bestSize == bytes, we could break here...
				// except we need to invalidate any existing instance of this state, so we continue
			}
		}

		if(!bAlign16 && pStates[a].vectors == 0)
			pLastSmall = pStates + a;
	}

	if(!pState)
	{
		// if we didn't find an vacant slot
		if(pLastSmall && bytes <= (4 - (pLastSmall->offset & 3) - pLastSmall->size)*4)
		{
			pState = AllocStateChange(0);
			pState->offset = pLastSmall->offset + pLastSmall->size;
			pState->vectors = 0;
			pState->size = bytes / 4;
		}
		else
		{
			pState = AllocStateChange(bytes);
		}
	}
	else if(bestSize > bytes)
	{
		// if we don't entirely fill the state we're overwriting,
		// write a new state to the end of it's memory, and shrink the existing one

		// TODO: We'll just waste the space for the time being (by doing nothing here)...
	}

	pState->constantType = type;
	pState->constant = constant;
	pState->stateSet = 1;

	return pState;
}

MFStateBlock::MFStateBlockStateChange* MFStateBlock::AllocStateChange(size_t stateBytes)
{
	bool bAlign16 = (stateBytes & 0xC) == 0;
	bool bGrowStateList = (numStateChanges & 3) == 0;

	size_t alignedBytes = MFALIGN16(stateBytes);
	size_t memNeeded = (bGrowStateList ? 16 : 0) + alignedBytes;
	if(GetFree() < memNeeded)
		return NULL;

	if(bGrowStateList)
	{
		// TODO: use SIMD regs to do this...
#if defined(MF_64BIT)
		uint64 *pData = (uint64*)GetStateData();
		for(int a = used; a > 0; --a)
		{
			pData[a*2    ] = pData[(a-1)*2    ];
			pData[a*2 + 1] = pData[(a-1)*2 + 1];
		}
#else
		uint32 *pData = (uint32*)GetStateData();
		for(int a = used; a > 0; --a)
		{
			pData[a*4    ] = pData[(a-1)*4    ];
			pData[a*4 + 1] = pData[(a-1)*4 + 1];
			pData[a*4 + 2] = pData[(a-1)*4 + 2];
			pData[a*4 + 3] = pData[(a-1)*4 + 3];
		}
#endif
	}

	MFStateBlockStateChange *pState = &GetStateChanges()[numStateChanges++];
	if(stateBytes)
	{
		pState->offset = used*4;
		if(bAlign16)
		{
			pState->vectors = 1;
			pState->size = stateBytes / 16;
		}
		else
		{
			pState->vectors = 0;
			pState->size = stateBytes / 4;
		}
		used += (uint16)(alignedBytes / 16);
	}
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
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->AllocState(MFSB_CT_Vector, constant, sizeof(MFVector));
	MFDebug_Assert(pSC, "Stateblock is full!");
	if(!pSC)
		return false;

	MFVector *pVector = (MFVector*)pStateBlock->GetStateData(pSC->offset * 4);
	*pVector = state;

	return true;
}

MF_API bool MFStateBlock_SetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, const MFMatrix &state)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->AllocState(MFSB_CT_Matrix, constant, sizeof(MFMatrix));
	MFDebug_Assert(pSC, "Stateblock is full!");
	if(!pSC)
		return false;

	pSC->constant = constant;
	pSC->stateSet = 1;

	MFMatrix *pMat = (MFMatrix*)pStateBlock->GetStateData(pSC->offset * 4);
	*pMat = state;

	return true;
}

MF_API bool MFStateBlock_SetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture *pTexture)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->AllocState(MFSB_CT_Texture, constant, sizeof(MFTexture*));
	MFDebug_Assert(pSC, "Stateblock is full!");
	if(!pSC)
		return false;

	MFTexture **ppTex = (MFTexture**)pStateBlock->GetStateData(pSC->offset * 4);
	*ppTex = pTexture;

	MFStateBlock_SetBool(pStateBlock, MFSCB_TexSet(constant), true);

	return true;
}

MF_API bool MFStateBlock_SetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void *pState)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->AllocState(MFSB_CT_RenderState, renderState, sizeof(void*));
	MFDebug_Assert(pSC, "Stateblock is full!");
	if(!pSC)
		return false;

	void **ppState = (void**)pStateBlock->GetStateData(pSC->offset * 4);
	*ppState = pState;

	return true;
}

MF_API bool MFStateBlock_SetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, const void *pStateData, size_t dataSize)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->AllocState(MFSB_CT_Misc, miscState, dataSize);
	MFDebug_Assert(pSC, "Stateblock is full!");
	if(!pSC)
		return false;

	MFCopyMemory(pStateBlock->GetStateData(pSC->offset * 4), pStateData, dataSize);

	return false;
}

MF_API bool MFStateBlock_GetBool(const MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool *pState)
{
	uint32 bit = 1 << constant;
	bool isSet = !!(pStateBlock->boolsSet & bit);
	if(pState)
		*pState = isSet && (pStateBlock->bools & bit);
	return isSet;
}

MF_API bool MFStateBlock_GetVector(const MFStateBlock *pStateBlock, MFStateConstant_Vector constant, MFVector *pState)
{
	const MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->FindState(MFSB_CT_Vector, constant);
	if(!pSC)
		return false;
	if(pState)
		*pState = *(MFVector*)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetMatrix(const MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, MFMatrix *pState)
{
	const MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->FindState(MFSB_CT_Matrix, constant);
	if(!pSC)
		return false;
	if(pState)
		*pState = *(MFMatrix*)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetTexture(const MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture **ppTexture)
{
	if(!ppTexture)
		return MFStateBlock_GetBool(pStateBlock, MFSCB_TexSet(constant), NULL);

	const MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->FindState(MFSB_CT_Texture, constant);
	if(!pSC)
		return false;
	*ppTexture = *(MFTexture**)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetRenderState(const MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void **ppState)
{
	const MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->FindState(MFSB_CT_RenderState, renderState);
	if(!pSC)
		return false;
	if(ppState)
		*ppState = *(void**)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

MF_API bool MFStateBlock_GetMiscState(const MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, void **ppStateData)
{
	const MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->FindState(MFSB_CT_Misc, miscState);
	if(!pSC)
		return false;
	if(ppStateData)
		*ppStateData = *(void**)pStateBlock->GetStateData(pSC->offset * 4);
	return true;
}

//MF_API void MFStateBlock_GetLight(const MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, MKLight **ppLight)

//MF_API void MFStateBlock_GetLightCounts(const MFStateBlock *pStateBlock, int *pOmniLightCount, int *pSpotLightCount, int *pDirectionalLightCount)

MF_API void MFStateBlock_ClearBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant)
{
	pStateBlock->boolsSet &= ~(1 << constant);
}

MF_API void MFStateBlock_ClearVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->GetState(MFSB_CT_Vector, constant);
	if(pSC)
		pSC->stateSet = 0;
}

MF_API void MFStateBlock_ClearMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->GetState(MFSB_CT_Matrix, constant);
	if(pSC)
		pSC->stateSet = 0;
}

MF_API void MFStateBlock_ClearTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->GetState(MFSB_CT_Texture, constant);
	if(pSC)
		pSC->stateSet = 0;

	MFStateBlock_ClearBool(pStateBlock, MFSCB_TexSet(constant));
}

MF_API void MFStateBlock_ClearRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->GetState(MFSB_CT_RenderState, renderState);
	if(pSC)
		pSC->stateSet = 0;
}

MF_API void MFStateBlock_ClearMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState)
{
	MFStateBlock::MFStateBlockStateChange *pSC = pStateBlock->GetState(MFSB_CT_Misc, miscState);
	if(pSC)
		pSC->stateSet = 0;
}
