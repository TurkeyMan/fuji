#pragma once
#if !defined(_MFEFFECT_INTERNAL_H)
#define _MFEFFECT_INTERNAL_H

#include "MFEffect.h"
#include "MFRenderState.h"
#include "MFShader_Internal.h"
#include "MFResource.h"

// functions
MFInitStatus MFEffect_InitModule();
void MFEffect_DeinitModule();

void MFEffect_InitModulePlatformSpecific();
void MFEffect_DeinitModulePlatformSpecific();

bool MFEffect_CreatePlatformSpecific(MFEffect *pEffect);
void MFEffect_DestroyPlatformSpecific(MFEffect *pEffect);


struct MFEffectTechnique
{
	MFShader *pShaders[MFST_Max];

	// conditions...
	uint32 bools;
	int numVertexWeights;
	int numLights;

	// render state requirements
	uint32 renderStateRequirements[MFSB_CT_TypeCount];

	// compile macros
	const char **ppMacros;
	int numMacros;

	void *pPlatformData;
};

struct MFEffect : MFResource
{
	MFEffectTechnique *pTechniques;
	int numTechniques;

	void *pPlatformData;
};

#endif
