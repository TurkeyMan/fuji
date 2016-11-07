/**** Includes ****/

#include "Fuji_Internal.h"

#include "MFCallstack_Internal.h"
#include "MFHeap_Internal.h"
#include "MFSystem_Internal.h"
#include "MFWindow_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFResource_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFRenderTarget_Internal.h"
#include "MFModel_Internal.h"
#include "MFAnimation_Internal.h"
#include "MFInput_Internal.h"
#include "MFView_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFFont_Internal.h"
#include "MFPrimitive_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFRenderState_Internal.h"
#include "MFShader_Internal.h"
#include "MFEffect_Internal.h"
#include "MFSound_Internal.h"
#include "MFMidi_Internal.h"
#include "MFSockets_Internal.h"
#include "MFNetwork_Internal.h"
#include "MFScript_Internal.h"
#include "MFCollision_Internal.h"
#include "MFAnimScript_Internal.h"
#include "MFVertex_Internal.h"
#include "MFThread_Internal.h"
#include "MFCompute_Internal.h"
#include "MFDevice_Internal.h"
#include "DebugMenu.h"
#include "Timer.h"

//HACK:
MFInitStatus Timer_InitModule(int moduleId, bool bPerformInitialisation);
void Timer_DeinitModule();


MFInitStatus MFUtil_InitModule(int moduleId, bool bPerformInitialisation);

MFInitStatus MFString_InitModule(int moduleId, bool bPerformInitialisation);
void MFString_DeinitModule();

MFInitStatus Timer_InitModule(int moduleId, bool bPerformInitialisation);
void Timer_DeinitModule();

uint64 MFFileSystem_RegisterFilesystemModules(int filesystemModule);


/**** Functions ****/

MF_API int MFModule_RegisterModule(const char *pModuleName, MFInitCallback *pInitFunction, MFDeinitCallback *pDeinitFunction, uint64 prerequisites)
{
	MFDebug_Assert(gpEngineInstance->numModules < MFEngineInstance::MaxModules, "Maximum modules!");

	int id = gpEngineInstance->numModules++;
	gpEngineInstance->modules[id].pModuleName = pModuleName;
	gpEngineInstance->modules[id].pInitFunction = pInitFunction;
	gpEngineInstance->modules[id].pDeinitFunction = pDeinitFunction;
	gpEngineInstance->modules[id].prerequisites = prerequisites;
	return id;
}

MF_API int MFModule_GetNumModules()
{
	return gpEngineInstance->numModules;
}

MF_API const char *MFModule_GetModuleName(int id)
{
	return gpEngineInstance->modules[id].pModuleName;
}

MF_API int MFModule_GetModuleID(const char *pName)
{
	for(int a=0; a<gpEngineInstance->numModules; ++a)
	{
		if(!MFString_Compare(pName, gpEngineInstance->modules[a].pModuleName))
			return a;
	}
	return -1;
}

MF_API bool MFModule_IsModuleInitialised(int id)
{
	return (gpEngineInstance->moduleInitComplete & ~gpEngineInstance->moduleInitFailed & (1ULL << id)) != 0;
}

MF_API bool MFModule_DidModuleInitialisationFail(int id)
{
	return (gpEngineInstance->moduleInitFailed & (1ULL << id)) != 0;
}

MF_API uint64 MFModule_GetModuleMask(const char **ppModuleNames)
{
	uint64 mask = 0;
	for(int b=0; ppModuleNames[b]; ++b)
	{
		if(!MFString_Compare(ppModuleNames[b], "MFCore"))
		{
			mask |= gpEngineInstance->coreModules;
		}
		else
		{
			for(int a=0; a<gpEngineInstance->numModules; ++a)
			{
				if(!MFString_Compare(ppModuleNames[b], gpEngineInstance->modules[a].pModuleName))
					mask |= 1ULL << a;
			}
		}
	}
	return mask;
}

uint64 MFModule_RegisterCoreModules()
{
	gpEngineInstance->builtinModuleIDs[MFBIM_MFUtil] = (char)MFModule_RegisterModule("MFUtil", MFUtil_InitModule, NULL, 0);
	gpEngineInstance->coreModules = MFModule_GetBuiltinModuleMask(MFBIM_MFUtil);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFHeap] = (char)MFModule_RegisterModule("MFHeap", MFHeap_InitModule, MFHeap_DeinitModule, 0);
	uint64 heap = MFModule_GetBuiltinModuleMask(MFBIM_MFHeap);
	gpEngineInstance->coreModules |= heap;

	gpEngineInstance->builtinModuleIDs[MFBIM_MFThread] = (char)MFModule_RegisterModule("MFThread", MFThread_InitModule, MFThread_DeinitModule, heap);
	gpEngineInstance->coreModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFThread);
	gpEngineInstance->builtinModuleIDs[MFBIM_MFString] = (char)MFModule_RegisterModule("MFString", MFString_InitModule, MFString_DeinitModule, heap);
	gpEngineInstance->coreModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFString);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFSockets] = (char)MFModule_RegisterModule("MFSockets", MFSockets_InitModule, MFSockets_DeinitModule, heap);
	gpEngineInstance->coreModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSockets);

	int fs = MFModule_RegisterModule("MFFileSystem_pre-init", MFFileSystem_InitModule, MFFileSystem_DeinitModule, heap);
	gpEngineInstance->coreModules |= 1ULL << fs;

	// register the filesystems
	gpEngineInstance->coreModules |= MFFileSystem_RegisterFilesystemModules(fs);

	return gpEngineInstance->coreModules;
}

uint64 MFModule_RegisterEngineModules()
{
	uint64 modules = MFModule_RegisterCoreModules();

	uint64 basicModules = MFModule_GetBuiltinModuleMask(MFBIM_MFHeap) | MFModule_GetBuiltinModuleMask(MFBIM_MFString) | MFModule_GetBuiltinModuleMask(MFBIM_MFThread);

	gpEngineInstance->builtinModuleIDs[MFBIM_DebugMenu] = (char)MFModule_RegisterModule("DebugMenu", DebugMenu_InitModule, DebugMenu_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_DebugMenu);
	gpEngineInstance->builtinModuleIDs[MFBIM_MFCallstack] = (char)MFModule_RegisterModule("MFCallstack", MFCallstack_InitModule, MFCallstack_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFCallstack);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFDevice] = (char)MFModule_RegisterModule("MFDevice", MFDevice_InitModule, MFDevice_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFDevice);

	basicModules |= MFModule_GetBuiltinModuleMask(MFBIM_DebugMenu) | MFModule_GetBuiltinModuleMask(MFBIM_MFCallstack) | MFModule_GetBuiltinModuleMask(MFBIM_MFDevice);
	uint64 fs = MFModule_GetBuiltinModuleMask(MFBIM_MFFileSystem);
	uint64 net = MFModule_GetBuiltinModuleMask(MFBIM_MFSockets);

	gSystemTimer.Init(NULL);
	gpEngineInstance->timeDelta = gSystemTimer.TimeDeltaF();

	gpEngineInstance->builtinModuleIDs[MFBIM_Timer] = (char)MFModule_RegisterModule("Timer", Timer_InitModule, Timer_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_Timer);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFSystem] = (char)MFModule_RegisterModule("MFSystem", MFSystem_InitModule, MFSystem_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSystem);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFWindow] = (char)MFModule_RegisterModule("MFWindow", MFWindow_InitModule, MFWindow_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFWindow);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFResource] = (char)MFModule_RegisterModule("MFResource", MFResource_InitModule, MFResource_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFResource);

	basicModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSystem) | MFModule_GetBuiltinModuleMask(MFBIM_MFResource);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFView] = (char)MFModule_RegisterModule("MFView", MFView_InitModule, MFView_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFView);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFRenderTarget] = (char)MFModule_RegisterModule("MFRenderTarget", MFRenderTarget_InitModule, MFRenderTarget_DeinitModule, basicModules);
	uint64 renderTarget = MFModule_GetBuiltinModuleMask(MFBIM_MFRenderTarget);
	modules |= renderTarget;
	gpEngineInstance->builtinModuleIDs[MFBIM_MFRenderer] = (char)MFModule_RegisterModule("MFRenderer", MFRenderer_InitModule, MFRenderer_DeinitModule, renderTarget);
	uint64 renderer = MFModule_GetBuiltinModuleMask(MFBIM_MFRenderer);
	modules |= renderer;
	gpEngineInstance->builtinModuleIDs[MFBIM_MFDisplay] = (char)MFModule_RegisterModule("MFDisplay", MFDisplay_InitModule, MFDisplay_DeinitModule, renderer);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFDisplay);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFInput] = (char)MFModule_RegisterModule("MFInput", MFInput_InitModule, MFInput_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFInput);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFSound] = (char)MFModule_RegisterModule("MFSound", MFSound_InitModule, MFSound_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSound);
	gpEngineInstance->builtinModuleIDs[MFBIM_MFMidi] = (char)MFModule_RegisterModule("MFMidi", MFMidi_InitModule, MFMidi_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFMidi);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFRenderState] = (char)MFModule_RegisterModule("MFRenderState", MFRenderState_InitModule, MFRenderState_DeinitModule, renderer);
	uint64 renderState = MFModule_GetBuiltinModuleMask(MFBIM_MFRenderState);
	renderer |= renderState;
	modules |= renderState;

	gpEngineInstance->builtinModuleIDs[MFBIM_MFShader] = (char)MFModule_RegisterModule("MFShader", MFShader_InitModule, MFShader_DeinitModule, renderer);
	uint64 shader = MFModule_GetBuiltinModuleMask(MFBIM_MFShader);
	modules |= shader;
	gpEngineInstance->builtinModuleIDs[MFBIM_MFEffect] = (char)MFModule_RegisterModule("MFEffect", MFEffect_InitModule, MFEffect_DeinitModule, shader);
	uint64 effect = MFModule_GetBuiltinModuleMask(MFBIM_MFEffect);
	modules |= effect;

	gpEngineInstance->builtinModuleIDs[MFBIM_MFTexture] = (char)MFModule_RegisterModule("MFTexture", MFTexture_InitModule, MFTexture_DeinitModule, renderer);
	uint64 texture = MFModule_GetBuiltinModuleMask(MFBIM_MFTexture);
	modules |= texture;
	gpEngineInstance->builtinModuleIDs[MFBIM_MFMaterial] = (char)MFModule_RegisterModule("MFMaterial", MFMaterial_InitModule, MFMaterial_DeinitModule, texture);
	uint64 material = MFModule_GetBuiltinModuleMask(MFBIM_MFMaterial);
	modules |= material;

	gpEngineInstance->builtinModuleIDs[MFBIM_MFVertex] = (char)MFModule_RegisterModule("MFVertex", MFVertex_InitModule, MFVertex_DeinitModule, renderer);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFVertex);
	gpEngineInstance->builtinModuleIDs[MFBIM_MFModel] = (char)MFModule_RegisterModule("MFModel", MFModel_InitModule, MFModel_DeinitModule, material);
	uint64 model = MFModule_GetBuiltinModuleMask(MFBIM_MFModel);
	modules |= model;
	gpEngineInstance->builtinModuleIDs[MFBIM_MFAnimation] = (char)MFModule_RegisterModule("MFAnimation", MFAnimation_InitModule, MFAnimation_DeinitModule, model);
	uint64 animation = MFModule_GetBuiltinModuleMask(MFBIM_MFAnimation);
	modules |= animation;
	gpEngineInstance->builtinModuleIDs[MFBIM_MFAnimScript] = (char)MFModule_RegisterModule("MFAnimScript", MFAnimScript_InitModule, MFAnimScript_DeinitModule, animation);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFAnimScript);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFPrimitive] = (char)MFModule_RegisterModule("MFPrimitive", MFPrimitive_InitModule, MFPrimitive_DeinitModule, material);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFPrimitive);
	gpEngineInstance->builtinModuleIDs[MFBIM_MFFont] = (char)MFModule_RegisterModule("MFFont", MFFont_InitModule, MFFont_DeinitModule, material | fs);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFFont);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFCompute] = (char)MFModule_RegisterModule("MFCompute", MFCompute_InitModule, MFCompute_DeinitModule, renderer);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFCompute);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFCollision] = (char)MFModule_RegisterModule("MFCollision", MFCollision_InitModule, MFCollision_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFCollision);

	gpEngineInstance->builtinModuleIDs[MFBIM_MFNetwork] = (char)MFModule_RegisterModule("MFNetwork", MFNetwork_InitModule, MFNetwork_DeinitModule, basicModules | net);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFNetwork);

#if defined(_ENABLE_SCRIPTING)
	gpEngineInstance->builtinModuleIDs[MFBIM_MFScript] = (char)MFModule_RegisterModule("MFScript", MFScript_InitModule, MFScript_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFScript);
#endif

	void *pUserData;
	MFSystemCallbackFunction pCallback = MFSystem_GetSystemCallback(MFCB_RegisterModules, &pUserData);
	if(pCallback)
		pCallback(pUserData);

	return modules;
}

MF_API bool MFModule_InitModules()
{
	uint64 timer = 0;

	for(int a=0; a<gpEngineInstance->numModules; ++a)
	{
		uint64 bit = 1ULL << a;

		if(!(gpEngineInstance->moduleInitComplete & bit) && (gpEngineInstance->moduleInitComplete & gpEngineInstance->modules[a].prerequisites) == gpEngineInstance->modules[a].prerequisites)
		{
			MFInitStatus complete = MFIS_Failed;
			if((gpEngineInstance->modules[a].prerequisites & gpEngineInstance->moduleInitFailed) == 0)
			{
				MFDebug_Message(MFStr("Init %s...", gpEngineInstance->modules[a].pModuleName));

				timer = MFSystem_ReadRTC();
				complete = gpEngineInstance->modules[a].pInitFunction(a, true);
			}
			else
			{
				// TODO: list pre-requisite failures
				if(MFModule_IsModuleInitialised(MFModule_GetBuiltinModuleID(MFBIM_MFString)))
				{
					MFDebug_Message(MFStr("Prerequisite failure"));
				}
			}

			if(complete == MFIS_Succeeded)
			{
				uint64 initTime = (MFSystem_ReadRTC() - timer) * 1000 / MFSystem_GetRTCFrequency();

				gpEngineInstance->moduleInitComplete |= bit;

				// if logging is initialised
				MFDebug_Message(MFStr("Init %s complete in %dms", gpEngineInstance->modules[a].pModuleName, (int)initTime));
			}
			else if(complete == MFIS_Failed)
			{
				uint64 initTime = (MFSystem_ReadRTC() - timer) * 1000 / MFSystem_GetRTCFrequency();

				gpEngineInstance->moduleInitComplete |= bit;
				gpEngineInstance->moduleInitFailed |= bit;

				// if logging is initialised
				MFDebug_Error(MFStr("Init %s FAILED in %dms!", gpEngineInstance->modules[a].pModuleName, (int)initTime));
			}
		}
	}

	if(gpEngineInstance->moduleInitComplete == (1ULL << gpEngineInstance->numModules) - 1)
	{
		gpEngineInstance->bIsInitialised = true;

		if(gpEngineInstance->moduleInitFailed)
		{
			MFDebug_Message("Fuji initialisation completed with errors...");

			// TODO: list the failed modules
			//...
		}
		else
		{
			MFDebug_Message("Fuji initialisation complete!");
		}

		MFHeap_Mark();

		// let the game perform any post-init work
		void *pUserData;
		MFSystemCallbackFunction pCallback = MFSystem_GetSystemCallback(MFCB_InitDone, &pUserData);
		if(pCallback)
			pCallback(pUserData);

		// init the timedelta to the moment after initialisation completes
		MFSystem_UpdateTimeDelta();
		return true;
	}

	return false;
}

MF_API bool MFModule_BindModules()
{
	for(int a=0; a<gpEngineInstance->numModules; ++a)
	{
		uint64 bit = 1ULL << a;

		if(gpEngineInstance->moduleInitComplete & bit)
			gpEngineInstance->modules[a].pInitFunction(a, false);
	}

	return true;
}

MF_API void MFModule_DeinitModules()
{
	// TODO: clean up...
}

int MFModule_GetBuiltinModuleID(MFBuiltinModule module)
{
	return gpEngineInstance->builtinModuleIDs[module];
}

uint64 MFModule_GetBuiltinModuleMask(MFBuiltinModule module)
{
	if(gpEngineInstance->builtinModuleIDs[module] >= 0)
		return 1ULL << gpEngineInstance->builtinModuleIDs[module];
	return 0;
}
