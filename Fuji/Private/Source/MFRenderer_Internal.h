#if !defined(_MFRENDERER_INTERNAL_H)
#define _MFRENDERER_INTERNAL_H

#include "MFRenderer.h"

// init/deinit
void MFRenderer_InitModule();
void MFRenderer_DeinitModule();

enum MFRenderState
{
	MFRS_MaterialOverride,
	MFRS_NoZRead,
	MFRS_NoZWrite,
	MFRS_Untextured,
	MFRS_PreLit,
	MFRS_PrimType,

	MFRS_ShowZBuffer,
	MFRS_ShowOverDraw,

	MFRS_Max,
	MFRS_ForceInt = 0x7FFFFFFF
};

#endif
