#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_PORTAUDIO

#include "MFHeap.h"
#include "MFPtrList.h"
#include "MFSound_Internal.h"

#include <portaudio.h>

/**** Structures ****/

struct MFSoundDataInternal
{
	uint32 reserved;
};


/**** Globals ****/

extern MFPtrListDL<MFVoice> gVoices;

static const PaHostApiInfo *pHostAPIInfo;
static const PaDeviceInfo *pDeviceInfo;
static PaStream *pPAStream = NULL;


/**** Functions ****/

int MFSound_MixCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *pUserData)
{
	MFZeroMemory(output, frameCount*sizeof(int16)*2);
	MFSoundMixer_MixVoices((int16*)output, frameCount);
	return paContinue;
}

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);

	MFDebug_Log(2, "Initialising PortAudio driver.");

	// init portaudio
	Pa_Initialize();

	// choose the output device
	PaDeviceIndex device = Pa_GetDefaultOutputDevice();

	// HACK: try and find an ALSA device (default OSS sucks)
	PaHostApiIndex alsaAPI = Pa_HostApiTypeIdToHostApiIndex(paALSA);
	if(alsaAPI >= 0)
	{
		int numDevices = Pa_GetDeviceCount();
		for(int a=0; a<numDevices; ++a)
		{
			pDeviceInfo = Pa_GetDeviceInfo(a);
			if(pDeviceInfo->hostApi == alsaAPI)
			{
				device = a;
				break;
			}
		}
	}

	pDeviceInfo = Pa_GetDeviceInfo(device);
	pHostAPIInfo = Pa_GetHostApiInfo(pDeviceInfo->hostApi);

	MFDebug_Log(2, MFStr("PortAudio output: %s", pDeviceInfo->name));
	MFDebug_Log(2, MFStr("PortAudio host: %s", pHostAPIInfo->name));
	MFDebug_Log(2, MFStr("Sample rate: %g", (float)pDeviceInfo->defaultSampleRate));
	MFDebug_Log(2, MFStr("In/Out channels: %d/%d", pDeviceInfo->maxInputChannels, pDeviceInfo->maxOutputChannels));
	MFDebug_Log(2, MFStr("Input latency: %g-%g", (float)pDeviceInfo->defaultLowInputLatency, (float)pDeviceInfo->defaultHighInputLatency));
	MFDebug_Log(2, MFStr("Output latency: %g-%g", (float)pDeviceInfo->defaultLowOutputLatency, (float)pDeviceInfo->defaultHighOutputLatency));

	// create a very low latency audio output stream
	PaStreamParameters params;
	params.device = Pa_GetDefaultOutputDevice();
	params.channelCount = 2;
	params.sampleFormat = paInt16;
	params.suggestedLatency = 0.0167;
	params.hostApiSpecificStreamInfo = NULL;

	PaError error = Pa_OpenStream(&pPAStream, NULL, &params, pDeviceInfo->defaultSampleRate, paFramesPerBufferUnspecified, paPrimeOutputBuffersUsingStreamCallback, MFSound_MixCallback, NULL);

	if(error != paNoError)
		MFDebug_Log(2, MFStr("Error: %s", Pa_GetErrorText(error)));
	else
		Pa_StartStream(pPAStream);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	if(pPAStream)
		Pa_StopStream(pPAStream);

	Pa_Terminate();
}

void MFSound_UpdateInternal()
{
	MFCALLSTACKc;
}

bool MFSound_UpdateVoiceInternal(MFVoice *pVoice)
{
	MFCALLSTACK;

	MFSoundMixer_UpdateVoice(pVoice);
}

void MFSound_PlayInternal(MFVoice *pVoice)
{
	MFCALLSTACK;

	MFSoundMixer_PlayVoice(pVoice);
}

void MFSound_Pause(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;

	MFSoundMixer_PauseVoice(pVoice, pause);
}

void MFSound_Stop(MFVoice *pVoice)
{
	MFCALLSTACK;

	MFSoundMixer_StopVoice(pVoice);
}

void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
}

void MFSound_SetMasterVolume(float volume)
{
}

void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
	MFCALLSTACK;

	MFSoundMixer_SetVolume(pVoice, volume);
}

void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;

	MFSoundMixer_SetPlaybackRate(pVoice, rate);
}

void MFSound_SetPan(MFVoice *pVoice, float pan)
{
	MFCALLSTACK;

	MFSoundMixer_SetPan(pVoice, pan);
}

void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
	MFSoundMixer_SetPlaybackOffset(pVoice, seconds);
}

size_t MFSound_GetPlayCursorInternal(MFVoice *pVoice, size_t *pWriteCursor)
{
	return 	MFSoundMixer_GetPlayCursor(pVoice, pWriteCursor);
}

#endif
