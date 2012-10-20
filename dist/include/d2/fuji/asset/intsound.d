module fuji.asset.intsound;

import fuji.fuji;

enum MFWaveFormat
{
	Unknown = -1,

	PCM_u8,
	PCM_s16,
	PCM_s24,
	PCM_f32
}

struct MFSoundTemplate
{
	int magic;

	MFWaveFormat format = MFWaveFormat.Unknown;
	uint flags;
	int sampleRate;
	int numSamples;
	int bitsPerSample;
	ushort numChannels;
	ushort numStreams;

	char** ppStreams;
}

struct MFIntSound
{
	MFSoundTemplate soundTemplate;
	void* pSampleBuffer;
	void* pInternal;
}

extern (C) MFIntSound *MFIntSound_CreateFromFile(const(char*) pFilename);
extern (C) MFIntSound *MFIntSound_CreateFromFileInMemory(const(void*) pMemory, size_t size, const(char*) pFormatExtension);

extern (C) void MFIntSound_Destroy(MFIntSound* pSound);

extern (C) void MFIntSound_CreateRuntimeData(MFIntSound* pSound, void** ppOutput, size_t* pSize, MFPlatform platform);

