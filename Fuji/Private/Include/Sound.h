#if !defined(_SOUND_H)
#define _SOUND_H

void Sound_InitModule();
void Sound_DeinitModule();

void Sound_Update();
void Sound_Draw();

int Sound_LoadBank(const char *pFilename);
void Sound_UnloadBank(int bankID);

int Sound_FindSound(const char *pSoundName, int searchBankID = 0);

void Sound_Play(int soundID);
void Sound_Stop(int soundID);

void Sound_Play3D(int soundID);
void Sound_Stop3D(int soundID);

void Sound_SetListenerPos(const Vector3& listenerPos);

void Sound_SetVolume(int soundID, float volume);
void Sound_SetMasterVolume(int soundID, float volume);

void Sound_SetPlaybackRate(int soundID, float rate);


int Sound_MusicPlay(const char *pFilename, bool pause = false);
void Sound_MusicUnload(int track);

void Sound_MusicSeek(int track, float seconds);
void Sound_MusicPause(int track, bool pause);
void Sound_MusicSetVolume(int track, float volume);

#endif
