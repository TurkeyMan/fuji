#include "Fuji_Internal.h"

#if MF_MIDI == MF_DRIVER_PC

#include "MFMidi_Internal.h"

#include <windows.h>

#pragma comment(lib, "winmm")

struct MFMidiPC_MidiDevice
{
	MFMidiDeviceInfo info;

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
};

static int numDevices = 0;
static MFMidiPC_MidiDevice *pDevices = NULL;

void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)dwInstance;

	switch(wMsg)
	{
		case MIM_OPEN:
			break;
		case MIM_CLOSE:
			break;
		case MIM_DATA:
		{
			uint8 channel = dwParam1 & 0xF;
			uint8 byte0 = (dwParam1 >> 8) & 0xFF;
			uint8 byte1 = (dwParam1 >> 16) & 0xFF;

			switch((dwParam1 >> 4) & 0x7)
			{
				case 0:
					pDevice->channels[channel].notes[byte0] = 0;
					break;
				case 1:
				case 2:
					pDevice->channels[channel].notes[byte0] = byte1;
					break;
				case 3:
					pDevice->channels[channel].control[byte0] = byte1;
					break;
				case 4:
					pDevice->channels[channel].program = byte0;
					break;
				case 5:
					// TODO: ... what is this?
					break;
				case 6:
					pDevice->channels[channel].pitch = byte0 | (byte1 << 7);
					break;
				default:
					MFDebug_Assert(false, "Why are we getting sys events?");
					break;
			}

			if(pDevice->bBuffered || pDevice->pEventCallback)
			{
				MFMidiEvent ev;
				ev.timestamp = (uint32)dwParam2;
				ev.command = dwParam1 & 0xF0;
				ev.channel = channel;
				ev.data0 = byte0;
				ev.data1 = byte1;

				if(pDevice->bBuffered)
				{
					if(pDevice->numEvents >= pDevice->numAllocated)
					{
						pDevice->numAllocated *= 2;
						pDevice->pEvents = (MFMidiEvent*)MFHeap_Realloc(pDevice->pEvents, sizeof(MFMidiEvent)*pDevice->numAllocated);
					}

					pDevice->pEvents[pDevice->numEvents++] = ev;
				}
				if(pDevice->pEventCallback)
				{
					pDevice->pEventCallback((MFMidiInput*)dwInstance, &ev);
				}
			}
			break;
		}
		case MIM_ERROR:
		case MIM_LONGDATA:
		case MIM_LONGERROR:
		{
			// TODO: what shall we do with this?
			break;
		}
	}
}

void MFMidi_InitModulePlatformSpecific()
{
	numDevices = midiInGetNumDevs();
	if(numDevices)
		pDevices = (MFMidiPC_MidiDevice*)MFHeap_Alloc(sizeof(MFMidiPC_MidiDevice)*numDevices);

	for(int i = 0; i < numDevices; ++i)
	{
		MIDIINCAPS caps;
		midiInGetDevCaps(i, &caps, sizeof(caps));
		MFString_CopyUTF16ToUTF8(pDevices[i].info.name, caps.szPname);
		pDevices[i].info.mid = caps.wMid;
		pDevices[i].info.pid = caps.wPid;
		pDevices[i].info.status = MFMS_Available;
	}
}

void MFMidi_DeinitModulePlatformSpecific()
{
	for(int i = 0; i < numDevices; ++i)
	{
		if(pDevices[i].info.status > MFMS_Available)
		{
			MFDebug_Warn(1, MFStr("Midi device not closed: %s", pDevices[i].info.name));

			midiInReset(pDevices[i].hMidiIn);
			midiInClose(pDevices[i].hMidiIn);
		}
	}

	if(pDevices)
		MFHeap_Free(pDevices);
}

void MFMidi_UpdateInternal()
{
	// copy notes into lastNotes
	for(int i = 0; i < numDevices; ++i)
	{
		if(pDevices[i].info.status == MFMS_Active)
		{
			for(int j = 0; j < 16; ++j)
				MFCopyMemory(pDevices[i].channels[j].lastNotes, pDevices[i].channels[j].notes, sizeof(pDevices[i].channels[j].lastNotes));
		}
	}
}

MF_API int MFMidi_GetNumDevices()
{
	return numDevices;
}

MF_API const char *MFMidi_GetDeviceName(int deviceId)
{
	MFDebug_Assert(deviceId < numDevices, "Invalid device ID!");
	return pDevices[deviceId].info.name;
}

MF_API MFMidiDeviceStatus MFMidi_GetStatus(int deviceId)
{
	MFDebug_Assert(deviceId < numDevices, "Invalid device ID!");
	return pDevices[deviceId].info.status;
}

MF_API MFMidiInput *MFMidi_OpenInput(int deviceId, bool bBuffered, MFMidiEventCallback *pEventCallback)
{
	MFDebug_Assert(deviceId < numDevices, "Invalid device ID!");

	MFMidiPC_MidiDevice *pDevice = &pDevices[deviceId];

	MMRESULT r = midiInOpen(&pDevice->hMidiIn, deviceId, (DWORD_PTR)MidiInProc, (DWORD_PTR)pDevice, CALLBACK_FUNCTION | MIDI_IO_STATUS);
	if(r != MMSYSERR_NOERROR)
	{
		pDevice->hMidiIn = NULL;
		pDevice->info.status = MFMS_Unknown;

		wchar_t errorBuffer[256];
		midiInGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Failed to open MIDI device: %s", MFString_WCharAsUTF8(errorBuffer)));

		return NULL;
	}

	pDevice->bBuffered = bBuffered;
	pDevice->pEventCallback = pEventCallback;

	pDevice->numAllocated = 256;
	pDevice->pEvents = (MFMidiEvent*)MFHeap_Alloc(sizeof(MFMidiEvent) * pDevice->numAllocated);

	pDevice->info.status = MFMS_Ready;

	if(!bBuffered && !pEventCallback)
		MFMidi_Start((MFMidiInput*)pDevice);

	return (MFMidiInput*)pDevice;
}

MF_API void MFMidi_CloseInput(MFMidiInput *pMidiInput)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)pMidiInput;

	midiInReset(pDevice->hMidiIn);
	midiInClose(pDevice->hMidiIn);

	MFHeap_Free(pDevice->pEvents);

	pDevice->info.status = MFMS_Available;
}

MF_API uint32 MFMidi_GetState(MFMidiInput *pMidiInput, MFMidiDataType type, int channel, int note)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)pMidiInput;

	switch(type)
	{
		case MFMD_Note:
			return pDevice->channels[channel].notes[note];
		case MFMD_Controller:
			return pDevice->channels[channel].control[note];
		case MFMD_Program:
			return pDevice->channels[channel].program;
		case MFMD_PitchWheel:
			return pDevice->channels[channel].pitch;
	}
	return 0;
}

MF_API uint32 MFMidi_WasPressed(MFMidiInput *pMidiInput, int channel, int note)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)pMidiInput;

	return pDevice->channels[channel].notes[note] && !pDevice->channels[channel].lastNotes[note];
}

MF_API uint32 MFMidi_WasReleased(MFMidiInput *pMidiInput, int channel, int note)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)pMidiInput;

	return !pDevice->channels[channel].notes[note] && pDevice->channels[channel].lastNotes[note];
}

MF_API bool MFMidi_Start(MFMidiInput *pMidiInput)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)pMidiInput;

	MFDebug_Assert(pDevice->info.status == MFMS_Ready, MFStr("Midi device not ready: %s", pDevice->info.name));

	pDevice->numEvents = pDevice->numEventsRead = 0;

	MMRESULT r = midiInStart(pDevices->hMidiIn);
	if(r != MMSYSERR_NOERROR)
	{
		pDevice->info.status = MFMS_Unknown;

		wchar_t errorBuffer[256];
		midiInGetErrorText(r, errorBuffer, sizeof(errorBuffer));
		MFDebug_Warn(1, MFStr("Couldn't start MIDI device: %s", MFString_WCharAsUTF8(errorBuffer)));
		return false;
	}

	pDevice->info.status = MFMS_Active;
	return true;
}

MF_API void MFMidi_Stop(MFMidiInput *pMidiInput)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)pMidiInput;

	midiInReset(pDevice->hMidiIn);

	pDevice->info.status = MFMS_Ready;
}

MF_API size_t MFMidi_GetEvents(MFMidiInput *pMidiInput, MFMidiEvent *pEvents, size_t maxEvents, bool bPeek)
{
	MFMidiPC_MidiDevice *pDevice = (MFMidiPC_MidiDevice*)pMidiInput;

	if(pDevice->numEvents == 0)
		return 0;

	uint32 toRead = MFMin((uint32)maxEvents, pDevice->numEvents - pDevice->numEventsRead);
	MFCopyMemory(pEvents, pDevice->pEvents + pDevice->numEventsRead, sizeof(MFMidiEvent)*toRead);

	if(!bPeek)
	{
		pDevice->numEventsRead += toRead;
		if(pDevice->numEventsRead == pDevice->numEvents)
			pDevice->numEvents = pDevice->numEventsRead = 0;
	}

	return toRead;
}

#endif // MF_MIDI
