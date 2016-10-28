module fuji.c.MFMidi;

import fuji.c.MFDevice : MFDevice;

nothrow:
@nogc:

enum MFMidiDataType
{
	Note = 0,
	Controller,
	Program,		// program/patch
	PitchWheel
}

enum MFMidiEventType : ushort
{
	NoteOff = 0,
	NoteOn,
	NoteAftertouch,
	ControlChange,
	ProgramChange,
	ChannelAftertouch,
	PitchBend,

	// system common messages
	Sysex,
	MidiTimeCodeQuarterFrame,
	SongPositionPointer,
	SongSelect,
	TuneRequest,

	// system realtime messages
	TimingClock,
	Start,
	Continue,
	Stop,
	ActiveSensing,
	Reset,

	// channel mode messages
	AllSoundOff,
	ResetAllControls,
	LocalControl,
	AllNotesOff,
	OmniModeOff,	// also causes AllNotesOff
	OmniModeOn,	// also causes AllNotesOff
	MonoModeOn,	// also causes PolyOff, AllNotesOff
	PolyModeOn,	// also causes MonoOff, AllNotesOff

	// non-realtime universal exclusive messages
	SampleDumpHeader,
	SampleDataPacket,
	SampleDumpRequest,
	MidiTimeCode_Special,
	MidiTimeCode_PinchInPoints,
	MidiTimeCode_PunchOutPoints,
	MidiTimeCode_DeletePunchInPoint,
	MidiTimeCode_DeletePunchOutPoint,
	MidiTimeCode_EventStartPoint,
	MidiTimeCode_EventStopPoint,
	MidiTimeCode_EventStartPointsWithAdditionalInfo,
	MidiTimeCode_EventStopPointsWithAdditionalInfo,
	MidiTimeCode_DeleteEventStartPoint,
	MidiTimeCode_DeleteEventStopPoint,
	MidiTimeCode_CuePoints,
	MidiTimeCode_CuePointsWithAdditionalInfo,
	MidiTimeCode_DeleteCuePoint,
	MidiTimeCode_EventNameInAdditionalInfo,
	SampleDumpExt_LoopPointsTransmission,
	SampleDumpExt_LoopPointsRequest,
	SampleDumpExt_SampleNameTransmission,
	SampleDumpExt_SampleNameRequest,
	SampleDumpExt_ExtendedDumpHeader,
	SampleDumpExt_ExtendedLoopPointsTransmission,
	SampleDumpExt_ExtendedLoopPointsRequest,
	GeneralInformation_IdentityRequest,
	GeneralInformation_IdentityReply,
	FileDump_Header,
	FileDump_DataPacket,
	FileDump_Request,
	MidiTuningStandard_NonRealtime_BulkDumpRequest,
	MidiTuningStandard_NonRealtime_BulkDumpReply,
	MidiTuningStandard_NonRealtime_TuningDumpRequest,
	MidiTuningStandard_NonRealtime_KeyBasedTuningDump,
	MidiTuningStandard_NonRealtime_ScaleOctaveTuningDump1,
	MidiTuningStandard_NonRealtime_ScaleOctaveTuningDump2,
	MidiTuningStandard_NonRealtime_SingleNoteTuningChangeWithBankSelect,
	MidiTuningStandard_NonRealtime_ScaleOctaveTuning1,
	MidiTuningStandard_NonRealtime_ScaleOctaveTuning2,
	GeneralMidi_GeneralMidi1SystemOn,
	GeneralMidi_GeneralMidiSystemOff,
	GeneralMidi_GeneralMidi2SystemOn,
	DownloadableSounds_TurnDlsOn,
	DownloadableSounds_TurnDlsOff,
	DownloadableSounds_TurnDlsVoiceAllocationOff,
	DownloadableSounds_TurnDlsVoiceAllocationOn,
	FileReferenceMessage_OpenFile,
	FileReferenceMessage_SelectOrReselectContents,
	FileReferenceMessage_OpenFileAndSelectContents,
	FileReferenceMessage_CloseFile,
	MidiVisualControl_,
	EndOfFile,
	Wait,
	Cancel,
	Nak,
	Ack,

	// realtime universal exclusive messages
	MidiTimeCode_FullMessage,
	MidiTimeCode_UserBits,
	MidiShowControl_MscExtensions,
	MidiShowControl_MscCommands,
	NotificationInformation_BarNumber,
	NotificationInformation_TimeSignatureImmediate,
	NotificationInformation_TimeSignatureDelayed,
	DeviceControl_MasterVolume,
	DeviceControl_MasterBalance,
	DeviceControl_MasterFineTuning,
	DeviceControl_MasterCoarseTuning,
	DeviceControl_GlobalParameterControl,
	RealTimeMtcCueing_Special,
	RealTimeMtcCueing_PunchInPoints,
	RealTimeMtcCueing_PunchOutPoints,
	RealTimeMtcCueing_EventStartPoints,
	RealTimeMtcCueing_EventStopPoints,
	RealTimeMtcCueing_EventStartPointsWithAdditionalInfo,
	RealTimeMtcCueing_EventStopPointsWithAdditionalInfo,
	RealTimeMtcCueing_CuePoints,
	RealTimeMtcCueing_CuePointsWithAdditionalInfo,
	RealTimeMtcCueing_EventNameInAdditionalInfo,
	MmcCommands_,
	MmcResponses_,
	MidiTuningStandard_Realtime_SingleNoteTuningChange,
	MidiTuningStandard_Realtime_SingleNoteTuningChangeWithBankSelect,
	MidiTuningStandard_Realtime_ScaleOctaveTuning1,
	MidiTuningStandard_Realtime_ScaleOctaveTuning2,
	ControllerDestinationSetting_ChannelAftertouch,
	ControllerDestinationSetting_PolyphonicKeyAftertouch,
	ControllerDestinationSetting_ControlChange,
	KeyBasedInstrumentControl,
	ScalablePolyphonyMidiMipMessage,
	MobilePhoneControlMessage,

	// roland events

	// yamaha events
}

enum MFMidiControl : ubyte
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
}

struct MFMidiEvent
{
	uint timestamp;
	MFMidiEventType ev;
	ubyte channel;
	ubyte hasAllocation;
	union
	{
		NoteOff noteOff;
		NoteOn noteOn;
		NoteAftertouch noteAftertouch;
		ControlChange controlChange;
		ProgramChange programChange;
		ChanelAftertouch channelAftertouch;
		PitchBend pitchBend;
		SysEx sysex;
		MidiTimeCodeQuarterFrame midiTimeCodeQuarterFrame;
		SongPositionPointer songPositionPointer;
		SongSelect songSelect;
		ChannelMode channelMode;
		GeneralInformation generalInformation;
	}

private:
	struct NoteOff {
		MFMidiNote note;
	}
	struct NoteOn {
		MFMidiNote note;
		ubyte velocity;
	}
	struct NoteAftertouch {
		MFMidiNote note;
		ubyte pressure;
	}
	struct ControlChange {
		MFMidiControlCode control;
		ubyte value;
	}
	struct ProgramChange {
		ubyte program;
	}
	struct ChanelAftertouch {
		ubyte pressure;
	}
	struct PitchBend {
		float asFloat() const { return (value - 0x2000) / cast(float)0x2000; }
		ushort value;
	}
	struct SysEx {
		@property uint id() const { return (_id[0] << 16) | (_id[1] << 8) | _id[2]; }
		@property const(ubyte)[] data() const { return len == 0xFF ? pBuffer[0..bufferLen] : shortData[0..len]; }

		// data may need allocation...
		private union {
			struct {
				ubyte[20] shortData;
				ubyte[3] _id;
				ubyte len;
			}
			struct {
				ubyte* pBuffer;
				size_t bufferLen;
			}
		}
	}
	struct MidiTimeCodeQuarterFrame {
		ubyte messageType, values;
	}
	struct SongPositionPointer {
		ushort beats; // 1 beat = 6 midi clocks
	}
	struct SongSelect {
		ubyte song;
	}
	struct ChannelMode {
		LocalControl localControl;
		MonoModeOn monoModeOn;

		struct LocalControl {
			bool enable;
		}
		struct MonoModeOn {
			ubyte numChannels;
		}
	}
	struct GeneralInformation {
		IdentityReply identityReply;

		struct IdentityReply {
			uint vendor;
			ushort family, member;
			ushort major, minor;
		}
	}
};
static assert(MFMidiEvent.sizeof == 32, "MFMidiEvent got big!");

alias MFMidiEventCallback = void function(MFDevice* pMidiInput, const MFMidiEvent* pEvent);

extern (C) int MFMidi_GetTimestampFrequency();

extern (C) bool MFMidi_OpenInput(MFDevice* pDevice, bool bBuffered = false, MFMidiEventCallback* pEventCallback = null);
extern (C) void MFMidi_CloseInput(MFDevice* pDevice);

extern (C) uint MFMidi_GetState(MFDevice* pDevice, MFMidiDataType type, int channel, int note = 0);
extern (C) bool MFMidi_WasPressed(MFDevice* pDevice, int channel, int note);
extern (C) bool MFMidi_WasReleased(MFDevice* pDevice, int channel, int note);

extern (C) bool MFMidi_Start(MFDevice* pDevice);
extern (C) void MFMidi_Stop(MFDevice* pDevice);
extern (C) size_t MFMidi_GetEvents(MFDevice* pDevice, MFMidiEvent* pEvents, size_t maxEvents, bool bPeek = false);

extern (C) bool MFMidi_OpenOutput(MFDevice* pDevice);
extern (C) void MFMidi_CloseOutput(MFDevice* pDevice);

extern (C) void MFMidi_SendPacket(MFDevice* pDevice, const(ubyte)* pBytes, size_t len);
extern (C) void MFMidi_SendShortMessage(MFDevice* pDevice, uint message);
extern (C) void MFMidi_SendEvent(MFDevice* pDevice, MFMidiEvent* pEvent);

extern (C) void MFMidi_SetControlWord(MFDevice* pDevice, ubyte channel, MFMidiControl control, ushort value);
extern (C) void MFMidi_SetControlByte(MFDevice* pDevice, ubyte channel, MFMidiControl control, ubyte value);
extern (C) void MFMidi_SetControlBool(MFDevice* pDevice, ubyte channel, MFMidiControl control, bool value);

extern (C) void MFMidi_ProgramChange(MFDevice* pDevice, ubyte channel, ubyte program, ushort bank = 0xFFFF);

extern (C) void MFMidi_SetParameterB(MFDevice* pDevice, ubyte channel, bool bRegistered, ushort parameter, ubyte data);
extern (C) void MFMidi_SetParameter2B(MFDevice* pDevice, ubyte channel, bool bRegistered, ushort parameter, ubyte msb, ubyte lsb);
extern (C) void MFMidi_SetParameterW(MFDevice* pDevice, ubyte channel, bool bRegistered, ushort parameter, ushort data);
extern (C) void MFMidi_IncParameter(MFDevice* pDevice, ubyte channel, bool bRegistered, ushort parameter);
extern (C) void MFMidi_DecParameter(MFDevice* pDevice, ubyte channel, bool bRegistered, ushort parameter);

extern (C) void MFMidi_DecodePacket(const(ubyte)* pBytes, size_t numBytes, MFMidiEvent* pEvent, uint timestamp = 0);
extern (C) void MFMidi_DecodeShortMessage(uint data, MFMidiEvent* pEvent, uint timestamp = 0);

extern (C) size_t MFMidi_BuildPacket(MFMidiEvent* pEvent, ubyte *pBuffer, size_t bufferLen);


enum MFMidiControlCode : ubyte
{
	BankSelectMSB = 0x00,					// 0 - 127
	ModulationWheelOrLeverMSB = 0x01,		// 0 - 127
	BreathControllerMSB = 0x02,				// 0 - 127
	FootControllerMSB = 0x04,				// 0 - 127
	PortamentoTimeMSB = 0x05,				// 0 - 127
	DataEntryMSB = 0x06,					// 0 - 127
	ChannelVolumeMSB = 0x07,				// 0 - 127 (formerly MainVolume)
	BalanceMSB = 0x08,						// 0 - 127
	PanMSB = 0x0A,							// 0 - 127
	ExpressionControllerMSB = 0x0B,			// 0 - 127
	EffectControl1MSB = 0x0C,				// 0 - 127
	EffectControl2MSB = 0x0D,				// 0 - 127
	GeneralPurposeController1MSB = 0x10,	// 0 - 127
	GeneralPurposeController2MSB = 0x11,	// 0 - 127
	GeneralPurposeController3MSB = 0x12,	// 0 - 127
	GeneralPurposeController4MSB = 0x13,	// 0 - 127
	BankSelectLSB = 0x20,					// 0 - 127
	ModulationWheelOrLeverLSB = 0x21,		// 0 - 127
	BreathControllerLSB = 0x22,				// 0 - 127
	FootControllerLSB = 0x24,				// 0 - 127
	PortamentoTimeLSB = 0x25,				// 0 - 127
	DataEntryLSB = 0x26,					// 0 - 127
	ChannelVolumeLSB = 0x27,				// 0 - 127 (formerly MainVolume)
	BalanceLSB = 0x28,						// 0 - 127
	PanLSB = 0x2A,							// 0 - 127
	ExpressionControllerLSB = 0x2B,			// 0 - 127
	EffectControl1LSB = 0x2C,				// 0 - 127
	EffectControl2LSB = 0x2D,				// 0 - 127
	GeneralPurposeController1LSB = 0x30,	// 0 - 127
	GeneralPurposeController2LSB = 0x31,	// 0 - 127
	GeneralPurposeController3LSB = 0x32,	// 0 - 127
	GeneralPurposeController4LSB = 0x33,	// 0 - 127
	DamperPedal = 0x40,						// ≤63 off, ≥64 on
	Portamento = 0x41,						// ≤63 off, ≥64 on
	Sostenuto = 0x42,						// ≤63 off, ≥64 on
	SoftPedal = 0x43,						// ≤63 off, ≥64 on
	LegatoFootswitch = 0x44,				// ≤63 Normal, ≥64 Legato
	Hold2 = 0x45,							// ≤63 off, ≥64 on
	SoundController1 = 0x46,				// 0 - 127 (default: SoundVariation)
	SoundController2 = 0x47,				// 0 - 127 (default: Timbre / Harmonic Intens.)
	SoundController3 = 0x48,				// 0 - 127 (default: ReleaseTime)
	SoundController4 = 0x49,				// 0 - 127 (default: AttackTime)
	SoundController5 = 0x4A,				// 0 - 127 (default: Brightness)
	SoundController6 = 0x4B,				// 0 - 127 (default: DecayTime - see MMA RP - 021)
	SoundController7 = 0x4C,				// 0 - 127 (default: VibratoRate - see MMA RP - 021)
	SoundController8 = 0x4D,				// 0 - 127 (default: VibratoDepth - see MMA RP - 021)
	SoundController9 = 0x4E,				// 0 - 127 (default: VibratoDelay - see MMA RP - 021)
	SoundController10 = 0x4F,				// 0 - 127 (default undefined - see MMA RP - 021)
	GeneralPurposeController5 = 0x50,		// 0 - 127
	GeneralPurposeController6 = 0x51,		// 0 - 127
	GeneralPurposeController7 = 0x52,		// 0 - 127
	GeneralPurposeController8 = 0x53,		// 0 - 127
	PortamentoControl = 0x54,				// 0 - 127
	HighResolutionVelocityPrefix = 0x58,	// 0 - 127
	Effects1Depth = 0x5B,					// 0 - 127 (default: ReverbSendLevel - see MMA RP - 023, formerly ExternalEffectsDepth)
	Effects2Depth = 0x5C,					// 0 - 127 (formerly TremoloDepth)
	Effects3Depth = 0x5D,					// 0 - 127 (default: ChorusSendLevel - see MMA RP - 023, formerly ChorusDepth)
	Effects4Depth = 0x5E,					// 0 - 127 (formerly Celeste[Detune]Depth)
	Effects5Depth = 0x5F,					// 0 - 127 (formerly PhaserDepth)
	DataIncrement = 0x60,					// (DataEntry + 1, see MMA RP - 018)
	DataDecrement = 0x61,					// (DataEntry - 1, see MMA RP - 018)
	NonRegisteredParameterNumberLSB = 0x62,	// 0 - 127 (NRPN)
	NonRegisteredParameterNumberMSB = 0x63,	// 0 - 127 (NRPN)
	RegisteredParameterNumberLSB = 0x64,	// 0 - 127 (RPN)
	RegisteredParameterNumberMSB = 0x65,	// 0 - 127 (RPN)

	// Controller numbers 120 - 127 are reserved for Channel Mode Messages, which rather than controlling sound parameters, affect the channel's operating mode. (See also Table 1.)
	CMM_AllSoundOff = 0x78,			// 0
	CMM_ResetAllControllers = 0x79,	// 0 (See MMA RP - 015)
	CMM_LocalControl = 0x7A,		// 0 off, 127 on
	CMM_AllNotesOff = 0x7B,			// 0
	CMM_OmniModeOff = 0x7C,			// 0 (+all notes off)
	CMM_OmniModeOn = 0x7D,			// 0 (+all notes off)
	CMM_MonoModeOn = 0x7E,			// N (+poly off, +all notes off) Note: This equals the number of channels, or zero if the number of channels equals the number of voices in the receiver
	CMM_PolyModeOn = 0x7F			// 0 (+mono off, +all notes off)
}

enum MFMidiRegisteredParameters : ushort
{
	PitchBendSensitivity = 0x0000,	// 0xSSCC S = semi, C = cent
	ChannelFineTuning = 0x0001,		// Resolution 100/8192 cents: 0000 = -100 cents, 4000 = A440, 7F7F = +100 cents
	ChannelCoarseTuning = 0x0002,	// Only MSB used, Resolution 100 cent: 00 = -6400 cents, 40 = A440, 7F = +6300 cents
	TuningProgramChange = 0x0003,	// Tuning Program Number
	TuningBankSelect = 0x0004,		// Tuning Bank Number
	ModulationDepthRange = 0x0005,	// For GM2, defined in GM2 Specification. For other systems, defined by manufacturer.
	NullFunctionNumber = 0x3FFF,	// Setting RPN to 7FH,7FH will disable the data entry, data increment, and data decrement controllers until a new RPN or NRPN is selected.

	// 3D sound controllers (See RP-049)
	AzimuthAngle = 0x1E80,
	ElevationAngle = 0x1E81,
	Gain = 0x1E82,
	DistanceRatio = 0x1E83,
	MaximumDistance = 0x1E84,
	GainAtMaximumDistance = 0x1E85,
	ReferenceDistanceRatio = 0x1E86,
	PanSpreadAngle = 0x1E87,
	RollAngle = 0x1E88
}

enum MFMidiNote : ubyte
{
	Unknown = 0xFF,

	C0 = 0,
	Cs0,
	D0,
	Ds0,
	E0,
	F0,
	Fs0,
	G0,
	Gs0,
	A0,
	As0,
	B0,
	C1,
	Cs1,
	D1,
	Ds1,
	E1,
	F1,
	Fs1,
	G1,
	Gs1,
	A1,
	As1,
	B1,
	C2,
	Cs2,
	D2,
	Ds2,
	E2,
	F2,
	Fs2,
	G2,
	Gs2,
	A2,
	As2,
	B2,
	C3,
	Cs3,
	D3,
	Ds3,
	E3,
	F3,
	Fs3,
	G3,
	Gs3,
	A3,
	As3,
	B3,
	C4,
	Cs4,
	D4,
	Ds4,
	E4,
	F4,
	Fs4,
	G4,
	Gs4,
	A4,
	As4,
	B4,
	C5,
	Cs5,
	D5,
	Ds5,
	E5,
	F5,
	Fs5,
	G5,
	Gs5,
	A5,
	As5,
	B5,
	C6,
	Cs6,
	D6,
	Ds6,
	E6,
	F6,
	Fs6,
	G6,
	Gs6,
	A6,
	As6,
	B6,
	C7,
	Cs7,
	D7,
	Ds7,
	E7,
	F7,
	Fs7,
	G7,
	Gs7,
	A7,
	As7,
	B7,
	C8,
	Cs8,
	D8,
	Ds8,
	E8,
	F8,
	Fs8,
	G8,
	Gs8,
	A8,
	As8,
	B8,
	C9,
	Cs9,
	D9,
	Ds9,
	E9,
	F9,
	Fs9,
	G9,
	Gs9,
	A9,
	As9,
	B9,
	C10,
	Cs10,
	D10,
	Ds10,
	E10,
	F10,
	Fs10,
	G10
}
