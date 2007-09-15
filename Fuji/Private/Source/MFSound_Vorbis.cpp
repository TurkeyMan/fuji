#include "Fuji.h"

#if defined(VORBIS_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

// vorbis stream handler

#if defined(VORBIS_TREMOR)
	#include <tremor/ivorbisfile.h>
#else
	#include <vorbis/vorbisfile.h>
#endif

struct MFVorbisStream
{
	OggVorbis_File vorbisFile;
	vorbis_info *pInfo;
};

int MFSound_VorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	return MFFile_Seek((MFFile*)datasource, (int)offset, (MFFileSeek)whence);
}

void DestroyVorbisStream(MFAudioStream *pStream)
{
	MFVorbisStream *pVS = (MFVorbisStream*)pStream->pStreamData;

	pVS->pInfo = NULL;
	ov_clear(&pVS->vorbisFile);

	MFHeap_Free(pVS);
}

void CreateVorbisStream(MFAudioStream *pStream, const char *pFilename)
{
	MFCALLSTACK;

	MFVorbisStream *pVS = (MFVorbisStream*)MFHeap_Alloc(sizeof(MFVorbisStream));
	pStream->pStreamData = pVS;

	// open vorbis file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return;

	// attempt to cache the vorbis stream
	MFOpenDataCachedFile cachedOpen;
	cachedOpen.cbSize = sizeof(MFOpenDataCachedFile);
	cachedOpen.openFlags = MFOF_Read | MFOF_Binary | MFOF_Cached_CleanupBaseFile;
	cachedOpen.maxCacheSize = 256*1024; // 256k cache for vorbis stream should be heaps!!
	cachedOpen.pBaseFile = hFile;

	MFFile *pCachedFile = MFFile_Open(MFFileSystem_GetInternalFileSystemHandle(MFFSH_CachedFileSystem), &cachedOpen);
	if(pCachedFile)
		hFile = pCachedFile;

	// setup vorbis read callbacks
	ov_callbacks callbacks;
	callbacks.read_func = MFFile_StdRead;
	callbacks.seek_func = MFSound_VorbisSeek;
	callbacks.close_func = MFFile_StdClose;
	callbacks.tell_func = MFFile_StdTell;

	// open vorbis file
	if(ov_test_callbacks(hFile, &pVS->vorbisFile, NULL, 0, callbacks))
	{
		MFDebug_Assert(false, "Not a vorbis file.");
		MFHeap_Free(pVS);
		return;
	}

	ov_test_open(&pVS->vorbisFile);

	// get vorbis file info
	pVS->pInfo = ov_info(&pVS->vorbisFile, -1);

#if defined(VORBIS_TREMOR)
//	pStream->trackLength = (float)ov_pcm_total(&pVS->vorbisFile, -1) / (float)pVS->pInfo->rate;
	pStream->trackLength = 1000.0f;
#else
	pStream->trackLength = (float)ov_time_total(&pVS->vorbisFile, -1);
#endif
	pStream->bufferSize = pVS->pInfo->rate * pVS->pInfo->channels * 2;

	pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, pVS->pInfo->rate, pVS->pInfo->channels, 16, pVS->pInfo->rate, MFSF_Dynamic | MFSF_Circular);

	if(!pStream->pStreamBuffer)
		DestroyVorbisStream(pStream);
}

int GetVorbisSamples(MFAudioStream *pStream, void *pBuffer, uint32 bytes)
{
	MFVorbisStream *pVS = (MFVorbisStream*)pStream->pStreamData;

	int currentBitstream;
#if defined(VORBIS_TREMOR)
	return ov_read(&pVS->vorbisFile, (char*)pBuffer, bytes, &currentBitstream);
#else
	return ov_read(&pVS->vorbisFile, (char*)pBuffer, bytes, 0, 2, 1, &currentBitstream);
#endif
}

void SeekVorbisStream(MFAudioStream *pStream, float seconds)
{
	MFVorbisStream *pVS = (MFVorbisStream*)pStream->pStreamData;
#if defined(VORBIS_TREMOR)
	ov_pcm_seek(&pVS->vorbisFile, (ogg_int64_t)(seconds*(float)pVS->pInfo->rate));
#else
	ov_time_seek(&pVS->vorbisFile, seconds);
#endif
}

float GetVorbisTime(MFAudioStream *pStream)
{
	MFVorbisStream *pVS = (MFVorbisStream*)pStream->pStreamData;
#if defined(VORBIS_TREMOR)
	return (float)ov_pcm_tell(&pVS->vorbisFile) / (float)pVS->pInfo->rate;
#else
	return (float)ov_time_tell(&pVS->vorbisFile);
#endif
}

void MFSound_RegisterVorbis()
{
	MFStreamCallbacks callbacks;
	callbacks.pCreateStream = CreateVorbisStream;
	callbacks.pDestroyStream = DestroyVorbisStream;
	callbacks.pGetSamples = GetVorbisSamples;
	callbacks.pGetTime = GetVorbisTime;
	callbacks.pSeekStream = SeekVorbisStream;

	MFSound_RegisterStreamHandler("Vorbis Audio", ".ogg", &callbacks);
}
#endif
