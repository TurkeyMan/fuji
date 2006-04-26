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


int MFSound_LoadBank(const char *pFilename)
{
	MFCALLSTACK;

	return -1;
}

void MFSound_UnloadBank(int bankID)
{
	MFCALLSTACK;


}

int MFSound_FindSound(const char *pSoundName, int searchBankID)
{
	MFCALLSTACK;

	return -1;
}

int MFSound_Play(int soundID)
{
	MFCALLSTACK;

	return -1;
}

int MFSound_Play3D(int soundID)
{
	MFCALLSTACK;

	return -1;
}

void MFSound_Stop(int voice)
{
	MFCALLSTACK;


}

void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
	MFCALLSTACK;

}

void MFSound_SetVolume(int voice, float volume)
{
	MFCALLSTACK;

}

void MFSound_SetPlaybackRate(int voice, float rate)
{
	MFCALLSTACK;

}

void MFSound_SetMasterVolume(float volume)
{
	MFCALLSTACK;

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

