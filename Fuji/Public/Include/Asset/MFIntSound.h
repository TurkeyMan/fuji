#if !defined(_MFINTSOUND_H)
#define _MFINTSOUND_H

struct MFIntSound
{
	MFSoundTemplate soundTemplate;
	void *pSampleBuffer;
	void *pInternal;
};

MFIntSound *MFIntSound_CreateFromFile(const char *pFilename);
MFIntSound *MFIntSound_CreateFromFileInMemory(const void *pMemory, size_t size, const char *pFormatExtension);

void MFIntSound_Destroy(MFIntSound *pSound);

void MFIntSound_CreateRuntimeData(MFIntSound *pSound, void **ppOutput, size_t *pSize, MFPlatform platform);

#endif
