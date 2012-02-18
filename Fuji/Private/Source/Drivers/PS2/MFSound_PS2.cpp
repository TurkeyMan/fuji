#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_PS2

#include "MFVector.h"

MFInitStatus MFSound_InitModule()
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


int MFSound_LoadBank(const char *pFilename)
{
	return 0;
}

void MFSound_UnloadBank(int bankID)
{
}

int MFSound_FindSound(const char *pSoundName, int searchBankID)
{
	return 0;
}

void MFSound_Play(int soundID)
{
}

void MFSound_Stop(int soundID)
{
}

void MFSound_Play3D(int soundID)
{
}

void MFSound_Stop3D(int soundID)
{
}

void MFSound_SetListenerPos(const MFVector& listenerPos)
{
}

void MFSound_SetVolume(int soundID, float volume)
{
}

void MFSound_SetMasterVolume(int soundID, float volume)
{
}

void MFSound_SetPlaybackRate(int soundID, float rate)
{
}

int MFSound_MusicPlay(const char *pFilename, bool pause)
{
	return 0;
}

void MFSound_MusicUnload(int track)
{
}

void MFSound_MusicSeek(int track, float seconds)
{
}

void MFSound_MusicPause(int track, bool pause)
{
}

void MFSound_MusicSetVolume(int track, float volume)
{
}

#endif
