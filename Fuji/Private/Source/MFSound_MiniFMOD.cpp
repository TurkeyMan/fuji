#include "Fuji.h"

#if defined(MINIFMOD_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

#include "minifmod170/lib/minifmod.h"
#include "minifmod170/lib/system_file.h"

int GetMiniFMODSamples(MFAudioStream *pStream, void *pBuffer, uint32 bytes)
{
	FMUSIC_MODULE *pMod = (FMUSIC_MODULE*)pStream->pStreamData;

	FSOUND_Software_Fill(pMod, (char*)pBuffer, bytes);
	return bytes;
}

void DestroyMiniFMODStream(MFAudioStream *pStream)
{
	FMUSIC_MODULE *pMod = (FMUSIC_MODULE*)pStream->pStreamData;
	FMUSIC_FreeSong(pMod);
}

void CreateMiniFMODStream(MFAudioStream *pStream, const char *pFilename)
{
	MFCALLSTACK;

	// init the decoder
	FMUSIC_MODULE *pMod = FMUSIC_LoadSong((char*)pFilename, NULL);
	if(!pMod)
		return;

	pStream->pStreamData = pMod;

	int sampleRate = 44100;
	int numSamples = sampleRate;
	int channels = 2;

	pStream->trackLength = 1000.f;
	pStream->bufferSize = numSamples * channels * 2;
	pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, numSamples, channels, 16, sampleRate, MFSF_Dynamic | MFSF_Circular);

	if(!pStream->pStreamBuffer)
		DestroyMiniFMODStream(pStream);
}

void SeekMiniFMODStream(MFAudioStream *pStream, float seconds)
{
	FMUSIC_MODULE *pMod = (FMUSIC_MODULE*)pStream->pStreamData;
	// this is not so easy :/
	// we'll just have to speed-play the mod and not fix the samples...
}

float GetMiniFMODTime(MFAudioStream *pStream)
{
	FMUSIC_MODULE *pMod = (FMUSIC_MODULE*)pStream->pStreamData;
	return (float)FMUSIC_GetTime(pMod) * 0.001f;
}

// fmod filesystem callbacks
unsigned int MiniFMOD_OpenCallback(char *pFilename)
{
	// open mp3 file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return 0;
/*
	// attempt to cache the mod file stream
	MFOpenDataCachedFile cachedOpen;
	cachedOpen.cbSize = sizeof(MFOpenDataCachedFile);
	cachedOpen.openFlags = MFOF_Read | MFOF_Binary | MFOF_Cached_CleanupBaseFile;
	cachedOpen.maxCacheSize = 8*1024; // 8k cache for a mod should be plenty...
	cachedOpen.pBaseFile = hFile;

	MFFile *pCachedFile = MFFile_Open(MFFileSystem_GetInternalFileSystemHandle(MFFSH_CachedFileSystem), &cachedOpen);
	if(pCachedFile)
		hFile = pCachedFile;
*/
	return (unsigned int)hFile;
}

void MiniFMOD_CloseCallback(unsigned int handle)
{
	MFFile_Close((MFFile*)handle);
}

int MiniFMOD_ReadCallback(void *buffer, int size, unsigned int handle)
{
	return MFFile_Read((MFFile*)handle, buffer, size);
}

void MiniFMOD_SeekCallback(unsigned int handle, int pos, signed char mode)
{
	MFFile_Seek((MFFile*)handle, pos, (MFFileSeek)mode);
}

int MiniFMOD_TellCallback(unsigned int handle)
{
	return MFFile_Tell((MFFile*)handle);
}

void MFSound_RegisterMiniFMOD()
{
	// register FSOUND callbacks
	FSOUND_File_SetCallbacks(MiniFMOD_OpenCallback, MiniFMOD_CloseCallback, MiniFMOD_ReadCallback, MiniFMOD_SeekCallback, MiniFMOD_TellCallback);

	MFStreamCallbacks callbacks;
	callbacks.pCreateStream = CreateMiniFMODStream;
	callbacks.pDestroyStream = DestroyMiniFMODStream;
	callbacks.pGetSamples = GetMiniFMODSamples;
	callbacks.pGetTime = GetMiniFMODTime;
	callbacks.pSeekStream = SeekMiniFMODStream;

	MFSound_RegisterStreamHandler("ProTracker Sequence", ".mod", &callbacks);
	MFSound_RegisterStreamHandler("FastTracker Sequence", ".xm", &callbacks);
}
#endif
