#include "Common.h"
#include "FileSystem.h"
#include "Sound.h"
#include "PtrList.h"
#include "Font.h"
#include "Primitive.h"


void Sound_InitModule()
{
	CALLSTACK;


#if !defined(_RETAIL)
	DebugMenu_AddMenu("Sound Options", "Fuji Options");
#endif
}

void Sound_DeinitModule()
{
	CALLSTACK;

}

void Sound_Update()
{
	CALLSTACKc;

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

	return -1;
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

