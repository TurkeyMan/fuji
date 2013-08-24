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
#include "Asset/MFIntSound.h"

//#define USE_THREADED_AUDIO

#if defined(USE_THREADED_AUDIO)
	#include <windows.h>
#endif

#define ALLOW_LOAD_FROM_SOURCE_DATA

/**** Forward Declarations ****/

void MFSound_FillBuffer(MFAudioStream *pStream, size_t bytes);
void MFSound_ServiceStreamBuffer(MFAudioStream *pStream);


/**** Structures ****/


/**** Globals ****/

MFPtrListDL<MFSound> gSoundBank;
MFPtrListDL<MFVoice> gVoices;
MFPtrList<MFStreamHandler> gStreamHandlers;

static int internalSoundDataSize = 0;
static int internalVoiceDataSize = 0;

static MFAudioStream *gMusicTracks;

#if !defined(_RETAIL)
MenuItemBool showSoundStats;
#endif


/**** Functions ****/

volatile static bool gbShutdownAudioThread = false;
volatile static bool gbAudioThreadFinished = false;
static MFMutex gSoundMutex;
static MFThread gAudioThread = NULL;

void MFSound_LockMutex(bool lock)
{
#if defined(USE_THREADED_AUDIO)
	if(lock)
		MFThread_LockMutex(gSoundMutex);
	else
		MFThread_ReleaseMutex(gSoundMutex);
#endif
}

void MFSound_UpdateSound()
{
	MFVoice **ppI = gVoices.Begin();

	while(ppI && *ppI)
	{
		MFVoice *pV = *ppI;

		bool bFinished = MFSound_UpdateVoiceInternal(pV);

		if(bFinished)
			gVoices.Destroy(ppI);

		ppI++;
	}

	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		bool playing = !(gMusicTracks[a].playFlags & MFPF_Paused);
		if(gMusicTracks[a].pStreamBuffer && playing)
		{
			MFSound_ServiceStreamBuffer(&gMusicTracks[a]);
		}
	}

	MFSound_UpdateInternal();
}

#if defined(USE_THREADED_AUDIO)
int MFSound_UpdateThread(void *pUserData)
{
	while(!gbShutdownAudioThread)
	{
		MFSound_LockMutex(true);
		MFSound_UpdateSound();
		MFSound_LockMutex(false);

		Sleep(10);
	}

	gbAudioThreadFinished = true;

	MFThread_ExitThread(0);
	return 0;
}
#endif

MFInitStatus MFSound_InitModule()
{
	MFCALLSTACK;

#if defined(USE_MFMIXER)
	MFSoundMixer_Init(&internalVoiceDataSize);
	gVoices.Init("Voice Bank", gDefaults.sound.maxVoices, sizeof(MFVoice) + internalVoiceDataSize);
	MFSound_InitModulePlatformSpecific(&internalSoundDataSize, &internalVoiceDataSize);
#else
	MFSound_InitModulePlatformSpecific(&internalSoundDataSize, &internalVoiceDataSize);
	gVoices.Init("Voice Bank", gDefaults.sound.maxVoices, sizeof(MFVoice) + internalVoiceDataSize);
#endif
	gSoundBank.Init("Sound Bank", gDefaults.sound.maxSounds, sizeof(MFSound) + internalSoundDataSize);
	gStreamHandlers.Init("Stream handlers", 16);

	gMusicTracks = (MFAudioStream*)MFHeap_Alloc(sizeof(MFAudioStream) * gDefaults.sound.maxMusicTracks);
	MFZeroMemory(gMusicTracks, sizeof(MFAudioStream)*gDefaults.sound.maxMusicTracks);

#if !defined(_RETAIL)
	DebugMenu_AddMenu("Sound Options", "Fuji Options");
	DebugMenu_AddItem("Show Sound Stats", "Sound Options", &showSoundStats);
#endif

	// register stream handlers...
#if defined(VORBIS_STREAM)
	void MFSound_RegisterVorbis();
	MFSound_RegisterVorbis();
#endif
#if defined(MAD_STREAM)
	void MFSound_RegisterMAD();
	MFSound_RegisterMAD();
#endif
#if defined(MINIFMOD_STREAM)
	void MFSound_RegisterMiniFMOD();
	MFSound_RegisterMiniFMOD();
#endif
#if defined(PSPAUDIOCODEC_STREAM)
	void MFSound_RegisterPSPAudioCodec();
	MFSound_RegisterPSPAudioCodec();
#endif
#if defined(WAV_STREAM)
	void MFSound_RegisterWAV();
	MFSound_RegisterWAV();
#endif

#if defined(USE_THREADED_AUDIO)
	gSoundMutex = MFThread_CreateMutex("MFSound Mutex");
	gAudioThread = MFThread_CreateThread("MFSound Thread", MFSound_UpdateThread, NULL, MFPriority_AboveNormal);
#endif

	return MFAIC_Succeeded;
}

void MFSound_DeinitModule()
{
	MFCALLSTACK;

#if defined(USE_THREADED_AUDIO)
	if(gAudioThread)
	{
		gbShutdownAudioThread = true;
		while(!gbAudioThreadFinished)
			Sleep(1);
	}
#endif

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
#if defined(USE_MFMIXER)
	void MFSoundMixer_Deinit();
#endif

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

	if(gAudioThread)
	{
		// what we should do, is pool all voice related requests, and then commit them all at once so we only lock the mutex once per frame..
	}
	else
	{
		MFSound_UpdateSound();
	}
}

MF_API MFSound *MFSound_Create(const char *pName)
{
	MFCALLSTACK;

	MFSound *pSound = MFSound_FindSound(pName);

	if(!pSound)
	{
		size_t fileSize;

		// load the template
		const char *pFileName = MFStr("%s.snd", pName);

		MFSoundTemplate *pTemplate = (MFSoundTemplate*)MFFileSystem_Load(pFileName, &fileSize);

		if(!pTemplate)
		{
#if defined(ALLOW_LOAD_FROM_SOURCE_DATA)
			// try to load from source data
			MFIntSound *pIS = NULL;

			MFStreamHandler **ppI = gStreamHandlers.Begin();
			while(!pIS && *ppI)
			{
				pIS = MFIntSound_CreateFromFile(MFStr("%s%s", pName, (*ppI)->streamExtension));
				++ppI;
			}

			if(pIS)
			{
				size_t size;
				MFIntSound_CreateRuntimeData(pIS, (void**)&pTemplate, &size, MFSystem_GetCurrentPlatform());

				MFFile *pFile = MFFileSystem_Open(MFStr("cache:%s.snd", pName), MFOF_Write | MFOF_Binary);
				if(pFile)
				{
					MFFile_Write(pFile, pTemplate, size, false);
					MFFile_Close(pFile);
				}

				MFIntSound_Destroy(pIS);
			}
#endif

			if(!pTemplate)
			{
				MFDebug_Error(MFStr("Sound '%s' does not exist.\n", pFileName));
				return NULL;
			}
		}

		MFDebug_Assert(pTemplate->magic == MFMAKEFOURCC('S', 'N', 'D', '1'), MFStr("File '%s' is not a valid sound file.", pFileName));

		MFFixUp(pTemplate->ppStreams, pTemplate, true);
		for(int a=0; a<pTemplate->numStreams; a++)
			MFFixUp(pTemplate->ppStreams[a], pTemplate, true);

		// create the sound
		pSound = gSoundBank.Create();
		MFZeroMemory(pSound, sizeof(MFSound) + internalSoundDataSize);
		pSound->pTemplate = pTemplate;
		pSound->pInternal = (MFSoundDataInternal*)((char*)pSound + sizeof(MFSound));
		MFString_CopyN(pSound->name, pName, sizeof(pSound->name) - 1);
		pSound->name[sizeof(pSound->name) - 1] = 0;

		MFSound_CreateInternal(pSound);

		uint32 bytes = pTemplate->numSamples*pTemplate->numChannels*pTemplate->bitsPerSample / 8;
		MFSound_SetBufferData(pSound, pTemplate->ppStreams[0], bytes);
	}

	++pSound->refCount;

	return pSound;
}

MF_API MFSound *MFSound_CreateDynamic(const char *pName, int numSamples, int numChannels, int bitsPerSample, int samplerate, uint32 flags)
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
		pTemplate->numStreams = 1;
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
		size_t len;

		MFSound_Lock(pSound, 0, 0, &pBuffer, &len);
		MFZeroMemory(pBuffer, len);
		MFSound_Unlock(pSound);
	}

	++pSound->refCount;

	return pSound;
}

MF_API int MFSound_Destroy(MFSound *pSound)
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

MF_API MFSound *MFSound_FindSound(const char *pSoundName)
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

MF_API MFVoice *MFSound_Play(MFSound *pSound, uint32 playFlags)
{
	MFCALLSTACK;

	if(!pSound)
		return NULL;

	MFSound_LockMutex(true);

	MFVoice *pVoice = gVoices.Create();
	if(!pVoice)
		return NULL;

	MFZeroMemory(pVoice, sizeof(MFVoice) + internalVoiceDataSize);
	pVoice->flags = playFlags;
	pVoice->pSound = pSound;
	pVoice->pInternal = (MFVoiceDataInternal*)((char*)pVoice + sizeof(MFVoice));

	MFSound_PlayInternal(pVoice);

	MFSound_LockMutex(false);

	return pVoice;
}

MF_API uint32 MFSound_GetPlayCursor(MFVoice *pVoice, uint32 *pWriteCursor)
{
	MFSound_LockMutex(true);

	size_t write = 0;
	size_t play = MFSound_GetPlayCursorInternal(pVoice, pWriteCursor ? &write : NULL);

	MFSoundTemplate *pT = pVoice->pSound->pTemplate;
	int bytesPerSample = (pT->bitsPerSample * pT->numChannels) >> 3;

	MFSound_LockMutex(false);

	if(pWriteCursor)
		*pWriteCursor = (uint32)(write / bytesPerSample);

	return (uint32)(play / bytesPerSample);
}

MF_API MFSound *MFSound_GetSoundFromVoice(MFVoice *pVoice)
{
	return pVoice->pSound;
}

MF_API void MFSound_GetSoundInfo(MFSound *pSound, MFSoundInfo *pInfo)
{
	pInfo->sampleRate = pSound->pTemplate->sampleRate;
	pInfo->numSamples = pSound->pTemplate->numSamples;
	pInfo->numChannels = pSound->pTemplate->numChannels;
	pInfo->bitsPerSample = (int16)pSound->pTemplate->bitsPerSample;
}

//
// Fuji default sound buffer implementation.
//

#if defined(USE_MFSOUNDBUFFER)
void MFSound_CreateInternal(MFSound *pSound)
{
	MFCALLSTACK;

	// if dynamic, allocate buffer
	if(pSound->pTemplate->flags & MFSF_Dynamic)
	{
		long bufferSize = ((pSound->pTemplate->numChannels * pSound->pTemplate->bitsPerSample) >> 3) * pSound->pTemplate->numSamples;
		pSound->pTemplate->ppStreams = (char**)MFHeap_Alloc(sizeof(char*) + bufferSize);
		pSound->pTemplate->ppStreams[0] = (char*)&pSound->pTemplate->ppStreams[1];
	}
}

void MFSound_DestroyInternal(MFSound *pSound)
{
	MFCALLSTACK;

	// if dynamic, free buffer
	if(pSound->pTemplate->flags & MFSF_Dynamic)
	{
		MFHeap_Free(pSound->pTemplate->ppStreams);
	}
}

MF_API int MFSound_Lock(MFSound *pSound, size_t offset, size_t bytes, void **ppData, uint32 *pSize, void **ppData2, uint32 *pSize2)
{
	MFCALLSTACK;

	MFDebug_Assert(!(pSound->flags & MFPF_Locked), MFStr("Dynamic sound '%s' is already locked.", pSound->name));

	long bufferSize = ((pSound->pTemplate->numChannels * pSound->pTemplate->bitsPerSample) >> 3) * pSound->pTemplate->numSamples;

	MFDebug_Assert(offset < bufferSize, "MFSound_Lock: Invalid buffer offset.");

	pSound->pLock1 = pSound->pTemplate->ppStreams[0] + offset;

	if(offset + bytes > bufferSize)
	{
		pSound->lockSize1 = bufferSize - offset;
		pSound->pLock2 = pSound->pTemplate->ppStreams[0];
		pSound->lockSize2 = bytes - pSound->lockSize1;
	}
	else
	{
		pSound->lockSize1 = bytes;
		pSound->pLock2 = NULL;
		pSound->lockSize2 = 0;
	}

	pSound->flags |= MFPF_Locked;
	pSound->lockOffset = offset;
	pSound->lockBytes = bytes;

	*ppData = pSound->pLock1;
	*pSize = pSound->lockSize1;
	if(ppData2)
	{
		*ppData2 = pSound->pLock2;
		*pSize2 = pSound->lockSize2;
	}

	return 0;
}

MF_API void MFSound_Unlock(MFSound *pSound)
{
	MFCALLSTACK;

	MFDebug_Assert(pSound->flags & MFPF_Locked, MFStr("Dynamic sound '%s' is not locked.", pSound->name));

	pSound->pLock1 = NULL;
	pSound->lockSize1 = 0;
	pSound->pLock2 = NULL;
	pSound->lockSize2 = 0;

	pSound->flags = pSound->flags & ~MFPF_Locked;
}
#endif

//
// MFAudioStream related functions
//

MF_API void MFSound_RegisterStreamHandler(const char *pStreamType, const char *pStreamExtension, MFStreamCallbacks *pCallbacks)
{
	MFStreamHandler *pHandler = (MFStreamHandler*)MFHeap_Alloc(sizeof(MFStreamHandler));

	MFString_Copy(pHandler->streamType, pStreamType);
	MFString_Copy(pHandler->streamExtension, pStreamExtension);
	pHandler->callbacks = *pCallbacks;

	gStreamHandlers.Create(pHandler);
}

MF_API MFAudioStream *MFSound_CreateStream(const char *pFilename, uint32 flags)
{
	MFCALLSTACK;

	MFSound_LockMutex(true);

	// find free music track
	int t = 0;
	while(gMusicTracks[t].pStreamHandler && t < gDefaults.sound.maxMusicTracks) t++;
	if(t == gDefaults.sound.maxMusicTracks)
	{
		MFDebug_Warn(2, "Maximum number of streams reached, unable to create audio stream.");
		return NULL;
	}

	MFAudioStream *pStream = &gMusicTracks[t];
	MFZeroMemory(pStream, sizeof(*pStream));
	pStream->createFlags = flags;

	MFDebug_Log(4, MFStr("Attempting to create audio stream '%s'.", pFilename));

	// find matching extension
	const char *pExt = MFString_GetFileExtension(pFilename);
	if(!pExt)
	{
		MFDebug_Warn(2, MFStr("Audio stream '%s' has no file extension. Can't select stream handler.", pFilename));
		return NULL;
	}

	MFStreamHandler **ppI = gStreamHandlers.Begin();
	while(*ppI)
	{
		if(!MFString_CaseCmp((*ppI)->streamExtension, pExt))
		{
			pStream->pStreamHandler = *ppI;
			break;
		}

		++ppI;
	}

	if(!pStream->pStreamHandler)
	{
		MFDebug_Warn(2, MFStr("No audio stream handler registered to service '%s' streams.", pExt));
		return NULL;
	}

	// attempt to create the stream...
	pStream->pStreamHandler->callbacks.pCreateStream(pStream, pFilename);

	// check if the stream was created
	if(!pStream->streamInfo.bitsPerSample || !pStream->streamInfo.channels)
	{
		pStream->pStreamHandler = NULL;
		return NULL;
	}

	// create the playback sound buffer
	if(!(pStream->createFlags & MFASF_DecodeOnly))
	{
		pStream->bufferSize = (pStream->streamInfo.bufferLength*pStream->streamInfo.bitsPerSample*pStream->streamInfo.channels) >> 3;

		if(pStream->bufferSize)
			pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, pStream->streamInfo.bufferLength, pStream->streamInfo.channels, pStream->streamInfo.bitsPerSample, pStream->streamInfo.sampleRate, MFSF_Dynamic | MFSF_Circular);

		if(!pStream->pStreamBuffer)
		{
			pStream->pStreamHandler->callbacks.pDestroyStream(pStream);
			pStream->pStreamHandler = NULL;
			return NULL;
		}
	}

	// init the stream
	MFString_CopyN(pStream->name, pFilename, sizeof(pStream->name) - 1);
	pStream->name[sizeof(pStream->name) - 1] = 0;

	MFSound_LockMutex(false);

	return pStream;
}

MF_API void MFSound_PlayStream(MFAudioStream *pStream, uint32 playFlags)
{
	MFCALLSTACK;

	// copy flags
	pStream->playFlags = playFlags;
	if(playFlags & MFPF_BeginPaused)
		pStream->playFlags |= MFPF_Paused;

	// fill the buffer
	MFSound_FillBuffer(pStream, pStream->bufferSize);

	// play buffer
	pStream->pStreamVoice = MFSound_Play(pStream->pStreamBuffer, MFPF_Looping | (playFlags & MFPF_BeginPaused));
}

void MFSound_ServiceStreamBuffer(MFAudioStream *pStream)
{
	MFCALLSTACK;

	size_t playCursor;
	size_t lockSize;

	// get cursor pos
	playCursor = MFSound_GetPlayCursorInternal(pStream->pStreamVoice);

	if(pStream->writePointer == playCursor)
		return;

	// calculate lock size
	if(playCursor < pStream->writePointer)
		lockSize = playCursor + (pStream->bufferSize - pStream->writePointer);
	else
		lockSize = playCursor - pStream->writePointer;

	// update the buffer
	MFSound_FillBuffer(pStream, lockSize);
}

MF_API void MFSound_DestroyStream(MFAudioStream *pStream)
{
	MFCALLSTACK;

	MFSound_LockMutex(true);

	// destroy the playback voice
	if(pStream->pStreamVoice)
	{
		bool playing = !(pStream->playFlags & MFPF_Paused);
		if(playing)
			MFSound_Stop(pStream->pStreamVoice);
	}

	// call stream handler destroy
	pStream->pStreamHandler->callbacks.pDestroyStream(pStream);
	pStream->pStreamHandler = NULL;

	// destroy the sound buffer
	if(pStream->pStreamBuffer)
	{
		MFSound_Destroy(pStream->pStreamBuffer);
		pStream->pStreamBuffer = NULL;
	}

	MFSound_LockMutex(false);
}

MF_API void MFSound_SeekStream(MFAudioStream *pStream, float seconds)
{
	MFCALLSTACK;

	MFSound_LockMutex(true);

	bool playing = !(pStream->playFlags & MFPF_Paused);
	if(playing)
		MFSound_Pause(pStream->pStreamVoice, true);

	pStream->pStreamHandler->callbacks.pSeekStream(pStream, seconds);

	MFSound_SetPlaybackOffset(pStream->pStreamVoice, 0.0f);
	pStream->writePointer = 0;

	MFSound_FillBuffer(pStream, pStream->bufferSize);

	if(playing)
		MFSound_Pause(pStream->pStreamVoice, false);

	MFSound_LockMutex(false);
}

MF_API void MFSound_PauseStream(MFAudioStream *pStream, bool pause)
{
	MFCALLSTACK;

	MFSound_LockMutex(true);

	bool playing = !(pStream->playFlags & MFPF_Paused);
	if(pause)
	{
		if(playing)
			MFSound_Pause(pStream->pStreamVoice, true);
	}
	else
	{
		if(!playing)
			MFSound_Pause(pStream->pStreamVoice, false);
	}

	pStream->playFlags = (pStream->playFlags & ~MFPF_Paused) | (pause ? MFPF_Paused : 0);

	MFSound_LockMutex(false);
}

MF_API MFVoice *MFSound_GetStreamVoice(MFAudioStream *pStream)
{
	return pStream->pStreamVoice;
}

MF_API const char *MFSound_GetStreamInfo(MFAudioStream *pStream, MFStreamInfoType infoType)
{
	switch(infoType)
	{
		case MFSIT_TrackName:
			if(pStream->streamInfo.songName[0])
				return pStream->streamInfo.songName;
			break;
		case MFSIT_AlbumName:
			if(pStream->streamInfo.albumName[0])
				return pStream->streamInfo.albumName;
			break;
		case MFSIT_ArtistName:
			if(pStream->streamInfo.artistName[0])
				return pStream->streamInfo.artistName;
			break;
		case MFSIT_Genre:
			if(pStream->streamInfo.genre[0])
				return pStream->streamInfo.genre;
			break;
		default:
			MFUNREACHABLE;
	}

	return NULL;
}

void MFSound_FillBuffer(MFAudioStream *pStream, size_t bytes)
{
	MFCALLSTACKc;

	void *pData1, *pData2;
	size_t bytes1, bytes2;
	size_t bufferFed = 0;

	// fill buffer
	MFSound_Lock(pStream->pStreamBuffer, pStream->writePointer, bytes, &pData1, &bytes1, &pData2, &bytes2);

	char *pData = (char*)pData1;
	size_t bytesToWrite = bytes1;

	while(bufferFed < bytesToWrite)
	{
		size_t r = pStream->pStreamHandler->callbacks.pGetSamples(pStream, pData, bytesToWrite-bufferFed);

		if(!r)
		{
			if((pStream->playFlags & MFPF_Looping) && pStream->pStreamHandler->callbacks.pSeekStream)
				pStream->pStreamHandler->callbacks.pSeekStream(pStream, 0.0f);
			else
			{
				// TODO: end of the track.. should we stop playback somehow?

				// write silence for the time being...
				r = bytesToWrite-bufferFed;
				MFZeroMemory(pData, r);
			}
		}

		pData += r;
		bufferFed += r;

		if(bufferFed == bytesToWrite && bytes2)
		{
			bufferFed = 0;
			bytesToWrite = bytes2;
			pData = (char*)pData2;
			bytes2 = 0;
		}
	}

	// unlock buffer
	MFSound_Unlock(pStream->pStreamBuffer);

	// increment playback cursor
	pStream->writePointer = (pStream->writePointer + bytes) % pStream->bufferSize;

	// update playback time
	if(pStream->pStreamHandler->callbacks.pGetTime)
		pStream->currentTime = pStream->pStreamHandler->callbacks.pGetTime(pStream);
}

MF_API size_t MFSound_ReadStreamSamples(MFAudioStream *pStream, void *pBuffer, size_t bytes)
{
	return pStream->pStreamHandler->callbacks.pGetSamples(pStream, pBuffer, bytes);
}

// MFSound debug draw

void MFSound_Draw()
{
	MFCALLSTACK;

#if !defined(_RETAIL)
	if(!showSoundStats) return;

	float y = 20.0f;

	for(int a=0; a<gDefaults.sound.maxMusicTracks; ++a)
	{
		if(gMusicTracks[a].pStreamBuffer)
		{
			MFSoundTemplate *pT = gMusicTracks[a].pStreamVoice->pSound->pTemplate;
			uint32 bufferSize = (pT->numSamples * pT->numChannels * pT->bitsPerSample) >> 3;

			MFFont_DrawText2f(MFFont_GetDebugFont(), 20.0f, y, 20.0f, MakeVector(1,1,0,1), "Stream: '%s'", gMusicTracks[a].name);
			y += 20.0f;
			MFFont_DrawText2f(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "TrackLength: %d:%02d, CurrentTime: %d:%02d", ((int)gMusicTracks[a].trackLength) / 60, ((int)gMusicTracks[a].trackLength) % 60, ((int)gMusicTracks[a].currentTime) / 60, ((int)gMusicTracks[a].currentTime) % 60);
			y += 25.0f;

			MFPrimitive(PT_TriStrip|PT_Untextured);

			MFBegin(46);
			MFSetColourV(MFVector::white);
			MFSetPosition(23.0f, y-2.0f, 0.0f);
			MFSetPosition(617.0f, y-2.0f, 0.0f);
			MFSetPosition(23.0f, y+22.0f, 0.0f);
			MFSetPosition(617.0f, y+22.0f, 0.0f);

			MFSetPosition(617.0f, y+22.0f, 0.0f);
			MFSetPosition(25.0f, y, 0.0f);

			MFSetColour(.25f, .25f, .25f, 1.f);
			MFSetPosition(25.0f, y, 0.0f);
			MFSetPosition(615.0f, y, 0.0f);
			MFSetPosition(25.0f, y+20.0f, 0.0f);
			MFSetPosition(615.0f, y+20.0f, 0.0f);

			float xPlayback = 25.0f + (615.0f-25.0f) * (gMusicTracks[a].currentTime/gMusicTracks[a].trackLength);

			MFSetPosition(615.0f, y+20.0f, 0.0f);
			MFSetPosition(xPlayback-1.0f, y-1.0f, 0.0f);

			MFSetColour(0.5f, 0.5f, 1, 1);
			MFSetPosition(xPlayback-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayback+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayback-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xPlayback+1.0f, y+21.0f, 0.0f);

			MFSetPosition(xPlayback+1.0f, y+21.0f, 0.0f);
			y += 30.0f;
			MFSetPosition(98.0f, y-2.0f, 0.0f);

			MFSetColourV(MFVector::white);
			MFSetPosition(98.0f, y-2.0f, 0.0f);
			MFSetPosition(502.0f, y-2.0f, 0.0f);
			MFSetPosition(98.0f, y+22.0f, 0.0f);
			MFSetPosition(502.0f, y+22.0f, 0.0f);

			MFSetPosition(502.0f, y+22.0f, 0.0f);
			MFSetPosition(100.0f, y, 0.0f);

			MFSetColour(.25f, .25f, .25f, 1.f);
			MFSetPosition(100.0f, y, 0.0f);
			MFSetPosition(500.0f, y, 0.0f);
			MFSetPosition(100.0f, y+20.0f, 0.0f);
			MFSetPosition(500.0f, y+20.0f, 0.0f);

			size_t playCursor, writeCursor;
			playCursor = MFSound_GetPlayCursorInternal(gMusicTracks[a].pStreamVoice, &writeCursor);

			float xPlayCursor = 100.0f + (500.0f-100.0f) * ((float)playCursor / (float)bufferSize);

			MFSetPosition(500.0f, y+20.0f, 0.0f);
			MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);

			MFSetColourV(MFVector::yellow);
			MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayCursor+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayCursor-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);

			float xWriteCursor = 100.0f + (500.0f-100.0f) * ((float)writeCursor / (float)bufferSize);

			MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);
			MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);

			MFSetColourV(MFVector::red);
			MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xWriteCursor+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xWriteCursor-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);

			float xBufferFilled = 100.0f + (500.0f-100.0f) * ((float)gMusicTracks[a].writePointer/(float)gMusicTracks[a].bufferSize);

			MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);
			MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);

			MFSetColour(1.f, .5f, 0.f, 1.f);
			MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xBufferFilled+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xBufferFilled-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xBufferFilled+1.0f, y+21.0f, 0.0f);

			y += 30.0f;

			MFEnd();
		}
	}

	MFVoice **ppI = gVoices.Begin();

	while(*ppI)
	{
		MFVoice *pV = *ppI;
		MFSoundTemplate *pT = pV->pSound->pTemplate;

		uint32 bufferSize = (pT->numSamples * pT->numChannels * pT->bitsPerSample) >> 3;

		MFFont_DrawText2f(MFFont_GetDebugFont(), 20.0f, y, 20.0f, MakeVector(1,1,0,1), "Voice: '%s'", pV->pSound->name);
		y += 20.0f;

//		MFFont_DrawTextf(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "Channels: %d, Samplerate: %d, AvgBitrate: %dkbps, Version: %d", pV->pSound->pTemplate->numChannels, pV->pSound->pTemplate->sampleRate, gMusicTracks[a].pInfo->bitrate_nominal/1000, gMusicTracks[a].pInfo->version);
		MFFont_DrawText2f(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "Channels: %d, Samplerate: %d", pT->numChannels, pT->sampleRate);
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

		MFFont_DrawText2f(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "Buffer:");
/*
		MFPrimitive_DrawUntexturedQuad(MakeVector(98.0f, y-2.0f), MakeVector(502.0f, y+22.0f), 0xFFFFFFFF);
		MFPrimitive_DrawUntexturedQuad(MakeVector(100.0f, y), MakeVector(500.0f, y+20.0f), 0xFF404040);

		MFPrimitive_DrawUntexturedQuad(MakeVector(xPlayCursor-1.0f, y-1.0f), MakeVector(xPlayCursor+1.0f, y+21.0f), 0xFFFFFF00);
		MFPrimitive_DrawUntexturedQuad(MakeVector(xWriteCursor-1.0f, y-1.0f), MakeVector(xWriteCursor+1.0f, y+21.0f), 0xFF0000FF);
*/
		MFPrimitive(PT_TriStrip|PT_Untextured);

		MFBegin(22);
		MFSetColourV(MFVector::white);
		MFSetPosition(98.0f, y-2.0f, 0.0f);
		MFSetPosition(502.0f, y-2.0f, 0.0f);
		MFSetPosition(98.0f, y+22.0f, 0.0f);
		MFSetPosition(502.0f, y+22.0f, 0.0f);

		MFSetPosition(502.0f, y+22.0f, 0.0f);
		MFSetPosition(100.0f, y, 0.0f);

		MFSetColour(.25f, .25f, .25f, 1.f);
		MFSetPosition(100.0f, y, 0.0f);
		MFSetPosition(500.0f, y, 0.0f);
		MFSetPosition(100.0f, y+20.0f, 0.0f);
		MFSetPosition(500.0f, y+20.0f, 0.0f);

		size_t playCursor, writeCursor;
		playCursor = MFSound_GetPlayCursorInternal(pV, &writeCursor);

		float xPlayCursor = 100.0f + (500.0f-100.0f) * ((float)playCursor / (float)bufferSize);

		MFSetPosition(500.0f, y+20.0f, 0.0f);
		MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);

		MFSetColourV(MFVector::yellow);
		MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);
		MFSetPosition(xPlayCursor+1.0f, y-1.0f, 0.0f);
		MFSetPosition(xPlayCursor-1.0f, y+21.0f, 0.0f);
		MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);

		float xWriteCursor = 100.0f + (500.0f-100.0f) * ((float)writeCursor / (float)bufferSize);

		MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);
		MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);

		MFSetColourV(MFVector::red);
		MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);
		MFSetPosition(xWriteCursor+1.0f, y-1.0f, 0.0f);
		MFSetPosition(xWriteCursor-1.0f, y+21.0f, 0.0f);
		MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);
/*
		float xBufferFilled = 100.0f + (500.0f-100.0f) * ((float)gMusicTracks[a].writePointer / (float)bufferSize);

		MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);
		MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);

		MFSetColour(1.f, .5f, 0.f, 1.f);
		MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);
		MFSetPosition(xBufferFilled+1.0f, y-1.0f, 0.0f);
		MFSetPosition(xBufferFilled-1.0f, y+21.0f, 0.0f);
		MFSetPosition(xBufferFilled+1.0f, y+21.0f, 0.0f);
*/
		MFEnd();

		y += 35.0f;

		ppI++;
	}
#endif
}
