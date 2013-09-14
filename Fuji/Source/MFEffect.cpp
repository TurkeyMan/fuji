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
		int nameLen = pFilename ? MFString_Length(pFilename) + 1 : 0;

		size_t fileSize;
		pEffect = (MFEffect*)MFFileSystem_Load(MFStr("%s.bfx", pFilename), &fileSize);
		if(!pEffect)
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
			// try to load from source data
			MFIntEffect *pIE = MFIntEffect_CreateFromSourceData(pFilename);
			if(pIE)
			{
				size_t size;
				MFIntEffect_CreateRuntimeData(pIE, &pEffect, &size, MFSystem_GetCurrentPlatform());

				MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.bfx", pFilename), MFOF_Write | MFOF_Binary);
				if(pFile)
				{
					MFFile_Write(pFile, pEffect, size, false);
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

		pEffect = (MFEffect*)MFHeap_AllocAndZero(sizeof(MFEffect) + nameLen);

		if(pFilename)
			pFilename = MFString_Copy((char*)&pEffect[1], pFilename);

		if(MFEffect_CreatePlatformSpecific(pEffect))
		{
			MFResource_AddResource(pEffect, MFRT_Effect, MFUtil_HashString(pFilename) ^ 0xeffec7ef, pFilename);
		}
		else
		{
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
