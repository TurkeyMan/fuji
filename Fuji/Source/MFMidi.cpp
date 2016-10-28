#include "Fuji_Internal.h"
#include "MFMidi_Internal.h"
#include "MFSystem.h"

/**** Forward Declarations ****/

static void PromoteControlMessage(MFMidiEvent *pEvent);
static void PromoteUniversalSysexMessage(MFMidiEvent *pEvent, const uint8 *pBytes, size_t len);

/**** Structures ****/


/**** Globals ****/


/**** Functions ****/

MFInitStatus MFMidi_InitModule(int moduleId, bool bPerformInitialisation)
{
	if(!gDefaults.midi.useMidi)
		return MFIS_Succeeded;

	MFMidi_InitModulePlatformSpecific();

	return MFIS_Succeeded;
}

void MFMidi_DeinitModule()
{
	if(!gDefaults.midi.useMidi)
		return;

	MFMidi_DeinitModulePlatformSpecific();
}

void MFMidi_Update()
{
	MFMidi_UpdateInternal();
}

MF_API void MFMidi_SendEvent(MFDevice *pMidiOutput, MFMidiEvent *pEvent)
{
	uint8 buffer[1024];
	size_t packetLen = MFMidi_BuildPacket(pEvent, buffer, sizeof(buffer));
	switch (packetLen)
	{
		case 0:
			break;
		case 1:
			MFMidi_SendShortMessage(pMidiOutput, buffer[0]);
			break;
		case 2:
			MFMidi_SendShortMessage(pMidiOutput, buffer[0] | (buffer[1] << 8));
			break;
		case 3:
			MFMidi_SendShortMessage(pMidiOutput, buffer[0] | (buffer[1] << 8) | (buffer[2] << 16));
			break;
		default:
			MFMidi_SendPacket(pMidiOutput, buffer, packetLen);
			break;
	}
}

MF_API void MFMidi_SetControlWord(MFDevice *pDevice, uint8 channel, MFMidiControl control, uint16 value)
{
	switch (control)
	{
		// word controls
		case MFMC_BankSelect:
		case MFMC_ModulationWheelOrLever:
		case MFMC_BreathController:
		case MFMC_FootController:
		case MFMC_PortamentoTime:
		case MFMC_DataEntry:
		case MFMC_ChannelVolume:
		case MFMC_Balance:
		case MFMC_Pan:
		case MFMC_ExpressionController:
		case MFMC_EffectControl1:
		case MFMC_EffectControl2:
		case MFMC_GeneralPurposeController1:
		case MFMC_GeneralPurposeController2:
		case MFMC_GeneralPurposeController3:
		case MFMC_GeneralPurposeController4:
			MFMidi_SendShortMessage(pDevice, 0xB0 | (channel & 0xF) | (control << 8) | ((value & 0x3F80) << 9));
			MFMidi_SendShortMessage(pDevice, 0xB0 | (channel & 0xF) | ((control + MFMCF_BankSelectLSB) << 8) | ((value & 0x7F) << 16));
			break;
		case MFMC_RegisteredParameterNumber:
		case MFMC_NonRegisteredParameterNumber:
			MFMidi_SendShortMessage(pDevice, 0xB0 | (channel & 0xF) | (control << 8) | ((value & 0x3F80) << 9));
			MFMidi_SendShortMessage(pDevice, 0xB0 | (channel & 0xF) | ((control - 1) << 8) | ((value & 0x7F) << 16));
			break;
		case MFMC_DataIncrement:
		case MFMC_DataDecrement:
			MFMidi_SendShortMessage(pDevice, 0xB0 | (channel & 0xF) | (control << 8));
			break;
			// bool controls (<64 off, >=64 on)
		case MFMC_DamperPedal:
		case MFMC_Portamento:
		case MFMC_Sostenuto:
		case MFMC_SoftPedal:
		case MFMC_LegatoFootswitch:
		case MFMC_Hold2:
			// NOTE: we could handle 0 and != 0 here... but we'll just pass it through I guess...
		// byte controls
		case MFMC_SoundController1:
		case MFMC_SoundController2:
		case MFMC_SoundController3:
		case MFMC_SoundController4:
		case MFMC_SoundController5:
		case MFMC_SoundController6:
		case MFMC_SoundController7:
		case MFMC_SoundController8:
		case MFMC_SoundController9:
		case MFMC_SoundController10:
		case MFMC_GeneralPurposeController5:
		case MFMC_GeneralPurposeController6:
		case MFMC_GeneralPurposeController7:
		case MFMC_GeneralPurposeController8:
		case MFMC_PortamentoControl:
		case MFMC_HighResolutionVelocityPrefix:
		case MFMC_Effects1Depth:
		case MFMC_Effects2Depth:
		case MFMC_Effects3Depth:
		case MFMC_Effects4Depth:
		case MFMC_Effects5Depth:
			MFMidi_SendShortMessage(pDevice, 0xB0 | (channel & 0xF) | (control << 8) | ((value & 0x7F) << 16));
			break;
		default:
			MFDebug_Assert(false, "Invalid control");
	}
}
MF_API void MFMidi_SetControlByte(MFDevice *pDevice, uint8 channel, MFMidiControl control, uint8 value)
{
	if (control >= MFMC_BankSelect && control <= MFMC_GeneralPurposeController4)
		MFMidi_SendShortMessage(pDevice, 0xB0 | (channel & 0xF) | (control << 8) | ((value & 0x7F) << 16));
	else
		MFMidi_SetControlWord(pDevice, channel, control, value);
}
MF_API void MFMidi_SetControlBool(MFDevice *pDevice, uint8 channel, MFMidiControl control, bool value)
{
	if (control >= MFMC_BankSelect && control <= MFMC_GeneralPurposeController4)
		MFMidi_SetControlWord(pDevice, channel, control, value ? 0x3FFF : 0);
	else
		MFMidi_SetControlWord(pDevice, channel, control, value ? 0x7F : 0);
}

MF_API void MFMidi_ProgramChange(MFDevice *pDevice, uint8 channel, uint8 program, uint16 bank)
{
	if (bank != 0xFFFF)
		MFMidi_SetControlWord(pDevice, channel, MFMC_BankSelect, bank);
	MFMidi_SendShortMessage(pDevice, 0xC0 | (channel & 0xF) | ((program << 8) & 0x7F));
}

MF_API void MFMidi_SetParameterB(MFDevice *pMidiOutput, uint8 channel, bool bRegistered, uint16 parameter, uint8 data)
{
	MFMidi_SetControlWord(pMidiOutput, channel, bRegistered ? MFMC_RegisteredParameterNumber : MFMC_NonRegisteredParameterNumber, parameter);
	MFMidi_SetControlByte(pMidiOutput, channel, MFMC_DataEntry, data);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_RegisteredParameterNumber, MFRPN_NullFunctionNumber);
}

MF_API void MFMidi_SetParameter2B(MFDevice *pMidiOutput, uint8 channel, bool bRegistered, uint16 parameter, uint8 msb, uint8 lsb)
{
	MFMidi_SetControlWord(pMidiOutput, channel, bRegistered ? MFMC_RegisteredParameterNumber : MFMC_NonRegisteredParameterNumber, parameter);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_DataEntry, (msb << 7) | (lsb & 0x7F));
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_RegisteredParameterNumber, MFRPN_NullFunctionNumber);
}

MF_API void MFMidi_SetParameterW(MFDevice *pMidiOutput, uint8 channel, bool bRegistered, uint16 parameter, uint16 data)
{
	MFMidi_SetControlWord(pMidiOutput, channel, bRegistered ? MFMC_RegisteredParameterNumber : MFMC_NonRegisteredParameterNumber, parameter);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_DataEntry, data);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_RegisteredParameterNumber, MFRPN_NullFunctionNumber);
}

MF_API void MFMidi_IncParameter(MFDevice *pMidiOutput, uint8 channel, bool bRegistered, uint16 parameter)
{
	MFMidi_SetControlWord(pMidiOutput, channel, bRegistered ? MFMC_RegisteredParameterNumber : MFMC_NonRegisteredParameterNumber, parameter);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_DataIncrement, 0);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_RegisteredParameterNumber, MFRPN_NullFunctionNumber);
}

MF_API void MFMidi_DecParameter(MFDevice *pMidiOutput, uint8 channel, bool bRegistered, uint16 parameter)
{
	MFMidi_SetControlWord(pMidiOutput, channel, bRegistered ? MFMC_RegisteredParameterNumber : MFMC_NonRegisteredParameterNumber, parameter);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_DataDecrement, 0);
	MFMidi_SetControlWord(pMidiOutput, channel, MFMC_RegisteredParameterNumber, MFRPN_NullFunctionNumber);
}

MF_API void MFMidi_DecodePacket(const uint8 *pBytes, size_t numBytes, MFMidiEvent *pEvent, uint32 timestamp)
{
	pEvent->timestamp = timestamp;
	pEvent->hasAllocation = false;

	uint8 command = pBytes[0] & 0xF0;
	uint8 channel = pBytes[0] & 0xF;

	switch ((command >> 4) & 0x7)
	{
		case 0:
			pEvent->ev = MFMET_NoteOff;
			pEvent->channel = channel;
			pEvent->noteOff.note = pBytes[1];
			pEvent->noteOn.velocity = 0;
			break;
		case 1:
			pEvent->ev = MFMET_NoteOn;
			pEvent->channel = channel;
			pEvent->noteOn.note = pBytes[1];
			pEvent->noteOn.velocity = pBytes[2];
			break;
		case 2:
			pEvent->ev = MFMET_NoteAftertouch;
			pEvent->channel = channel;
			pEvent->noteAftertouch.note = pBytes[1];
			pEvent->noteAftertouch.pressure = pBytes[2];
			break;
		case 3:
			pEvent->channel = channel;
			pEvent->ev = MFMET_ControlChange;
			pEvent->controlChange.control = pBytes[1];
			pEvent->controlChange.value = pBytes[2];
			if (pBytes[2] >= 120) // TODO: flag to control this behaviour?
				PromoteControlMessage(pEvent);
			break;
		case 4:
			pEvent->ev = MFMET_ProgramChange;
			pEvent->channel = channel;
			pEvent->programChange.program = pBytes[1];
			break;
		case 5:
			pEvent->ev = MFMET_ChannelAftertouch;
			pEvent->channel = channel;
			pEvent->channelAftertouch.pressure = pBytes[1];
			break;
		case 6:
			pEvent->ev = MFMET_PitchBend;
			pEvent->channel = channel;
			pEvent->pitchBend.value = pBytes[1] | (pBytes[2] << 7);
			break;
		case 7:
			pEvent->channel = 0;
			switch (channel)
			{
				// system control messages
				case 0:
					pEvent->ev = MFMET_Sysex;
					break;
				case 1:
					pEvent->ev = MFMET_MidiTimeCodeQuarterFrame;
					pEvent->midiTimeCodeQuarterFrame.messageType = pBytes[1] >> 4;
					pEvent->midiTimeCodeQuarterFrame.values = pBytes[1] & 0xF;
					break;
				case 2:
					pEvent->ev = MFMET_SongPositionPointer;
					pEvent->songPositionPointer.beats = pBytes[1] | (pBytes[2] << 7);
					break;
				case 3:
					pEvent->ev = MFMET_SongSelect;
					pEvent->songSelect.song = pBytes[1];
					break;
				case 6:
					pEvent->ev = MFMET_TuneRequest;
					break;

				// system realtime messages
				case 8:
					pEvent->ev = MFMET_TimingClock;
					break;
				case 10:
					pEvent->ev = MFMET_Start;
					break;
				case 11:
					pEvent->ev = MFMET_Continue;
					break;
				case 12:
					pEvent->ev = MFMET_Stop;
					break;
				case 14:
					pEvent->ev = MFMET_ActiveSensing;
					break;
				case 15:
					pEvent->ev = MFMET_Reset;
					break;

				case 7:
					// sysex end... unexpected?
				default:
					MFDebug_Assert(false, "Unexpected");
					break;
			}
			break;
	}

	// translate known sysex messages...
	if (pEvent->ev == MFMET_Sysex)
	{
		pEvent->buffer._id[0] = pBytes[1];
		if (pBytes[1] == 0)
		{
			pEvent->buffer._id[1] = pBytes[2];
			pEvent->buffer._id[2] = pBytes[3];
			pBytes += 4; numBytes -= 4;
		}
		else
		{
			pEvent->buffer._id[1] = 0;
			pEvent->buffer._id[2] = 0;
			pBytes += 2; numBytes -= 2;
		}

		// find end token
		size_t sysexLen = 0;
		for (; sysexLen < numBytes; ++sysexLen)
			if (pBytes[sysexLen] == 0xF7)
				break;
		MFDebug_Assert(sysexLen < numBytes, "Sysex message has no terminator!");

		// decode known messages...
		if (pEvent->buffer._id[0] == 0x7E || pEvent->buffer._id[0] == 0x7F)
		{
			PromoteUniversalSysexMessage(pEvent, pBytes, sysexLen);
		}

		// if it's still a sysex event, it's unknown, just keep the data
		if (pEvent->ev == MFMET_Sysex)
		{
			if (sysexLen <= sizeof(pEvent->buffer.shortData))
			{
				MFCopyMemory(pEvent->buffer.shortData, pBytes, sysexLen);
				pEvent->buffer.len = (uint8)sysexLen;
			}
			else
			{
				pEvent->buffer.pBuffer = (uint8*)MFHeap_Alloc(sysexLen);
				pEvent->buffer.bufferLen = sysexLen;
				pEvent->buffer.len = 0xFF;
				pEvent->hasAllocation = 1;
				MFCopyMemory(pEvent->buffer.pBuffer, pBytes, sysexLen);
			}
		}
	}
}

MF_API void MFMidi_DecodeShortMessage(uint32 data, MFMidiEvent *pEvent, uint32 timestamp)
{
#if defined(MF_ENDIAN_BIG)
# error "Doesn't work on big endian systems!"
#endif
	MFMidi_DecodePacket((const uint8*)&data, 3, pEvent, timestamp);
}

inline size_t PutSysexHeader(uint8 (&vendor)[3], uint8 *pBuffer)
{
	pBuffer[0] = 0xF0;
	pBuffer[1] = vendor[0];
	if (vendor[0])
		return 2;
	pBuffer[2] = vendor[1];
	pBuffer[3] = vendor[2];
	return 4;
}

MF_API size_t MFMidi_BuildPacket(MFMidiEvent *pEvent, uint8 *pBuffer, size_t bufferLen)
{
	switch (pEvent->ev)
	{
		case MFMET_NoteOff:
			pBuffer[0] = 0x80 | pEvent->channel;
			pBuffer[1] = pEvent->noteOff.note & 0x7F;
			return 2;
		case MFMET_NoteOn:
			pBuffer[0] = 0x90 | pEvent->channel;
			pBuffer[1] = pEvent->noteOn.note & 0x7F;
			pBuffer[2] = pEvent->noteOn.velocity & 0x7F;
			return 3;
		case MFMET_NoteAftertouch:
			pBuffer[0] = 0xA0 | pEvent->channel;
			pBuffer[1] = pEvent->noteAftertouch.note & 0x7F;
			pBuffer[2] = pEvent->noteAftertouch.pressure & 0x7F;
			return 3;
		case MFMET_ControlChange:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = pEvent->controlChange.control & 0x7F;
			pBuffer[2] = pEvent->controlChange.value & 0x7F;
			return 3;
		case MFMET_ProgramChange:
			pBuffer[0] = 0xC0 | pEvent->channel;
			pBuffer[1] = pEvent->programChange.program & 0x7F;
			return 2;
		case MFMET_ChannelAftertouch:
			pBuffer[0] = 0xD0 | pEvent->channel;
			pBuffer[1] = pEvent->channelAftertouch.pressure & 0x7F;
			return 2;
		case MFMET_PitchBend:
			pBuffer[0] = 0xE0 | pEvent->channel;
			pBuffer[1] = pEvent->pitchBend.value & 0x7F;
			pBuffer[2] = (pEvent->pitchBend.value >> 7) & 0x7F;
			return 3;

		// system common messages
		case MFMET_Sysex:
		{
			size_t sysexLen = PutSysexHeader(pEvent->buffer._id, pBuffer);
			size_t dataLen = pEvent->buffer.length();
			MFCopyMemory(pBuffer + sysexLen, pEvent->buffer.data(), dataLen);
			sysexLen += dataLen;
			pBuffer[sysexLen] = 0xF7;
			return sysexLen + 1;
		}
		case MFMET_MidiTimeCodeQuarterFrame:
			pBuffer[0] = 0xF1;
			pBuffer[1] = ((pEvent->midiTimeCodeQuarterFrame.messageType & 0x7) << 4) | (pEvent->midiTimeCodeQuarterFrame.values & 0xF);
			return 2;
		case MFMET_SongPositionPointer:
			pBuffer[0] = 0xF2;
			pBuffer[1] = pEvent->songPositionPointer.beats & 0x7F;
			pBuffer[2] = (pEvent->songPositionPointer.beats >> 7) & 0x7F;
			return 3;
		case MFMET_SongSelect:
			pBuffer[0] = 0xF3;
			pBuffer[1] = pEvent->songSelect.song & 0x7F;
			return 2;
		case MFMET_TuneRequest:
			pBuffer[0] = 0xF6;
			return 1;

		// system realtime messages
		case MFMET_TimingClock:
			pBuffer[0] = 0xF8;
			return 1;
		case MFMET_Start:
			pBuffer[0] = 0xFA;
			return 1;
		case MFMET_Continue:
			pBuffer[0] = 0xFB;
			return 1;
		case MFMET_Stop:
			pBuffer[0] = 0xFC;
			return 1;
		case MFMET_ActiveSensing:
			pBuffer[0] = 0xFE;
			return 1;
		case MFMET_Reset:
			pBuffer[0] = 0xFF;
			return 1;

		// channel mode messages
		case MFMET_AllSoundOff:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_AllSoundOff;
			pBuffer[2] = 0;
			return 3;
		case MFMET_ResetAllControls:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_ResetAllControllers;
			pBuffer[2] = 0;
			return 3;
		case MFMET_LocalControl:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_LocalControl;
			pBuffer[2] = pEvent->channelMode.localControl.enable ? 0x7F : 0;
			return 3;
		case MFMET_AllNotesOff:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_AllNotesOff;
			pBuffer[2] = 0;
			return 3;
		case MFMET_OmniModeOff:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_OmniModeOff;
			pBuffer[2] = 0;
			return 3;
		case MFMET_OmniModeOn:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_OmniModeOn;
			pBuffer[2] = 0;
			return 3;
		case MFMET_MonoModeOn:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_MonoModeOn;
			pBuffer[2] = pEvent->channelMode.monoModeOn.numChannels;
			return 3;
		case MFMET_PolyModeOn:
			pBuffer[0] = 0xB0 | pEvent->channel;
			pBuffer[1] = MFMCF_CMM_PolyModeOn;
			pBuffer[2] = 0;
			return 3;

		// non-realtime universal exclusive messages

		case MFMET_GeneralInformation_IdentityRequest:
			pBuffer[0] = 0xF0;
			pBuffer[1] = 0x7E;
			pBuffer[2] = pEvent->channel;
			pBuffer[3] = 0x06;
			pBuffer[4] = 0x01;
			pBuffer[5] = 0xF7;
			return 6;

//		case MFMET_GeneralInformation_IdentityReply:

		// realtime universal exclusive messages
		case MFMET_MidiTimeCode_FullMessage:
			pBuffer[0] = 0xF0;
			pBuffer[1] = 0x7F;
			pBuffer[2] = pEvent->channel;
			pBuffer[3] = 0x01;
			pBuffer[4] = 0x01;
			pBuffer[5] = (pEvent->midiTimeCode.fullTimeCode.rate << 5) | (pEvent->midiTimeCode.fullTimeCode.hour & 0x1F);
			pBuffer[6] = pEvent->midiTimeCode.fullTimeCode.minute;
			pBuffer[7] = pEvent->midiTimeCode.fullTimeCode.second;
			pBuffer[8] = pEvent->midiTimeCode.fullTimeCode.frame;
			pBuffer[9] = 0xF7;
			return 10;

		default:
			MFDebug_Assert(false, "Bad message!");
			break;
	}
	return 0;
}

// additional event parsing...
static void PromoteControlMessage(MFMidiEvent *pEvent)
{
	switch (pEvent->controlChange.control - 120)
	{
		case 0:
			pEvent->ev = MFMET_AllSoundOff;
			break;
		case 1:
			pEvent->ev = MFMET_ResetAllControls;
			break;
		case 2:
			pEvent->ev = MFMET_LocalControl;
			MFDebug_Assert(pEvent->controlChange.value == 0 || pEvent->controlChange.value == 0x7F, "Invalid value!");
			pEvent->channelMode.localControl.enable = pEvent->controlChange.value == 0x7F;
			break;
		case 3:
			pEvent->ev = MFMET_AllNotesOff;
			break;
		case 4:
			pEvent->ev = MFMET_OmniModeOff;
			break;
		case 5:
			pEvent->ev = MFMET_OmniModeOn;
			break;
		case 6:
			pEvent->ev = MFMET_MonoModeOn;
			pEvent->channelMode.monoModeOn.numChannels = pEvent->controlChange.value;
			break;
		case 7:
			pEvent->ev = MFMET_PolyModeOn;
			break;
		default:
			MFDebug_Assert(false, "Bad data!");
			break;
	}
}

static void PromoteUniversalSysexMessage(MFMidiEvent *pEvent, const uint8 *pBytes, size_t len)
{
	if (pEvent->buffer._id[0] == 0x7E)
	{
		pEvent->channel = pBytes[0];

		switch (pBytes[1])
		{
		case 0x01:
			pEvent->ev = MFMET_SampleDumpHeader;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		case 0x02:
			pEvent->ev = MFMET_SampleDataPacket;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		case 0x03:
			pEvent->ev = MFMET_SampleDumpRequest;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		case 0x04:
			pEvent->ev = MFMET_MidiTimeCode_Special + pBytes[2];
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x05:
			pEvent->ev = MFMET_SampleDumpExt_LoopPointsTransmission + pBytes[2] - 1;
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x06:
			switch (pBytes[2])
			{
				case 1:
					pEvent->ev = MFMET_GeneralInformation_IdentityRequest;
					break;
				case 2:
					pEvent->ev = MFMET_GeneralInformation_IdentityReply;
					if (pBytes[3] == 0)
					{
						pEvent->generalInformation.identityReply.vendor = (pBytes[4] << 8) | pBytes[5];
						pBytes += 2;
					}
					else
						pEvent->generalInformation.identityReply.vendor = pBytes[3] << 16;
					pEvent->generalInformation.identityReply.family = pBytes[4] | (pBytes[5] << 7);
					pEvent->generalInformation.identityReply.member = pBytes[6] | (pBytes[7] << 7);
					pEvent->generalInformation.identityReply.minor =  pBytes[8] | (pBytes[9] << 7);
					pEvent->generalInformation.identityReply.major = pBytes[10] | (pBytes[11] << 7);
					break;
				default:
					MFDebug_Assert(false, "Unknown General Information request!");
					break;
			}
			break;
		case 0x07:
			pEvent->ev = MFMET_FileDump_Header + pBytes[2] - 1;
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x08:
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x09:
			pEvent->ev = MFMET_GeneralMidi_GeneralMidi1SystemOn + pBytes[2] - 1;
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x0A:
			pEvent->ev = MFMET_DownloadableSounds_TurnDlsOn + pBytes[2] - 1;
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x0B:
			pEvent->ev = MFMET_FileReferenceMessage_OpenFile + pBytes[2] - 1;
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x0C:
			pEvent->ev = MFMET_MidiVisualControl_;// +pBytes[2];
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x7B:
			pEvent->ev = MFMET_EndOfFile;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		case 0x7C:
			pEvent->ev = MFMET_Wait;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		case 0x7D:
			pEvent->ev = MFMET_Cancel;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		case 0x7E:
			pEvent->ev = MFMET_Nak;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		case 0x7F:
			pEvent->ev = MFMET_Ack;
			MFDebug_Assert(false, "TODO: parse data");
			break;
		default:
			break;

		}
	}
	else if (pEvent->buffer._id[0] == 0x7F)
	{
		pEvent->channel = pBytes[0];

		switch (pBytes[1])
		{
		case 0x01:
			switch (pBytes[2])
			{
				case 1:
					pEvent->ev = MFMET_MidiTimeCode_FullMessage;
					pEvent->midiTimeCode.fullTimeCode.rate = (pBytes[3] >> 5) & 0x3;
					pEvent->midiTimeCode.fullTimeCode.hour = pBytes[3] & 0x1F;
					pEvent->midiTimeCode.fullTimeCode.minute = pBytes[4] & 0x7F;
					pEvent->midiTimeCode.fullTimeCode.second = pBytes[5] & 0x7F;
					pEvent->midiTimeCode.fullTimeCode.frame = pBytes[6] & 0x7F;
					break;
				case 2:
					pEvent->ev = MFMET_MidiTimeCode_UserBits;
					MFDebug_Assert(false, "TODO: parse data");
					break;
				default:
					MFDebug_Assert(false, "Unknown MTC event!");
					break;
			}
			break;
		case 0x02:
			pEvent->ev = MFMET_MidiShowControl_MscExtensions + pBytes[2];
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x03:
			switch (pBytes[2])
			{
			case 0:
				pEvent->ev = MFMET_NotificationInformation_BarNumber;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 1:
				pEvent->ev = MFMET_NotificationInformation_TimeSignatureImmediate;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0x42:
				pEvent->ev = MFMET_NotificationInformation_TimeSignatureDelayed;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x04:
			pEvent->ev = MFMET_DeviceControl_MasterVolume + pBytes[2] - 1;
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x05:
			switch (pBytes[2])
			{
			case 0x0:
				pEvent->ev = MFMET_RealTimeMtcCueing_Special;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0x1:
				pEvent->ev = MFMET_RealTimeMtcCueing_PunchInPoints;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0x2:
				pEvent->ev = MFMET_RealTimeMtcCueing_PunchOutPoints;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0x5:
				pEvent->ev = MFMET_RealTimeMtcCueing_EventStartPoints;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0x6:
				pEvent->ev = MFMET_RealTimeMtcCueing_EventStopPoints;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0x7:
				pEvent->ev = MFMET_RealTimeMtcCueing_EventStartPointsWithAdditionalInfo;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0x8:
				pEvent->ev = MFMET_RealTimeMtcCueing_EventStopPointsWithAdditionalInfo;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0xB:
				pEvent->ev = MFMET_RealTimeMtcCueing_CuePoints;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0xC:
				pEvent->ev = MFMET_RealTimeMtcCueing_CuePointsWithAdditionalInfo;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 0xE:
				pEvent->ev = MFMET_RealTimeMtcCueing_EventNameInAdditionalInfo;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			default:
				MFDebug_Assert(false, "Unknown MTC cueing event!");
				break;
			}
			break;
		case 0x06:
			pEvent->ev = MFMET_MmcCommands_;// +pBytes[2];
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x07:
			pEvent->ev = MFMET_MmcResponses_;// +pBytes[2];
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x08:
			switch (pBytes[2])
			{
			case 2:
				pEvent->ev = MFMET_MidiTuningStandard_Realtime_SingleNoteTuningChange;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 7:
				pEvent->ev = MFMET_MidiTuningStandard_Realtime_SingleNoteTuningChangeWithBankSelect;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 8:
				pEvent->ev = MFMET_MidiTuningStandard_Realtime_ScaleOctaveTuning1;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			case 9:
				pEvent->ev = MFMET_MidiTuningStandard_Realtime_ScaleOctaveTuning2;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			default:
				MFDebug_Assert(false, "Unknown MTS event!");
				break;
			}
			break;
		case 0x09:
			pEvent->ev = MFMET_ControllerDestinationSetting_ChannelAftertouch + pBytes[2] - 1;
			switch (pBytes[2])
			{
			default:
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x0A:
			switch (pBytes[2])
			{
			case 1:
				pEvent->ev = MFMET_KeyBasedInstrumentControl;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x0B:
			switch (pBytes[2])
			{
			case 1:
				pEvent->ev = MFMET_ScalablePolyphonyMidiMipMessage;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		case 0x0C:
			switch (pBytes[2])
			{
			case 0:
				pEvent->ev = MFMET_MobilePhoneControlMessage;
				MFDebug_Assert(false, "TODO: parse data");
				break;
			}
			break;
		}
	}
}
