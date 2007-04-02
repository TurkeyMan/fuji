#if !defined(_MFSOUND_INTERNAL_H)
#define _MFSOUND_INTERNAL_H

#include "MFSound.h"

void MFSound_InitModule();
void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize);
void MFSound_DeinitModule();
void MFSound_DeinitModulePlatformSpecific();

void MFSound_Update();
bool MFSound_UpdateInternal(MFVoice *pVoice);

void MFSound_Draw();

void MFSound_CreateInternal(MFSound *pSound);
void MFSound_DestroyInternal(MFSound *pSound);

void MFSound_PlayInternal(MFVoice *pVoice);

uint32 MFSound_GetPlayCursor(MFVoice *pVoice, uint32 *pWriteCursor = NULL);


enum MFWaveFormat
{
	MFWF_Unknown = -1,

	MFWF_PCM_u8,
	MFWF_PCM_s16,
	MFWF_PCM_32f,

	MFWF_Max,
	MFWF_ForceInt = 0x7FFFFFFF
};

struct MFSoundDataInternal;
struct MFVoiceDataInternal;

struct MFSoundTemplate
{
	int magic;

	MFWaveFormat format;
	uint32 flags;
	int sampleRate;
	int numSamples;
	int bitsPerSample;
	uint16 numChannels;
	uint16 numStreams;

	char **ppStreams;
};

struct MFSound
{
	char name[64];

	MFSoundTemplate *pTemplate;
	int flags;
	int refCount;

	MFSoundDataInternal *pInternal;

	// for locking dynamic buffers
	int lockOffset, lockBytes;
	void *pLock1, *pLock2;
	uint32 lockSize1, lockSize2;
};

struct MFVoice
{
	MFSound *pSound;
	MFVoiceDataInternal *pInternal;
	uint32 flags;
};

enum MFPlayFlagsInternal
{
	MFPF_Paused = MFBIT(28),
	MFPF_Locked = MFBIT(29)
};

#endif
