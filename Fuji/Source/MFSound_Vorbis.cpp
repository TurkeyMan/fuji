#include "Fuji_Internal.h"

#if defined(VORBIS_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

// vorbis stream handler
#if defined(MF_WINDOWS) || defined(MF_XBOX)
	#pragma comment(lib, "ogg_static")
	#pragma comment(lib, "vorbis_static")
	#pragma comment(lib, "vorbisfile_static")
#endif

#if defined(VORBIS_TREMOR)
	#include <tremor/ivorbisfile.h>
#else
	#include <vorbis/vorbisfile.h>
#endif

struct MFVorbisStream
{
	OggVorbis_File vorbisFile;
	vorbis_info *pInfo;
	vorbis_comment *pComment;
};

int MFSound_VorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	MFFile_Seek((MFFile*)datasource, offset, (MFFileSeek)whence);
	return 0;
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

	// fill out the stream info
	pStream->streamInfo.sampleRate = pVS->pInfo->rate;
	pStream->streamInfo.channels = pVS->pInfo->channels;
	pStream->streamInfo.bitsPerSample = 16;
	pStream->streamInfo.bufferLength = pVS->pInfo->rate;

	// read the vorbis comment data
	pVS->pComment = ov_comment(&pVS->vorbisFile, -1);
	if(pVS->pComment)
	{
		const char *pTitle = vorbis_comment_query(pVS->pComment, "TITLE", 0);
		const char *pArtist = vorbis_comment_query(pVS->pComment, "ALBUM", 0);
		const char *pAlbum = vorbis_comment_query(pVS->pComment, "ARTIST", 0);
		const char *pGenre = vorbis_comment_query(pVS->pComment, "GENRE", 0);

		if(pTitle)
			MFString_CopyN(pStream->streamInfo.songName, pTitle, sizeof(pStream->streamInfo.songName)-1);
		if(pArtist)
			MFString_CopyN(pStream->streamInfo.artistName, pArtist, sizeof(pStream->streamInfo.artistName)-1);
		if(pAlbum)
			MFString_CopyN(pStream->streamInfo.albumName, pAlbum, sizeof(pStream->streamInfo.albumName)-1);
		if(pGenre)
			MFString_CopyN(pStream->streamInfo.genre, pGenre, sizeof(pStream->streamInfo.genre)-1);
	}
}

size_t GetVorbisSamples(MFAudioStream *pStream, void *pBuffer, size_t bytes)
{
	MFVorbisStream *pVS = (MFVorbisStream*)pStream->pStreamData;

	int currentBitstream;
#if defined(VORBIS_TREMOR)
	return ov_read(&pVS->vorbisFile, (char*)pBuffer, (int)bytes, &currentBitstream);
#else
	return ov_read(&pVS->vorbisFile, (char*)pBuffer, (int)bytes, 0, 2, 1, &currentBitstream);
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
