#include "Fuji_Internal.h"

#if defined(USE_MFMIXER)

#include "MFSound_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"
#include "MFPtrList.h"

/**** Structures ****/

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


/**** Functions ****/

void MFSoundMixer_Init(int *pVoiceDataSize)
{
	MFCALLSTACK;

	// return the size of the mixers voice structure
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSoundMixer_Deinit()
{
	MFCALLSTACK;
}

void MFSoundMixer_MixVoice_Stereo16(MFVoice *pVoice, int16 *pOutput, uint32 numSamples)
{
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	uint32 offset = pInt->offset;
	int lVolume = pInt->lVolume;
	int rVolume = pInt->lVolume;
	uint32 rate = (uint32)((float)0x400 * ((float)pT->sampleRate / 44100.0f) * pInt->rate);

	int16 *pData = pOutput;

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
					int attenuatedSample = ((int)pSamples[sample] * lVolume) >> 15;
					int l = (int)pData[0] + attenuatedSample;
					int r = (int)pData[1] + attenuatedSample;
					pData[0] = (int16)MFClamp(-32768, l, 32767);
					pData[1] = (int16)MFClamp(-32768, r, 32767);

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
//			uint8 *pSamples = (uint8*)pT->ppStreams[0];
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
}

void MFSoundMixer_MixVoices(int16 *pBuffer, int numSamples)
{
	MFCALLSTACKc;

	MFVoice **ppI = gVoices.Begin();
	while(*ppI)
	{
		MFVoice *pVoice = (*ppI);
		MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;

		if(!(pVoice->flags & MFPF_Paused || pInt->bFinished))
			MFSoundMixer_MixVoice_Stereo16(pVoice, pBuffer, numSamples);

		++ppI;
	}
}

bool MFSoundMixer_UpdateVoice(MFVoice *pVoice)
{
	MFCALLSTACK;

	// check if the voice has finished playing and destroy it if it has..
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	return pInt->bFinished;
}

void MFSoundMixer_PlayVoice(MFVoice *pVoice)
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
}

void MFSoundMixer_PauseVoice(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;

	if(pause)
		pVoice->flags |= MFPF_Paused;
	else if(!pause)
		pVoice->flags &= ~MFPF_Paused;
}

void MFSoundMixer_StopVoice(MFVoice *pVoice)
{
	MFCALLSTACK;

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->bFinished = true;
}

void MFSoundMixer_SetVolume(MFVoice *pVoice, float volume)
{
	MFCALLSTACK;

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->volume = volume;

	pInt->lVolume = (uint16)(volume * MFMin(1.0f - pInt->pan, 1.0f) * (float)0x8000);
	pInt->rVolume = (uint16)(volume * MFMin(1.0f + pInt->pan, 1.0f) * (float)0x8000);
}

void MFSoundMixer_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->rate = rate;
}

void MFSoundMixer_SetPan(MFVoice *pVoice, float pan)
{
	MFCALLSTACK;

	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	pInt->pan = pan;

	pInt->lVolume = (uint16)(pInt->volume * MFMin(1.0f - pan, 1.0f) * (float)0x8000);
	pInt->rVolume = (uint16)(pInt->volume * MFMin(1.0f + pan, 1.0f) * (float)0x8000);
}

void MFSoundMixer_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;

	pInt->offset = MFMin((uint32)((float)pVoice->pSound->pTemplate->sampleRate * seconds), (uint32)pVoice->pSound->pTemplate->numSamples) << 10;
}

uint32 MFSoundMixer_GetPlayCursor(MFVoice *pVoice, uint32 *pWriteCursor)
{
	MFVoiceDataInternal *pInt = (MFVoiceDataInternal*)pVoice->pInternal;
	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	int bytesPerSample = (pT->numChannels * pT->bitsPerSample) >> 3;

	uint32 playCursor = (pInt->offset >> 10) * bytesPerSample;

	if(pWriteCursor)
		*pWriteCursor = playCursor;
	return playCursor;
}

#endif
