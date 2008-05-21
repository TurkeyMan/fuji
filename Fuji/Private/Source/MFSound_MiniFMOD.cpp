#include "Fuji.h"

#if defined(MINIFMOD_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

#include "minifmod170/lib/minifmod.h"
#include "minifmod170/lib/system_file.h"

#pragma comment(lib, "winmm.lib")

struct MFMiniFMODStream
{
	FMUSIC_MODULE *pMod;
};

int GetMiniFMODSamples(MFAudioStream *pStream, void *pBuffer, uint32 bytes)
{
	MFMiniFMODStream *pModStream = (MFMiniFMODStream*)pStream->pStreamData;

	int written = 0;
	return written;
}

void DestroyMiniFMODStream(MFAudioStream *pStream)
{
	MFMiniFMODStream *pModStream = (MFMiniFMODStream*)pStream->pStreamData;
	FMUSIC_StopSong(pModStream->pMod);
	FMUSIC_FreeSong(pModStream->pMod);
	MFHeap_Free(pModStream);
}

void CreateMiniFMODStream(MFAudioStream *pStream, const char *pFilename)
{
	MFCALLSTACK;

	// init the decoder
	MFMiniFMODStream *pModStream = (MFMiniFMODStream*)MFHeap_Alloc(sizeof(MFMiniFMODStream));
	pStream->pStreamData = pModStream;
	pModStream->pMod = FMUSIC_LoadSong((char*)pFilename, NULL);
	FMUSIC_PlaySong(pModStream->pMod);
}

void SeekMiniFMODStream(MFAudioStream *pStream, float seconds)
{
	MFMiniFMODStream *pModStream = (MFMiniFMODStream*)pStream->pStreamData;

}

float GetMiniFMODTime(MFAudioStream *pStream)
{
	MFMiniFMODStream *pModStream = (MFMiniFMODStream*)pStream->pStreamData;
	return 0.0f;
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
