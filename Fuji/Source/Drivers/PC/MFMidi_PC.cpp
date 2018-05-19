#include "Fuji_Internal.h"

#if MF_MIDI == MF_DRIVER_PC

#include "MFMidi_Internal.h"
#include "MFDevice_Internal.h"

#include <windows.h>

#pragma comment(lib, "winmm")

struct MFMidiPC_MidiInputDevice
{
	short mid, pid; // TODO: confirm, is a 'manufacturer identifier' the same as a 'vendor identifier'?

	HMIDIIN hMidiIn;
	bool bBuffered;

	MFMidiEventCallback *pEventCallback;

	MFMidiEvent *pEvents;
	uint32 numEvents;
	uint32 numAllocated;
	uint32 numEventsRead;

	struct ChannelState
	{
		uint8 lastNotes[128];
		uint8 notes[128];
		uint8 control[128];
		uint8 program;
		uint16 pitch;
	} channels[16];

	uint8 sysexRecvBuffer[1024];
	MIDIHDR sysexRecv;
};

struct MFMidiPC_MidiOutputDevice
{
	short mid, pid; // TODO: confirm, is a 'manufacturer identifier' the same as a 'vendor identifier'?

	HMIDIOUT hMidiOut;
};

static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	MFDevice *pDevice = (MFDevice*)dwInstance;
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	switch(wMsg)
	{
		case MIM_OPEN:
			MFDebug_Log(0, MFStr("Opened MIDI input device: %s", pDevice->strings[MFDS_ID]));
			break;
		case MIM_CLOSE:
			MFDebug_Log(0, MFStr("Closed MIDI input device: %s", pDevice->strings[MFDS_ID]));
			break;
		case MIM_MOREDATA:
			MFDebug_Log(0, "MIDI message: MIM_MOREDATA");
			break;
		case MIM_DATA:
		{
			MFMidiEvent ev;
			MFMidi_DecodeShortMessage((uint32)dwParam1, &ev, (uint32)dwParam2);

			switch(ev.ev)
			{
				case MFMET_NoteOff:
					pMidi->channels[ev.channel].notes[ev.noteOff.note] = 0;
					break;
				case MFMET_NoteOn:
				case MFMET_NoteAftertouch:
					pMidi->channels[ev.channel].notes[ev.noteOn.note] = ev.noteOn.velocity;
					break;
				case MFMET_ControlChange:
					pMidi->channels[ev.channel].control[ev.controlChange.control] = ev.controlChange.value;
					break;
				case MFMET_ProgramChange:
					pMidi->channels[ev.channel].program = ev.programChange.program;
					break;
				case MFMET_ChannelAftertouch:
					// TODO: ... what is this?
					break;
				case MFMET_PitchBend:
					pMidi->channels[ev.channel].pitch = ev.pitchBend.value;
					break;
				default:
					MFDebug_Assert(false, "Why are we getting sys events?");
					break;
			}

			if (pMidi->bBuffered || pMidi->pEventCallback)
			{
				if (pMidi->bBuffered)
				{
					if (pMidi->numEvents >= pMidi->numAllocated)
					{
						pMidi->numAllocated *= 2;
						pMidi->pEvents = (MFMidiEvent*)MFHeap_Realloc(pMidi->pEvents, sizeof(MFMidiEvent)*pMidi->numAllocated);
					}

					pMidi->pEvents[pMidi->numEvents++] = ev;
				}
				if (pMidi->pEventCallback)
				{
					pMidi->pEventCallback(pDevice, &ev);
				}
			}
			break;
		}
		case MIM_LONGDATA:
		{
			MIDIHDR *pHdr = (MIDIHDR*)dwParam1;

			MFMidiEvent ev;
			MFMidi_DecodePacket((const uint8*)pHdr->lpData, pHdr->dwBytesRecorded, &ev, (uint32)dwParam2);

			if (pMidi->bBuffered || pMidi->pEventCallback)
			{
				if (pMidi->bBuffered)
				{
					if (pMidi->numEvents >= pMidi->numAllocated)
					{
						pMidi->numAllocated *= 2;
						pMidi->pEvents = (MFMidiEvent*)MFHeap_Realloc(pMidi->pEvents, sizeof(MFMidiEvent)*pMidi->numAllocated);
					}

					pMidi->pEvents[pMidi->numEvents++] = ev;
				}
				if (pMidi->pEventCallback)
				{
					pMidi->pEventCallback(pDevice, &ev);
				}
			}

			MMRESULT r = midiInAddBuffer(pMidi->hMidiIn, pHdr, sizeof(*pHdr));
			if (r != MMSYSERR_NOERROR)
			{
				wchar_t errorBuffer[256];
				midiInGetErrorText(r, errorBuffer, sizeof(errorBuffer));
				MFDebug_Warn(1, MFStr("Failed to open MIDI input device: %s", MFString_WCharAsUTF8(errorBuffer)));
			}
			break;
		}
		case MIM_ERROR:
		case MIM_LONGERROR:
		{
			MFDebug_Log(0, MFStr("MIDI input error: %d, 0x%08X : 0x%08X", wMsg, dwParam1, dwParam2));
			break;
		}
	}
}

static void CALLBACK MidiOutProc(HMIDIOUT hMidiOut, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	MFDevice *pDevice = (MFDevice*)dwInstance;
	MFMidiPC_MidiOutputDevice *pMidi = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;

	switch (wMsg)
	{
		case MOM_OPEN:
			MFDebug_Log(0, MFStr("Opened MIDI output device: %s", pDevice->strings[MFDS_ID]));
			break;
		case MOM_CLOSE:
			MFDebug_Log(0, MFStr("Opened MIDI output device: %s", pDevice->strings[MFDS_ID]));
			break;
		case MOM_DONE:
		{
			MIDIHDR *pHdr = (MIDIHDR*)dwParam1;
			MMRESULT r = midiOutUnprepareHeader(pMidi->hMidiOut, pHdr, sizeof(*pHdr));
			if (r != MMSYSERR_NOERROR)
			{
				wchar_t errorBuffer[256];
				midiOutGetErrorText(r, errorBuffer, sizeof(errorBuffer));
				MFDebug_Warn(1, MFStr("Failed to cleanup MIDI message: %s", MFString_WCharAsUTF8(errorBuffer)));
			}
			// TODO: return to pool...
			break;
		}
		case MOM_POSITIONCB:
			MFDebug_Log(0, "MIDI output device: Position CB");
			break;
	}
}

static void DestroyInputDevice(MFDevice *pDevice)
{
	MFMidiPC_MidiInputDevice *pDev = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;
	if (pDev->hMidiIn)
	{
		MFDebug_Warn(1, MFStr("MIDI output device not closed: %s", pDevice->strings[MFDS_ID]));

		midiInReset(pDev->hMidiIn);
		midiInClose(pDev->hMidiIn);
	}
	MFHeap_Free(pDev);
}
static void DestroyOutputDevice(MFDevice *pDevice)
{
	MFMidiPC_MidiOutputDevice *pDev = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;
	if (pDev->hMidiOut)
	{
		MFDebug_Warn(1, MFStr("MIDI output device not closed: %s", pDevice->strings[MFDS_ID]));

		midiOutReset(pDev->hMidiOut);
		midiOutClose(pDev->hMidiOut);
	}
	MFHeap_Free(pDev);
}

void MFMidi_InitModulePlatformSpecific()
{
	UINT numInputDevices = midiInGetNumDevs();
	for (UINT i = 0; i < numInputDevices; ++i)
	{
		MIDIINCAPS caps;
		MMRESULT r = midiInGetDevCaps(i, &caps, sizeof(caps));
		if (r != MMSYSERR_NOERROR)
		{
			wchar_t errorBuffer[256];
			midiOutGetErrorText(r, errorBuffer, sizeof(errorBuffer));
			MFDebug_Warn(1, MFStr("Failed to query midi input device: %s", MFString_WCharAsUTF8(errorBuffer)));
			continue;
		}

		MFDevice *pDevice = MFDevice_AllocDevice(MFDT_MidiInput, &DestroyInputDevice);
		pDevice->pInternal = MFHeap_AllocAndZero(sizeof(MFMidiPC_MidiInputDevice));
		pDevice->state = MFDevState_Available;

		MFMidiPC_MidiOutputDevice *pDev = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;
		pDev->mid = caps.wMid;
		pDev->pid = caps.wPid;

		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_ID], caps.szPname);
		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_DeviceName], caps.szPname);
		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_Description], caps.szPname);
		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_InterfaceName], caps.szPname);
//		MFDS_Manufacturer

		MFDebug_Log(0, MFStr("Found midi input device: %s (%04X:%04X) - state: %d", pDevice->strings[MFDS_ID], caps.wMid, caps.wPid, pDevice->state));
	}

	UINT numOutputDevices = midiOutGetNumDevs();
	for (UINT i = 0; i < numOutputDevices; ++i)
	{
		MIDIOUTCAPS caps;
		MMRESULT r = midiOutGetDevCaps(i, &caps, sizeof(caps));
		if (r != MMSYSERR_NOERROR)
		{
			wchar_t errorBuffer[256];
			midiOutGetErrorText(r, errorBuffer, sizeof(errorBuffer));
			MFDebug_Warn(1, MFStr("Failed to query midi output device: %s", MFString_WCharAsUTF8(errorBuffer)));
			continue;
		}

		MFDevice *pDevice = MFDevice_AllocDevice(MFDT_MidiOutput, &DestroyOutputDevice);
		pDevice->pInternal = MFHeap_AllocAndZero(sizeof(MFMidiPC_MidiOutputDevice));
		pDevice->state = MFDevState_Available;

		MFMidiPC_MidiOutputDevice *pDev = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;
		pDev->mid = caps.wMid;
		pDev->pid = caps.wPid;

		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_ID], caps.szPname);
		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_DeviceName], caps.szPname);
		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_Description], caps.szPname);
		MFString_CopyUTF16ToUTF8(pDevice->strings[MFDS_InterfaceName], caps.szPname);
//		MFDS_Manufacturer

		MFDebug_Log(0, MFStr("Found midi output device: %s (%04X:%04X) - state: %d", pDevice->strings[MFDS_ID], caps.wMid, caps.wPid, pDevice->state));
	}
}

void MFMidi_DeinitModulePlatformSpecific()
{
}

void MFMidi_UpdateInternal()
{
	// copy notes into lastNotes
	size_t numInputDevices = MFDevice_GetNumDevices(MFDT_MidiInput);
	for (size_t i = 0; i < numInputDevices; ++i)
	{
		MFDevice *pDevice = MFDevice_GetDeviceByIndex(MFDT_MidiInput, i);
		if (pDevice->state == MFDevState_Active)
		{
			MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;
			for (int j = 0; j < 16; ++j)
				MFCopyMemory(pMidi->channels[j].lastNotes, pMidi->channels[j].notes, sizeof(pMidi->channels[j].lastNotes));
		}
	}
}

MF_API int MFMidi_GetTimestampFrequency()
{
	return 1000;
}

MF_API bool MFMidi_OpenInput(MFDevice *pDevice, bool bBuffered, MFMidiEventCallback *pEventCallback)
{
	MFDebug_Assert(pDevice->type == MFDT_MidiInput, "Not a MIDI device!");

	if (pDevice->state == MFDevState_Ready || pDevice->state == MFDevState_Active)
	{
		MFDebug_Warn(1, "Midi input device already opened!");
		return false;
	}
	if (pDevice->state != MFDevState_Available)
	{
		MFDebug_Warn(1, "Unable to open midi input device!");
		return false;
	}

	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	// find and open the device
	// TODO: FIXME! this won't work if there are 2 instances of the same device attached to the PC!!!
	UINT numInputDevices = midiInGetNumDevs();
	UINT i = 0;
	for (; i < numInputDevices; ++i)
	{
		MIDIINCAPS caps;
		MMRESULT r = midiInGetDevCaps(i, &caps, sizeof(caps));
		if (r != MMSYSERR_NOERROR)
			continue;

		if (caps.wMid == pMidi->mid && caps.wPid == pMidi->pid)
			break;
	}
	if (i == numInputDevices)
	{
		MFDebug_Warn(1, MFStr("Midi output device '%s' not found!", pDevice->strings[MFDS_ID]));
		pDevice->state = MFDevState_Unknown; // set this flag?
		return false;
	}

	MMRESULT r = midiInOpen(&pMidi->hMidiIn, i, (DWORD_PTR)MidiInProc, (DWORD_PTR)pDevice, CALLBACK_FUNCTION | MIDI_IO_STATUS);
	if (r != MMSYSERR_NOERROR)
	{
		pMidi->hMidiIn = NULL;
		pDevice->state = MFDevState_Unknown;

		wchar_t errorBuffer[256];
		midiInGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to open MIDI input device: %s", MFString_WCharAsUTF8(errorBuffer)));

		return false;
	}

	MFZeroMemory(&pMidi->sysexRecv, sizeof(pMidi->sysexRecv));
	pMidi->sysexRecv.lpData = (LPSTR)pMidi->sysexRecvBuffer;
	pMidi->sysexRecv.dwBufferLength = sizeof(pMidi->sysexRecvBuffer);
	r = midiInPrepareHeader(pMidi->hMidiIn, &pMidi->sysexRecv, sizeof(pMidi->sysexRecv));
	if (r != MMSYSERR_NOERROR)
	{
		wchar_t errorBuffer[256];
		midiInGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to open MIDI input device: %s", MFString_WCharAsUTF8(errorBuffer)));
	}
	r = midiInAddBuffer(pMidi->hMidiIn, &pMidi->sysexRecv, sizeof(pMidi->sysexRecv));
	if (r != MMSYSERR_NOERROR)
	{
		wchar_t errorBuffer[256];
		midiInGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to open MIDI input device: %s", MFString_WCharAsUTF8(errorBuffer)));
	}

	pMidi->bBuffered = bBuffered;
	pMidi->pEventCallback = pEventCallback;

	pMidi->numAllocated = 256;
	pMidi->pEvents = (MFMidiEvent*)MFHeap_Alloc(sizeof(MFMidiEvent) * pMidi->numAllocated);

	pDevice->state = MFDevState_Ready;

	if (!bBuffered && !pEventCallback)
		MFMidi_Start(pDevice);

	return true;
}

MF_API void MFMidi_CloseInput(MFDevice *pDevice)
{
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	if (!pMidi->hMidiIn)
	{
		MFDebug_Warn(1, "Midi input device not opened!");
		return;
	}

	midiInReset(pMidi->hMidiIn);

	midiInUnprepareHeader(pMidi->hMidiIn, &pMidi->sysexRecv, sizeof(pMidi->sysexRecv));

	midiInClose(pMidi->hMidiIn);
	pMidi->hMidiIn = NULL;

	MFHeap_Free(pMidi->pEvents);

	pDevice->state = MFDevState_Available;
}

MF_API uint32 MFMidi_GetState(MFDevice *pDevice, MFMidiDataType type, int channel, int note)
{
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	switch(type)
	{
		case MFMD_Note:
			return pMidi->channels[channel].notes[note];
		case MFMD_Controller:
			return pMidi->channels[channel].control[note];
		case MFMD_Program:
			return pMidi->channels[channel].program;
		case MFMD_PitchWheel:
			return pMidi->channels[channel].pitch;
	}
	return 0;
}

MF_API bool MFMidi_WasPressed(MFDevice *pDevice, int channel, int note)
{
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	return pMidi->channels[channel].notes[note] && !pMidi->channels[channel].lastNotes[note];
}

MF_API bool MFMidi_WasReleased(MFDevice *pDevice, int channel, int note)
{
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	return !pMidi->channels[channel].notes[note] && pMidi->channels[channel].lastNotes[note];
}

MF_API bool MFMidi_Start(MFDevice *pDevice)
{
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	if (pDevice->state == MFDevState_Active)
	{
		MFDebug_Warn(1, "Midi input device already started!");
		return false;
	}
	if (pDevice->state != MFDevState_Ready)
	{
		MFDebug_Warn(1, "Midi input device not ready!");
		return false;
	}

	pMidi->numEvents = pMidi->numEventsRead = 0;

	MMRESULT r = midiInStart(pMidi->hMidiIn);
	if (r != MMSYSERR_NOERROR)
	{
		pDevice->state = MFDevState_Unknown;

		wchar_t errorBuffer[256];
		midiInGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Couldn't start MIDI device: %s", MFString_WCharAsUTF8(errorBuffer)));
		return false;
	}

	pDevice->state = MFDevState_Active;
	return true;
}

MF_API void MFMidi_Stop(MFDevice *pDevice)
{
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	if (pDevice->state != MFDevState_Active)
	{
		MFDebug_Warn(1, "Midi input device not started!");
		return;
	}

	midiInReset(pMidi->hMidiIn);

	pDevice->state = MFDevState_Ready;
}

MF_API size_t MFMidi_GetEvents(MFDevice *pDevice, MFMidiEvent *pEvents, size_t maxEvents, bool bPeek)
{
	MFMidiPC_MidiInputDevice *pMidi = (MFMidiPC_MidiInputDevice*)pDevice->pInternal;

	if (pMidi->numEvents == 0)
		return 0;
	if (!pEvents)
		return pMidi->numEvents - pMidi->numEventsRead;

	uint32 toRead = MFMin((uint32)maxEvents, pMidi->numEvents - pMidi->numEventsRead);
	MFCopyMemory(pEvents, pMidi->pEvents + pMidi->numEventsRead, sizeof(MFMidiEvent)*toRead);

	if (!bPeek)
	{
		pMidi->numEventsRead += toRead;
		if (pMidi->numEventsRead == pMidi->numEvents)
			pMidi->numEvents = pMidi->numEventsRead = 0;
	}

	return toRead;
}

MF_API bool MFMidi_OpenOutput(MFDevice *pDevice)
{
	MFDebug_Assert(pDevice->type == MFDT_MidiOutput, "Not a MIDI device!");

	if (pDevice->state == MFDevState_Ready)
	{
		MFDebug_Warn(1, "Midi output device already opened!");
		return false;
	}
	if (pDevice->state != MFDevState_Available)
	{
		MFDebug_Warn(1, "Unable to open midi output device!");
		return false;
	}

	MFMidiPC_MidiOutputDevice *pMidi = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;

	// find and open the device
	// TODO: FIXME! this won't work if there are 2 instances of the same device attached to the PC!!!
	UINT numOutputDevices = midiOutGetNumDevs();
	UINT i = 0;
	for (; i < numOutputDevices; ++i)
	{
		MIDIOUTCAPS caps;
		MMRESULT r = midiOutGetDevCaps(i, &caps, sizeof(caps));
		if (r != MMSYSERR_NOERROR)
			continue;

		if (caps.wMid == pMidi->mid && caps.wPid == pMidi->pid)
			break;
	}
	if (i == numOutputDevices)
	{
		MFDebug_Log(0, MFStr("Midi output device '%s' not found!", pDevice->strings[MFDS_ID]));
		pDevice->state = MFDevState_Unknown; // set this flag?
		return false;
	}

	MMRESULT r = midiOutOpen(&pMidi->hMidiOut, i, (DWORD_PTR)MidiOutProc, (DWORD_PTR)pDevice, CALLBACK_FUNCTION);
	if (r != MMSYSERR_NOERROR)
	{
		pMidi->hMidiOut = NULL;
		pDevice->state = MFDevState_Unknown;

		wchar_t errorBuffer[256];
		midiOutGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to open MIDI output device: %s", MFString_WCharAsUTF8(errorBuffer)));

		return false;
	}

	pDevice->state = MFDevState_Ready;

	return true;
}

MF_API void MFMidi_CloseOutput(MFDevice *pDevice)
{
	MFMidiPC_MidiOutputDevice *pMidi = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;

	if (!pMidi->hMidiOut)
	{
		MFDebug_Warn(1, "Midi output device not opened!");
		return;
	}

	midiOutReset(pMidi->hMidiOut);
	midiOutClose(pMidi->hMidiOut);
	pMidi->hMidiOut = NULL;

	pDevice->state = MFDevState_Available;
}

MF_API void MFMidi_SendPacket(MFDevice *pDevice, const uint8 *pBytes, size_t len)
{
	MFMidiPC_MidiOutputDevice *pMidi = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;

	// TODO: get hdr from pool...
	MIDIHDR hdr;
	MFZeroMemory(&hdr, sizeof(hdr));
	hdr.lpData = (LPSTR)pBytes;
	hdr.dwBufferLength = (DWORD)len;
	hdr.dwBytesRecorded = (DWORD)len;
	hdr.dwUser = (DWORD_PTR)pDevice;

	MMRESULT r = midiOutPrepareHeader(pMidi->hMidiOut, &hdr, sizeof(hdr));
	if (r != MMSYSERR_NOERROR)
	{
		wchar_t errorBuffer[256];
		midiOutGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to send MIDI message: %s", MFString_WCharAsUTF8(errorBuffer)));
		return;
	}

	r = midiOutLongMsg(pMidi->hMidiOut, &hdr, sizeof(hdr));
	if (r != MMSYSERR_NOERROR)
	{
		wchar_t errorBuffer[256];
		midiOutGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to send MIDI message: %s", MFString_WCharAsUTF8(errorBuffer)));
	}
}

MF_API void MFMidi_SendShortMessage(MFDevice *pDevice, uint32 message)
{
	MFMidiPC_MidiOutputDevice *pMidi = (MFMidiPC_MidiOutputDevice*)pDevice->pInternal;

	MMRESULT r = midiOutShortMsg(pMidi->hMidiOut, (DWORD)message);
	if (r != MMSYSERR_NOERROR)
	{
		wchar_t errorBuffer[256];
		midiOutGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to send MIDI message: %s", MFString_WCharAsUTF8(errorBuffer)));
	}
}

#endif // MF_MIDI
