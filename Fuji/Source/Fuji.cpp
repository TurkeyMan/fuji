#include "Fuji_Internal.h"
#include "MFHeap_Internal.h"

extern "C" MFEngineInstance *gpEngineInstance = NULL;

MF_API MFEngineInstance * Fuji_CreateEngineInstance()
{
	MFEngineInstance *pEngineInstance = (MFEngineInstance*)MFHeap_SystemMalloc(sizeof(MFEngineInstance));
	MFZeroMemory(pEngineInstance, sizeof(MFEngineInstance));

	pEngineInstance->bRestart = true;

	if(!gpEngineInstance)
		gpEngineInstance = pEngineInstance;

	return pEngineInstance;
}

MF_API void Fuji_DestroyEngineInstance(MFEngineInstance *pEngineInstance)
{
	if(!pEngineInstance || gpEngineInstance == pEngineInstance)
	{
		pEngineInstance = gpEngineInstance;
		gpEngineInstance = NULL;
	}

	MFHeap_SystemFree(pEngineInstance);
}

MF_API MFEngineInstance * Fuji_GetCurrentEngineInstance()
{
	return gpEngineInstance;
}

MF_API MFEngineInstance * Fuji_SetCurrentEngineInstance(MFEngineInstance *pEngineInstance)
{
	MFEngineInstance *pOldInstance = gpEngineInstance;
	gpEngineInstance = pEngineInstance;
	return pOldInstance;
}
