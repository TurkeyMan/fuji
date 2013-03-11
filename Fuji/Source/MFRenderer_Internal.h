#pragma once
#if !defined(_MFRENDERER_INTERNAL_H)
#define _MFRENDERER_INTERNAL_H

#include "MFRenderer.h"

// init/deinit
MFInitStatus MFRenderer_InitModule();
void MFRenderer_DeinitModule();

void MFRenderer_InitModulePlatformSpecific();
void MFRenderer_DeinitModulePlatformSpecific();

bool MFRenderer_BeginFrame();
void MFRenderer_EndFrame();

int MFRenderer_CreateDisplay();
void MFRenderer_DestroyDisplay();
void MFRenderer_ResetDisplay();
bool MFRenderer_SetDisplayMode(int width, int height, bool bFullscreen);

#endif
