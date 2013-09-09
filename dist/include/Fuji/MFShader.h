/**
 * @file MFShader.h
 * @brief Shader management.
 * @author Manu Evans
 * @defgroup MFShader Shaders
 * @{
 */

#pragma once
#if !defined(_MFSHADER_H)
#define _MFSHADER_H

struct MFShader;

typedef void (MFShader_ConfigureCallback)();
typedef void (MFShader_ExecuteCallback)(void *pWorkload);

enum MFShaderType
{
	MFST_VertexShader,
	MFST_PixelShader,
	MFST_GeometryShader,
	MFST_DomainShader,
	MFST_HullShader,
	MFST_ComputeShader,

	MFST_Max,
	MFST_ForceInt = 0x7FFFFFFF
};

struct MFShaderMacro
{
	const char *pDefine;
	const char *pValue;
};

MF_API MFShader* MFShader_CreateFromFile(MFShaderType type, const char *pFilename, MFShaderMacro *pMacros = NULL);
MF_API MFShader* MFShader_CreateFromString(MFShaderType type, const char *pShader, MFShaderMacro *pMacros = NULL, const char *pName = NULL);
MF_API MFShader* MFShader_CreateFromBinary(MFShaderType type, void *pShader, size_t bytes, MFShaderMacro *pMacros = NULL, const char *pName = NULL);
MF_API MFShader* MFShader_CreateFromCallbacks(MFShaderType type, MFShader_ConfigureCallback *pConfigureFunc, MFShader_ExecuteCallback *pExecuteFunc, MFShaderMacro *pMacros = NULL, const char *pName = NULL);
MF_API int MFShader_Release(MFShader *pShader);

#endif // _MFSHADER_H

/** @} */
