#if !defined(_MFINTSOUND_H)
#define _MFINTSOUND_H

struct MFIntSound
{
	MFSoundTemplate soundTemplate;
	void *pSampleBuffer;
	void *pInternal;
};

MFIntSound *MFIntSound_CreateFromFile(const char *pFilename);
MFIntSound *MFIntSound_CreateFromFileInMemory(const void *pMemory, uint32 size, const char *pFormatExtension);

void MFIntSound_Destroy(MFIntSound *pSound);

void MFIntSound_CreateRuntimeData(MFIntSound *pSound, void **ppOutput, uint32 *pSize, MFPlatform platform);

#endif
