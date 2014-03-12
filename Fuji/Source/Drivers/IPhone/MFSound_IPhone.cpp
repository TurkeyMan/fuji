#include "Fuji_Internal.h"

#if MF_SOUND == MF_DRIVER_IPHONE

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

#if 0
void MFSound_CreateInternal(MFSound *pSound)
{
}

void MFSound_DestroyInternal(MFSound *pSound)
{
}

MF_API int MFSound_Lock(MFSound *pSound, size_t offset, size_t bytes, void **ppData, uint32 *pSize, void **ppData2, uint32 *pSize2)
{
	return 0;
}

MF_API void MFSound_Unlock(MFSound *pSound)
{
}
#endif

void MFSound_PlayInternal(MFVoice *pVoice)
{
}

MF_API void MFSound_Pause(MFVoice *pVoice, bool pause)
{
}

MF_API void MFSound_Stop(MFVoice *pVoice)
{
}

MF_API void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
}

MF_API void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
}

MF_API void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
}

MF_API void MFSound_SetPan(MFVoice *pVoice, float pan)
{
}

MF_API void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
}

MF_API void MFSound_SetMasterVolume(float volume)
{
}

size_t MFSound_GetPlayCursorInternal(MFVoice *pVoice, size_t *pWriteCursor)
{
	return 0;
}

#endif
