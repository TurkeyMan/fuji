#if !defined(_SOUND_H)
#define _SOUND_H

int Sound_LoadBank(const char *pFilename);
void Sound_UnloadBank(int bankID);

int Sound_FindSound(const char *pSoundName, int searchBankID = 0);

void Sound_Play(int soundID);
void Sound_Stop(int soundID);

void Sound_Play3D(int soundID);
void Sound_Stop3D(int soundID);

void Sound_SetListenerPos(const Vector3& listenerPos);

#endif
