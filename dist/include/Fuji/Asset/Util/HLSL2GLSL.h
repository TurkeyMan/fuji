#pragma once
#if !defined(_HLSL2GLSL_H)
#define _HLSL2GLSL_H

#include "MFShader.h"

MF_API const char *HLSL2GLSL_TranslateShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename, const char **pError);
MF_API const char *HLSL2GLSL_OptimiseShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename, const char **ppOutput);
MF_API void HLSL2GLSL_FreeShader(const char *pShader);

#endif
