#pragma once
#if !defined(_MFRENDERTARGET_INTERNAL_H)
#define _MFRENDERTARGET_INTERNAL_H

#include "MFRenderTarget.h"
#include "MFResource.h"

struct MFTexture;

struct MFRenderTarget : public MFResource
{
	int width, height;

	MFTexture *pColourTargets[8];
	MFTexture *pDepthStencil;
	uint8 availableColourTargets;
	bool bHasDepth;
	bool bHasStencil;

	void *pPlatformData;
};

// functions
MFInitStatus MFRenderTarget_InitModule();
void MFRenderTarget_DeinitModule();

bool MFRenderTarget_InitModulePlatformSpecific();
void MFRenderTarget_DeinitModulePlatformSpecific();

bool MFRenderTarget_CreatePlatformSpecific(MFRenderTarget *pTexture);
void MFRenderTarget_DestroyPlatformSpecific(MFRenderTarget *pTexture);

#endif
