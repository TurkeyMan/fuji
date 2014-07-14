module fuji.c.Asset.MFIntShader;

import fuji.c.Fuji;
import fuji.c.MFDriver;
import fuji.c.MFShader;

nothrow:
@nogc:

extern (C) bool MFIntShader_CreateFromString(MFShaderType shaderType, const char* pShaderSource, const char* pFile, int line, MFShaderMacro* pMacros, void* *ppOutput, size_t* pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language = MFShaderLanguage.Unknown);
extern (C) bool MFIntShader_CreateFromFile(MFShaderType shaderType, const char* pFilename, MFShaderMacro* pMacros, void* *ppOutput, size_t* pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language = MFShaderLanguage.Unknown);
