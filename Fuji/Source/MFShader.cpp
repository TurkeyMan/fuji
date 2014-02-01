#include "Fuji.h"
#include "MFShader_Internal.h"
#include "Asset/MFIntShader.h"
#include "MFModule.h"
#include "MFResource.h"
#include "MFFileSystem.h"
#include "MFRenderer.h"
#include "MFSystem.h"

#define ALLOW_LOAD_FROM_SOURCE_DATA

static void MFShader_Destroy(MFResource *pRes)
{
	MFShader *pShader = (MFShader*)pRes;

	MFShader_DestroyPlatformSpecific(pShader);

	MFHeap_Free(pShader->pTemplate);
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
	MFShaderTemplate *pTemplate = pShader->pTemplate;
	for(int a = 0; a<pTemplate->numInputs; ++a)
	{
		for(int b=0; b<MFSB_CT_TypeCount; ++b)
		{
			int rsCount = MFStateBlock_GetNumRenderStates((MFStateBlockConstantType)b);
			for(int c=0; c<rsCount; ++c)
			{
				const char *pName = MFStateBlock_GetRenderStateName((MFStateBlockConstantType)b, c);
				if(!MFString_Compare(pTemplate->pInputs[a].pName, pName))
				{
					pShader->renderStateRequirements[b] |= 1 << c;
					c = rsCount;
					b = MFSB_CT_TypeCount;
				}
			}
		}
	}

	pShader->renderStateRequirements[MFSB_CT_Texture] = (pShader->renderStateRequirements[MFSB_CT_RenderState] >> MFSCRS_DiffuseSamplerState) & (MFBIT(MFSCT_Max)-1);
}

MF_API MFShader* MFShader_CreateFromFile(MFShaderType type, const char *pFilename, MFShaderMacro *pMacros)
{
	MFShader *pShader = MFShader_Find(pFilename);

	if(!pShader)
	{
		size_t fileSize;

		const char *pFileName = MFStr("%s.fsh", pFilename);

		MFShaderTemplate *pTemplate = (MFShaderTemplate*)MFFileSystem_Load(pFileName, &fileSize);

		if(!pTemplate)
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
			// try to load from source data
			const char * const pExt[] = { ".hlsl", ".glsl", ".vsh", ".psh", NULL };
			const char * const *ppExt = pExt;
			while(*ppExt)
			{
				size_t size;
				if(MFIntShader_CreateFromFile(type, MFStr("%s%s", pFilename, *ppExt), pMacros, (void**)&pTemplate, &size, MFSystem_GetCurrentPlatform(), MFRenderer_GetCurrentRenderDriver()))
				{
					// cache the shader template
					MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.fsh", pFilename), MFOF_Write | MFOF_Binary);
					if(pFile)
					{
						MFFile_Write(pFile, pTemplate, size, false);
						MFFile_Close(pFile);
					}

					break;
				}
				++ppExt;
			}
#endif

			if(!pTemplate)
			{
				MFDebug_Warn(2, MFStr("Couldn't create shader '%s'", pFileName));
				return NULL;
			}
		}

		MFFixUp(pTemplate->pProgram, pTemplate, 1);
		MFFixUp(pTemplate->pInputs, pTemplate, 1);

		size_t nameLen = MFString_Length(pFilename) + 1;
		pShader = (MFShader*)MFHeap_AllocAndZero(sizeof(MFShader) + nameLen);

		pFilename = MFString_Copy((char*)&pShader[1], pFilename);

		MFResource_AddResource(pShader, MFRT_Shader, MFUtil_HashString(pFilename) ^ 0x5ade5ade, pFilename);

		pShader->pTemplate = pTemplate;

		MFShader_CreatePlatformSpecific(pShader);
		MFShader_FindConstants(pShader);
	}

	return pShader;
}

MF_API MFShader* MFShader_CreateFromString(MFShaderType type, const char *pShaderSource, MFShaderMacro *pMacros, const char *pName, const char *pFilename, int startingLine)
{
	MFShader *pShader = MFShader_Find(pName);

	if(!pShader)
	{
		MFShaderTemplate *pTemplate = (MFShaderTemplate*)MFFileSystem_Load(MFStr("%s.fsh", pName));
		if(!pTemplate)
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
			// try and compile the shader
			size_t size;
			if(!MFIntShader_CreateFromString(type, pShaderSource, pFilename, startingLine, pMacros, (void**)&pTemplate, &size, MFSystem_GetCurrentPlatform(), MFRenderer_GetCurrentRenderDriver()))
			{
				MFDebug_Warn(2, MFStr("Couldn't compile shader '%s'", pName));
				return NULL;
			}

			// cache the shader template
			MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.fsh", pName), MFOF_Write | MFOF_Binary);
			if(pFile)
			{
				MFFile_Write(pFile, pTemplate, size, false);
				MFFile_Close(pFile);
			}
#else
			MFDebug_Assert(false, "Can't compile shaders at runtime!");
			return NULL;
#endif
		}

		MFFixUp(pTemplate->pProgram, pTemplate, 1);
		MFFixUp(pTemplate->pInputs, pTemplate, 1);
		for(int i = 0; i < pTemplate->numInputs; ++i)
			MFFixUp(pTemplate->pInputs[i].pName, pTemplate, 1);

		size_t nameLen = MFString_Length(pName) + 1;
		pShader = (MFShader*)MFHeap_AllocAndZero(sizeof(MFShader)+nameLen);

		pName = MFString_Copy((char*)&pShader[1], pName);

		MFResource_AddResource(pShader, MFRT_Shader, MFUtil_HashString(pName) ^ 0x5ade5ade, pName);

		pShader->pTemplate = pTemplate;

		MFShader_CreatePlatformSpecific(pShader);
		MFShader_FindConstants(pShader);
	}

	return pShader;
}

MF_API MFShader* MFShader_CreateFromCallbacks(MFShaderType type, MFShader_ConfigureCallback *pConfigureFunc, MFShader_ExecuteCallback *pExecuteFunc, const char *pName)
{
	MFShader *pShader = MFShader_Find(pName);

	if(!pShader)
	{
		size_t nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pShader = (MFShader*)MFHeap_AllocAndZero(sizeof(MFShader) + nameLen);
		pShader->pTemplate = (MFShaderTemplate*)MFHeap_AllocAndZero(sizeof(MFShaderTemplate));

		pShader->pTemplate->shaderType = type;
		pShader->pConfigure = pConfigureFunc;
		pShader->pExecute = pExecuteFunc;

		if(pName)
			pName = MFString_Copy((char*)&pShader[1], pName);

		MFResource_AddResource(pShader, MFRT_Shader, MFUtil_HashString(pName) ^ 0x5ade5ade, pName);

		MFShader_CreatePlatformSpecific(pShader);
		MFShader_FindConstants(pShader);
	}

	return pShader;
}

MF_API int MFShader_Release(MFShader *pShader)
{
	return MFResource_Release(pShader);
}
