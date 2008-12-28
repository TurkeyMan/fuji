#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_JACK

#include "MFHeap.h"
#include "MFPtrList.h"
#include "MFSound_Internal.h"

#include <jack/jack.h>

/**** Structures ****/

struct MFSoundDataInternal
{
};

struct MFVoiceDataInternal
{
	float volume;
	float pan;
	float rate;

	uint16 lVolume;
	uint16 rVolume;
	uint32 offset;
	bool bFinished;
};


/**** Globals ****/

extern MFPtrListDL<MFVoice> gVoices;

jack_client_t *pJack;
jack_status_t gJackStatus;

/**** Functions ****/

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);

	pJack = jack_client_open("MFSound_JACK", 0, &gJackStatus);
	if(!pJack)
		return;

	jack_activate(pJack);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	jack_deactivate(pJack);
	jack_client_close(pJack);
}

void MFSound_UpdateInternal()
{
	MFCALLSTACKc;
}

bool MFSound_UpdateVoiceInternal(MFVoice *pVoice)
{
	MFCALLSTACK;

	// check if the voice has finished playing and destroy it if it has..
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	return pInt->bFinished;
}

void MFSound_CreateInternal(MFSound *pSound)
{
	MFCALLSTACK;

	// if dynamic, allocate buffer
	if(pSound->pTemplate->flags & MFSF_Dynamic)
	{
		long bufferSize = ((pSound->pTemplate->numChannels * pSound->pTemplate->bitsPerSample) >> 3) * pSound->pTemplate->numSamples;
		pSound->pTemplate->ppStreams = (char**)MFHeap_Alloc(sizeof(char*) + bufferSize);
		pSound->pTemplate->ppStreams[0] = (char*)&pSound->pTemplate->ppStreams[1];
	}
}

void MFSound_DestroyInternal(MFSound *pSound)
{
	MFCALLSTACK;

	// if dynamic, free buffer
	if(pSound->pTemplate->flags & MFSF_Dynamic)
	{
		MFHeap_Free(pSound->pTemplate->ppStreams);
	}
}

int MFSound_Lock(MFSound *pSound, int offset, int bytes, void **ppData, uint32 *pSize, void **ppData2, uint32 *pSize2)
{
	MFCALLSTACK;

	MFDebug_Assert(!(pSound->flags & MFPF_Locked), MFStr("Dynamic sound '%s' is already locked.", pSound->name));

	long bufferSize = ((pSound->pTemplate->numChannels * pSound->pTemplate->bitsPerSample) >> 3) * pSound->pTemplate->numSamples;

	MFDebug_Assert(offset < bufferSize, "MFSound_Lock: Invalid buffer offset.");

	pSound->pLock1 = pSound->pTemplate->ppStreams[0] + offset;

	if(offset + bytes > bufferSize)
	{
		pSound->lockSize1 = bufferSize - offset;
		pSound->pLock2 = pSound->pTemplate->ppStreams[0];
		pSound->lockSize2 = bytes - pSound->lockSize1;
	}
	else
	{
		pSound->lockSize1 = bytes;
		pSound->pLock2 = NULL;
		pSound->lockSize2 = 0;
	}

	pSound->flags |= MFPF_Locked;
	pSound->lockOffset = offset;
	pSound->lockBytes = bytes;

	*ppData = pSound->pLock1;
	*pSize = pSound->lockSize1;
	if(ppData2)
	{
		*ppData2 = pSound->pLock2;
		*pSize2 = pSound->lockSize2;
	}

	return 0;
}

void MFSound_Unlock(MFSound *pSound)
{
	MFCALLSTACK;

	MFDebug_Assert(pSound->flags & MFPF_Locked, MFStr("Dynamic sound '%s' is not locked.", pSound->name));

	pSound->pLock1 = NULL;
	pSound->lockSize1 = 0;
	pSound->pLock2 = NULL;
	pSound->lockSize2 = 0;

	pSound->flags = pSound->flags & ~MFPF_Locked;
}

void MFSound_PlayInternal(MFVoice *pVoice)
{
	MFCALLSTACK;

	// we should put a pointer to the voice in some internal playing list...
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;

	pInt->volume = 1.0f;
	pInt->pan = 0.0f;
	pInt->rate = 1.0f;

	pInt->offset = 0;

	pInt->lVolume = 0x8000;
	pInt->rVolume = 0x8000;

	pInt->bFinished = false;

	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	MFDebug_Assert(pT->numChannels <= 2, "Unsupported channel count..");
	MFDebug_Assert(pT->bitsPerSample == 16 || pT->bitsPerSample == 8, "Unsupported number of bits per sample..");

	MFDebug_Assert(pT->bitsPerSample != 8, "8bit samples not yet supported..");

	if(pVoice->flags & MFPF_BeginPaused)
		pVoice->flags |= MFPF_Paused;
	else
		PrimeBuffer(pVoice);
}

void MFSound_Pause(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;

	if(pause)
		pVoice->flags |= MFPF_Paused;
	else if(!pause)
	{
		PrimeBuffer(pVoice);
		pVoice->flags &= ~MFPF_Paused;
	}
}

void MFSound_Stop(MFVoice *pVoice)
{
	MFCALLSTACK;

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->bFinished = true;
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

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->volume = volume;

	pInt->lVolume = (uint16)(volume * MFMin(1.0f - pInt->pan, 1.0f) * (float)0x8000);
	pInt->rVolume = (uint16)(volume * MFMin(1.0f + pInt->pan, 1.0f) * (float)0x8000);
}

void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->rate = rate;
}

void MFSound_SetPan(MFVoice *pVoice, float pan)
{
	MFCALLSTACK;

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->pan = pan;

	pInt->lVolume = (uint16)(pInt->volume * MFMin(1.0f - pan, 1.0f) * (float)0x8000);
	pInt->rVolume = (uint16)(pInt->volume * MFMin(1.0f + pan, 1.0f) * (float)0x8000);
}

void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;

	pInt->offset = MFMin((uint32)((float)pVoice->pSound->pTemplate->sampleRate * seconds), (uint32)pVoice->pSound->pTemplate->numSamples) << 10;
}

uint32 MFSound_GetPlayCursorInternal(MFVoice *pVoice, uint32 *pWriteCursor)
{
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	int bytesPerSample = (pT->numChannels * pT->bitsPerSample) >> 3;

	if(pWriteCursor)
		*pWriteCursor = (pInt->offset >> 10) * bytesPerSample;

	return (pInt->offset >> 10) * bytesPerSample;
}

#endif
