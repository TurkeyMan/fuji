#if !defined(_MFSOUND_INTERNAL_H)
#define _MFSOUND_INTERNAL_H

#include "MFSound.h"

void MFSound_InitModule();
void MFSound_DeinitModule();

void MFSound_Update();
void MFSound_Draw();

enum MFWaveFormat
{
	MFWF_Unknown = -1,

	MFWF_PCM_u8,
	MFWF_PCM_s16,
	MFWF_PCM_32f,

	MFWF_Max,
	MFWF_ForceInt = 0x7FFFFFFF
};

struct MFSoundTemplate
{
	int magic;

	MFWaveFormat format;
	int sampleRate;
	int numSamples;
	int bitsPerSample;
	int numChannels;
	int numStreams;

	char **ppStreams;
};

enum MFSoundFlagsInternal
{
	MFSF_Paused = MFBIT(28),
	MFSF_Locked = MFBIT(29)
};

#endif
