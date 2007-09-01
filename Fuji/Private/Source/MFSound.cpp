#include "Fuji.h"
#include "MFSound_Internal.h"
#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "MFThread.h"
#include "DebugMenu.h"

#if defined(MF_WINDOWS)
	#define VORBIS_STREAM
	#define MAD_STREAM
#endif

#if defined(_PSP)
	#define VORBIS_STREAM
//	#define VORBIS_TREMOR
#endif

#if defined(VORBIS_STREAM)
	#if defined(VORBIS_TREMOR)
		#include <tremor/ivorbisfile.h>
	#else
		#include <vorbis/vorbisfile.h>
	#endif

	void MFSound_RegisterVorbis();
#endif

#if defined(MAD_STREAM)
	#include <mad.h>

	void MFSound_RegisterMAD();
#endif


/**** Foreward Declarations ****/

void MFSound_FillBuffer(MFAudioStream *pStream, int bytes);
void MFSound_ServiceStreamBuffer(MFAudioStream *pStream);


/**** Structures ****/


/**** Globals ****/

MFPtrListDL<MFSound> gSoundBank;
MFPtrListDL<MFVoice> gVoices;
MFPtrList<MFStreamHandler> gStreamHandlers;

int internalSoundDataSize = 0;
int internalVoiceDataSize = 0;

static MFAudioStream *gMusicTracks;

#if !defined(_RETAIL)
MenuItemBool showSoundStats;
#endif


/**** Functions ****/

void MFSound_InitModule()
{
	MFCALLSTACK;

	MFSound_InitModulePlatformSpecific(&internalSoundDataSize, &internalVoiceDataSize);

	gSoundBank.Init("Sound Bank", gDefaults.sound.maxSounds, sizeof(MFSound) + internalSoundDataSize);
	gVoices.Init("Voice Bank", gDefaults.sound.maxVoices, sizeof(MFVoice) + internalVoiceDataSize);
	gStreamHandlers.Init("Stream handlers", 16);

	gMusicTracks = (MFAudioStream*)MFHeap_Alloc(sizeof(MFAudioStream) * gDefaults.sound.maxMusicTracks);
	MFZeroMemory(gMusicTracks, sizeof(MFAudioStream)*gDefaults.sound.maxMusicTracks);

#if defined(VORBIS_STREAM)
	MFSound_RegisterVorbis();
#endif
#if defined(MAD_STREAM)
	MFSound_RegisterMAD();
#endif

#if !defined(_RETAIL)
	DebugMenu_AddMenu("Sound Options", "Fuji Options");
	DebugMenu_AddItem("Show Sound Stats", "Sound Options", &showSoundStats);
#endif
}

void MFSound_DeinitModule()
{
	MFCALLSTACK;

	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		if(gMusicTracks[a].pStreamBuffer)
		{
			MFSound_DestroyStream(&gMusicTracks[a]);
		}
	}

	// list all non-freed textures...
	MFSound **ppI = gSoundBank.Begin();
	bool bShowHeader = true;

	while(*ppI)
	{
		if(bShowHeader)
		{
			bShowHeader = false;
			MFDebug_Message("\nUn-freed sounds:\n----------------------------------------------------------");
		}

		MFDebug_Message(MFStr("'%s' - x%d", (*ppI)->name, (*ppI)->refCount));

		(*ppI)->refCount = 1;
		MFSound_Destroy(*ppI);

		ppI++;
	}

	MFSound_DeinitModulePlatformSpecific();

	MFStreamHandler **ppSI = gStreamHandlers.Begin();
	while(*ppSI)
	{
		MFHeap_Free(*ppSI);
		++ppSI;
	}

	gStreamHandlers.Deinit();
	gVoices.Deinit();
	gSoundBank.Deinit();
}

void MFSound_Update()
{
	MFCALLSTACK;

	MFVoice **ppI = gVoices.Begin();

	while(*ppI)
	{
		MFVoice *pV = *ppI;

		bool bFinished = MFSound_UpdateVoiceInternal(pV);

		if(bFinished)
			gVoices.Destroy(ppI);

		ppI++;
	}

	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		if(gMusicTracks[a].pStreamBuffer && gMusicTracks[a].playing)
		{
			MFSound_ServiceStreamBuffer(&gMusicTracks[a]);
		}
	}

	MFSound_UpdateInternal();
}

MFSound *MFSound_Create(const char *pName)
{
	MFCALLSTACK;

	MFSound *pSound = MFSound_FindSound(pName);

	if(!pSound)
	{
		uint32 fileSize;

		// load the template
		const char *pFileName = MFStr("%s.snd", pName);

		MFSoundTemplate *pTemplate = (MFSoundTemplate*)MFFileSystem_Load(pFileName, &fileSize);

		if(!pTemplate)
		{
			MFDebug_Error(MFStr("Sound '%s' does not exist.\n", pFileName));
			return NULL;
		}

		MFDebug_Assert(pTemplate->magic == MFMAKEFOURCC('S', 'N', 'D', '1'), MFStr("File '%s' is not a valid sound file.", pFileName));

		MFFixUp(pTemplate->ppStreams, pTemplate, true);
		for(int a=0; a<pTemplate->numStreams; a++)
			MFFixUp(pTemplate->ppStreams[a], pTemplate, true);

		// create the sound
		pSound = gSoundBank.Create();
		MFZeroMemory(pSound, sizeof(MFSound));
		pSound->pTemplate = pTemplate;
		pSound->pInternal = (MFSoundDataInternal*)((char*)pSound + internalSoundDataSize);
		MFString_CopyN(pSound->name, pName, sizeof(pSound->name) - 1);
		pSound->name[sizeof(pSound->name) - 1] = 0;

		MFSound_CreateInternal(pSound);

		// lock the buffers and copy in the data
		void *pBuffer;
		uint32 len;

		MFSound_Lock(pSound, 0, 0, &pBuffer, &len);
		MFCopyMemory(pBuffer, pTemplate->ppStreams[0], len);
		MFSound_Unlock(pSound);
	}

	++pSound->refCount;

	return pSound;
}

MFSound *MFSound_CreateDynamic(const char *pName, int numSamples, int numChannels, int bitsPerSample, int samplerate, uint32 flags)
{
	MFCALLSTACK;

	MFSound *pSound = MFSound_FindSound(pName);

	if(!pSound)
	{
		// create a template
		MFSoundTemplate *pTemplate = (MFSoundTemplate*)MFHeap_Alloc(sizeof(MFSoundTemplate));

		MFZeroMemory(pTemplate, sizeof(MFSoundTemplate));
		pTemplate->magic = MFMAKEFOURCC('S', 'N', 'D', '1');
		pTemplate->flags = flags;
		pTemplate->sampleRate = samplerate;
		pTemplate->numSamples = numSamples;
		pTemplate->numChannels = (uint16)numChannels;
		pTemplate->bitsPerSample = bitsPerSample;
		pTemplate->format = MFWF_PCM_s16;

		// create the sound
		pSound = gSoundBank.Create();
		MFZeroMemory(pSound, sizeof(MFSound) + internalSoundDataSize);
		pSound->pTemplate = pTemplate;
		pSound->pInternal = (MFSoundDataInternal*)((char*)pSound + sizeof(MFSound));
		MFString_CopyN(pSound->name, pName, sizeof(pSound->name) - 1);
		pSound->name[sizeof(pSound->name) - 1] = 0;

		MFSound_CreateInternal(pSound);

		// clear the buffer
		void *pBuffer;
		uint32 len;

		MFSound_Lock(pSound, 0, 0, &pBuffer, &len);
		MFZeroMemory(pBuffer, len);
		MFSound_Unlock(pSound);
	}

	++pSound->refCount;

	return pSound;
}

int MFSound_Destroy(MFSound *pSound)
{
	MFCALLSTACK;

	--pSound->refCount;
	int refCount = pSound->refCount;

	if(!pSound->refCount)
	{
		MFSound_DestroyInternal(pSound);
		MFHeap_Free(pSound->pTemplate);

		gSoundBank.Destroy(pSound);
	}

	return refCount;
}

MFSound *MFSound_FindSound(const char *pSoundName)
{
	MFCALLSTACK;

	MFSound **ppIterator = gSoundBank.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pSoundName, (*ppIterator)->name))
			return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MFVoice *MFSound_Play(MFSound *pSound, uint32 playFlags)
{
	MFCALLSTACK;

	if(!pSound)
		return NULL;

	MFVoice *pVoice = gVoices.Create();
	MFZeroMemory(pVoice, sizeof(MFVoice) + internalVoiceDataSize);
	pVoice->flags = playFlags;
	pVoice->pSound = pSound;
	pVoice->pInternal = (MFVoiceDataInternal*)((char*)pVoice + sizeof(MFVoice));

	MFSound_PlayInternal(pVoice);

	return pVoice;
}


//
// MFAudioStream related functions
//

void MFSound_RegisterStreamHandler(const char *pStreamType, const char *pStreamExtension, MFStreamCallbacks *pCallbacks)
{
	MFStreamHandler *pHandler = (MFStreamHandler*)MFHeap_Alloc(sizeof(MFStreamHandler));

	MFString_Copy(pHandler->streamType, pStreamType);
	MFString_Copy(pHandler->streamExtension, pStreamExtension);
	pHandler->callbacks = *pCallbacks;

	gStreamHandlers.Create(pHandler);
}

MFAudioStream *MFSound_PlayStream(const char *pFilename, bool pause)
{
	MFCALLSTACK;

	int t = 0;

	// find free music track
	while(gMusicTracks[t].pStreamBuffer && t < gDefaults.sound.maxMusicTracks) t++;
	if(t == gDefaults.sound.maxMusicTracks) return NULL;

	MFAudioStream *pStream = &gMusicTracks[t];

	// find matching extension
	MFStreamHandler **ppI = gStreamHandlers.Begin();

	while(*ppI)
	{
		int extLen = MFString_Length((*ppI)->streamExtension);
		int fileLen = MFString_Length(pFilename);

		if(extLen < fileLen && !MFString_CaseCmp((*ppI)->streamExtension, pFilename + (fileLen - extLen)))
		{
			pStream->pStreamHandler = *ppI;
			break;
		}

		++ppI;
	}

	if(!pStream->pStreamHandler)
		return NULL;

	// attempt to create the stream...
	pStream->pStreamHandler->callbacks.pCreateStream(pStream, pFilename);

	if(!pStream->pStreamBuffer)
		return NULL;

	// init the stream
	MFString_CopyN(pStream->name, pFilename, sizeof(pStream->name) - 1);
	pStream->name[sizeof(pStream->name) - 1] = 0;
	pStream->currentTime = 0.0f;
	pStream->playBackOffset = 0;

	// fill the buffer
	MFSound_FillBuffer(pStream, pStream->bufferSize);

	// play buffer
	pStream->pStreamVoice = MFSound_Play(pStream->pStreamBuffer, MFPF_Looping | (pause ? MFPF_BeginPaused : 0));
	pStream->playing = !pause;

	return pStream;
}

void MFSound_ServiceStreamBuffer(MFAudioStream *pStream)
{
	MFCALLSTACK;

	uint32 playCursor;
	int lockSize;

	// get cursor pos
	playCursor = MFSound_GetPlayCursor(pStream->pStreamVoice);

	if(pStream->playBackOffset == playCursor)
		return;

	// calculate lock size
	if(playCursor < pStream->playBackOffset)
	{
		lockSize = playCursor + (pStream->bufferSize - pStream->playBackOffset);
	}
	else
	{
		lockSize = playCursor - pStream->playBackOffset;
	}

	// update the buffer
	MFSound_FillBuffer(pStream, lockSize);
}

void MFSound_DestroyStream(MFAudioStream *pStream)
{
	MFCALLSTACK;

	if(pStream->playing)
		MFSound_Stop(pStream->pStreamVoice);

	// call stream handler destroy
	pStream->pStreamHandler->callbacks.pDestroyStream(pStream);

	MFSound_Destroy(pStream->pStreamBuffer);
	pStream->pStreamBuffer = NULL;
}

void MFSound_SeekStream(MFAudioStream *pStream, float seconds)
{
	MFCALLSTACK;

	pStream->pStreamHandler->callbacks.pSeekStream(pStream, seconds);

	if(pStream->playing)
		MFSound_Pause(pStream->pStreamVoice, true);

	MFSound_SetPlaybackOffset(pStream->pStreamVoice, 0.0f);
	pStream->playBackOffset = 0;

	MFSound_FillBuffer(pStream, pStream->bufferSize);

	if(pStream->playing)
		MFSound_Pause(pStream->pStreamVoice, false);
}

void MFSound_PauseStream(MFAudioStream *pStream, bool pause)
{
	MFCALLSTACK;

	if(pause)
	{
		if(pStream->playing)
			MFSound_Pause(pStream->pStreamVoice, true);
	}
	else
	{
		if(!pStream->playing)
			MFSound_Pause(pStream->pStreamVoice, false);
	}

	pStream->playing = !pause;
}

MFVoice *MFSound_GetStreamVoice(MFAudioStream *pStream)
{
	return pStream->pStreamVoice;
}

void MFSound_FillBuffer(MFAudioStream *pStream, int bytes)
{
	MFCALLSTACKc;

	void *pData1, *pData2;
	uint32 bytes1, bytes2;
	uint32 bufferFed = 0;

	// fill buffer
	MFSound_Lock(pStream->pStreamBuffer, pStream->playBackOffset, bytes, &pData1, &bytes1, &pData2, &bytes2);

	char *pData = (char*)pData1;
	uint32 bytesToWrite = bytes1;
	bool wrapped = false;

	while(bufferFed < bytesToWrite)
	{
		int r = pStream->pStreamHandler->callbacks.pGetSamples(pStream, pData, bytesToWrite-bufferFed);

		if(!r)
			pStream->pStreamHandler->callbacks.pSeekStream(pStream, 0.0f);

		pData += r;
		bufferFed += r;

		if(bufferFed == bytesToWrite && bytes2 && !wrapped)
		{
			bufferFed = 0;
			bytesToWrite = bytes2;
			pData = (char*)pData2;
			wrapped = true;
		}
	}

	// unlock buffer
	MFSound_Unlock(pStream->pStreamBuffer);

	// increment playback cursor
	pStream->playBackOffset = (pStream->playBackOffset + bytes) % pStream->bufferSize;

	// update playback time
	pStream->currentTime = pStream->pStreamHandler->callbacks.pGetTime(pStream);
}


// vorbis stream handler

#if defined(VORBIS_STREAM)

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

#if defined(MAD_STREAM)
#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	8192 /* Must be an integer multiple of 4. */

struct MFID3
{
	unsigned char *pID3Data;
	uint32 size;
	uint8 major, minor;
	uint8 flags;
	uint8 reserved;
};

struct MFMADDecoder
{
	unsigned char inputBuffer[INPUT_BUFFER_SIZE+MAD_BUFFER_GUARD];
	unsigned char overflow[OUTPUT_BUFFER_SIZE];
	mad_stream stream;
	mad_frame frame;
	mad_synth synth;
	mad_header firstHeader;
	mad_timer_t timer;
	MFFile *pFile;
	MFID3 *pID3;

	uint32 *pFrameOffsets;
	uint32 numFrameOffsetsAllocated;
	uint32 frameOffsetCount;

	unsigned char *pGuardPtr;
	uint32 currentFrame;
	uint32 frameCount;
	uint32 overflowOffset;
	uint32 overflowBytes;
};

int GetSynchSafeInt(unsigned char *pStream)
{
	return (uint32)pStream[3] | ((uint32)pStream[2] << 7) | ((uint32)pStream[1] << 14) | ((uint32)pStream[0] << 21);
}

static inline signed short MadFixedToSshort(mad_fixed_t fixed)
{
	// clipping
	if(fixed >= MAD_F_ONE)
		return 32767;
	if(fixed <= -MAD_F_ONE)
		return -32767;

	// conversion
	fixed = fixed >> (MAD_F_FRACBITS - 15);
	return (signed short)fixed;
}

int GetMADSamples(MFAudioStream *pStream, void *pBuffer, uint32 bytes)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;

	int written = 0;

	if(pDecoder->overflowBytes)
	{
		// grab from the overflow until we run out...
		uint32 numBytes = MFMin(pDecoder->overflowBytes - pDecoder->overflowOffset, bytes);
		MFCopyMemory(pBuffer, pDecoder->overflow + pDecoder->overflowOffset, numBytes);
		pDecoder->overflowOffset += numBytes;
		if(pDecoder->overflowOffset == pDecoder->overflowBytes)
			pDecoder->overflowBytes = 0;

		// increment timer
		mad_timer_t t = { 0, MAD_TIMER_RESOLUTION / pDecoder->firstHeader.samplerate };
		mad_timer_multiply(&t, numBytes >> 2);
		mad_timer_add(&pDecoder->timer, t);

		if(bytes == numBytes)
			return numBytes;

		bytes -= numBytes;
		(char*&)pBuffer += numBytes;
		written = (int)numBytes;
	}

	do
	{
		if(pDecoder->stream.buffer==NULL || pDecoder->stream.error == MAD_ERROR_BUFLEN)
		{
			size_t readSize, remaining;
			unsigned char *readStart;

			if(pDecoder->stream.next_frame != NULL)
			{
				remaining = pDecoder->stream.bufend - pDecoder->stream.next_frame;
				for(size_t a=0; a<remaining; ++a)
					pDecoder->inputBuffer[a] = pDecoder->stream.next_frame[a];
//				memmove(pDecoder->inputBuffer, pDecoder->stream.next_frame, remaining);
				readStart = pDecoder->inputBuffer + remaining;
				readSize = INPUT_BUFFER_SIZE - remaining;
			}
			else
			{
				readSize = INPUT_BUFFER_SIZE;
				readStart = pDecoder->inputBuffer;
				remaining = 0;
			}

			readSize = MFFile_Read(pDecoder->pFile, readStart, readSize, false);
			if(readSize <= 0)
			{
				if(MFFile_IsEOF(pDecoder->pFile))
					MFDebug_Warn(3, "End of input stream.");
				else
					MFDebug_Warn(2, "Read error on bit-stream.");
				break;
			}
			if(MFFile_IsEOF(pDecoder->pFile))
			{
				pDecoder->pGuardPtr = readStart + readSize;
				MFZeroMemory(pDecoder->pGuardPtr, MAD_BUFFER_GUARD);
				readSize += MAD_BUFFER_GUARD;
			}

			mad_stream_buffer(&pDecoder->stream, pDecoder->inputBuffer, readSize + remaining);
			pDecoder->stream.error = MAD_ERROR_NONE;
		}

		if(mad_frame_decode(&pDecoder->frame, &pDecoder->stream))
		{
			if(MAD_RECOVERABLE(pDecoder->stream.error))
			{
				if(pDecoder->stream.error != MAD_ERROR_LOSTSYNC || pDecoder->stream.this_frame != pDecoder->pGuardPtr)
					MFDebug_Warn(3, MFStr("Recoverable frame level error (%s)\n", mad_stream_errorstr(&pDecoder->stream)));
				continue;
			}
			else
				if(pDecoder->stream.error == MAD_ERROR_BUFLEN)
					continue;
				else
				{
					MFDebug_Warn(3, MFStr("Unrecoverable frame level error (%s)\n", mad_stream_errorstr(&pDecoder->stream)));
					break;
				}
		}

		// we'll keep a copy of the first frames header (it has some useful information)...
		if(pDecoder->frameCount == 0)
			pDecoder->firstHeader = pDecoder->frame.header;

		// we need to keep a backlog of frame offsets for seeking...
		if(pDecoder->currentFrame == pDecoder->frameCount)
		{
			// we'll only take a frame offset once every 10 frames (otherwise we waste loads of memory!)
			// we need to push the counter along from time to time
			if(pDecoder->frameCount % 10 == 0)
			{
				++pDecoder->frameOffsetCount;

				// if we have overflowed the frame history, we need to realloc...
				if(pDecoder->frameOffsetCount == pDecoder->numFrameOffsetsAllocated)
				{
					pDecoder->numFrameOffsetsAllocated *= 2;
					pDecoder->pFrameOffsets = (uint32*)MFHeap_Realloc(pDecoder->pFrameOffsets, sizeof(uint32)*pDecoder->numFrameOffsetsAllocated);
				}

				pDecoder->pFrameOffsets[pDecoder->frameOffsetCount] = pDecoder->pFrameOffsets[pDecoder->frameOffsetCount - 1];
			}

			// add current frames size to frame offset counter
			int frameSize = pDecoder->stream.next_frame - pDecoder->stream.this_frame;
			pDecoder->pFrameOffsets[pDecoder->frameOffsetCount] += frameSize;

			// increase counters
			++pDecoder->frameCount;
		}
		++pDecoder->currentFrame;

/*
		// filters?? eq/etc?
		if(DoFilter)
			ApplyFilter(&pDecoder->frame);
*/
		const int bytesPerFrame = 1152*4;
		if(pBuffer || bytes < bytesPerFrame)
			mad_synth_frame(&pDecoder->synth, &pDecoder->frame);

		// increment timer
		mad_timer_t t = { 0, MAD_TIMER_RESOLUTION / pDecoder->firstHeader.samplerate };
		mad_timer_multiply(&t, MFMin((uint32)pDecoder->synth.pcm.length, bytes >> 2));
		mad_timer_add(&pDecoder->timer, t);

		bool bStereo = MAD_NCHANNELS(&pDecoder->frame.header) == 2;
		int16 *pOutputPtr = (int16*)pBuffer;

		int i=0;
		for(; i < pDecoder->synth.pcm.length && bytes; i++)
		{
			if(pOutputPtr)
			{
				signed short sample = MadFixedToSshort(pDecoder->synth.pcm.samples[0][i]);
				*(pOutputPtr++) = sample;
				if(bStereo)
					sample = MadFixedToSshort(pDecoder->synth.pcm.samples[1][i]);
				*(pOutputPtr++) = sample;

				written += 4;
			}
			bytes -= 4;
		}
		pBuffer = pOutputPtr;

		// write any remaining samples to the overflow buffer
		pOutputPtr = (int16*)pDecoder->overflow;
		pDecoder->overflowOffset = 0;
		pDecoder->overflowBytes = (pDecoder->synth.pcm.length - i) * 4;

		for(; i < pDecoder->synth.pcm.length; i++)
		{
			signed short sample = MadFixedToSshort(pDecoder->synth.pcm.samples[0][i]);
			*(pOutputPtr++) = sample;
			if(bStereo)
				sample = MadFixedToSshort(pDecoder->synth.pcm.samples[1][i]);
			*(pOutputPtr++) = sample;
		}
	}
	while(bytes);

	return written;
}

void DestroyMADStream(MFAudioStream *pStream)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;

	mad_synth_finish(&pDecoder->synth);
	mad_frame_finish(&pDecoder->frame);
	mad_stream_finish(&pDecoder->stream);

	MFFile_Close(pDecoder->pFile);

	if(pDecoder->pID3)
		MFHeap_Free(pDecoder->pID3);
	MFHeap_Free(pDecoder->pFrameOffsets);
	MFHeap_Free(pDecoder);
}

void CreateMADStream(MFAudioStream *pStream, const char *pFilename)
{
	MFCALLSTACK;

	// open mp3 file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return;

	// attempt to cache the mp3 stream
	MFOpenDataCachedFile cachedOpen;
	cachedOpen.cbSize = sizeof(MFOpenDataCachedFile);
	cachedOpen.openFlags = MFOF_Read | MFOF_Binary | MFOF_Cached_CleanupBaseFile;
	cachedOpen.maxCacheSize = 256*1024; // 256k cache for mp3 stream should be heaps!!
	cachedOpen.pBaseFile = hFile;

	MFFile *pCachedFile = MFFile_Open(MFFileSystem_GetInternalFileSystemHandle(MFFSH_CachedFileSystem), &cachedOpen);
	if(pCachedFile)
		hFile = pCachedFile;

	// init the decoder
	MFMADDecoder *pDecoder = (MFMADDecoder*)MFHeap_Alloc(sizeof(MFMADDecoder));
	pStream->pStreamData = pDecoder;
	pDecoder->pID3 = NULL;
	pDecoder->pFile = hFile;
	pDecoder->pGuardPtr = NULL;
	pDecoder->currentFrame = 0;
	pDecoder->frameCount = 0;
	pDecoder->overflowOffset = 0;
	pDecoder->overflowBytes = 0;
	pDecoder->pFrameOffsets = (uint32*)MFHeap_Alloc(sizeof(uint32)*2048);
	pDecoder->numFrameOffsetsAllocated = 2048;
	pDecoder->frameOffsetCount = 0;

	mad_stream_init(&pDecoder->stream);
	mad_frame_init(&pDecoder->frame);
	mad_synth_init(&pDecoder->synth);
	mad_header_init(&pDecoder->firstHeader);
	mad_timer_reset(&pDecoder->timer);

	// read the ID3 tag, if present...
	unsigned char buffer[10];
	int read = MFFile_Read(hFile, buffer, 10, false);
	if(!MFString_CompareN((char*)buffer, "ID3", 3))
	{
		uint32 size = GetSynchSafeInt(buffer + 6);
		pDecoder->pID3 = (MFID3*)MFHeap_Alloc(sizeof(MFID3) + size);
		pDecoder->pID3->pID3Data = (unsigned char*)(pDecoder->pID3 + 1);
		pDecoder->pID3->size = size;
		pDecoder->pID3->major = buffer[3];
		pDecoder->pID3->minor = buffer[4];
		pDecoder->pID3->flags = buffer[5];
		MFFile_Read(hFile, pDecoder->pID3->pID3Data, size, false);
		pDecoder->pFrameOffsets[0] = read + size;
		read = 0;
	}
	else
	{
		// no ID3, we'll copy what we read into the input buffer...
		MFCopyMemory(pDecoder->inputBuffer, buffer, read);
		pDecoder->pFrameOffsets[0] = 0;
	}

	// prime the input buffer
	read += MFFile_Read(hFile, pDecoder->inputBuffer + read, INPUT_BUFFER_SIZE - read, false);
	mad_stream_buffer(&pDecoder->stream, pDecoder->inputBuffer, read);
	pDecoder->stream.error = MAD_ERROR_NONE;

	// decode the first frame so we can get the frame header
	GetMADSamples(pStream, NULL, 0);

	int sampleRate = pDecoder->firstHeader.samplerate;
	if(!sampleRate)
	{
		DestroyMADStream(pStream);
		return;
	}

	pStream->trackLength = 1000.0f;				// mp3 sucks! we have no idea without skipping through the whole thing... :/
	pStream->bufferSize = sampleRate * 2 * 2;	// 1 second, 2 channels, 16bits per sample

	pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, sampleRate, 2, 16, sampleRate, MFSF_Dynamic | MFSF_Circular);

	if(!pStream->pStreamBuffer)
		DestroyMADStream(pStream);
}

void SeekMADStream(MFAudioStream *pStream, float seconds)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;

	// seeking mp3's is a bitch!!!
	uint32 sample = (uint32)((float)pDecoder->firstHeader.samplerate * seconds);
	uint32 frame = sample / 1152;
	uint32 sampleInFrame = sample % 1152;

	uint32 frameRecord = frame/10;
	if(frameRecord >= pDecoder->frameOffsetCount)
		frameRecord = pDecoder->frameOffsetCount-1;
	pDecoder->currentFrame = frameRecord*10;

	// seek to frame
	uint32 frameOffset = pDecoder->pFrameOffsets[frameRecord];
	MFFile_Seek(pDecoder->pFile, frameOffset, MFSeek_Begin);

	// prime input buffer
	int bytes = MFFile_Read(pDecoder->pFile, pDecoder->inputBuffer, INPUT_BUFFER_SIZE, false);
	mad_stream_buffer(&pDecoder->stream, pDecoder->inputBuffer, bytes);
	pDecoder->stream.error = MAD_ERROR_NONE;
	pDecoder->overflowBytes = 0;

	// initialise timer to our song position
	pDecoder->timer.seconds = 0;
	pDecoder->timer.fraction = MAD_TIMER_RESOLUTION / pDecoder->firstHeader.samplerate;
	mad_timer_multiply(&pDecoder->timer, pDecoder->currentFrame*1152 + sampleInFrame);

	// skip samples to where we want to be..
	int skipFrames = frame - pDecoder->currentFrame;
	int skipSamples = skipFrames*1152 + sampleInFrame;
	int skipBytes = skipSamples * 4;
	GetMADSamples(pStream, NULL, skipBytes);
}

float GetMADTime(MFAudioStream *pStream)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;
	return (float)mad_timer_count(pDecoder->timer, MAD_UNITS_MILLISECONDS) * 0.001f;
}

void MFSound_RegisterMAD()
{
	MFStreamCallbacks callbacks;
	callbacks.pCreateStream = CreateMADStream;
	callbacks.pDestroyStream = DestroyMADStream;
	callbacks.pGetSamples = GetMADSamples;
	callbacks.pGetTime = GetMADTime;
	callbacks.pSeekStream = SeekMADStream;

	MFSound_RegisterStreamHandler("MP3 Audio (MAD)", ".mp3", &callbacks);
}
#endif

// MFSound debug draw

void MFSound_Draw()
{
	MFCALLSTACK;

#if !defined(_RETAIL)
	if(!showSoundStats) return;

	float y = 20.0f;

	MFVoice **ppI = gVoices.Begin();

	while(*ppI)
	{
		MFVoice *pV = *ppI;
		MFSoundTemplate *pT = pV->pSound->pTemplate;

		uint32 bufferSize = (pT->numSamples * pT->numChannels * pT->bitsPerSample) >> 3;

		MFFont_DrawTextf(MFFont_GetDebugFont(), 20.0f, y, 20.0f, MakeVector(1,1,0,1), "Voice: '%s'", pV->pSound->name);
		y += 20.0f;

//		MFFont_DrawTextf(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "Channels: %d, Samplerate: %d, AvgBitrate: %dkbps, Version: %d", pV->pSound->pTemplate->numChannels, pV->pSound->pTemplate->sampleRate, gMusicTracks[a].pInfo->bitrate_nominal/1000, gMusicTracks[a].pInfo->version);
		MFFont_DrawTextf(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "Channels: %d, Samplerate: %d", pT->numChannels, pT->sampleRate);
		y += 20.0f;
/*
#if defined(VORBIS_STREAM)
		// render bitrate
		static float bitrate = 0.0f;
		long br = ov_bitrate_instant(&gMusicTracks[a].vorbisFile);

		if(br)
			bitrate = (float)br*0.02f + bitrate * 0.98f;

		MFFont_DrawTextf(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "CurrentBitrate: %dkbps", ((int)bitrate)/1000);
		y += 20.0f;

		MFFont_DrawTextf(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "TrackLength: %d:%02d, CurrentTime: %d:%02d", ((int)gMusicTracks[a].trackLength) / 60, ((int)gMusicTracks[a].trackLength) % 60, ((int)gMusicTracks[a].currentTime) / 60, ((int)gMusicTracks[a].currentTime) % 60);
		y += 25.0f;

		MFPrimitive(PT_TriStrip|PT_Untextured);

		MFBegin(16);
		MFSetColour(0xFFFFFFFF);
		MFSetPosition(23.0f, y-2.0f, 0.0f);
		MFSetPosition(617.0f, y-2.0f, 0.0f);
		MFSetPosition(23.0f, y+22.0f, 0.0f);
		MFSetPosition(617.0f, y+22.0f, 0.0f);

		MFSetPosition(617.0f, y+22.0f, 0.0f);
		MFSetPosition(25.0f, y, 0.0f);

		MFSetColour(0xFF404040);
		MFSetPosition(25.0f, y, 0.0f);
		MFSetPosition(615.0f, y, 0.0f);
		MFSetPosition(25.0f, y+20.0f, 0.0f);
		MFSetPosition(615.0f, y+20.0f, 0.0f);

		float xPlayback = 25.0f + (615.0f-25.0f) * (gMusicTracks[a].currentTime/gMusicTracks[a].trackLength);

		MFSetPosition(615.0f, y+20.0f, 0.0f);
		MFSetPosition(xPlayback-1.0f, y-1.0f, 0.0f);

		MFSetColour(MakeVector(0.5f, 0.5f, 1, 1));
		MFSetPosition(xPlayback-1.0f, y-1.0f, 0.0f);
		MFSetPosition(xPlayback+1.0f, y-1.0f, 0.0f);
		MFSetPosition(xPlayback-1.0f, y+21.0f, 0.0f);
		MFSetPosition(xPlayback+1.0f, y+21.0f, 0.0f);
		MFEnd();

		y += 30.0f;
#endif
*/

		MFFont_DrawTextf(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "Buffer:");

		MFPrimitive(PT_TriStrip|PT_Untextured);

		MFBegin(28);
		MFSetColour(0xFFFFFFFF);
		MFSetPosition(98.0f, y-2.0f, 0.0f);
		MFSetPosition(502.0f, y-2.0f, 0.0f);
		MFSetPosition(98.0f, y+22.0f, 0.0f);
		MFSetPosition(502.0f, y+22.0f, 0.0f);

		MFSetPosition(502.0f, y+22.0f, 0.0f);
		MFSetPosition(100.0f, y, 0.0f);

		MFSetColour(0xFF404040);
		MFSetPosition(100.0f, y, 0.0f);
		MFSetPosition(500.0f, y, 0.0f);
		MFSetPosition(100.0f, y+20.0f, 0.0f);
		MFSetPosition(500.0f, y+20.0f, 0.0f);

		uint32 playCursor, writeCursor;
		playCursor = MFSound_GetPlayCursor(pV, &writeCursor);

		float xPlayCursor = 100.0f + (500.0f-100.0f) * ((float)playCursor / (float)bufferSize);

		MFSetPosition(500.0f, y+20.0f, 0.0f);
		MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);

		MFSetColour(0xFFFFFF00);
		MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);
		MFSetPosition(xPlayCursor+1.0f, y-1.0f, 0.0f);
		MFSetPosition(xPlayCursor-1.0f, y+21.0f, 0.0f);
		MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);

		float xWriteCursor = 100.0f + (500.0f-100.0f) * ((float)writeCursor / (float)bufferSize);

		MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);
		MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);

		MFSetColour(0xFF0000FF);
		MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);
		MFSetPosition(xWriteCursor+1.0f, y-1.0f, 0.0f);
		MFSetPosition(xWriteCursor-1.0f, y+21.0f, 0.0f);
		MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);
/*
		float xBufferFilled = 100.0f + (500.0f-100.0f) * ((float)gMusicTracks[a].playBackOffset / (float)bufferSize);

		MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);
		MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);

		MFSetColour(0xFFFF8000);
		MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);
		MFSetPosition(xBufferFilled+1.0f, y-1.0f, 0.0f);
		MFSetPosition(xBufferFilled-1.0f, y+21.0f, 0.0f);
		MFSetPosition(xBufferFilled+1.0f, y+21.0f, 0.0f);
		MFEnd();
*/
		y += 35.0f;

		ppI++;
	}
#endif
}
