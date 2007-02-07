#include "Fuji.h"
#include "MFVector.h"

void MFSound_InitModule()
{
}

void MFSound_DeinitModule()
{
}

void MFSound_Update()
{
}

void MFSound_Draw()
{
}


MFSound *MFSound_Create(const char *pName)
{
	return NULL;
}

void MFSound_Destroy(MFSound *pSound)
{
}

MFSound *MFSound_FindSound(const char *pName)
{
	return NULL;
}

MFVoice *MFSound_Play(MFSound *pSound, uint32 playFlags)
{
	return NULL;
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

void MFSound_SetMasterVolume(float volume)
{
}

MFAudioStream *MFSound_PlayStream(const char *pFilename, bool pause)
{
	return NULL;
}

void MFSound_DestroyStream(MFAudioStream *pStream)
{
}

void MFSound_SeekStream(MFAudioStream *pStream, float seconds)
{
}

void MFSound_PauseStream(MFAudioStream *pStream, bool pause)
{
}

void MFSound_SetStreamVolume(MFAudioStream *pStream, float volume)
{
}

