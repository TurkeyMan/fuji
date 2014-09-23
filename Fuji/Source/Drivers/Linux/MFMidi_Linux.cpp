#include "Fuji_Internal.h"

#if MF_MIDI == MF_DRIVER_ALSA
#include "MFMidi_Internal.h"
#include <alsa/asoundlib.h>

struct MFMidiLinux_MidiDevice
{
	snd_seq_addr_t send;
	snd_seq_port_subscribe_t *subscription;
	MFMidiDeviceInfo info;
	bool bBuffered;
  
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
		uint8 channel_pressure;
		uint16 pitch;
	} channels[16];
};



static int numDevices = 0;
static MFMidiLinux_MidiDevice *pDevices = NULL;

snd_seq_t *sequencer;
snd_seq_event_t *event;
char * port_name = "fuji";
snd_seq_addr_t dest;
static const int read_bits = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;

inline int MFMidi_GetNumPorts(snd_seq_client_info_t * client_info, snd_seq_port_info_t * port_info)
{
	int ports = 0, count;
	MFDebug_Warn(1,"ALSA MIDI looking for devices\n");
	
	snd_seq_client_info_set_client(client_info, -1);
	while (snd_seq_query_next_client(sequencer, client_info) >= 0) 
	{
	  //filter clients we are not interested in
		if ( MFString_Compare(snd_seq_client_info_get_name(client_info),"System") == 0||
			 MFString_Compare(snd_seq_client_info_get_name(client_info),"Midi Through") == 0 ||
			 snd_seq_client_info_get_client(client_info) == dest.client ||
			 snd_seq_client_info_get_type(client_info) == SND_SEQ_USER_CLIENT ) { continue; }
			 
		MFDebug_Warn(3,MFStr("ALSA MIDI client: %s (%i)\n",snd_seq_client_info_get_name(client_info),snd_seq_client_info_get_client(client_info)));
		snd_seq_port_info_set_client(port_info, snd_seq_client_info_get_client(client_info));
		snd_seq_port_info_set_port(port_info, -1);
		
		count = 0;
		while (snd_seq_query_next_port(sequencer,port_info) >= 0) {
			if ((snd_seq_port_info_get_capability(port_info) & read_bits) == read_bits) { count++; }
		}
		ports += count;
	}
	return ports;
}

void MFMidi_InitModulePlatformSpecific() 
{
	
	if ( snd_seq_open(&sequencer, "hw", SND_SEQ_OPEN_INPUT, 0) < 0) { MFDebug_Error("Can't open alsa MIDI\n"); exit(1); }
	snd_seq_set_client_name(sequencer,port_name);
	
	dest.client = snd_seq_client_id(sequencer);
	snd_seq_set_client_name(sequencer,port_name);
	dest.port = snd_seq_create_simple_port( sequencer, "listen:in", 
											SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
											SND_SEQ_PORT_TYPE_APPLICATION);
  
	snd_seq_client_info_t * client_info;
	snd_seq_port_info_t * port_info;
	int count;

	snd_seq_client_info_malloc(&client_info);
	snd_seq_port_info_malloc(&port_info);
	
	numDevices = MFMidi_GetNumPorts(client_info,port_info);
	
	MFDebug_Warn(3,MFStr("ALSA MIDI found %i devices",numDevices));
	
	if (numDevices) pDevices = (MFMidiLinux_MidiDevice*)MFHeap_Alloc(sizeof(MFMidiLinux_MidiDevice)*numDevices);

	int deviceNum = 0;
	snd_seq_client_info_set_client(client_info, -1);
	
	while (snd_seq_query_next_client(sequencer, client_info) >= 0) 
	{
	  // filter clients we are not interested in including us and currently any 
		if ( MFString_Compare(snd_seq_client_info_get_name(client_info),"System") == 0||
			 MFString_Compare(snd_seq_client_info_get_name(client_info),"Midi Through") == 0 ||
			 snd_seq_client_info_get_client(client_info) == dest.client ||
			 snd_seq_client_info_get_type(client_info) == SND_SEQ_USER_CLIENT ) { continue; }
			
		snd_seq_port_info_set_client(port_info, snd_seq_client_info_get_client(client_info));
		snd_seq_port_info_set_port(port_info, -1);
		
		while (snd_seq_query_next_port(sequencer,port_info) >= 0) 
		{
			if ((snd_seq_port_info_get_capability(port_info) & read_bits) == read_bits) 
			{ 
				MFString_Copy(pDevices[deviceNum].info.name,snd_seq_client_info_get_name(client_info));
				pDevices[deviceNum].info.status = MFMS_Available;
				pDevices[deviceNum].send.client = snd_seq_port_info_get_client(port_info);
				pDevices[deviceNum].send.port = snd_seq_port_info_get_port(port_info);
				printf("device %i client %i port %i\n", deviceNum, pDevices[deviceNum].send.client, pDevices[deviceNum].send.port);
				deviceNum++;
			}
		}
	}
	
	snd_seq_client_info_free(client_info);
	snd_seq_port_info_free(port_info);
}

void MFMidi_DeinitModulePlatformSpecific() 
{
	for (int i=0; i < numDevices; ++i) 
	{
		if (pDevices[i].info.status > MFMS_Available) 
		{
			MFDebug_Warn(1, MFStr("MIDI device not closed: %s", pDevices[i].info.name));
			// reset midi
			// close midi
		}
	}
	snd_seq_close(sequencer);
}

void MFMidi_Process (snd_seq_event_t *event) {
//   printf("event->type %i sender %i.%i\n",event->type,event->source.client,event->source.port);
  
  MFMidiLinux_MidiDevice * targetDevice = NULL;
  for (int i=0; i < numDevices; i++) 
  {
	  if (event->source.client == pDevices[i].send.port && event->source.port == pDevices->send.port) 
	  {
		  targetDevice = &(pDevices[i]);
		  break;
	  }
  }
  
  
  if (targetDevice == NULL) { return; }
  
  switch (event->type) 
  {
	case SND_SEQ_EVENT_NOTEON:
	  targetDevice->channels[event->data.control.channel].notes[event->data.note.note] = event->data.note.velocity;
	  break;
	case SND_SEQ_EVENT_NOTEOFF:
	  targetDevice->channels[event->data.control.channel].notes[event->data.note.note] = 0;
	  break;
	case SND_SEQ_EVENT_CONTROLLER:
	  targetDevice->channels[event->data.control.channel].control[event->data.control.param] = event->data.control.value;
	  break;
	case SND_SEQ_EVENT_CHANPRESS:
	  targetDevice->channels[event->data.control.channel].channel_pressure = event->data.control.value;
	  break;
	case SND_SEQ_EVENT_PGMCHANGE:
	  targetDevice->channels[event->data.control.channel].program = event->data.control.value;
	break;
	case SND_SEQ_EVENT_PITCHBEND:
	  targetDevice->channels[event->data.control.channel].pitch = event->data.control.value;
	break;
  }
}

void MFMidi_UpdateInternal()
{
	// TODO reimpliment this in a separate thread with a ring buffer in order to work with lower latencies.
	if (snd_seq_event_input_pending (sequencer,1)) 
	{
		snd_seq_event_t *event = NULL;
		snd_seq_event_input(sequencer,&event);
		do { MFMidi_Process(event); } 
		while (snd_seq_event_input_pending (sequencer,0)); 
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
	MFMidiLinux_MidiDevice *pDevice = &pDevices[deviceId];
	//TODO open midi device
	
	pDevice->bBuffered = bBuffered;
	if (bBuffered) 
	{
		pDevice->numAllocated = 256;
		pDevice->pEvents = (MFMidiEvent*)MFHeap_Alloc(sizeof(MFMidiEvent) * pDevice->numAllocated);
	}
	

	pDevice->info.status = MFMS_Ready;
	
	snd_seq_port_subscribe_t * subscription;
	snd_seq_port_subscribe_malloc(&subscription);
	snd_seq_port_subscribe_set_dest(subscription,&dest);
	snd_seq_port_subscribe_set_queue(subscription, bBuffered);
	snd_seq_port_subscribe_set_exclusive(subscription, 0);
	snd_seq_port_subscribe_set_time_update(subscription, 0);
	snd_seq_port_subscribe_set_time_real(subscription, 0);
	
	for (int i = 0; i < numDevices; i++) 
	{
		snd_seq_port_subscribe_set_sender(subscription,&(pDevices[i].send));
		MFDebug_Warn(3,MFStr("ALSA MIDI: connecting to %i client %i port %i\n",i,pDevices[i].send.client,pDevices[i].send.port));
		if (snd_seq_get_port_subscription(sequencer, subscription) == 0) { MFDebug_Warn (3,"Connection is already subscribed\n"); continue; }
        if (snd_seq_subscribe_port(sequencer, subscription) < 0) { MFDebug_Warn(1,MFStr("Connection failed (%s)\n", snd_strerror(errno)));}
	}
	
	snd_seq_port_subscribe_free(subscription);
	
	return (MFMidiInput*) pDevice;
}

MF_API void MFMidi_CloseInput(MFMidiInput *pMidiInput)
{
	MFMidiLinux_MidiDevice *pDevice = (MFMidiLinux_MidiDevice*)pMidiInput;
	if(pDevice->bBuffered) MFHeap_Free(pDevice->pEvents);
	
	// reset device
	// close device
	// for linux I am just going to unsubscribe.
	
	snd_seq_unsubscribe_port(sequencer,pDevice->subscription);
	snd_seq_port_subscribe_free(pDevice->subscription);

	pDevice->info.status = MFMS_Available;
}

MF_API uint32 MFMidi_GetState (MFMidiInput *pMidiInput, MFMidiDataType type, int channel, int note) 
{
	MFMidiLinux_MidiDevice *pDevice = (MFMidiLinux_MidiDevice *)pMidiInput;
	
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
	MFMidiLinux_MidiDevice *pDevice = (MFMidiLinux_MidiDevice*)pMidiInput;

	return pDevice->channels[channel].notes[note] && !pDevice->channels[channel].lastNotes[note];
}

MF_API uint32 MFMidi_WasReleased(MFMidiInput *pMidiInput, int channel, int note)
{
	MFMidiLinux_MidiDevice *pDevice = (MFMidiLinux_MidiDevice*)pMidiInput;

	return !pDevice->channels[channel].notes[note] && pDevice->channels[channel].lastNotes[note];
}

MF_API bool MFMidi_Start(MFMidiInput *pMidiInput)
{
	MFMidiLinux_MidiDevice *pDevice = (MFMidiLinux_MidiDevice*)pMidiInput;

	MFDebug_Assert(pDevice->info.status == MFMS_Ready, MFStr("Midi device not ready: %s", pDevice->info.name));

	pDevice->numEvents = pDevice->numEventsRead = 0;
	pDevice->info.status = MFMS_Active;
	return true;
}


MF_API void MFMidi_Stop(MFMidiInput *pMidiInput)
{
	MFMidiLinux_MidiDevice *pDevice = (MFMidiLinux_MidiDevice*)pMidiInput;
	pDevice->info.status = MFMS_Ready;
}

MF_API size_t MFMidi_GetEvents(MFMidiInput *pMidiInput, MFMidiEvent *pEvents, size_t maxEvents, bool bPeek)
{
	MFMidiLinux_MidiDevice *pDevice = (MFMidiLinux_MidiDevice*)pMidiInput;

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

#endif // MF_MIDI == MF_DRIVER_LINUX