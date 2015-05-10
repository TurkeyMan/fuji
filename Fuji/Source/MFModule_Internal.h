#pragma once
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
	MFBIM_MFDevice,
	MFBIM_Timer,
	MFBIM_MFSystem,
	MFBIM_MFResource,
	MFBIM_MFView,
	MFBIM_MFWindow,
	MFBIM_MFRenderer,
	MFBIM_MFDisplay,
	MFBIM_MFRenderState,
	MFBIM_MFShader,
	MFBIM_MFEffect,
	MFBIM_MFInput,
	MFBIM_MFMidi,
	MFBIM_MFSound,
	MFBIM_MFTexture,
	MFBIM_MFMaterial,
	MFBIM_MFRenderTarget,
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

	MFBIM_Max
};

int MFModule_GetBuiltinModuleID(MFBuiltinModule module);
uint64 MFModule_GetBuiltinModuleMask(MFBuiltinModule module);

#endif
