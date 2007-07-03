#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_NULL

#include "MFSound_Internal.h"

/**** Structures ****/

struct MFSoundDataInternal
{
	uint32 reserved;
};

struct MFVoiceDataInternal
{
	uint32 reserved;
};


/**** Globals ****/


/**** Functions ****/

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;
}

void MFSound_UpdateInternal()
{
}

bool MFSound_UpdateVoiceInternal(MFVoice *pVoice)
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

#endif
