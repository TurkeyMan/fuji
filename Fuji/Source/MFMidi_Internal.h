#if !defined(_MFMIDI_INTERNAL_H)
#define _MFMIDI_INTERNAL_H

#include "MFMidi.h"

MFInitStatus MFMidi_InitModule();
void MFMidi_InitModulePlatformSpecific();
void MFMidi_DeinitModule();
void MFMidi_DeinitModulePlatformSpecific();

#endif
