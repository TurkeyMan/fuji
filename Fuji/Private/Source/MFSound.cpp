#include "Fuji.h"
#include "MFSound_Internal.h"
#include "MFPtrList.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "DebugMenu.h"

#if defined(_WINDOWS)
	#define VORBIS_STREAM
#endif

#if defined(VORBIS_STREAM)
	#include <vorbis/vorbisfile.h>
#endif

/**** Foreward Declarations ****/

void MFSound_FillBuffer(MFAudioStream *pStream, int bytes);
void MFSound_ServiceStreamBuffer(MFAudioStream *pStream);


/**** Structures ****/

struct MFAudioStream
{
	char name[256];

#if defined(VORBIS_STREAM)
	OggVorbis_File vorbisFile;
	vorbis_info *pInfo;
#endif

	MFSound *pStreamBuffer;
	MFVoice *pStreamVoice;

	uint32 bufferSize;
	uint32 playBackOffset;

	float trackLength;
	float currentTime;

	bool playing;
};


/**** Globals ****/

MFPtrListDL<MFSound> gSoundBank;
MFPtrListDL<MFVoice> gVoices;

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

	gMusicTracks = (MFAudioStream*)MFHeap_Alloc(sizeof(MFAudioStream) * gDefaults.sound.maxMusicTracks);
	MFZeroMemory(gMusicTracks, sizeof(MFAudioStream)*gDefaults.sound.maxMusicTracks);

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

		bool bFinished = MFSound_UpdateInternal(pV);

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
		MFString_Copy(pSound->name, pName);

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
		pTemplate->numChannels = numChannels;
		pTemplate->bitsPerSample = bitsPerSample;
		pTemplate->format = MFWF_PCM_s16;

		// create the sound
		pSound = gSoundBank.Create();
		MFZeroMemory(pSound, sizeof(MFSound) + internalSoundDataSize);
		pSound->pTemplate = pTemplate;
		pSound->pInternal = (MFSoundDataInternal*)((char*)pSound + sizeof(MFSound));
		MFString_Copy(pSound->name, pName);

		MFSound_CreateInternal(pSound);

		// clear buffer
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

	MFVoice *pVoice = gVoices.Create();
	MFZeroMemory(pVoice, sizeof(MFVoice) + internalVoiceDataSize);
	pVoice->flags = playFlags;
	pVoice->pSound = pSound;
	pVoice->pInternal = (MFVoiceDataInternal*)((char*)pVoice + sizeof(MFVoice));

	MFSound_PlayInternal(pVoice);

	return pVoice;
}


#if defined(VORBIS_STREAM)
//
// Vorbis Music Functions
//
int MFSound_VorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	return MFFile_StdSeek(datasource, (long)offset, whence);
}
#endif

//
// MFAudioStream related functions
//
MFAudioStream *MFSound_PlayStream(const char *pFilename, bool pause)
{
	MFCALLSTACK;

	int t = 0;

	// find free music track
	while(gMusicTracks[t].pStreamBuffer && t < gDefaults.sound.maxMusicTracks) t++;
	if(t == gDefaults.sound.maxMusicTracks) return NULL;

	MFAudioStream *pStream = &gMusicTracks[t];

#if defined(VORBIS_STREAM)
	// open vorbis file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return NULL;

	// setup vorbis read callbacks
	ov_callbacks callbacks;
	callbacks.read_func = MFFile_StdRead;
	callbacks.seek_func = MFSound_VorbisSeek;
	callbacks.close_func = MFFile_StdClose;
	callbacks.tell_func = MFFile_StdTell;

	// open vorbis file
	if(ov_test_callbacks(hFile, &pStream->vorbisFile, NULL, 0, callbacks))
	{
		MFDebug_Assert(false, "Not a vorbis file.");
		return NULL;
	}

	ov_test_open(&pStream->vorbisFile);

	// copy the filename
	MFString_Copy(pStream->name, pFilename);

	// get vorbis file info
	pStream->pInfo = ov_info(&pStream->vorbisFile, -1);

	pStream->trackLength = (float)ov_time_total(&pStream->vorbisFile, -1);
	pStream->currentTime = 0.0f;
	pStream->playBackOffset = 0;
	pStream->bufferSize = pStream->pInfo->rate * pStream->pInfo->channels * 2;

	pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, pStream->pInfo->rate, pStream->pInfo->channels, 16, pStream->pInfo->rate, MFSF_Dynamic | MFSF_Circular);
#else
	pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, 44100, 2, 16, 44100, MFSF_Dynamic | MFSF_Circular);
#endif

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

#if defined(VORBIS_STREAM)
	pStream->pInfo = NULL;
	ov_clear(&pStream->vorbisFile);
#endif

	MFSound_Destroy(pStream->pStreamBuffer);
	pStream->pStreamBuffer = NULL;
}

void MFSound_SeekStream(MFAudioStream *pStream, float seconds)
{
	MFCALLSTACK;

#if defined(VORBIS_STREAM)
	ov_time_seek(&pStream->vorbisFile, seconds);
#endif

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
	MFCALLSTACK;

	void *pData1, *pData2;
	uint32 bytes1, bytes2;
	int currentBitstream;
	uint32 bufferFed = 0;

	// fill buffer
	MFSound_Lock(pStream->pStreamBuffer, pStream->playBackOffset, bytes, &pData1, &bytes1, &pData2, &bytes2);

	char *pData = (char*)pData1;
	uint32 bytesToWrite = bytes1;
	bool wrapped = false;

#if defined(VORBIS_STREAM)
	while(bufferFed < bytesToWrite)
	{
		int r = ov_read(&pStream->vorbisFile, pData, bytesToWrite-bufferFed, 0, 2, 1, &currentBitstream);

		if(!r)
		{
			ov_time_seek(&pStream->vorbisFile, 0.0);
		}

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
#endif

	// unlock buffer
	MFSound_Unlock(pStream->pStreamBuffer);

	// increment playback cursor
	pStream->playBackOffset = (pStream->playBackOffset + bytes) % pStream->bufferSize;

	// update playback time
#if defined(VORBIS_STREAM)
	pStream->currentTime = (float)ov_time_tell(&pStream->vorbisFile);
#else
	pStream->currentTime = 0.0f;
#endif
}


void MFSound_Draw()
{
	MFCALLSTACK;

#if defined(VORBIS_STREAM)
#if !defined(_RETAIL)
	if(!showSoundStats) return;

	float y = 20.0f;

	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		if(gMusicTracks[a].playing)
		{
			MFFont_DrawTextf(MFFont_GetDebugFont(), 20.0f, y, 20.0f, MakeVector(1,1,0,1), "Track %d: %s", a, gMusicTracks[a].name);
			y += 20.0f;

			MFFont_DrawTextf(MFFont_GetDebugFont(), 30.0f, y, 20.0f, MFVector::one, "Channels: %d, Samplerate: %d, AvgBitrate: %dkbps, Version: %d", gMusicTracks[a].pInfo->channels, gMusicTracks[a].pInfo->rate, gMusicTracks[a].pInfo->bitrate_nominal/1000, gMusicTracks[a].pInfo->version);
			y += 20.0f;

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
			playCursor = MFSound_GetPlayCursor(gMusicTracks[a].pStreamVoice, &writeCursor);

			float xPlayCursor = 100.0f + (500.0f-100.0f) * ((float)playCursor / (float)gMusicTracks[a].bufferSize);

			MFSetPosition(500.0f, y+20.0f, 0.0f);
			MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);

			MFSetColour(0xFFFFFF00);
			MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayCursor+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayCursor-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);

			float xWriteCursor = 100.0f + (500.0f-100.0f) * ((float)writeCursor / (float)gMusicTracks[a].bufferSize);

			MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);
			MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);

			MFSetColour(0xFF0000FF);
			MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xWriteCursor+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xWriteCursor-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);

			float xBufferFilled = 100.0f + (500.0f-100.0f) * ((float)gMusicTracks[a].playBackOffset / (float)gMusicTracks[a].bufferSize);

			MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);
			MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);

			MFSetColour(0xFFFF8000);
			MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xBufferFilled+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xBufferFilled-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xBufferFilled+1.0f, y+21.0f, 0.0f);
			MFEnd();

			y += 35.0f;
		}
	}
#endif
#endif
}
