#pragma once
#if !defined(_MFSHADER_INTERNAL_H)
#define _MFSHADER_INTERNAL_H

#include "MFShader.h"
#include "MFResource.h"

// functions
MFInitStatus MFShader_InitModule(int moduleId, bool bPerformInitialisation);
void MFShader_DeinitModule();

void MFShader_InitModulePlatformSpecific();
void MFShader_DeinitModulePlatformSpecific();

bool MFShader_CreatePlatformSpecific(MFShader *pShader);
void MFShader_DestroyPlatformSpecific(MFShader *pShader);

void MFShader_Apply(MFShader *pShader);

struct MFShaderTemplate
{
	MFShaderType shaderType;

	int numInputs;
	MFShaderInput *pInputs;

	size_t bytes;
	const void *pProgram;
};

struct MFShader : MFResource
{
	// shader template
	MFShaderTemplate *pTemplate;

	// CPU shaders
	void (*pConfigure)();
	void (*pExecute)(void *pWorkload);

	// inputs
	uint32 renderStateRequirements[8];

	void *pPlatformData;
};

#endif
