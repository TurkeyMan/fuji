#include "Fuji.h"
#include "MFShader_Internal.h"
#include "MFModule.h"
#include "MFResource.h"

static void MFShader_Destroy(MFResource *pRes)
{
	MFShader *pShader = (MFShader*)pRes;

	MFShader_DestroyPlatformSpecific(pShader);

	if(pShader->pProgram)
		MFHeap_Free(pShader->pProgram);
	if(pShader->pInputs)
		MFHeap_Free(pShader->pInputs);
	MFHeap_Free(pShader);
}

MFInitStatus MFShader_InitModule()
{
	MFRT_Shader = MFResource_Register("MFShader", &MFShader_Destroy);

	MFShader_InitModulePlatformSpecific();

	return MFIS_Succeeded;
}

void MFShader_DeinitModule()
{
	MFShader_DeinitModulePlatformSpecific();
}

static MFShader* MFShader_Find(const char *pName)
{
	return (MFShader*)MFResource_Find(MFUtil_HashString(pName) ^ 0x5ade5ade);
}

MF_API MFShader* MFShader_CreateFromFile(MFShaderType type, const char *pFilename, MFShaderMacro *pMacros)
{
	MFShader *pShader = MFShader_Find(pFilename);

	if(!pShader)
	{
		int nameLen = pFilename ? MFString_Length(pFilename) + 1 : 0;
		pShader = (MFShader*)MFHeap_AllocAndZero(sizeof(MFShader) + nameLen);

		pShader->shaderType = type;

		if(pFilename)
			pFilename = MFString_Copy((char*)&pShader[1], pFilename);

		MFResource_AddResource(pShader, MFRT_Shader, MFUtil_HashString(pFilename) ^ 0x5ade5ade, pFilename);

		MFShader_CreatePlatformSpecific(pShader, pMacros, pFilename, NULL);
	}

	return pShader;
}

MF_API MFShader* MFShader_CreateFromString(MFShaderType type, const char *pShaderSource, MFShaderMacro *pMacros, const char *pName)
{
	MFShader *pShader = MFShader_Find(pName);

	if(!pShader)
	{
		int nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pShader = (MFShader*)MFHeap_AllocAndZero(sizeof(MFShader) + nameLen);

		pShader->shaderType = type;

		if(pName)
			pName = MFString_Copy((char*)&pShader[1], pName);

		MFResource_AddResource(pShader, MFRT_Shader, MFUtil_HashString(pName) ^ 0x5ade5ade, pName);

		MFShader_CreatePlatformSpecific(pShader, pMacros, NULL, pShaderSource);
	}

	return pShader;
}

MF_API MFShader* MFShader_CreateFromBinary(MFShaderType type, void *pShaderProgram, size_t bytes, MFShaderMacro *pMacros, const char *pName)
{
	MFShader *pShader = MFShader_Find(pName);

	if(!pShader)
	{
		int nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pShader = (MFShader*)MFHeap_AllocAndZero(sizeof(MFShader) + nameLen);

		pShader->shaderType = type;
		pShader->pProgram = pShaderProgram;
		pShader->bytes = bytes;

		if(pName)
			pName = MFString_Copy((char*)&pShader[1], pName);

		MFResource_AddResource(pShader, MFRT_Shader, MFUtil_HashString(pName) ^ 0x5ade5ade, pName);

		MFShader_CreatePlatformSpecific(pShader, pMacros, NULL, NULL);
	}

	return pShader;
}

MF_API MFShader* MFShader_CreateFromCallbacks(MFShaderType type, MFShader_ConfigureCallback *pConfigureFunc, MFShader_ExecuteCallback *pExecuteFunc, const char *pName)
{
	MFShader *pShader = MFShader_Find(pName);

	if(!pShader)
	{
		int nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pShader = (MFShader*)MFHeap_AllocAndZero(sizeof(MFShader) + nameLen);

		pShader->shaderType = type;
		pShader->pConfigure = pConfigureFunc;
		pShader->pExecute = pExecuteFunc;

		if(pName)
			pName = MFString_Copy((char*)&pShader[1], pName);

		MFResource_AddResource(pShader, MFRT_Shader, MFUtil_HashString(pName) ^ 0x5ade5ade, pName);

		MFShader_CreatePlatformSpecific(pShader, NULL, NULL, NULL);
	}

	return pShader;
}

MF_API int MFShader_Release(MFShader *pShader)
{
	return MFResource_Release(pShader);
}
