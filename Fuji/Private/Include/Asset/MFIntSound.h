#if !defined(_MFINTSOUND_H)
#define _MFINTSOUND_H

enum MFIntSoundFormat
{
	MFISF_Unknown = -1,

	MFISF_WAV = 0,

	MFISF_Max,
	MFISF_ForceInt = 0x7FFFFFFF
};

struct MFIntSound
{
	int x;
};

MFIntSound *MFIntTexture_CreateFromFile(const char *pFilename);
MFIntSound *MFIntTexture_CreateFromFileInMemory(const void *pMemory, uint32 size, MFIntSoundFormat format);

void MFIntTexture_Destroy(MFIntSound *pSound);

void MFIntTexture_CreateRuntimeData(MFIntSound *pSound, void **ppOutput, uint32 *pSize, MFPlatform platform);

#endif
