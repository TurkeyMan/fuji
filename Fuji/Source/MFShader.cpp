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

static void MFShader_FindConstants(MFShader *pShader)
{
	for(int a=0; a<pShader->numInputs; ++a)
	{
		for(int b=0; b<MFSB_CT_TypeCount; ++b)
		{
			int rsCount = MFStateBlock_GetNumRenderStates((MFStateBlockConstantType)b);
			for(int c=0; c<rsCount; ++c)
			{
				const char *pName = MFStateBlock_GetRenderStateName((MFStateBlockConstantType)b, c);
				if(!MFString_Compare(pShader->pInputs[a].pName, pName))
				{
					pShader->renderStateRequirements[b] |= 1 << c;
					c = rsCount;
					b = MFSB_CT_TypeCount;
				}
			}
		}
	}
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

		if(!MFShader_CreatePlatformSpecific(pShader, pMacros, pFilename, NULL))
		{
			MFHeap_Free(pShader);
			return NULL;
		}

		MFShader_FindConstants(pShader);
	}

	return pShader;
}

MF_API MFShader* MFShader_CreateFromString(MFShaderType type, const char *pShaderSource, MFShaderMacro *pMacros, const char *pName, const char *pFilename, int startingLine)
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

		if(!MFShader_CreatePlatformSpecific(pShader, pMacros, pFilename, pShaderSource))
		{
			MFHeap_Free(pShader);
			return NULL;
		}

		MFShader_FindConstants(pShader);
	}

	return pShader;
}

MF_API MFShader* MFShader_CreateFromBinary(MFShaderType type, void *pShaderProgram, size_t bytes, const char *pName)
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

		if(!MFShader_CreatePlatformSpecific(pShader, NULL, NULL, NULL))
		{
			MFHeap_Free(pShader);
			return NULL;
		}

		MFShader_FindConstants(pShader);
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

		if(!MFShader_CreatePlatformSpecific(pShader, NULL, NULL, NULL))
		{
			MFHeap_Free(pShader);
			return NULL;
		}

		MFShader_FindConstants(pShader);
	}

	return pShader;
}

MF_API int MFShader_Release(MFShader *pShader)
{
	return MFResource_Release(pShader);
}
