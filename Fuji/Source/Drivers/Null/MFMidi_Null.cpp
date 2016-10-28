#include "Fuji_Internal.h"

#if MF_MIDI == MF_DRIVER_NULL

#include "MFMidi_Internal.h"

void MFMidi_InitModulePlatformSpecific()
{
}

void MFMidi_DeinitModulePlatformSpecific()
{
}

void MFMidi_UpdateInternal()
{
}

MF_API int MFMidi_GetTimestampFrequency()
{
	return 0;
}

MF_API bool MFMidi_OpenInput(MFDevice *pDevice, bool bBuffered, MFMidiEventCallback *pEventCallback)
{
	return false;
}

MF_API void MFMidi_CloseInput(MFDevice *pDevice)
{
}

MF_API uint32 MFMidi_GetState(MFDevice *pDevice, MFMidiDataType type, int channel, int note)
{
	return 0;
}

MF_API bool MFMidi_WasPressed(MFDevice *pDevice, int channel, int note)
{
	return 0;
}

MF_API bool MFMidi_WasReleased(MFDevice *pDevice, int channel, int note)
{
	return 0;
}

MF_API bool MFMidi_Start(MFDevice *pDevice)
{
	return false;
}

MF_API void MFMidi_Stop(MFDevice *pDevice)
{
}

MF_API size_t MFMidi_GetEvents(MFDevice *pDevice, MFMidiEvent *pEvents, size_t maxEvents, bool bPeek)
{
	return 0;
}

MF_API bool MFMidi_OpenOutput(MFDevice *pDevice)
{
	return false;
}

MF_API void MFMidi_CloseOutput(MFDevice *pDevice)
{
}

#endif // MF_MIDI
