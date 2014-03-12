#include "Fuji_Internal.h"

#if defined(WAV_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

struct WAVChunk
{
	uint32 id;
	int32 size;
};

struct WAVFormatChunk
{
	int16 wFormatTag;
	uint16 nChannels;
	uint32 nSamplesPerSec;
	uint32 nAvgBytesPerSec;
	uint16 wBlockAlign;
	uint16 wBitsPerSample;
	uint16 cbSize; 
};

struct RIFFHeader
{
	uint32 RIFF;
	int32 size;
	uint32 WAVE;
};

struct MFWAVStream
{
	WAVFormatChunk format;

	MFFile *pStream;

	size_t dataOffset;
	size_t dataSize;
	size_t sampleOffset;
	size_t sampleSize;
};

void DestroyWAVStream(MFAudioStream *pStream)
{
	MFWAVStream *pWS = (MFWAVStream*)pStream->pStreamData;
	MFFile_Close(pWS->pStream);
	MFHeap_Free(pWS);
}

void CreateWAVStream(MFAudioStream *pStream, const char *pFilename)
{
	MFCALLSTACK;

	// open wav file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return;

	// attempt to cache the vorbis stream
	MFOpenDataCachedFile cachedOpen;
	cachedOpen.cbSize = sizeof(MFOpenDataCachedFile);
	cachedOpen.openFlags = MFOF_Read | MFOF_Binary | MFOF_Cached_CleanupBaseFile;
	cachedOpen.maxCacheSize = 256*1024; // 256k cache for wav stream should be heaps!!
	cachedOpen.pBaseFile = hFile;

	MFFile *pCachedFile = MFFile_Open(MFFileSystem_GetInternalFileSystemHandle(MFFSH_CachedFileSystem), &cachedOpen);
	if(pCachedFile)
		hFile = pCachedFile;

	RIFFHeader header;
	MFFile_Read(hFile, &header, sizeof(header));

	if(header.RIFF != MFMAKEFOURCC('R', 'I', 'F', 'F') || header.WAVE != MFMAKEFOURCC('W', 'A', 'V', 'E'))
		return;	// not a .wav file...

	// if everything's good, and it appears to be a valid wav file
	MFWAVStream *pWS = (MFWAVStream*)MFHeap_AllocAndZero(sizeof(MFWAVStream));
	pStream->pStreamData = pWS;
	pWS->pStream = hFile;

	int read;
	int fptr = sizeof(header);
	do
	{
		MFFile_Seek(hFile, fptr, MFSeek_Begin);

		WAVChunk dataChunk;
		MFZeroMemory(&dataChunk, sizeof(dataChunk));
		read = MFFile_Read(hFile, &dataChunk, sizeof(dataChunk));
		fptr += sizeof(dataChunk) + dataChunk.size;

		if(dataChunk.id == MFMAKEFOURCC('f', 'm', 't', ' '))
		{
			read = MFFile_Read(hFile, &pWS->format, dataChunk.size);
			if(pWS->format.cbSize)
				read = MFFile_Seek(hFile, pWS->format.cbSize, MFSeek_Current);
		}
		else if(dataChunk.id == MFMAKEFOURCC('d', 'a', 't', 'a'))
		{
			pWS->dataOffset = MFFile_Tell(hFile);
			pWS->dataSize = dataChunk.size;
		}
	}
	while(read);

	// return to the start of the audio data
	MFFile_Seek(pWS->pStream, (int)pWS->dataOffset, MFSeek_Begin);

	// calculate the track length
	pWS->sampleSize = (pWS->format.nChannels * pWS->format.wBitsPerSample) >> 3;
	pStream->trackLength = (float)(pWS->dataSize / pWS->sampleSize) / (float)pWS->format.nSamplesPerSec;

	// fill out the stream info
	pStream->streamInfo.sampleRate = pWS->format.nSamplesPerSec;
	pStream->streamInfo.channels = pWS->format.nChannels;
	pStream->streamInfo.bitsPerSample = pWS->format.wBitsPerSample;
	pStream->streamInfo.bufferLength = pWS->format.nSamplesPerSec;
}

size_t GetWAVSamples(MFAudioStream *pStream, void *pBuffer, size_t bytes)
{
	MFWAVStream *pWS = (MFWAVStream*)pStream->pStreamData;
	size_t read = MFFile_Read(pWS->pStream, pBuffer, MFMin(bytes, pWS->dataSize - pWS->sampleOffset));
	pWS->sampleOffset += read;
	return read;
}

void SeekWAVStream(MFAudioStream *pStream, float seconds)
{
	MFWAVStream *pWS = (MFWAVStream*)pStream->pStreamData;
	pWS->sampleOffset = (uint32)(seconds * (float)pWS->format.nSamplesPerSec) * pWS->sampleSize;
	MFFile_Seek(pWS->pStream, (int)(pWS->dataOffset + pWS->sampleOffset), MFSeek_Begin);
}

float GetWAVTime(MFAudioStream *pStream)
{
	MFWAVStream *pWS = (MFWAVStream*)pStream->pStreamData;
	return (float)(pWS->sampleOffset / pWS->sampleSize) / (float)pWS->format.nSamplesPerSec;
}

void MFSound_RegisterWAV()
{
	MFStreamCallbacks callbacks;
	callbacks.pCreateStream = CreateWAVStream;
	callbacks.pDestroyStream = DestroyWAVStream;
	callbacks.pGetSamples = GetWAVSamples;
	callbacks.pGetTime = GetWAVTime;
	callbacks.pSeekStream = SeekWAVStream;

	MFSound_RegisterStreamHandler("WAV Audio", ".wav", &callbacks);
}
#endif
