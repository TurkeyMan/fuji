#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_PSP

#include "MFSound_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"
#include "MFPtrList.h"

#include <pspkernel.h>
#include <pspaudiolib.h>


/**** Structures ****/

struct MFSoundDataInternal
{
	uint32 reserved;
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

#define MASTER_BUFFER_SAMPLES (44100/4)

static int16 gMasterBuffer[MASTER_BUFFER_SAMPLES * 2];
static const int16 * const pMasterEnd = gMasterBuffer + MASTER_BUFFER_SAMPLES*2;

static uint32 gMasterOffset = 0;
static uint32 gMixOffset = 0;

static uint32 gSampleCount = 0;

extern MFPtrListDL<MFVoice> gVoices;

/**** Functions ****/

void audioOutCallback(int channel, int16 *pBuf, unsigned int reqn)
{
	uint32 numSamples = MFMin(reqn, MASTER_BUFFER_SAMPLES - gMasterOffset);

	if(numSamples)
		MFCopyMemory(pBuf, gMasterBuffer + gMasterOffset*2, sizeof(int16)*2*numSamples);

	if(reqn > numSamples)
	{
		gMasterOffset = 0;
		gSampleCount += numSamples;
		reqn -= numSamples;

		pBuf += numSamples * 2;
		MFCopyMemory(pBuf, gMasterBuffer, sizeof(int16)*2*reqn);
	}

	gMasterOffset += reqn;
	gSampleCount += reqn;
}

void audioOutCallback0(void *buf, unsigned int reqn, void *userdata) { audioOutCallback(0, (int16*)buf, reqn); }


void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	MFZeroMemory(gMasterBuffer, sizeof(gMasterBuffer));

	pspAudioInit();
	pspAudioSetVolume(0, 0x4000, 0x4000);
	pspAudioSetChannelCallback(0, audioOutCallback0, NULL);

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	pspAudioEnd();
}

void MixVoice(MFVoice *pVoice, uint32 startSample, uint32 numSamples)
{
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	uint32 offset = pInt->offset;
	int lVolume = pInt->lVolume;
	int rVolume = pInt->lVolume;
	uint32 rate = (uint32)((float)0x400 * ((float)pT->sampleRate / 44100.0f) * pInt->rate);

	MFDebug_Log(4, MFStr("MasterOffset: %d NumSamples: %d", startSample, numSamples));

	int16 *pData = &gMasterBuffer[startSample*2];

	while(numSamples)
	{
		if(pT->bitsPerSample == 16)
		{
			int16 *pSamples = (int16*)pT->ppStreams[0];

			if(pT->numChannels == 1)
			{
				while(numSamples && pData < pMasterEnd)
				{
					int sample = offset >> 10;

					if(sample >= pT->numSamples)
						break;

					// mix in this voice..
					int attemuatedSample = ((int)pSamples[sample] * lVolume) >> 15;
					int l = (int)pData[0] + attemuatedSample;
					int r = (int)pData[1] + attemuatedSample;
					pData[0] = (int16)MFClamp(-32768, l, 32767);
					pData[1] = (int16)MFClamp(-32768, r, 32767);

					offset += rate;
					pData += 2;
					--numSamples;
				}
			}
			else if(pT->numChannels == 2)
			{
				while(numSamples && pData < pMasterEnd)
				{
					int sample = offset >> 10;

					if(sample >= pT->numSamples)
						break;

					sample *= 2;

					// mix in this voice..
					int l = (int)pData[0] + (((int)pSamples[sample] * lVolume) >> 15);
					int r = (int)pData[1] + (((int)pSamples[sample + 1] * rVolume) >> 15);
					pData[0] = (int16)MFClamp(-32768, l, 32767);
					pData[1] = (int16)MFClamp(-32768, r, 32767);

					offset += rate;
					pData += 2;
					--numSamples;
				}
			}
		}
		else if(pT->bitsPerSample == 8)
		{
//				uint8 *pSamples = (uint8*)pT->ppStreams[0];
		}

		if((offset >> 10) >= (uint32)pT->numSamples)
		{
			if(pVoice->flags & MFPF_Looping)
				offset -= pT->numSamples << 10;
			else
			{
				pInt->bFinished = true;
				numSamples = 0;
			}
		}

		if(pData >= pMasterEnd)
			pData = gMasterBuffer;
	}

	pInt->offset = offset;
}

void PrimeBuffer(MFVoice *pVoice)
{
	// HACK: we'll mix in the first bit of the sample to reduce trigger lag...
	uint32 offset = gMasterOffset + 1024;
	if(offset > MASTER_BUFFER_SAMPLES)
		offset -= MASTER_BUFFER_SAMPLES;

	uint32 numSamples = offset < gMixOffset ? gMixOffset - offset : (MASTER_BUFFER_SAMPLES - offset) + gMixOffset;

	MixVoice(pVoice, offset, numSamples);
}

void MFSound_UpdateInternal()
{
	MFCALLSTACKc;

	// if the master buffer hasn't progressed, bail out
	if(gMixOffset == gMasterOffset)
		return;

	// mix and write samples...

	// erase the part of the buffer we are about to write to
	int16 *pData = &gMasterBuffer[gMixOffset*2];

	if(gMixOffset < gMasterOffset)
	{
		MFZeroMemory(pData, sizeof(uint16) * (gMasterOffset - gMixOffset) * 2);
	}
	else
	{
		MFZeroMemory(pData, sizeof(uint16) * (pMasterEnd - pData));
		MFZeroMemory(gMasterBuffer, sizeof(uint16) * 2 * gMasterOffset);
	}

	// iterate voices and mix into master buffer
	uint32 numSamples = gMasterOffset > gMixOffset ? gMasterOffset-gMixOffset : (MASTER_BUFFER_SAMPLES - gMixOffset) + gMasterOffset;

	MFVoice **ppI = gVoices.Begin();

	while(*ppI)
	{
		MFVoice *pVoice = (*ppI);
		MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;

		if(!(pVoice->flags & MFPF_Paused || pInt->bFinished))
			MixVoice(pVoice, gMixOffset, numSamples);

		++ppI;
	}

	gMixOffset = gMasterOffset;
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

	MFDebug_Assert(offset < bufferSize, MFStr("MFSound_Lock: Invalid buffer offset.", pSound->name));

	pSound->pLock1 = pSound->pTemplate->ppStreams[0] + offset;

	if(offset + bytes > bufferSize)
	{
		pSound->lockSize1 = bufferSize - offset;
		pSound->pLock2 = pSound->pTemplate->ppStreams[0];
		pSound->lockSize2 = bytes - *pSize;
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

void MFSound_SetMasterVolume(float volume)
{
}

uint32 MFSound_GetPlayCursor(MFVoice *pVoice, uint32 *pWriteCursor)
{
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	int bytesPerSample = (pT->numChannels * pT->bitsPerSample) >> 3;

	if(pWriteCursor)
		*pWriteCursor = (pInt->offset >> 10) * bytesPerSample;

	return (pInt->offset >> 10) * bytesPerSample;
}

#endif
