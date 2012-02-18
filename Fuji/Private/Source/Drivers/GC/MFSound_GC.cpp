#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_GC

#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"
#include "MFSound.h"
#include "MFPtrList.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "DebugMenu_Internal.h"

MFInitStatus MFSound_InitModule()
{
	MFCALLSTACK;
}

void MFSound_DeinitModule()
{
	MFCALLSTACK;
}

void MFSound_Update()
{
	MFCALLSTACK;
}

void MFSound_Draw()
{
	MFCALLSTACK;
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

	return 0;
}

void MFSound_Stop(int soundID)
{
	MFCALLSTACK;


}

int MFSound_Play3D(int soundID)
{
	MFCALLSTACK;

	return 0;
}

void MFSound_Stop3D(int soundID)
{
	MFCALLSTACK;


}

void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
	MFCALLSTACK;


}


void MFSound_SetVolume(int soundID, float volume)
{
	MFCALLSTACK;


}

void MFSound_SetMasterVolume(int soundID, float volume)
{
	MFCALLSTACK;

//	pDSPrimaryBuffer->SetVolume();
}

void MFSound_SetPlaybackRate(int soundID, float rate)
{
	MFCALLSTACK;


}


//
// Music Functions
//
int MFSound_MusicPlay(const char *pFilename, bool pause)
{
	MFCALLSTACK;

	return 0;
}

void MFSound_ServiceMusicBuffer(int trackID)
{
	MFCALLSTACK;
}

void MFSound_MusicUnload(int track)
{
	MFCALLSTACK;
}

void MFSound_MusicSeek(int track, float seconds)
{
	MFCALLSTACK;
}

void MFSound_MusicPause(int track, bool pause)
{
	MFCALLSTACK;
}

void MFSound_MusicSetVolume(int track, float volume)
{
	MFCALLSTACK;
}

#endif
