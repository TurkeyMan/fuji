/**
 * @file MFMidi.h
 * @brief Provides access to MIDI I/O hardware.
 * @author Manu Evans
 * @defgroup MFMidi MIDI I/O access
 * @{
 */

#if !defined(_MFMIDI_H)
#define _MFMIDI_H

struct MFMidiInput;

enum MFMidiDeviceStatus
{
	MFMS_Unknown = -1,

	MFMS_Disconnected = 0,
	MFMS_Available,
	MFMS_Ready,
	MFMS_Active
};

enum MFMidiDataType
{
	MFMD_Note = 0,
	MFMD_Controller,
	MFMD_Program,		// program/patch
	MFMD_PitchWheel
};

struct MFMidiEvent
{
	uint32 timestamp;
	uint8 command;
	uint8 channel;
	uint8 data0;
	uint8 data1;
};

typedef void MFMidiEventCallback(MFMidiInput *pMidiInput, const MFMidiEvent *pEvent);


MF_API int MFMidi_GetNumDevices();
MF_API const char *MFMidi_GetDeviceName(int deviceId);

MF_API MFMidiDeviceStatus MFMidi_GetStatus(int deviceId);

MF_API MFMidiInput *MFMidi_OpenInput(int deviceId, bool bBuffered = false, MFMidiEventCallback *pEventCallback = NULL);
MF_API void MFMidi_CloseInput(MFMidiInput *pMidiInput);

MF_API uint32 MFMidi_GetState(MFMidiInput *pMidiInput, MFMidiDataType type, int channel, int note = 0);
MF_API uint32 MFMidi_WasPressed(MFMidiInput *pMidiInput, int channel, int note);
MF_API uint32 MFMidi_WasReleased(MFMidiInput *pMidiInput, int channel, int note);

MF_API bool MFMidi_Start(MFMidiInput *pMidiInput);
MF_API void MFMidi_Stop(MFMidiInput *pMidiInput);
MF_API size_t MFMidi_GetEvents(MFMidiInput *pMidiInput, MFMidiEvent *pEvents, size_t maxEvents, bool bPeek = false);

enum MFMidiNote
{
	MFMN_Unknown = 0xFF,

	MFMN_C0 = 0,
	MFMN_Cs0,
	MFMN_D0,
	MFMN_Ds0,
	MFMN_E0,
	MFMN_F0,
	MFMN_Fs0,
	MFMN_G0,
	MFMN_Gs0,
	MFMN_A0,
	MFMN_As0,
	MFMN_B0,
	MFMN_C1,
	MFMN_Cs1,
	MFMN_D1,
	MFMN_Ds1,
	MFMN_E1,
	MFMN_F1,
	MFMN_Fs1,
	MFMN_G1,
	MFMN_Gs1,
	MFMN_A1,
	MFMN_As1,
	MFMN_B1,
	MFMN_C2,
	MFMN_Cs2,
	MFMN_D2,
	MFMN_Ds2,
	MFMN_E2,
	MFMN_F2,
	MFMN_Fs2,
	MFMN_G2,
	MFMN_Gs2,
	MFMN_A2,
	MFMN_As2,
	MFMN_B2,
	MFMN_C3,
	MFMN_Cs3,
	MFMN_D3,
	MFMN_Ds3,
	MFMN_E3,
	MFMN_F3,
	MFMN_Fs3,
	MFMN_G3,
	MFMN_Gs3,
	MFMN_A3,
	MFMN_As3,
	MFMN_B3,
	MFMN_C4,
	MFMN_Cs4,
	MFMN_D4,
	MFMN_Ds4,
	MFMN_E4,
	MFMN_F4,
	MFMN_Fs4,
	MFMN_G4,
	MFMN_Gs4,
	MFMN_A4,
	MFMN_As4,
	MFMN_B4,
	MFMN_C5,
	MFMN_Cs5,
	MFMN_D5,
	MFMN_Ds5,
	MFMN_E5,
	MFMN_F5,
	MFMN_Fs5,
	MFMN_G5,
	MFMN_Gs5,
	MFMN_A5,
	MFMN_As5,
	MFMN_B5,
	MFMN_C6,
	MFMN_Cs6,
	MFMN_D6,
	MFMN_Ds6,
	MFMN_E6,
	MFMN_F6,
	MFMN_Fs6,
	MFMN_G6,
	MFMN_Gs6,
	MFMN_A6,
	MFMN_As6,
	MFMN_B6,
	MFMN_C7,
	MFMN_Cs7,
	MFMN_D7,
	MFMN_Ds7,
	MFMN_E7,
	MFMN_F7,
	MFMN_Fs7,
	MFMN_G7,
	MFMN_Gs7,
	MFMN_A7,
	MFMN_As7,
	MFMN_B7,
	MFMN_C8,
	MFMN_Cs8,
	MFMN_D8,
	MFMN_Ds8,
	MFMN_E8,
	MFMN_F8,
	MFMN_Fs8,
	MFMN_G8,
	MFMN_Gs8,
	MFMN_A8,
	MFMN_As8,
	MFMN_B8,
	MFMN_C9,
	MFMN_Cs9,
	MFMN_D9,
	MFMN_Ds9,
	MFMN_E9,
	MFMN_F9,
	MFMN_Fs9,
	MFMN_G9,
	MFMN_Gs9,
	MFMN_A9,
	MFMN_As9,
	MFMN_B9,
	MFMN_C10,
	MFMN_Cs10,
	MFMN_D10,
	MFMN_Ds10,
	MFMN_E10,
	MFMN_F10,
	MFMN_Fs10,
	MFMN_G10
};

#endif // _MFMIDI_H

/** @} */
