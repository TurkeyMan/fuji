#if !defined(_MFMODULE_INTERNAL_H)
#define _MFMODULE_INTERNAL_H

#include "MFModule.h"

/**
 * Module init completion status.
 * Module initialisation completion status.
 */
enum MFBuiltinModule
{
	MFBIM_MFUtil = 0,
	MFBIM_MFHeap,
	MFBIM_MFThread,
	MFBIM_MFString,
	MFBIM_MFSockets,
	MFBIM_MFFileSystem,
	MFBIM_DebugMenu,
	MFBIM_MFCallstack,
	MFBIM_Timer,
	MFBIM_MFSystem,
	MFBIM_MFResource,
	MFBIM_MFView,
	MFBIM_MFRenderer,
	MFBIM_MFDisplay,
	MFBIM_MFRenderState,
	MFBIM_MFInput,
	MFBIM_MFSound,
	MFBIM_MFTexture,
	MFBIM_MFMaterial,
	MFBIM_MFVertex,
	MFBIM_MFModel,
	MFBIM_MFAnimation,
	MFBIM_MFAnimScript,
	MFBIM_MFPrimitive,
	MFBIM_MFFont,
	MFBIM_MFCompute,
	MFBIM_MFCollision,
	MFBIM_MFNetwork,
	MFBIM_MFScript,

	MFBIM_Max,					/**< Force enum to int type. */
	MFBIM_ForceInt = 0x7FFFFFFF	/**< Force enum to int type. */
};


extern char gBuiltinModuleIDs[MFBIM_Max];

__forceinline int MFModule_GetBuiltinModuleID(MFBuiltinModule module)
{
	return gBuiltinModuleIDs[module];
}

__forceinline uint64 MFModule_GetBuiltinModuleMask(MFBuiltinModule module)
{
	if(gBuiltinModuleIDs[module] >= 0)
		return 1ULL << gBuiltinModuleIDs[module];
	return 0;
}

#endif
