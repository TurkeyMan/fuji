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

	FMUSIC_GetSamples(pMod, (char*)pBuffer, bytes);
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
	pStream->trackLength = 1000.f;

	// fill out the stream info
	pStream->streamInfo.sampleRate = 44100;
	pStream->streamInfo.channels = 2;
	pStream->streamInfo.bitsPerSample = 16;
	pStream->streamInfo.bufferLength = pStream->streamInfo.sampleRate;
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

	// attempt to cache the mod file stream (music_formatxm.cpp does a lot of random file access, not good from a .zip)
	MFOpenDataCachedFile cachedOpen;
	cachedOpen.cbSize = sizeof(MFOpenDataCachedFile);
	cachedOpen.openFlags = MFOF_Read | MFOF_Binary | MFOF_Cached_CleanupBaseFile;
	cachedOpen.maxCacheSize = 128*1024; // 128k cache will make a big .xm load fast..
	cachedOpen.pBaseFile = hFile;

	MFFile *pCachedFile = MFFile_Open(MFFileSystem_GetInternalFileSystemHandle(MFFSH_CachedFileSystem), &cachedOpen);
	if(pCachedFile)
		hFile = pCachedFile;

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

void* MiniFMOD_Alloc(unsigned int bytes)
{
	return MFHeap_Alloc(bytes);
}

void MiniFMOD_Free(void *pMem)
{
	MFHeap_Free(pMem);
}

void MFSound_RegisterMiniFMOD()
{
	// register FSOUND callbacks
	FSOUND_File_SetCallbacks(MiniFMOD_OpenCallback, MiniFMOD_CloseCallback, MiniFMOD_ReadCallback, MiniFMOD_SeekCallback, MiniFMOD_TellCallback);
	FSOUND_Memory_SetCallbacks(MiniFMOD_Alloc, MiniFMOD_Free);

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
