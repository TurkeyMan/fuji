#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_PULSEAUDIO

#include "MFSystem.h"
#include "MFSound_Internal.h"
#include "MFSystem.h"

#include <pulse/pulseaudio.h>

/**** Structures ****/

struct MFSoundDataInternal
{
};

struct MFVoiceDataInternal
{
};

struct pa_devicelist
{
	char name[512];
	char description[256];
	uint32 index;
};

/**** Globals ****/

static pa_mainloop *gpMainLoop;
//static pa_operation *pa_op;
static pa_context *gpContext;

static int gContextState = 0;

extern MFInitParams gInitParams;

pa_devicelist gInputDevices[16];
pa_devicelist gOutputDevices[16];
int gNumInputDevices = 0;
int gumOutputDevices = 0;

/**** Functions ****/

void StateCallback(pa_context *pContext, void *pUserData)
{
	pa_context_state_t state = pa_context_get_state(pContext);
	switch(state)
	{
		// There are just here for reference
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
		default:
			break;
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_TERMINATED:
			gContextState = 2;
			break;
		case PA_CONTEXT_READY:
			gContextState = 1;
			break;
	}
}

void SinkListCallback(pa_context *pContext, const pa_sink_info *pSI, int eol, void *pUserData)
{
	if(eol > 0)
		return;

	if(gumOutputDevices >= 16)
	{
		MFDebug_Warn(1, MFStr("PulseAudio: Too many synks! Ignoring: %s", pSI->name));
		return;
	}

	pa_devicelist &device = gOutputDevices[gumOutputDevices++];
	MFString_Copy(device.name, pSI->name);
	MFString_Copy(device.description, pSI->description);
	device.index = pSI->index;

	MFDebug_Log(2, MFStr("PulseAudio: Sink - %s", device.name));
}

void SourceListCallback(pa_context *pContext, const pa_source_info *pSI, int eol, void *pUserData)
{
	if(eol > 0)
		return;

	if(gNumInputDevices >= 16)
	{
		MFDebug_Warn(1, MFStr("PulseAudio: Too many inputs! Ignoring: %s", pSI->name));
		return;
	}

	pa_devicelist &device = gInputDevices[gNumInputDevices++];
	MFString_Copy(device.name, pSI->name);
	MFString_Copy(device.description, pSI->description);
	device.index = pSI->index;

	MFDebug_Log(2, MFStr("PulseAudio: Input - %s", device.name));
}

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	// Create a mainloop API and connection to the default server
	gpMainLoop = pa_mainloop_new();
	pa_mainloop_api *pMainLoopAPI = pa_mainloop_get_api(gpMainLoop);
	gpContext = pa_context_new(pMainLoopAPI, gInitParams.pAppTitle);

	// This function connects to the pulse server
	MFDebug_Log(2, "PulseAudio: Connecting to server...");
	pa_context_connect(gpContext, NULL, PA_CONTEXT_NOFLAGS, NULL);
	pa_context_set_state_callback(gpContext, StateCallback, NULL);

	// wait for the context to become ready
	while(gContextState == 0)
		pa_mainloop_iterate(gpMainLoop, 1, NULL);

	// if there was an error
	if(gContextState == 2)
	{
		MFDebug_Log(0, "Couldn't connect to PulseAudio server!");
		MFSound_DeinitModulePlatformSpecific();
		return;
	}

	// collect information about the input and output devices available to this machine
	pa_operation *pGetInputOp = pa_context_get_sink_info_list(gpContext, SinkListCallback, NULL);
	pa_operation *pGetOutputOp = pa_context_get_source_info_list(gpContext, SourceListCallback, NULL);

	MFDebug_Log(2, "PulseAudio: Enumerating devices...");
	while(pa_operation_get_state(pGetInputOp) != PA_OPERATION_DONE ||
			pa_operation_get_state(pGetOutputOp) != PA_OPERATION_DONE)
		pa_mainloop_iterate(gpMainLoop, 1, NULL);

	pa_operation_unref(pGetInputOp);
	pa_operation_unref(pGetOutputOp);

	MFDebug_Log(2, "PulseAudio: Ready.");

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	// clean up...

	pa_context_disconnect(gpContext);
	pa_context_unref(gpContext);

	pa_mainloop_free(gpMainLoop);
}

void MFSound_UpdateInternal()
{
	pa_mainloop_iterate(gpMainLoop, 0, NULL);
}

bool MFSound_UpdateVoiceInternal(MFVoice *pVoice)
{
	if(pVoice->flags & MFPF_Paused)
		return false;

/*
	DWORD stat;
	pVoice->pInternal->pBuffer8->GetStatus(&stat);

	if(stat & DSBSTATUS_PLAYING)
		return false;

	// destroy voice
#if !defined(MF_XBOX)
	if(pVoice->pInternal->p3DBuffer8)
		pVoice->pInternal->p3DBuffer8->Release();
#endif
	pVoice->pInternal->pBuffer8->Release();
	pVoice->pInternal->pBuffer->Release();
*/
	return true;
}

void MFSound_PlayInternal(MFVoice *pVoice)
{
	MFCALLSTACK;

	MFSound *pSound = pVoice->pSound;
/*
#if !defined(MF_XBOX)
	if(pVoice->flags & MFPF_3D && pSound->pInternal->p3DBuffer)
	{
		pDirectSound->DuplicateSoundBuffer(pSound->pInternal->p3DBuffer, &pVoice->pInternal->pBuffer);
		pVoice->pInternal->pBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pVoice->pInternal->p3DBuffer8);
	}
	else
#endif
	{
		pDirectSound->DuplicateSoundBuffer(pSound->pInternal->pBuffer, &pVoice->pInternal->pBuffer);
		pVoice->pInternal->p3DBuffer8 = NULL;
	}

	pVoice->pInternal->pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (VOID**)&pVoice->pInternal->pBuffer8);

	if(!(pVoice->flags & MFPF_BeginPaused))
	{
		DWORD playbackFlags = (pVoice->flags & MFPF_Looping) ? DSBPLAY_LOOPING : 0;
		pVoice->pInternal->pBuffer8->Play(0, 0, playbackFlags);
	}
	else
		pVoice->flags |= MFPF_Paused;
*/
}

MF_API void MFSound_Pause(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;
/*
	if(pause && !(pVoice->flags & MFPF_Paused))
	{
		pVoice->pInternal->pBuffer8->Stop();
		pVoice->flags |= MFPF_Paused;
	}
	else if(!pause && (pVoice->flags & MFPF_Paused))
	{
		DWORD playbackFlags = (pVoice->flags & MFPF_Looping) ? DSBPLAY_LOOPING : 0;
		pVoice->pInternal->pBuffer8->Play(0, 0, playbackFlags);
		pVoice->flags &= ~MFPF_Paused;
	}
*/
}

MF_API void MFSound_Stop(MFVoice *pVoice)
{
	MFCALLSTACK;
/*
	pVoice->pInternal->pBuffer8->Stop();
*/
}

MF_API void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
	MFCALLSTACK;

}

MF_API void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
	MFCALLSTACK;
/*
	pVoice->pInternal->pBuffer8->SetVolume(MFSoundInternal_GetDecibels(volume));
*/
}

MF_API void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;
/*
	pVoice->pInternal->pBuffer8->SetFrequency((DWORD)((float)pVoice->pSound->pTemplate->sampleRate * rate));
*/
}

MF_API void MFSound_SetPan(MFVoice *pVoice, float pan)
{
/*
	if(pVoice->pSound->pTemplate->numChannels == 2)
	{
		if(pan >= 0)
		{
			float level = 1-pan;
			pVoice->pInternal->pBuffer8->SetPan(-MFSoundInternal_GetDecibels(level));
		}
		else
		{
			float level = 1+pan;
			pVoice->pInternal->pBuffer8->SetPan(MFSoundInternal_GetDecibels(level));
		}
	}
*/
}

MF_API void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
	MFCALLSTACK;

	MFSoundTemplate *pT = pVoice->pSound->pTemplate;
/*
	DWORD offset = (DWORD)((float)((pT->sampleRate * pT->numChannels * pT->bitsPerSample) >> 3) * seconds);
	pVoice->pInternal->pBuffer8->SetCurrentPosition(offset);
*/
}

MF_API void MFSound_SetMasterVolume(float volume)
{
	MFCALLSTACK;
/*
	pDSPrimaryBuffer->SetVolume(MFSoundInternal_GetDecibels(volume));
*/
}

uint32 MFSound_GetPlayCursorInternal(MFVoice *pVoice, uint32 *pWriteCursor)
{
	MFCALLSTACK;

	uint32 play, write;
/*
	pVoice->pInternal->pBuffer8->GetCurrentPosition(&play, &write);
*/
	if(pWriteCursor)
		*pWriteCursor = write;

	return play;
}

#endif // MF_SOUND
