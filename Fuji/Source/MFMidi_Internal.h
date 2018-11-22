#if !defined(_MFMIDI_INTERNAL_H)
#define _MFMIDI_INTERNAL_H

#include "MFMidi.h"

MFInitStatus MFMidi_InitModule(int moduleId, bool bPerformInitialisation);
void MFMidi_InitModulePlatformSpecific();
void MFMidi_DeinitModule();
void MFMidi_DeinitModulePlatformSpecific();
void MFMidi_UpdateInternal();
void MFMidi_Update();

#endif
