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

MFShader* MFShader_CreateFromFile(MFShaderType type, const char *pFilename);
MFShader* MFShader_CreateFromString(MFShaderType type, const char *pShader, const char *pName = NULL);
MFShader* MFShader_CreateFromBinary(MFShaderType type, const void *pShader, size_t bytes, const char *pName = NULL);
MFShader* MFShader_CreateFromCallbacks(MFShaderType type, MFShader_ConfigureCallback *pConfigureFunc, MFShader_ExecuteCallback *pExecuteFunc, const char *pName = NULL);
void MFShader_Destroy(MFShader *pShader);

#endif // _MFSHADER_H

/** @} */
