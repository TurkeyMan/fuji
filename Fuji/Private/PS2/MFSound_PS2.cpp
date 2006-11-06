#include "Fuji.h"
#include "MFVector.h"

void Sound_InitModule()
{
}

void Sound_DeinitModule()
{
}

void Sound_Update()
{
}

void Sound_Draw()
{
}


int Sound_LoadBank(const char *pFilename)
{
	return 0;
}

void Sound_UnloadBank(int bankID)
{
}

int Sound_FindSound(const char *pSoundName, int searchBankID)
{
	return 0;
}

void Sound_Play(int soundID)
{
}

void Sound_Stop(int soundID)
{
}

void Sound_Play3D(int soundID)
{
}

void Sound_Stop3D(int soundID)
{
}

void Sound_SetListenerPos(const MFVector& listenerPos)
{
}

void Sound_SetVolume(int soundID, float volume)
{
}

void Sound_SetMasterVolume(int soundID, float volume)
{
}

void Sound_SetPlaybackRate(int soundID, float rate)
{
}

int Sound_MusicPlay(const char *pFilename, bool pause)
{
	return 0;
}

void Sound_MusicUnload(int track)
{
}

void Sound_MusicSeek(int track, float seconds)
{
}

void Sound_MusicPause(int track, bool pause)
{
}

void Sound_MusicSetVolume(int track, float volume)
{
}
