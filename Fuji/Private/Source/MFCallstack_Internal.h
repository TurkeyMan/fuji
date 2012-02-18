#if !defined(_MFCALLSTACK_INTERNAL_H)
#define _MFCALLSTACK_INTERNAL_H

#include "MFCallstack.h"

MFInitStatus MFCallstack_InitModule();
void MFCallstack_DeinitModule();

#if defined(_MFCALLSTACK_PROFILING)

void MFCallstack_BeginFrame();
void MFCallstack_EndFrame();
void MFCallstack_Draw();

#else

#define MFCallstack_BeginFrame()
#define MFCallstack_EndFrame()
#define MFCallstack_Draw()

#endif

#endif
