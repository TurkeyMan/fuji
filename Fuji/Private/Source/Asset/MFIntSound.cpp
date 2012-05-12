#include "Fuji.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFString.h"
#include "MFSound_Internal.h"
#include "Asset/MFIntSound.h"

/**** Structures ****/

MF_API MFIntSound *MFIntSound_CreateFromFile(const char *pFilename)
{
	MFAudioStream *pStream = MFSound_CreateStream(pFilename, MFASF_DecodeOnly | MFASF_QueryLength);
	if(!pStream)
		return NULL;

	MFIntSound *pSound = (MFIntSound*)MFHeap_Alloc(sizeof(MFIntSound));

	pSound->pInternal = pStream;

	pSound->soundTemplate.magic = MFMAKEFOURCC('S', 'N', 'D', '1');
	pSound->soundTemplate.format = pStream->streamInfo.bitsPerSample == 8 ? MFWF_PCM_u8 : MFWF_PCM_s16;
	pSound->soundTemplate.flags = 0;
	pSound->soundTemplate.sampleRate = pStream->streamInfo.sampleRate;
	pSound->soundTemplate.numSamples = 0;
	pSound->soundTemplate.bitsPerSample = pStream->streamInfo.bitsPerSample;
	pSound->soundTemplate.numChannels = (uint16)pStream->streamInfo.channels;
	pSound->soundTemplate.numStreams = 1;

	return pSound;
}

MF_API MFIntSound *MFIntSound_CreateFromFileInMemory(const void *pMemory, size_t size, const char *pFormatExtension)
{
	// create a memory file, and then use CreateFromFile()
	return NULL;
}

MF_API void MFIntSound_Destroy(MFIntSound *pSound)
{
	MFSound_DestroyStream((MFAudioStream*)pSound->pInternal);
	MFHeap_Free(pSound);
}

MF_API void MFIntSound_CreateRuntimeData(MFIntSound *pSound, void **ppOutput, size_t *pSize, MFPlatform platform)
{
	*ppOutput = NULL;

	MFAudioStream *pStream = (MFAudioStream*)pSound->pInternal;

	// decode audio into buffer
	uint32 sampleSize = (pSound->soundTemplate.bitsPerSample * pSound->soundTemplate.numChannels) >> 3;
	uint32 bytesAllocated = 44100 * sampleSize;
	uint32 bytes = 0;

	char *pAudioData = (char*)MFHeap_Alloc(bytesAllocated);

	int read;
	do
	{
		// read samples from stream
		read = MFSound_ReadStreamSamples(pStream, pAudioData + bytes, bytesAllocated - bytes);
		bytes += read;

		// if we fill the buffer, increase it's size
		if(bytes == bytesAllocated)
		{
			bytesAllocated *= 4;
			pAudioData = (char*)MFHeap_Realloc(pAudioData, bytesAllocated);
		}
	}
	while(read);

	// calculate the number of samples from the bytes read
	int numSamples = bytes / sampleSize;

	// construct MFSoundTemplate
	size_t templateBytes = sizeof(MFSoundTemplate) + sizeof(char*)*pSound->soundTemplate.numStreams + sampleSize*numSamples;
	MFSoundTemplate *pTemplate = (MFSoundTemplate*)MFHeap_Alloc(templateBytes);
	MFCopyMemory(pTemplate, &pSound->soundTemplate, sizeof(MFSoundTemplate));
	pTemplate->numSamples = numSamples;
	pTemplate->ppStreams = (char**)&pTemplate[1];
	pTemplate->ppStreams[0] = (char*)&pTemplate->ppStreams[1];

	// copy sample data
	MFCopyMemory(pTemplate->ppStreams[0], pAudioData, sampleSize * numSamples);

	// free decode buffer
	MFHeap_Free(pAudioData);

	// fix down pointers
	for(int a=0; a<pTemplate->numStreams; a++)
		MFFixUp(pTemplate->ppStreams[a], pTemplate, false);
	MFFixUp(pTemplate->ppStreams, pTemplate, false);

	// return template data
	*ppOutput = pTemplate;
	if(pSize)
		*pSize = templateBytes;
}
