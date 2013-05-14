#pragma once
#if !defined(_MFSHADER_INTERNAL_H)
#define _MFSHADER_INTERNAL_H

#include "MFShader.h"
#include "MFResource.h"

// functions
MFInitStatus MFShader_InitModule();
void MFShader_DeinitModule();

void MFShader_InitModulePlatformSpecific();
void MFShader_DeinitModulePlatformSpecific();

bool MFShader_CreatePlatformSpecific(MFShader *pShader);
void MFShader_DestroyPlatformSpecific();

struct MFShader : MFResource
{
	MFShaderType shaderType;

	// GPU shaders
	void *pProgram;

	// CPU shaders
	void (*pConfigure)();
	void (*pExecute)(void *pWorkload);

	// inputs
	//...

	void *pPlatformData;
};

#endif
