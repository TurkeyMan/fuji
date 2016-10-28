/**
 * @file MFMidi.h
 * @brief Provides access to MIDI I/O hardware.
 * @author Manu Evans
 * @defgroup MFMidi MIDI I/O access
 * @{
 */

#if !defined(_MFMIDI_H)
#define _MFMIDI_H

#include "MFMidi_Tables.h"

struct MFDevice;

enum MFMidiDataType
{
	MFMD_Note = 0,
	MFMD_Controller,
	MFMD_Program,		// program/patch
	MFMD_PitchWheel
};

enum MFMidiEventType
{
	MFMET_NoteOff = 0,
	MFMET_NoteOn,
	MFMET_NoteAftertouch,
	MFMET_ControlChange,
	MFMET_ProgramChange,
	MFMET_ChannelAftertouch,
	MFMET_PitchBend,

	// system common messages
	MFMET_Sysex,
	MFMET_MidiTimeCodeQuarterFrame,
	MFMET_SongPositionPointer,
	MFMET_SongSelect,
	MFMET_TuneRequest,

	// system realtime messages
	MFMET_TimingClock,
	MFMET_Start,
	MFMET_Continue,
	MFMET_Stop,
	MFMET_ActiveSensing,
	MFMET_Reset,

	// channel mode messages
	MFMET_AllSoundOff,
	MFMET_ResetAllControls,
	MFMET_LocalControl,
	MFMET_AllNotesOff,
	MFMET_OmniModeOff,	// also causes AllNotesOff
	MFMET_OmniModeOn,	// also causes AllNotesOff
	MFMET_MonoModeOn,	// also causes PolyOff, AllNotesOff
	MFMET_PolyModeOn,	// also causes MonoOff, AllNotesOff

	// non-realtime universal exclusive messages
	MFMET_SampleDumpHeader,
	MFMET_SampleDataPacket,
	MFMET_SampleDumpRequest,
	MFMET_MidiTimeCode_Special,
	MFMET_MidiTimeCode_PinchInPoints,
	MFMET_MidiTimeCode_PunchOutPoints,
	MFMET_MidiTimeCode_DeletePunchInPoint,
	MFMET_MidiTimeCode_DeletePunchOutPoint,
	MFMET_MidiTimeCode_EventStartPoint,
	MFMET_MidiTimeCode_EventStopPoint,
	MFMET_MidiTimeCode_EventStartPointsWithAdditionalInfo,
	MFMET_MidiTimeCode_EventStopPointsWithAdditionalInfo,
	MFMET_MidiTimeCode_DeleteEventStartPoint,
	MFMET_MidiTimeCode_DeleteEventStopPoint,
	MFMET_MidiTimeCode_CuePoints,
	MFMET_MidiTimeCode_CuePointsWithAdditionalInfo,
	MFMET_MidiTimeCode_DeleteCuePoint,
	MFMET_MidiTimeCode_EventNameInAdditionalInfo,
	MFMET_SampleDumpExt_LoopPointsTransmission,
	MFMET_SampleDumpExt_LoopPointsRequest,
	MFMET_SampleDumpExt_SampleNameTransmission,
	MFMET_SampleDumpExt_SampleNameRequest,
	MFMET_SampleDumpExt_ExtendedDumpHeader,
	MFMET_SampleDumpExt_ExtendedLoopPointsTransmission,
	MFMET_SampleDumpExt_ExtendedLoopPointsRequest,
	MFMET_GeneralInformation_IdentityRequest,
	MFMET_GeneralInformation_IdentityReply,
	MFMET_FileDump_Header,
	MFMET_FileDump_DataPacket,
	MFMET_FileDump_Request,
	MFMET_MidiTuningStandard_NonRealtime_BulkDumpRequest,
	MFMET_MidiTuningStandard_NonRealtime_BulkDumpReply,
	MFMET_MidiTuningStandard_NonRealtime_TuningDumpRequest,
	MFMET_MidiTuningStandard_NonRealtime_KeyBasedTuningDump,
	MFMET_MidiTuningStandard_NonRealtime_ScaleOctaveTuningDump1,
	MFMET_MidiTuningStandard_NonRealtime_ScaleOctaveTuningDump2,
	MFMET_MidiTuningStandard_NonRealtime_SingleNoteTuningChangeWithBankSelect,
	MFMET_MidiTuningStandard_NonRealtime_ScaleOctaveTuning1,
	MFMET_MidiTuningStandard_NonRealtime_ScaleOctaveTuning2,
	MFMET_GeneralMidi_GeneralMidi1SystemOn,
	MFMET_GeneralMidi_GeneralMidiSystemOff,
	MFMET_GeneralMidi_GeneralMidi2SystemOn,
	MFMET_DownloadableSounds_TurnDlsOn,
	MFMET_DownloadableSounds_TurnDlsOff,
	MFMET_DownloadableSounds_TurnDlsVoiceAllocationOff,
	MFMET_DownloadableSounds_TurnDlsVoiceAllocationOn,
	MFMET_FileReferenceMessage_OpenFile,
	MFMET_FileReferenceMessage_SelectOrReselectContents,
	MFMET_FileReferenceMessage_OpenFileAndSelectContents,
	MFMET_FileReferenceMessage_CloseFile,
	MFMET_MidiVisualControl_,
	MFMET_EndOfFile,
	MFMET_Wait,
	MFMET_Cancel,
	MFMET_Nak,
	MFMET_Ack,

	// realtime universal exclusive messages
	MFMET_MidiTimeCode_FullMessage,
	MFMET_MidiTimeCode_UserBits,
	MFMET_MidiShowControl_MscExtensions,
	MFMET_MidiShowControl_MscCommands,
	MFMET_NotificationInformation_BarNumber,
	MFMET_NotificationInformation_TimeSignatureImmediate,
	MFMET_NotificationInformation_TimeSignatureDelayed,
	MFMET_DeviceControl_MasterVolume,
	MFMET_DeviceControl_MasterBalance,
	MFMET_DeviceControl_MasterFineTuning,
	MFMET_DeviceControl_MasterCoarseTuning,
	MFMET_DeviceControl_GlobalParameterControl,
	MFMET_RealTimeMtcCueing_Special,
	MFMET_RealTimeMtcCueing_PunchInPoints,
	MFMET_RealTimeMtcCueing_PunchOutPoints,
	MFMET_RealTimeMtcCueing_EventStartPoints,
	MFMET_RealTimeMtcCueing_EventStopPoints,
	MFMET_RealTimeMtcCueing_EventStartPointsWithAdditionalInfo,
	MFMET_RealTimeMtcCueing_EventStopPointsWithAdditionalInfo,
	MFMET_RealTimeMtcCueing_CuePoints,
	MFMET_RealTimeMtcCueing_CuePointsWithAdditionalInfo,
	MFMET_RealTimeMtcCueing_EventNameInAdditionalInfo,
	MFMET_MmcCommands_,
	MFMET_MmcResponses_,
	MFMET_MidiTuningStandard_Realtime_SingleNoteTuningChange,
	MFMET_MidiTuningStandard_Realtime_SingleNoteTuningChangeWithBankSelect,
	MFMET_MidiTuningStandard_Realtime_ScaleOctaveTuning1,
	MFMET_MidiTuningStandard_Realtime_ScaleOctaveTuning2,
	MFMET_ControllerDestinationSetting_ChannelAftertouch,
	MFMET_ControllerDestinationSetting_PolyphonicKeyAftertouch,
	MFMET_ControllerDestinationSetting_ControlChange,
	MFMET_KeyBasedInstrumentControl,
	MFMET_ScalablePolyphonyMidiMipMessage,
	MFMET_MobilePhoneControlMessage,

	// roland events

	// yamaha events
};

enum MFMidiControl
{
	MFMC_BankSelect = 0x00,					// 0 - 3FFF
	MFMC_ModulationWheelOrLever = 0x01,		// 0 - 3FFF
	MFMC_BreathController = 0x02,			// 0 - 3FFF
	MFMC_FootController = 0x04,				// 0 - 3FFF
	MFMC_PortamentoTime = 0x05,				// 0 - 3FFF
	MFMC_DataEntry = 0x06,					// 0 - 3FFF
	MFMC_ChannelVolume = 0x07,				// 0 - 3FFF (formerly MainVolume)
	MFMC_Balance = 0x08,					// 0 - 3FFF
	MFMC_Pan = 0x0A,						// 0 - 3FFF
	MFMC_ExpressionController = 0x0B,		// 0 - 3FFF
	MFMC_EffectControl1 = 0x0C,				// 0 - 3FFF
	MFMC_EffectControl2 = 0x0D,				// 0 - 3FFF
	MFMC_GeneralPurposeController1 = 0x10,	// 0 - 3FFF
	MFMC_GeneralPurposeController2 = 0x11,	// 0 - 3FFF
	MFMC_GeneralPurposeController3 = 0x12,	// 0 - 3FFF
	MFMC_GeneralPurposeController4 = 0x13,	// 0 - 3FFF
	MFMC_DamperPedal = 0x40,				// ≤63 off, ≥64 on
	MFMC_Portamento = 0x41,					// ≤63 off, ≥64 on
	MFMC_Sostenuto = 0x42,					// ≤63 off, ≥64 on
	MFMC_SoftPedal = 0x43,					// ≤63 off, ≥64 on
	MFMC_LegatoFootswitch = 0x44,			// ≤63 Normal, ≥64 Legato
	MFMC_Hold2 = 0x45,						// ≤63 off, ≥64 on
	MFMC_SoundController1 = 0x46,			// 0 - 127 (default: SoundVariation)
	MFMC_SoundController2 = 0x47,			// 0 - 127 (default: Timbre / Harmonic Intens.)
	MFMC_SoundController3 = 0x48,			// 0 - 127 (default: ReleaseTime)
	MFMC_SoundController4 = 0x49,			// 0 - 127 (default: AttackTime)
	MFMC_SoundController5 = 0x4A,			// 0 - 127 (default: Brightness)
	MFMC_SoundController6 = 0x4B,			// 0 - 127 (default: DecayTime - see MMA RP - 021)
	MFMC_SoundController7 = 0x4C,			// 0 - 127 (default: VibratoRate - see MMA RP - 021)
	MFMC_SoundController8 = 0x4D,			// 0 - 127 (default: VibratoDepth - see MMA RP - 021)
	MFMC_SoundController9 = 0x4E,			// 0 - 127 (default: VibratoDelay - see MMA RP - 021)
	MFMC_SoundController10 = 0x4F,			// 0 - 127 (default undefined - see MMA RP - 021)
	MFMC_GeneralPurposeController5 = 0x50,	// 0 - 127 // on/off?
	MFMC_GeneralPurposeController6 = 0x51,	// 0 - 127 // on/off?
	MFMC_GeneralPurposeController7 = 0x52,	// 0 - 127 // on/off?
	MFMC_GeneralPurposeController8 = 0x53,	// 0 - 127 // on/off?
	MFMC_PortamentoControl = 0x54,			// 0 - 127
	MFMC_HighResolutionVelocityPrefix = 0x58,//0 - 127
	MFMC_Effects1Depth = 0x5B,				// 0 - 127 (default: ReverbSendLevel - see MMA RP - 023, formerly ExternalEffectsDepth)
	MFMC_Effects2Depth = 0x5C,				// 0 - 127 (formerly TremoloDepth)
	MFMC_Effects3Depth = 0x5D,				// 0 - 127 (default: ChorusSendLevel - see MMA RP - 023, formerly ChorusDepth)
	MFMC_Effects4Depth = 0x5E,				// 0 - 127 (formerly Celeste[Detune]Depth)
	MFMC_Effects5Depth = 0x5F,				// 0 - 127 (formerly PhaserDepth)
	MFMC_DataIncrement = 0x60,				// (DataEntry + 1, see MMA RP - 018)
	MFMC_DataDecrement = 0x61,				// (DataEntry - 1, see MMA RP - 018)
	MFMC_RegisteredParameterNumber = 0x63,	// 0 - 3FFF (RPN)
	MFMC_NonRegisteredParameterNumber = 0x65,//0 - 3FFF (NRPN)
};

struct MFMidiEvent
{
	uint32 timestamp;
	uint16 ev;
	uint8 channel;
	uint8 hasAllocation;
	union
	{
		struct {
			uint8 note;
		} noteOff;
		struct {
			uint8 note, velocity;
		} noteOn;
		struct {
			uint8 note, pressure;
		} noteAftertouch;
		struct {
			uint8 control, value;
		} controlChange;
		struct {
			uint8 program;
		} programChange;
		struct {
			uint8 pressure;
		} channelAftertouch;
		struct {
			float asFloat() const { return (value - 0x2000) / (float)0x2000; }
			uint16 value;
		} pitchBend;
		struct {
			uint8 messageType, values;
		} midiTimeCodeQuarterFrame;
		struct {
			uint16 beats; // 1 beat = 6 midi clocks
		} songPositionPointer;
		struct {
			uint8 song;
		} songSelect;
		struct {
			struct {
				bool enable;
			} localControl;
			struct {
				uint8 numChannels;
			} monoModeOn;
		} channelMode;
		struct {
			struct {
				uint32 vendor;
				uint16 family, member;
				uint16 major, minor;
			} identityReply;
		} generalInformation;
		struct {
			struct {
				uint8 hour, minute, second, frame;
				uint8 rate;
			} fullTimeCode;
		} midiTimeCode;
		struct {
			const uint8 *data() const { return len == 0xFF ? pBuffer : shortData; }
			size_t length() const { return len == 0xFF ? bufferLen : len; }
			uint32 id() const { return (_id[0] << 16) | (_id[1] << 8) | _id[2]; }

			// data may need allocation...
			union {
				struct {
					uint8 shortData[20];
					uint8 _id[3];
					uint8 len;
				};
				struct {
					uint8 *pBuffer;
					size_t bufferLen;
				};
			};
		} buffer;
	};
};
static_assert(sizeof(MFMidiEvent) == 32, "MFMidiEvent got big!");

typedef void MFMidiEventCallback(MFDevice *pMidiInput, const MFMidiEvent *pEvent);

MF_API int MFMidi_GetTimestampFrequency();

MF_API bool MFMidi_OpenInput(MFDevice *pDevice, bool bBuffered = false, MFMidiEventCallback *pEventCallback = NULL);
MF_API void MFMidi_CloseInput(MFDevice *pDevice);

MF_API uint32 MFMidi_GetState(MFDevice *pDevice, MFMidiDataType type, int channel, int note = 0);
MF_API bool MFMidi_WasPressed(MFDevice *pDevice, int channel, int note);
MF_API bool MFMidi_WasReleased(MFDevice *pDevice, int channel, int note);

MF_API bool MFMidi_Start(MFDevice *pDevice);
MF_API void MFMidi_Stop(MFDevice *pDevice);
MF_API size_t MFMidi_GetEvents(MFDevice *pDevice, MFMidiEvent *pEvents, size_t maxEvents, bool bPeek = false);

MF_API bool MFMidi_OpenOutput(MFDevice *pDevice);
MF_API void MFMidi_CloseOutput(MFDevice *pDevice);

MF_API void MFMidi_SendPacket(MFDevice *pDevice, const uint8 *pBytes, size_t len);
MF_API void MFMidi_SendShortMessage(MFDevice *pDevice, uint32 message);
MF_API void MFMidi_SendEvent(MFDevice *pDevice, MFMidiEvent *pEvent);

MF_API void MFMidi_SetControlWord(MFDevice *pDevice, uint8 channel, MFMidiControl control, uint16 value);
MF_API void MFMidi_SetControlByte(MFDevice *pDevice, uint8 channel, MFMidiControl control, uint8 value);
MF_API void MFMidi_SetControlBool(MFDevice *pDevice, uint8 channel, MFMidiControl control, bool value);

MF_API void MFMidi_ProgramChange(MFDevice *pDevice, uint8 channel, uint8 program, uint16 bank = 0xFFFF);

MF_API void MFMidi_SetParameterB(MFDevice *pDevice, uint8 channel, bool bRegistered, uint16 parameter, uint8 data);
MF_API void MFMidi_SetParameter2B(MFDevice *pDevice, uint8 channel, bool bRegistered, uint16 parameter, uint8 msb, uint8 lsb);
MF_API void MFMidi_SetParameterW(MFDevice *pDevice, uint8 channel, bool bRegistered, uint16 parameter, uint16 data);
MF_API void MFMidi_IncParameter(MFDevice *pDevice, uint8 channel, bool bRegistered, uint16 parameter);
MF_API void MFMidi_DecParameter(MFDevice *pDevice, uint8 channel, bool bRegistered, uint16 parameter);

MF_API void MFMidi_DecodePacket(const uint8 *pBytes, size_t numBytes, MFMidiEvent *pEvent, uint32 timestamp = 0);
MF_API void MFMidi_DecodeShortMessage(uint32 data, MFMidiEvent *pEvent, uint32 timestamp = 0);

MF_API size_t MFMidi_BuildPacket(MFMidiEvent *pEvent, uint8 *pBuffer, size_t bufferLen);

#endif // _MFMIDI_H

/** @} */
