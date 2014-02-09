module fuji.asset.intshader;

import fuji.fuji;
import fuji.shader;

extern (C) bool MFIntShader_CreateFromString(MFShaderType shaderType, const char* pShaderSource, const char* pFile, int line, MFShaderMacro* pMacros, void* *ppOutput, size_t* pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language = MFSL_Unknown);
extern (C) bool MFIntShader_CreateFromFile(MFShaderType shaderType, const char* pFilename, MFShaderMacro* pMacros, void* *ppOutput, size_t* pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language = MFSL_Unknown);
