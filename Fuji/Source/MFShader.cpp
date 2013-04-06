#include "Fuji.h"
#include "MFShader_Internal.h"
#include "MFModule.h"

MFInitStatus MFShader_InitModule()
{
	MFShader_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFShader_DeinitModule()
{
	MFShader_DeinitModulePlatformSpecific();
}

MFShader* MFShader_CreateFromFile(MFShaderType type, void *pFilename)
{
	return NULL;
}

MFShader* MFShader_CreateFromString(MFShaderType type, const char *pShader, const char *pName)
{
	return NULL;
}

MFShader* MFShader_CreateFromBinary(MFShaderType type, void *pShader, size_t bytes, const char *pName)
{
	return NULL;
}

MFShader* MFShader_CreateFromCallbacks(MFShaderType type, MFShader_ConfigureCallback *pConfigureFunc, MFShader_ExecuteCallback *pExecuteFunc, const char *pName)
{
	return NULL;
}

void MFShader_Destroy(MFShader *pShader)
{

}
