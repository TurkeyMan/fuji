#pragma once
#if !defined(_FUJI_INTERNAL_H)
#define _FUJI_INTERNAL_H

#include "Fuji.h"
#include "MFModule_Internal.h"
#include "MFSystem.h"

#define ALLOC_MODULE_DATA(Struct)										\
	Struct *pModuleData = (Struct*)MFHeap_SystemMalloc(sizeof(Struct));	\
	MFZeroMemory(pModuleData, sizeof(Struct));							\
	pModuleData = new(pModuleData) Struct;								\
	gpEngineInstance->modules[moduleId].pModuleData = pModuleData;

#define GET_MODULE_DATA(Struct) \
	Struct *pModuleData = (Struct*)gpEngineInstance->modules[gModuleId].pModuleData;

#define GET_MODULE_DATA_ID(Struct, id) \
	Struct *pModuleData = (Struct*)gpEngineInstance->modules[id].pModuleData;

struct MFEngineInstance
{
	struct MFModule
	{
		void *pModuleData;

		const char *pModuleName;

		MFInitCallback *pInitFunction;
		MFDeinitCallback *pDeinitFunction;

		uint64 prerequisites;
	};

	static const int MaxModules = 64;

	bool bIsInitialised;

	MFPlatform currentPlatform;

	int bQuit;
	int bRestart;

	uint32 frameCount;
	float timeDelta;

	bool bDrawSystemInfo;

	MFModule modules[MaxModules];
	int numModules;

	uint64 moduleInitComplete;
	uint64 moduleInitFailed;

	uint64 coreModules;

	char builtinModuleIDs[MFBIM_Max];

	MFSystemCallbackFunction pSystemCallbacks[MFCB_Max];
};

extern "C"
{
	extern MFEngineInstance *gpEngineInstance;
}


__forceinline float MFTimeDelta()
{
	return gpEngineInstance->timeDelta;
}

__forceinline uint32 MFFrameCounter()
{
	return gpEngineInstance->frameCount;
}

#endif
