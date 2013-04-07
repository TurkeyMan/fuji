#pragma once
#if !defined(_MFRENDERSTATE_INTERNAL_H)
#define _MFRENDERSTATE_INTERNAL_H

#include "MFRenderState.h"
#include "MFResource.h"

// functions
MFInitStatus MFRenderState_InitModule();
void MFRenderState_DeinitModule();


struct MFBlendState : public MFResource
{
	MFBlendStateDesc stateDesc;
	void *pPlatformData;
};

struct MFSamplerState : public MFResource
{
	MFSamplerStateDesc stateDesc;
	void *pPlatformData;
};

struct MFDepthStencilState : public MFResource
{
	MFDepthStencilStateDesc stateDesc;
	void *pPlatformData;
};

struct MFRasteriserState : public MFResource
{
	MFRasteriserStateDesc stateDesc;
	void *pPlatformData;
};

MFALIGN_BEGIN(16)
struct MFStateBlock
{
	static const int MINIMUM_SIZE = 64;

	enum MFStateBlockConstantSize
	{
		MFSB_CS_Word,
		MFSB_CS_Vector,
		MFSB_CS_Matrix,
		MFSB_CS_Large,
	};

	struct MFStateBlockStateChange
	{
		uint32 stateSet     : 1;	// is the state set?
		uint32 constantSize : 2;	// MFStateBlockConstantSize
		uint32 constantType : 7;	// MFStateBlockConstantType
		uint32 constant     : 8;
		uint32 offset       : 14;	// in words: offsetInBytes = offset * 4;
	};

	uint32 bools;
	uint32 boolsSet;
	uint16 allocated	: 4;	// sizeInBytes = MINIMUM_SIZE << allocated
	uint16 used			: 12;	// multiple of 16 bytes
	uint8 numStateChanges;

	uint8 dirLightCount;
	uint8 spotLightCount;
	uint8 omniLightCount;

	uint16 unused;

	__forceinline MFStateBlockStateChange* GetStateChanges() { return (MFStateBlockStateChange*)((char*)this + sizeof(MFStateBlock)); }
	__forceinline const MFStateBlockStateChange* GetStateChanges() const { return (const MFStateBlockStateChange*)((const char*)this + sizeof(MFStateBlock)); }
	__forceinline void* GetStateData(size_t offset = 0) { return (void*)(MFALIGN16(GetStateChanges() + numStateChanges) + offset); }
	__forceinline const void* GetStateData(size_t offset = 0) const { return (const void*)(MFALIGN16(GetStateChanges() + numStateChanges) + offset); }

	__forceinline size_t GetSize() const { return MINIMUM_SIZE << allocated; }
	__forceinline size_t GetUsed() const { return 16 * used + ((char*)GetStateData() - (char*)this); }
	__forceinline size_t GetFree() const { return GetSize() - GetUsed(); }
}
MFALIGN_END(16);

void MFRenderState_InitModulePlatformSpecific();
void MFRenderState_DeinitModulePlatformSpecific();
bool MFBlendState_CreatePlatformSpecific(MFBlendState *pBS);
void MFBlendState_DestroyPlatformSpecific(MFBlendState *pBS);
bool MFSamplerState_CreatePlatformSpecific(MFSamplerState *pSS);
void MFSamplerState_DestroyPlatformSpecific(MFSamplerState *pSS);
bool MFDepthStencilState_CreatePlatformSpecific(MFDepthStencilState *pDSS);
void MFDepthStencilState_DestroyPlatformSpecific(MFDepthStencilState *pDSS);
bool MFRasteriserState_CreatePlatformSpecific(MFRasteriserState *pRS);
void MFRasteriserState_DestroyPlatformSpecific(MFRasteriserState *pRS);

#endif
