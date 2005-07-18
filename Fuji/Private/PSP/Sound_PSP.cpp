#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"
#include "Sound.h"
#include "PtrList.h"
#include "Font.h"
#include "Primitive.h"
#include "DebugMenu_Internal.h"

void Sound_InitModule()
{
	CALLSTACK;
}

void Sound_DeinitModule()
{
	CALLSTACK;
}

void Sound_Update()
{
	CALLSTACK;
}

void Sound_Draw()
{
	CALLSTACK;
}


int Sound_LoadBank(const char *pFilename)
{
	CALLSTACK;

	return -1;
}

void Sound_UnloadBank(int bankID)
{
	CALLSTACK;

}

int Sound_FindSound(const char *pSoundName, int searchBankID)
{
	CALLSTACK;

	return -1;
}

void Sound_Play(int soundID)
{
	CALLSTACK;


}

void Sound_Stop(int soundID)
{
	CALLSTACK;


}

void Sound_Play3D(int soundID)
{
	CALLSTACK;


}

void Sound_Stop3D(int soundID)
{
	CALLSTACK;


}

void Sound_SetListenerPos(const Vector3& listenerPos)
{
	CALLSTACK;


}


void Sound_SetVolume(int soundID, float volume)
{
	CALLSTACK;


}

void Sound_SetMasterVolume(int soundID, float volume)
{
	CALLSTACK;

//	pDSPrimaryBuffer->SetVolume();
}

void Sound_SetPlaybackRate(int soundID, float rate)
{
	CALLSTACK;


}


//
// Music Functions
//
int Sound_MusicPlay(const char *pFilename, bool pause)
{
	CALLSTACK;

	return 0;
}

void Sound_ServiceMusicBuffer(int trackID)
{
	CALLSTACK;
}

void Sound_MusicUnload(int track)
{
	CALLSTACK;
}

void Sound_MusicSeek(int track, float seconds)
{
	CALLSTACK;
}

void Sound_MusicPause(int track, bool pause)
{
	CALLSTACK;
}

void Sound_MusicSetVolume(int track, float volume)
{
	CALLSTACK;
}

