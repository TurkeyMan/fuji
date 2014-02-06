#include "Fuji.h"

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

MF_API int MFMidi_GetNumDevices()
{
	return 0;
}

MF_API const char *MFMidi_GetDeviceName(int deviceId)
{
	return "Unavailable";
}

MF_API MFMidiDeviceStatus MFMidi_GetStatus(int deviceId)
{
	return MFMS_Disconnected;
}

MF_API MFMidiInput *MFMidi_OpenInput(int deviceId, bool bBuffered, MFMidiEventCallback *pEventCallback)
{
	return NULL;
}

MF_API void MFMidi_CloseInput(MFMidiInput *pMidiInput)
{
}

MF_API uint32 MFMidi_GetState(MFMidiInput *pMidiInput, MFMidiDataType type, int channel, int note)
{
	return 0;
}

MF_API uint32 MFMidi_WasPressed(MFMidiInput *pMidiInput, int channel, int note)
{
	return 0;
}

MF_API uint32 MFMidi_WasReleased(MFMidiInput *pMidiInput, int channel, int note)
{
	return 0;
}

MF_API bool MFMidi_Start(MFMidiInput *pMidiInput)
{
	return false;
}

MF_API void MFMidi_Stop(MFMidiInput *pMidiInput)
{
}

MF_API size_t MFMidi_GetEvents(MFMidiInput *pMidiInput, MFMidiEvent *pEvents, size_t maxEvents, bool bPeek)
{
	return 0;
}

#endif // MF_MIDI
