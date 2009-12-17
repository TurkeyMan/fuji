#if !defined(_MFRENDERER_INTERNAL_H)
#define _MFRENDERER_INTERNAL_H

#include "MFRenderer.h"

// init/deinit
void MFRenderer_InitModule();
void MFRenderer_DeinitModule();

void MFRenderer_InitModulePlatformSpecific();
void MFRenderer_DeinitModulePlatformSpecific();

void MFRenderer_BeginFrame();
void MFRenderer_EndFrame();

int MFRenderer_CreateDisplay();
void MFRenderer_DestroyDisplay();
void MFRenderer_ResetDisplay();

#endif
