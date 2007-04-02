#include "Fuji.h"
#include "MFSound_Internal.h"

#include <pspkernel.h>
#include <pspaudiolib.h>


/**** Structures ****/

struct MFSoundDataInternal
{
};

struct MFVoiceDataInternal
{
};


/**** Globals ****/


/**** Functions ****/

void audioOutCallback(int channel, uint16 *buf, unsigned int reqn)
{
	// mix and write samples...
/*
	ChannelState_t* state = &channelStates[channel];
	unsigned int i;
	for (i = 0; i < reqn; i++) {
		float time = ((float) state->currentTime) / ((float) SAMPLE_RATE);
		if (state->currentTime++ == state->currentNote.duration) nextNote(channel);
		float value;
		if (state->currentsampleIncrement == 0.0) {
			value = 0.0;
		} else {
			value = sample[(int)state->currentsampleIndex] * adsr(time, ((float) state->currentNote.duration) / ((float) SAMPLE_RATE));
			value *= (float) 0x7000;
			state->currentsampleIndex += state->currentsampleIncrement;
			if (state->currentsampleIndex >= SAMPLE_COUNT) state->currentsampleIndex -= (float) SAMPLE_COUNT;
		}
		buf[0] = value;
		buf[1] = value;
		buf += 2;
	}
*/
}

void audioOutCallback0(void *buf, unsigned int reqn, void *userdata) { audioOutCallback(0, (uint16*)buf, reqn); }
void audioOutCallback1(void *buf, unsigned int reqn, void *userdata) { audioOutCallback(1, (uint16*)buf, reqn); }

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	pspAudioInit();
//	pspAudioSetVolume(0, 0x4000, 0x4000);
//	pspAudioSetVolume(1, 0x4000, 0x4000);
//	pspAudioSetChannelCallback(0, audioOutCallback0, NULL);
//	pspAudioSetChannelCallback(1, audioOutCallback1, NULL);

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
	// check if the voice has finished playing and destroy it if it has..
	return true;
}

void MFSound_CreateInternal(MFSound *pSound)
{
}

void MFSound_DestroyInternal(MFSound *pSound)
{
}

int MFSound_Lock(MFSound *pSound, int offset, int bytes, void **ppData, uint32 *pSize, void **ppData2, uint32 *pSize2)
{
	return 0;
}

void MFSound_Unlock(MFSound *pSound)
{
}

void MFSound_PlayInternal(MFVoice *pVoice)
{
}

void MFSound_Pause(MFVoice *pVoice, bool pause)
{
}

void MFSound_Stop(MFVoice *pVoice)
{
}

void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
}

void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
}

void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
}

void MFSound_SetPan(MFVoice *pVoice, float pan)
{
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
