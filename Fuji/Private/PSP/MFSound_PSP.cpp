#include "Fuji.h"
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

uint32 gSampleCount = 0;

extern MFPtrListDL<MFVoice> gVoices;

extern MFMutex gSoundMutex;

float gSoundTime = 0.0f;

/**** Functions ****/

void audioOutCallback(int channel, int16 *pBuf, unsigned int reqn)
{
	// mix and write samples...
	int16 *pData = pBuf;
	for(uint32 a=0; a<reqn; a++)
	{
		pData[0] = 0;
		pData[1] = 0;

		pData += 2;
	}

//	MFThread_LockMutex(gSoundMutex);

	MFVoice **ppI = gVoices.Begin();

	while(*ppI)
	{
		MFVoice *pVoice = (*ppI);
		MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
		MFSoundTemplate *pT = pVoice->pSound->pTemplate;

		if(pVoice->flags & MFPF_Paused || pInt->bFinished)
			continue;

		uint32 offset = pInt->offset;
		int lVolume = pInt->lVolume;
		int rVolume = pInt->lVolume;
		uint32 rate = (uint32)((float)0x400 * ((float)pT->sampleRate / 44100.0f) * pInt->rate);

		pData = pBuf;

		uint32 numSamples = reqn;

		while(numSamples)
		{
			if(pT->bitsPerSample == 16)
			{
				int16 *pSamples = (int16*)pT->ppStreams[0];

				if(pT->numChannels == 1)
				{
					while(numSamples)
					{
						int sample = offset >> 10;

						if(sample >= pT->numSamples)
							break;

						// mix in this voice..
						pData[1] = pData[0] += (int16)(((int32)pSamples[sample] * lVolume) >> 15);

						offset += rate;
						pData += 2;
						--numSamples;
					}
				}
				else if(pT->numChannels == 2)
				{
					while(numSamples)
					{
						int sample = offset >> 10;

						if(sample >= pT->numSamples)
							break;

						sample *= 2;

						// mix in this voice..
						pData[0] += (int16)(((int32)pSamples[sample] * lVolume) >> 15);
						pData[1] += (int16)(((int32)pSamples[sample + 1] * rVolume) >> 15);

						offset += rate;
						pData += 2;
						--numSamples;
					}
				}
				else
				{
					MFDebug_Assert(false, "Unsupported channel count..");
				}
			}
			else if(pT->bitsPerSample == 8)
			{
				uint8 *pSamples = (uint8*)pT->ppStreams[0];

				MFDebug_Assert(false, "8bit samples not yet supported..");
			}
			else
			{
				MFDebug_Assert(false, "Unsupported number of bits per sample..");
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
		}

		pInt->offset = offset;

		++ppI;
	}

//	MFThread_ReleaseMutex(gSoundMutex);

	gSampleCount += reqn;
}

void audioOutCallback0(void *buf, unsigned int reqn, void *userdata) { audioOutCallback(0, (int16*)buf, reqn); }

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	pspAudioInit();
	pspAudioSetVolume(0, 0x4000, 0x4000);
	pspAudioSetChannelCallback(0, audioOutCallback0, NULL);

	gSoundTime = 0.0f;

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	// cleanup?
}

bool MFSound_UpdateInternal(MFVoice *pVoice)
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
		pSound->pTemplate->ppStreams[0] = (char*)MFHeap_Alloc(bufferSize);
	}
}

void MFSound_DestroyInternal(MFSound *pSound)
{
	MFCALLSTACK;

	// if dynamic, free buffer
	if(pSound->pTemplate->flags & MFSF_Dynamic)
	{
		MFHeap_Free(pSound->pTemplate->ppStreams[0]);
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
}

void MFSound_Pause(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;

	if(pause)
		pVoice->flags |= MFPF_Paused;
	else if(!pause)
		pVoice->flags &= ~MFPF_Paused;
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
}

void MFSound_SetMasterVolume(float volume)
{
}

uint32 MFSound_GetPlayCursor(MFVoice *pVoice, uint32 *pWriteCursor)
{
	return 0;
}
