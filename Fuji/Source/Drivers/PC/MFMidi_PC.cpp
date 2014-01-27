#include "Fuji.h"

#if MF_MIDI == MF_DRIVER_PC

#include "MFMidi_Internal.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm")

void MFMidi_InitModulePlatformSpecific()
{
}

void MFMidi_DeinitModulePlatformSpecific()
{
}

#endif // MF_MIDI
