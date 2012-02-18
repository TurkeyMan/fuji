/**** Includes ****/

#include "Fuji.h"

#include "MFModule_Internal.h"

#include "MFCallstack_Internal.h"
#include "MFHeap_Internal.h"
#include "MFSystem_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFModel_Internal.h"
#include "MFAnimation_Internal.h"
#include "MFInput_Internal.h"
#include "MFView_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFFont_Internal.h"
#include "MFPrimitive_Internal.h"
#include "DebugMenu.h"
#include "Timer.h"
#include "MFFont.h"
#include "MFRenderer_Internal.h"
#include "MFSound_Internal.h"
#include "MFSockets_Internal.h"
#include "MFNetwork_Internal.h"
#include "MFScript_Internal.h"
#include "MFCollision_Internal.h"
#include "MFAnimScript_Internal.h"
#include "MFVertex_Internal.h"
#include "MFThread_Internal.h"
#include "MFCompute_Internal.h"


MFInitStatus MFString_InitModule();
void MFString_DeinitModule();

uint64 MFFileSystem_RegisterFilesystemModules(int filesystemModule);


/**** Globals ****/

struct MFModule
{
	const char *pModuleName;
	MFInitCallback *pInitFunction;
	MFDeinitCallback *pDeinitFunction;
	uint64 prerequisites;
};

static const int MaxModules = 64;
static MFModule gModules[MaxModules];
static int gNumModules = 0;
static uint64 gModuleInitComplete = 0;
static uint64 gModuleInitFailed = 0;

static uint64 gCoreModules = 0;

bool gFujiInitialised = false;

char gBuiltinModuleIDs[MFBIM_Max] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };

extern MFSystemCallbackFunction pSystemCallbacks[MFCB_Max];


/**** Functions ****/

int MFModule_RegisterModule(const char *pModuleName, MFInitCallback *pInitFunction, MFDeinitCallback *pDeinitFunction, uint64 prerequisites)
{
	MFDebug_Assert(gNumModules < MaxModules, "Maximum modules!");

	int id = gNumModules++;
	gModules[id].pModuleName = pModuleName;
	gModules[id].pInitFunction = pInitFunction;
	gModules[id].pDeinitFunction = pDeinitFunction;
	gModules[id].prerequisites = prerequisites;
	return id;
}

int MFModule_GetNumModules()
{
	return gNumModules;
}

const char *MFModule_GetModuleName(int id)
{
	return gModules[id].pModuleName;
}

int MFModule_GetModuleID(const char *pName)
{
	for(int a=0; a<gNumModules; ++a)
	{
		if(!MFString_Compare(pName, gModules[a].pModuleName))
			return a;
	}
	return -1;
}

bool MFModule_IsModuleInitialised(int id)
{
	return (gModuleInitComplete & (1ULL << id)) != 0;
}

bool MFModule_DidModuleInitialisationFail(int id)
{
	return (gModuleInitFailed & (1ULL << id)) != 0;
}

uint64 MFModule_GetModuleMask(const char **ppModuleNames)
{
	uint64 mask = 0;
	for(int b=0; ppModuleNames[b]; ++b)
	{
		if(!MFString_Compare(ppModuleNames[b], "MFCore"))
		{
			mask |= gCoreModules;
		}
		else
		{
			for(int a=0; a<gNumModules; ++a)
			{
				if(!MFString_Compare(ppModuleNames[b], gModules[a].pModuleName))
					mask |= 1ULL << a;
			}
		}
	}
	return mask;
}

uint64 MFModule_RegisterCoreModules()
{
	gBuiltinModuleIDs[MFBIM_MFUtil] = (char)MFModule_RegisterModule("MFUtil", MFUtil_InitModule, NULL, 0);
	gCoreModules = MFModule_GetBuiltinModuleMask(MFBIM_MFUtil);

	gBuiltinModuleIDs[MFBIM_MFHeap] = (char)MFModule_RegisterModule("MFHeap", MFHeap_InitModule, MFHeap_DeinitModule, 0);
	uint64 heap = MFModule_GetBuiltinModuleMask(MFBIM_MFHeap);
	gCoreModules |= heap;

	gBuiltinModuleIDs[MFBIM_MFThread] = (char)MFModule_RegisterModule("MFThread", MFThread_InitModule, MFThread_DeinitModule, heap);
	gCoreModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFThread);
	gBuiltinModuleIDs[MFBIM_MFString] = (char)MFModule_RegisterModule("MFString", MFString_InitModule, MFString_DeinitModule, heap);
	gCoreModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFString);

	gBuiltinModuleIDs[MFBIM_MFSockets] = (char)MFModule_RegisterModule("MFSockets", MFSockets_InitModule, MFSockets_DeinitModule, heap);
	gCoreModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSockets);

	int fs = MFModule_RegisterModule("MFFileSystem_pre-init", MFFileSystem_InitModule, MFFileSystem_DeinitModule, heap);
	gCoreModules |= 1ULL << fs;

	// register the filesystems
	gCoreModules |= MFFileSystem_RegisterFilesystemModules(fs);

	return gCoreModules;
}

uint64 MFModule_RegisterModules()
{
	uint64 modules = MFModule_RegisterCoreModules();

	uint64 basicModules = MFModule_GetBuiltinModuleMask(MFBIM_MFHeap) | MFModule_GetBuiltinModuleMask(MFBIM_MFString) | MFModule_GetBuiltinModuleMask(MFBIM_MFThread);

	gBuiltinModuleIDs[MFBIM_DebugMenu] = (char)MFModule_RegisterModule("DebugMenu", DebugMenu_InitModule, DebugMenu_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_DebugMenu);
	gBuiltinModuleIDs[MFBIM_MFCallstack] = (char)MFModule_RegisterModule("MFCallstack", MFCallstack_InitModule, MFCallstack_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFCallstack);

	basicModules |= MFModule_GetBuiltinModuleMask(MFBIM_DebugMenu) | MFModule_GetBuiltinModuleMask(MFBIM_MFCallstack);
	uint64 fs = MFModule_GetBuiltinModuleMask(MFBIM_MFFileSystem);
	uint64 net = MFModule_GetBuiltinModuleMask(MFBIM_MFSockets);

	gSystemTimer.Init(NULL);
	gSystemTimeDelta = gSystemTimer.TimeDeltaF();

	gBuiltinModuleIDs[MFBIM_Timer] = (char)MFModule_RegisterModule("Timer", Timer_InitModule, Timer_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_Timer);

	gBuiltinModuleIDs[MFBIM_MFSystem] = (char)MFModule_RegisterModule("MFSystem", MFSystem_InitModule, MFSystem_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSystem);

	basicModules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSystem);

	gBuiltinModuleIDs[MFBIM_MFView] = (char)MFModule_RegisterModule("MFView", MFView_InitModule, MFView_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFView);

	gBuiltinModuleIDs[MFBIM_MFRenderer] = (char)MFModule_RegisterModule("MFRenderer", MFRenderer_InitModule, MFRenderer_DeinitModule, basicModules);
	uint64 renderer = MFModule_GetBuiltinModuleMask(MFBIM_MFRenderer);
	modules |= renderer;
	gBuiltinModuleIDs[MFBIM_MFDisplay] = (char)MFModule_RegisterModule("MFDisplay", MFDisplay_InitModule, MFDisplay_DeinitModule, renderer);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFDisplay);

	gBuiltinModuleIDs[MFBIM_MFInput] = (char)MFModule_RegisterModule("MFInput", MFInput_InitModule, MFInput_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFInput);

	gBuiltinModuleIDs[MFBIM_MFSound] = (char)MFModule_RegisterModule("MFSound", MFSound_InitModule, MFSound_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFSound);

	gBuiltinModuleIDs[MFBIM_MFTexture] = (char)MFModule_RegisterModule("MFTexture", MFTexture_InitModule, MFTexture_DeinitModule, renderer);
	uint64 texture = MFModule_GetBuiltinModuleMask(MFBIM_MFTexture);
	modules |= texture;
	gBuiltinModuleIDs[MFBIM_MFMaterial] = (char)MFModule_RegisterModule("MFMaterial", MFMaterial_InitModule, MFMaterial_DeinitModule, texture);
	uint64 material = MFModule_GetBuiltinModuleMask(MFBIM_MFMaterial);
	modules |= material;

	gBuiltinModuleIDs[MFBIM_MFVertex] = (char)MFModule_RegisterModule("MFVertex", MFVertex_InitModule, MFVertex_DeinitModule, renderer);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFVertex);
	gBuiltinModuleIDs[MFBIM_MFModel] = (char)MFModule_RegisterModule("MFModel", MFModel_InitModule, MFModel_DeinitModule, material);
	uint64 model = MFModule_GetBuiltinModuleMask(MFBIM_MFModel);
	modules |= model;
	gBuiltinModuleIDs[MFBIM_MFAnimation] = (char)MFModule_RegisterModule("MFAnimation", MFAnimation_InitModule, MFAnimation_DeinitModule, model);
	uint64 animation = MFModule_GetBuiltinModuleMask(MFBIM_MFAnimation);
	modules |= animation;
	gBuiltinModuleIDs[MFBIM_MFAnimScript] = (char)MFModule_RegisterModule("MFAnimScript", MFAnimScript_InitModule, MFAnimScript_DeinitModule, animation);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFAnimScript);

	gBuiltinModuleIDs[MFBIM_MFPrimitive] = (char)MFModule_RegisterModule("MFPrimitive", MFPrimitive_InitModule, MFPrimitive_DeinitModule, material);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFPrimitive);
	gBuiltinModuleIDs[MFBIM_MFFont] = (char)MFModule_RegisterModule("MFFont", MFFont_InitModule, MFFont_DeinitModule, material | fs);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFFont);

	gBuiltinModuleIDs[MFBIM_MFCompute] = (char)MFModule_RegisterModule("MFCompute", MFCompute_InitModule, MFCompute_DeinitModule, renderer);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFCompute);

	gBuiltinModuleIDs[MFBIM_MFCollision] = (char)MFModule_RegisterModule("MFCollision", MFCollision_InitModule, MFCollision_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFCollision);

	gBuiltinModuleIDs[MFBIM_MFNetwork] = (char)MFModule_RegisterModule("MFNetwork", MFNetwork_InitModule, MFNetwork_DeinitModule, basicModules | net);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFNetwork);

#if defined(_ENABLE_SCRIPTING)
	gBuiltinModuleIDs[MFBIM_MFScript] = (char)MFModule_RegisterModule("MFScript", MFScript_InitModule, MFScript_DeinitModule, basicModules);
	modules |= MFModule_GetBuiltinModuleMask(MFBIM_MFScript);
#endif

	if(pSystemCallbacks[MFCB_RegisterModules])
		pSystemCallbacks[MFCB_RegisterModules]();

	return modules;
}

bool MFModule_InitModules()
{
	for(int a=0; a<gNumModules; ++a)
	{
		uint64 bit = 1 << a;

		if(!(gModuleInitComplete & bit) && (gModuleInitComplete & gModules[a].prerequisites) == gModules[a].prerequisites)
		{
			MFInitStatus complete = MFAIC_Failed;
			if((gModules[a].prerequisites & gModuleInitFailed) == 0)
			{
				MFDebug_Message(MFStr("Init %s...", gModules[a].pModuleName));

				complete = gModules[a].pInitFunction();
			}
			else
			{
				// list pre-requisite failures
				if(MFModule_IsModuleInitialised(MFModule_GetBuiltinModuleID(MFBIM_MFString)))
				{
					MFDebug_Message(MFStr("Prerequisite failure"));
				}
			}

			if(complete == MFAIC_Succeeded)
			{
				gModuleInitComplete |= bit;

				// if logging is initialised
				MFDebug_Message(MFStr("Init %s complete", gModules[a].pModuleName));
			}
			else if(complete == MFAIC_Failed)
			{
				gModuleInitComplete |= bit;
				gModuleInitFailed |= bit;

				// if logging is initialised
				MFDebug_Error(MFStr("Init %s FAILED!", gModules[a].pModuleName));
			}
		}
	}

MFDebug_Message("check finished");
	if(gModuleInitComplete == (1ULL << gNumModules) - 1)
	{
		gFujiInitialised = true;

		if(gModuleInitFailed)
		{
			MFDebug_Message("Fuji initialisation completed with errors...");

			// list the failed modules
			//...
		}
		else
		{
			MFDebug_Message("Fuji initialisation complete!");
		}

		MFHeap_Mark();

		// let the game perform any post-init work
		if(pSystemCallbacks[MFCB_InitDone])
			pSystemCallbacks[MFCB_InitDone]();

		// init the timedelta to the moment after initialisation completes
		MFSystem_UpdateTimeDelta();
		return true;
	}

	return false;
}
