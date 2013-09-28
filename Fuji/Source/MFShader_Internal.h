#pragma once
#if !defined(_MFSHADER_INTERNAL_H)
#define _MFSHADER_INTERNAL_H

#include "MFShader.h"
#include "MFRenderState.h"
#include "MFResource.h"

// functions
MFInitStatus MFShader_InitModule();
void MFShader_DeinitModule();

void MFShader_InitModulePlatformSpecific();
void MFShader_DeinitModulePlatformSpecific();

bool MFShader_CreatePlatformSpecific(MFShader *pShader, MFShaderMacro *pMacros, const char *pFilename, const char *pSource);
void MFShader_DestroyPlatformSpecific(MFShader *pShader);

void MFShader_Apply(MFShader *pShader);

struct MFShader : MFResource
{
	MFShaderType shaderType;

	// GPU shaders
	void *pProgram;
	size_t bytes;

	// CPU shaders
	void (*pConfigure)();
	void (*pExecute)(void *pWorkload);

	// inputs
	uint32 renderStateRequirements[MFSB_CT_TypeCount];
	MFShaderInput *pInputs;
	int numInputs;

	void *pPlatformData;
};

#endif
