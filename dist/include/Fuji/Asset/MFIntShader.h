#pragma once
#if !defined(_MFINTSHADER_H)
#define _MFINTSHADER_H

#include "MFShader.h"

enum MFShaderLanguage
{
	MFSL_Unknown = -1,

	MFSL_HLSL = 0,
	MFSL_GLSL,
	MFSL_Cg,
	MFSL_VSAsm,
	MFSL_PSAsm,
};

MF_API bool MFIntShader_CreateFromString(MFShaderType shaderType, const char *pShaderSource, const char *pFile, int line, MFShaderMacro *pMacros, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language = MFSL_Unknown);
MF_API bool MFIntShader_CreateFromFile(MFShaderType shaderType, const char *pFilename, MFShaderMacro *pMacros, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language = MFSL_Unknown);

#endif
