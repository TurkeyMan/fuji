#include "Fuji.h"
#include "MFEffect_Internal.h"
#include "Asset/MFIntEffect.h"
#include "MFModule.h"
#include "MFFileSystem.h"
#include "MFSystem.h"

#define ALLOW_LOAD_FROM_SOURCE_DATA

static void MFEffect_Destroy(MFResource *pRes)
{
	MFEffect *pEffect = (MFEffect*)pRes;

	MFEffect_DestroyPlatformSpecific(pEffect);

	for(int a=0; a<pEffect->numTechniques; ++a)
	{
		MFEffectTechnique &t = pEffect->pTechniques[a];
		for(int j=0; j<MFST_Max; ++j)
		{
			if(t.shaders[j].pShader)
				MFShader_Release(t.shaders[j].pShader);
		}
	}

	MFHeap_Free(pEffect);
}

MFInitStatus MFEffect_InitModule()
{
	MFRT_Effect = MFResource_Register("MFEffect", &MFEffect_Destroy);

	MFEffect_InitModulePlatformSpecific();

	return MFIS_Succeeded;
}

void MFEffect_DeinitModule()
{
	MFEffect_DeinitModulePlatformSpecific();
}

static MFEffect* MFEffect_Find(const char *pName)
{
	return (MFEffect*)MFResource_Find(MFUtil_HashString(pName) ^ 0xeffec7ef);
}

MF_API MFEffect* MFEffect_Create(const char *pFilename)
{
	MFEffect *pEffect = MFEffect_Find(pFilename);

	if(!pEffect)
	{
		size_t nameLen = pFilename ? MFString_Length(pFilename) + 1 : 0;
		size_t platformDataSize = MFEffect_PlatformDataSize();

		size_t fileSize;
		pEffect = (MFEffect*)MFFileSystem_Load(MFStr("%s.bfx", pFilename), &fileSize, platformDataSize + nameLen);
		if(!pEffect)
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
			// try to load from source data
			MFIntEffect *pIE = MFIntEffect_CreateFromSourceData(pFilename);
			if(pIE)
			{
				MFIntEffect_CreateRuntimeData(pIE, &pEffect, &fileSize, MFSystem_GetCurrentPlatform(), platformDataSize + nameLen);

				MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.bfx", pFilename), MFOF_Write | MFOF_Binary);
				if(pFile)
				{
					MFFile_Write(pFile, pEffect, fileSize, false);
					MFFile_Close(pFile);
				}

				MFIntEffect_Destroy(pIE);
			}
#endif

			if(!pEffect)
			{
				MFDebug_Warn(2, MFStr("Effect '%s' does not exist.", pFilename));
				return NULL;
			}
		}

		pEffect->pPlatformData = platformDataSize ? (char*)pEffect + fileSize : NULL;
		if(pFilename)
			pFilename = MFString_Copy((char*)pEffect + fileSize + platformDataSize, pFilename);

		// fix up...
		if(pEffect->pEffectName)
			(char*&)pEffect->pEffectName += (size_t)pEffect;
		(char*&)pEffect->pTechniques += (size_t)pEffect;
		for(int a=0; a<pEffect->numTechniques; ++a)
		{
			MFEffectTechnique &t = pEffect->pTechniques[a];
			if(t.pName)
				(char*&)t.pName += (size_t)pEffect;
			for(int j=0; j<MFST_Max; ++j)
			{
				if(t.shaders[j].pShaderSource)
					(char*&)t.shaders[j].pShaderSource += (size_t)pEffect;
				if(t.shaders[j].pShaderLanguage)
					(char*&)t.shaders[j].pShaderLanguage += (size_t)pEffect;
			}
			if(t.pMacros)
				(char*&)t.pMacros += (size_t)pEffect;
		}

		// create the shaders
		for(int a=0; a<pEffect->numTechniques; ++a)
		{
			MFEffectTechnique &t = pEffect->pTechniques[a];
			for(int j=0; j<MFST_Max; ++j)
			{
				if(t.shaders[j].pShaderSource)
				{
					if(t.shaders[j].bFromFile)
						t.shaders[j].pShader = MFShader_CreateFromFile((MFShaderType)j, t.shaders[j].pShaderSource, NULL);
					else
					{
						MFShaderLanguage language = MFSL_Unknown;
						if(t.shaders[j].pShaderLanguage)
						{
							if(!MFString_CaseCmp(t.shaders[j].pShaderLanguage, "hlsl"))
								language = MFSL_HLSL;
							else if(!MFString_CaseCmp(t.shaders[j].pShaderLanguage, "glsl"))
								language = MFSL_GLSL;
							else if(!MFString_CaseCmp(t.shaders[j].pShaderLanguage, "cg"))
								language = MFSL_Cg;
							else if(!MFString_CaseCmp(t.shaders[j].pShaderLanguage, "vsh"))
								language = MFSL_VSAsm;
							else if(!MFString_CaseCmp(t.shaders[j].pShaderLanguage, "psh"))
								language = MFSL_PSAsm;
						}

						static const char *sShaderNames[MFST_Max] =
						{
							"VertexShader",
							"PixelShader",		// Fragment shader
							"GeometryShader",
							"DomainShader",		// Evaluation shader
							"HullShader",		// Control shader
							"ComputeShader"
						};
						const char *pName = MFStr("%s:%s_%s", pEffect->pEffectName ? pEffect->pEffectName : "Unnamed", t.pName ? t.pName : MFStr("Technique%d", a), sShaderNames[j]);
						t.shaders[j].pShader = MFShader_CreateFromString((MFShaderType)j, t.shaders[j].pShaderSource, NULL, pName, pFilename, t.shaders[j].startLine, language);
					}

					if(!t.shaders[j].pShader)
					{
						// default shader?
					}
					if(t.shaders[j].pShader)
					{
						for(int rs=0; rs<MFSB_CT_TypeCount; ++rs)
							t.renderStateRequirements[rs] |= t.shaders[j].pShader->renderStateRequirements[rs];
					}
				}
			}
		}

		// initialise for platform
		if(MFEffect_CreatePlatformSpecific(pEffect))
		{
			MFResource_AddResource(pEffect, MFRT_Effect, MFUtil_HashString(pFilename) ^ 0xeffec7ef, pFilename);
		}
		else
		{
			for(int a=0; a<pEffect->numTechniques; ++a)
			{
				MFEffectTechnique &t = pEffect->pTechniques[a];
				for(int j=0; j<MFST_Max; ++j)
				{
					if(t.shaders[j].pShader)
						MFShader_Release(t.shaders[j].pShader);
				}
			}

			MFHeap_Free(pEffect);
			pEffect = NULL;
		}
	}

	return pEffect;
}

MF_API int MFEffect_Release(MFEffect *pEffect)
{
	return MFResource_Release(pEffect);
}

MFEffectTechnique *MFEffect_GetTechnique(MFEffect *pEffect, MFRendererState &state)
{
	for(int a=0; a<pEffect->numTechniques; ++a)
	{
		MFEffectTechnique &t = pEffect->pTechniques[a];
		if((t.bools & state.rsSet[MFSB_CT_Bool]) == t.bools && (t.bools & state.bools) == t.boolValue)
			return &t;
	}
	return NULL;
}
