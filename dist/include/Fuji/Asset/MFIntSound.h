#if !defined(_MFINTSOUND_H)
#define _MFINTSOUND_H

struct MFIntSound
{
	MFSoundTemplate soundTemplate;
	void *pSampleBuffer;
	void *pInternal;
};

MF_API MFIntSound *MFIntSound_CreateFromFile(const char *pFilename);
MF_API MFIntSound *MFIntSound_CreateFromFileInMemory(const void *pMemory, size_t size, const char *pFormatExtension);

MF_API void MFIntSound_Destroy(MFIntSound *pSound);

MF_API void MFIntSound_CreateRuntimeData(MFIntSound *pSound, void **ppOutput, size_t *pSize, MFPlatform platform);

#endif
