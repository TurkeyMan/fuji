/**
* @file MFMidi_Tables.h
* @brief MIDI data tables.
* @author Manu Evans
* @defgroup MFMidi MIDI I/O access
* @{
*/

#if !defined(_MFMIDI_TABLES_H)
#define _MFMIDI_TABLES_H

enum MFMidiControlCode
{
	MFMCF_BankSelectMSB = 0x00,						// 0 - 127
	MFMCF_ModulationWheelOrLeverMSB = 0x01,			// 0 - 127
	MFMCF_BreathControllerMSB = 0x02,				// 0 - 127
	MFMCF_FootControllerMSB = 0x04,					// 0 - 127
	MFMCF_PortamentoTimeMSB = 0x05,					// 0 - 127
	MFMCF_DataEntryMSB = 0x06,						// 0 - 127
	MFMCF_ChannelVolumeMSB = 0x07,					// 0 - 127 (formerly MainVolume)
	MFMCF_BalanceMSB = 0x08,						// 0 - 127
	MFMCF_PanMSB = 0x0A,							// 0 - 127
	MFMCF_ExpressionControllerMSB = 0x0B,			// 0 - 127
	MFMCF_EffectControl1MSB = 0x0C,					// 0 - 127
	MFMCF_EffectControl2MSB = 0x0D,					// 0 - 127
	MFMCF_GeneralPurposeController1MSB = 0x10,		// 0 - 127
	MFMCF_GeneralPurposeController2MSB = 0x11,		// 0 - 127
	MFMCF_GeneralPurposeController3MSB = 0x12,		// 0 - 127
	MFMCF_GeneralPurposeController4MSB = 0x13,		// 0 - 127
	MFMCF_BankSelectLSB = 0x20,						// 0 - 127
	MFMCF_ModulationWheelOrLeverLSB = 0x21,			// 0 - 127
	MFMCF_BreathControllerLSB = 0x22,				// 0 - 127
	MFMCF_FootControllerLSB = 0x24,					// 0 - 127
	MFMCF_PortamentoTimeLSB = 0x25,					// 0 - 127
	MFMCF_DataEntryLSB = 0x26,						// 0 - 127
	MFMCF_ChannelVolumeLSB = 0x27,					// 0 - 127 (formerly MainVolume)
	MFMCF_BalanceLSB = 0x28,						// 0 - 127
	MFMCF_PanLSB = 0x2A,							// 0 - 127
	MFMCF_ExpressionControllerLSB = 0x2B,			// 0 - 127
	MFMCF_EffectControl1LSB = 0x2C,					// 0 - 127
	MFMCF_EffectControl2LSB = 0x2D,					// 0 - 127
	MFMCF_GeneralPurposeController1LSB = 0x30,		// 0 - 127
	MFMCF_GeneralPurposeController2LSB = 0x31,		// 0 - 127
	MFMCF_GeneralPurposeController3LSB = 0x32,		// 0 - 127
	MFMCF_GeneralPurposeController4LSB = 0x33,		// 0 - 127
	MFMCF_DamperPedal = 0x40,						// ≤63 off, ≥64 on
	MFMCF_Portamento = 0x41,						// ≤63 off, ≥64 on
	MFMCF_Sostenuto = 0x42,							// ≤63 off, ≥64 on
	MFMCF_SoftPedal = 0x43,							// ≤63 off, ≥64 on
	MFMCF_LegatoFootswitch = 0x44,					// ≤63 Normal, ≥64 Legato
	MFMCF_Hold2 = 0x45,								// ≤63 off, ≥64 on
	MFMCF_SoundController1 = 0x46,					// 0 - 127 (default: SoundVariation)
	MFMCF_SoundController2 = 0x47,					// 0 - 127 (default: Timbre / Harmonic Intens.)
	MFMCF_SoundController3 = 0x48,					// 0 - 127 (default: ReleaseTime)
	MFMCF_SoundController4 = 0x49,					// 0 - 127 (default: AttackTime)
	MFMCF_SoundController5 = 0x4A,					// 0 - 127 (default: Brightness)
	MFMCF_SoundController6 = 0x4B,					// 0 - 127 (default: DecayTime - see MMA RP - 021)
	MFMCF_SoundController7 = 0x4C,					// 0 - 127 (default: VibratoRate - see MMA RP - 021)
	MFMCF_SoundController8 = 0x4D,					// 0 - 127 (default: VibratoDepth - see MMA RP - 021)
	MFMCF_SoundController9 = 0x4E,					// 0 - 127 (default: VibratoDelay - see MMA RP - 021)
	MFMCF_SoundController10 = 0x4F,					// 0 - 127 (default undefined - see MMA RP - 021)
	MFMCF_GeneralPurposeController5 = 0x50,			// 0 - 127
	MFMCF_GeneralPurposeController6 = 0x51,			// 0 - 127
	MFMCF_GeneralPurposeController7 = 0x52,			// 0 - 127
	MFMCF_GeneralPurposeController8 = 0x53,			// 0 - 127
	MFMCF_PortamentoControl = 0x54,					// 0 - 127
	MFMCF_HighResolutionVelocityPrefix = 0x58,		// 0 - 127
	MFMCF_Effects1Depth = 0x5B,						// 0 - 127 (default: ReverbSendLevel - see MMA RP - 023, formerly ExternalEffectsDepth)
	MFMCF_Effects2Depth = 0x5C,						// 0 - 127 (formerly TremoloDepth)
	MFMCF_Effects3Depth = 0x5D,						// 0 - 127 (default: ChorusSendLevel - see MMA RP - 023, formerly ChorusDepth)
	MFMCF_Effects4Depth = 0x5E,						// 0 - 127 (formerly Celeste[Detune]Depth)
	MFMCF_Effects5Depth = 0x5F,						// 0 - 127 (formerly PhaserDepth)
	MFMCF_DataIncrement = 0x60,						// (DataEntry + 1, see MMA RP - 018)
	MFMCF_DataDecrement = 0x61,						// (DataEntry - 1, see MMA RP - 018)
	MFMCF_NonRegisteredParameterNumberLSB = 0x62,	// 0 - 127 (NRPN)
	MFMCF_NonRegisteredParameterNumberMSB = 0x63,	// 0 - 127 (NRPN)
	MFMCF_RegisteredParameterNumberLSB = 0x64,		// 0 - 127 (RPN)
	MFMCF_RegisteredParameterNumberMSB = 0x65,		// 0 - 127 (RPN)

	// Controller numbers 120 - 127 are reserved for Channel Mode Messages, which rather than controlling sound parameters, affect the channel's operating mode. (See also Table 1.)
	MFMCF_CMM_AllSoundOff = 0x78,			// 0
	MFMCF_CMM_ResetAllControllers = 0x79,	// 0 (See MMA RP - 015)
	MFMCF_CMM_LocalControl = 0x7A,			// 0 off, 127 on
	MFMCF_CMM_AllNotesOff = 0x7B,			// 0
	MFMCF_CMM_OmniModeOff = 0x7C,			// 0 (+all notes off)
	MFMCF_CMM_OmniModeOn = 0x7D,			// 0 (+all notes off)
	MFMCF_CMM_MonoModeOn = 0x7E,			// N (+poly off, +all notes off) Note: This equals the number of channels, or zero if the number of channels equals the number of voices in the receiver
	MFMCF_CMM_PolyModeOn = 0x7F				// 0 (+mono off, +all notes off)
};

enum MFMidiRegisteredParameters
{
	MFRPN_PitchBendSensitivity = 0x0000,	// 0xSSCC S = semi, C = cent
	MFRPN_ChannelFineTuning = 0x0001,		// Resolution 100/8192 cents: 0000 = -100 cents, 4000 = A440, 7F7F = +100 cents
	MFRPN_ChannelCoarseTuning = 0x0002,		// Only MSB used, Resolution 100 cent: 00 = -6400 cents, 40 = A440, 7F = +6300 cents
	MFRPN_TuningProgramChange = 0x0003,		// Tuning Program Number
	MFRPN_TuningBankSelect = 0x0004,		// Tuning Bank Number
	MFRPN_ModulationDepthRange = 0x0005,	// For GM2, defined in GM2 Specification. For other systems, defined by manufacturer.

	// 3D sound controllers (See RP-049)
	MFRPN_AzimuthAngle = 0x1E80,
	MFRPN_ElevationAngle = 0x1E81,
	MFRPN_Gain = 0x1E82,
	MFRPN_DistanceRatio = 0x1E83,
	MFRPN_MaximumDistance = 0x1E84,
	MFRPN_GainAtMaximumDistance = 0x1E85,
	MFRPN_ReferenceDistanceRatio = 0x1E86,
	MFRPN_PanSpreadAngle = 0x1E87,
	MFRPN_RollAngle = 0x1E88,

	// reset parameter number
	MFRPN_NullFunctionNumber = 0x3FFF		// Setting RPN to 7FH,7FH will disable the data entry, data increment, and data decrement controllers until a new RPN or NRPN is selected.
};

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

#endif // _MFMIDI_TABLES_H

/** @} */
