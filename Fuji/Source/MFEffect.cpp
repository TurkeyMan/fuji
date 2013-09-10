#include "Fuji.h"
#include "MFEffect_Internal.h"
#include "MFModule.h"

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
