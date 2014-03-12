#pragma once
#if !defined(_MFEFFECT_INTERNAL_H)
#define _MFEFFECT_INTERNAL_H

#include "MFEffect.h"
#include "MFShader_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFResource.h"

// functions
MFInitStatus MFEffect_InitModule(int moduleId, bool bPerformInitialisation);
void MFEffect_DeinitModule();

void MFEffect_InitModulePlatformSpecific();
void MFEffect_DeinitModulePlatformSpecific();

bool MFEffect_CreatePlatformSpecific(MFEffect *pEffect);
void MFEffect_DestroyPlatformSpecific(MFEffect *pEffect);
size_t MFEffect_PlatformDataSize();

struct MFEffectTechnique
{
	const char *pName;

	// conditions...
	uint32 bools;
	uint32 boolValue;
	int numVertexWeights;
	int numLights;

	// shaders
	struct Shader
	{
		MFShader *pShader;
		const char *pShaderSource;
		const char *pShaderLanguage;
		bool bFromFile;
		int startLine;
	};

	Shader shaders[MFST_Max];
	uint32 renderStateRequirements[MFSB_CT_TypeCount];

	// compile macros

	MFShaderMacro *pMacros;
	int numMacros;

	void *pPlatformData;
};

struct MFEffect : MFResource
{
	const char *pEffectName;

	MFEffectTechnique *pTechniques;
	int numTechniques;

	void *pPlatformData;
};

MFEffectTechnique *MFEffect_GetTechnique(MFEffect *pEffect, MFRendererState &state);

#endif
