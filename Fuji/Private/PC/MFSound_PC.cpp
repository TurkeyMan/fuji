#include "Fuji.h"
#include "MFFileSystem.h"
#include "MFPtrList.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "DebugMenu.h"
#include "MFSound_Internal.h"
#include "MFSystem.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Mmreg.h>
#include <dsound.h>
#include <vorbis/vorbisfile.h>

struct MFSound
{
	char name[64];

	MFSoundTemplate *pTemplate;
	int refCount;

	IDirectSoundBuffer *pBuffer;
	IDirectSoundBuffer *p3DBuffer;
};

struct MFVoice
{
	MFSound *pSound;

	IDirectSoundBuffer *pBuffer;
	IDirectSoundBuffer8 *pBuffer8;
	IDirectSound3DBuffer8 *p3DBuffer8;

	uint32 flags;
};

struct MFAudioStream
{
	char name[256];

	OggVorbis_File vorbisFile;

	vorbis_info *pInfo;

	IDirectSoundBuffer *pDSMusicBuffer;

	uint32 bufferSize;
	uint32 playBackOffset;

	float trackLength;
	float currentTime;

	bool playing;
};


void MFSound_FillBufferPC(MFAudioStream *pStream, int bytes);
void MFSound_ServiceStreamBuffer(MFAudioStream *pStream);


MFPtrListDL<MFSound> gSoundBank;
MFPtrListDL<MFVoice> gVoices;

extern HWND apphWnd;

IDirectSound8 *pDirectSound;
static IDirectSoundBuffer *pDSPrimaryBuffer;

static MFAudioStream *gMusicTracks;

#if !defined(_RETAIL)
MenuItemBool showSoundStats;
#endif


void MFSound_InitModule()
{
	MFCALLSTACK;

	HRESULT hr;

	gSoundBank.Init("Sound Bank", gDefaults.sound.maxSounds);
	gVoices.Init("Voice Bank", gDefaults.sound.maxVoices);

	gMusicTracks = (MFAudioStream*)MFHeap_Alloc(sizeof(MFAudioStream) * gDefaults.sound.maxMusicTracks);
	MFZeroMemory(gMusicTracks, sizeof(MFAudioStream)*gDefaults.sound.maxMusicTracks);

	DirectSoundCreate8(NULL, &pDirectSound, NULL);

	// create the primary sound buffer
	// fill out DSBuffer creation data
	DSBUFFERDESC desc;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
	desc.dwBufferBytes = 0;
	desc.lpwfxFormat = NULL;
	desc.dwReserved = 0; 
	desc.guid3DAlgorithm = DS3DALG_DEFAULT; 

	// create the DSBuffer
	hr = pDirectSound->CreateSoundBuffer(&desc, &pDSPrimaryBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create the Primary Sound Buffer");

	hr = pDirectSound->SetCooperativeLevel(apphWnd, DSSCL_PRIORITY);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to set the DirectSound cooperative level");

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
		if(gMusicTracks[a].pDSMusicBuffer)
		{
			MFSound_DestroyStream(&gMusicTracks[a]);
		}
	}

	pDSPrimaryBuffer->Release();
	pDirectSound->Release();

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
		DWORD stat;

		pV->pBuffer8->GetStatus(&stat);

		if(stat != DSBSTATUS_PLAYING)
		{
			// destroy voice
			if(pV->p3DBuffer8)
				pV->p3DBuffer8->Release();
			pV->pBuffer8->Release();
			pV->pBuffer->Release();

			gVoices.Destroy(ppI);
		}

		ppI++;
	}

	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		if(gMusicTracks[a].pDSMusicBuffer && gMusicTracks[a].playing)
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

		pSound = gSoundBank.Create();
		pSound->pTemplate = pTemplate;
		pSound->refCount = 0;
		MFString_Copy(pSound->name, pName);

		int bytesPerSample = pTemplate->numChannels * (pTemplate->bitsPerSample>>3);

		DSBUFFERDESC desc;
		WAVEFORMATEX wfx;

		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
		desc.dwBufferBytes = pTemplate->numSamples * bytesPerSample;
		desc.dwReserved = 0;
		desc.guid3DAlgorithm = DS3DALG_DEFAULT;
		desc.lpwfxFormat = &wfx;
		
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nChannels = pTemplate->numChannels;
		wfx.nSamplesPerSec = pTemplate->sampleRate;
		wfx.nAvgBytesPerSec = pTemplate->sampleRate * bytesPerSample;
		wfx.nBlockAlign = bytesPerSample;
		wfx.wBitsPerSample = pTemplate->bitsPerSample;
		wfx.cbSize = 0;

		// create the 2d buffer
		pDirectSound->CreateSoundBuffer(&desc, &pSound->pBuffer, NULL);

		// and create the 3d buffer
		desc.dwFlags |= DSBCAPS_CTRL3D;
		pDirectSound->CreateSoundBuffer(&desc, &pSound->p3DBuffer, NULL);

		// lock the buffers and copy in the data
		VOID *pBuffer, *pB2;
		DWORD bufferLen, l2;

		pSound->pBuffer->Lock(0, 0, &pBuffer, &bufferLen, &pB2, &l2, DSBLOCK_ENTIREBUFFER);
		MFDebug_Assert(desc.dwBufferBytes == bufferLen, "Error locking sound buffer.");
		MFCopyMemory(pBuffer, pTemplate->ppStreams[0], bufferLen);
		pSound->pBuffer->Unlock(pBuffer, bufferLen, pB2, l2);

		pSound->p3DBuffer->Lock(0, 0, &pBuffer, &bufferLen, &pB2, &l2, DSBLOCK_ENTIREBUFFER);
		MFDebug_Assert(desc.dwBufferBytes == bufferLen, "Error locking sound buffer.");
		MFCopyMemory(pBuffer, pTemplate->ppStreams[0], bufferLen);
		pSound->p3DBuffer->Unlock(pBuffer, bufferLen, pB2, l2);
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
		pSound->pBuffer->Release();
		pSound->p3DBuffer->Release();

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
	pVoice->flags = playFlags;
	pVoice->pSound = pSound;

	if(playFlags & MFSF_3D)
	{
		pDirectSound->DuplicateSoundBuffer(pSound->p3DBuffer, &pVoice->pBuffer);
		pVoice->pBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pVoice->p3DBuffer8);

	}
	else
	{
		pDirectSound->DuplicateSoundBuffer(pSound->pBuffer, &pVoice->pBuffer);
		pVoice->p3DBuffer8 = NULL;
	}

	pVoice->pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (VOID**)&pVoice->pBuffer8);

	if(!(playFlags & MFSF_BeginPaused))
	{
		DWORD playbackFlags = (playFlags & MFSF_Looping) ? DSBPLAY_LOOPING : 0;
		pVoice->pBuffer8->Play(0, 0, playbackFlags);
	}
	else
		pVoice->flags |= MFBIT(28);

	return pVoice;
}

void MFSound_Pause(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;

	if(pause && !(pVoice->flags & MFBIT(28)))
	{
		pVoice->pBuffer8->Stop();
		pVoice->flags |= MFBIT(28);
	}
	else if(!pause && (pVoice->flags & MFBIT(28)))
	{
		DWORD playbackFlags = (pVoice->flags & MFSF_Looping) ? DSBPLAY_LOOPING : 0;
		pVoice->pBuffer8->Play(0, 0, playbackFlags);
		pVoice->flags &= ~MFBIT(28);
	}
}

void MFSound_Stop(MFVoice *pVoice)
{
	MFCALLSTACK;

	pVoice->pBuffer8->Stop();
}

void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
	MFCALLSTACK;

}

void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
	MFCALLSTACK;

	float vol = (float)(DSBVOLUME_MAX - DSBVOLUME_MIN) * volume;
	pVoice->pBuffer8->SetVolume((LONG)vol + DSBVOLUME_MIN);
}

void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;

	pVoice->pBuffer8->SetFrequency((DWORD)((float)pVoice->pSound->pTemplate->sampleRate * rate));
}

void MFSound_SetMasterVolume(float volume)
{
	MFCALLSTACK;

//	pDSPrimaryBuffer->SetVolume();
}

void MFSound_Draw()
{
	MFCALLSTACK;

#if !defined(_RETAIL)
	if(!showSoundStats) return;

	float y = 20.0f;

	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		if(gMusicTracks[a].pDSMusicBuffer)
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

			DWORD playCursor, writeCursor;
			gMusicTracks[a].pDSMusicBuffer->GetCurrentPosition(&playCursor, &writeCursor);

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
}



//
// Vorbis Music Functions
//
int MFSoundPC_VorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	return MFFile_StdSeek(datasource, (long)offset, whence);
}

//
// MFAudioStream related functions
//
MFAudioStream *MFSound_PlayStream(const char *pFilename, bool pause)
{
	MFCALLSTACK;

	int t = 0;

	// find free music track
	while(gMusicTracks[t].pDSMusicBuffer && t < gDefaults.sound.maxMusicTracks) t++;
	if(t == gDefaults.sound.maxMusicTracks) return NULL;

	MFAudioStream *pStream = &gMusicTracks[t];

	// open vorbis file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return NULL;

	// setup vorbis read callbacks
	ov_callbacks callbacks;
	callbacks.read_func = MFFile_StdRead;
	callbacks.seek_func = MFSoundPC_VorbisSeek;
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

	// fill out DSBuffer creation data
	DSBUFFERDESC desc;
	WAVEFORMATEX wfx;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)pStream->pInfo->channels;
	wfx.nSamplesPerSec = pStream->pInfo->rate;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample/8);
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = 0;

	pStream->bufferSize = wfx.nAvgBytesPerSec;
	pStream->playBackOffset = 0;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
	desc.dwBufferBytes = pStream->bufferSize;
	desc.lpwfxFormat = &wfx;
	desc.dwReserved = 0; 
	desc.guid3DAlgorithm = DS3DALG_DEFAULT; 

	// create the DSBuffer
	pDirectSound->CreateSoundBuffer(&desc, &pStream->pDSMusicBuffer, NULL);

	// fill the buffer
	MFSound_FillBufferPC(pStream, pStream->bufferSize);

	// dont begin playback is we start paused
	pStream->playing = !pause;

	if(!pause)
	{
		// play buffer
		pStream->pDSMusicBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}

	return pStream;
}

void MFSound_ServiceStreamBuffer(MFAudioStream *pStream)
{
	MFCALLSTACK;

	DWORD playCursor;
	int lockSize;

	// get cursor pos
	pStream->pDSMusicBuffer->GetCurrentPosition(&playCursor, NULL);

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
	MFSound_FillBufferPC(pStream, lockSize);
}

void MFSound_DestroyStream(MFAudioStream *pStream)
{
	MFCALLSTACK;

	if(pStream->playing) pStream->pDSMusicBuffer->Stop();

	pStream->pInfo = NULL;
	ov_clear(&pStream->vorbisFile);

	pStream->pDSMusicBuffer->Release();
	pStream->pDSMusicBuffer = NULL;
}

void MFSound_SeekStream(MFAudioStream *pStream, float seconds)
{
	MFCALLSTACK;

	ov_time_seek(&pStream->vorbisFile, seconds);

	if(pStream->playing)
		pStream->pDSMusicBuffer->Stop();

	pStream->pDSMusicBuffer->SetCurrentPosition(0);
	pStream->playBackOffset = 0;
	MFSound_FillBufferPC(pStream, pStream->bufferSize);

	if(pStream->playing)
		pStream->pDSMusicBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

void MFSound_PauseStream(MFAudioStream *pStream, bool pause)
{
	MFCALLSTACK;

	if(pause)
	{
		if(pStream->playing)
			pStream->pDSMusicBuffer->Stop();
	}
	else
	{
		if(!pStream->playing)
			pStream->pDSMusicBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}

	pStream->playing = !pause;
}

void MFSound_SetStreamVolume(MFAudioStream *pStream, float volume)
{
	MFCALLSTACK;

//	pStream->pDSMusicBuffer->SetVolume();
}


void MFSound_FillBufferPC(MFAudioStream *pStream, int bytes)
{
	MFCALLSTACK;

	void *pData1, *pData2;
	DWORD bytes1, bytes2;
	int currentBitstream;
	uint32 bufferFed = 0;

	// fill buffer
	pStream->pDSMusicBuffer->Lock(pStream->playBackOffset, bytes, &pData1, &bytes1, &pData2, &bytes2, NULL);

	char *pData = (char*)pData1;
	uint32 bytesToWrite = bytes1;
	bool wrapped = false;

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

	// unlock buffer;
	pStream->pDSMusicBuffer->Unlock(pData1, bytes1, pData2, bytes2);

	// increment playback cursor
	pStream->playBackOffset = (pStream->playBackOffset + bytes) % pStream->bufferSize;

	// update playback time
	pStream->currentTime = (float)ov_time_tell(&pStream->vorbisFile);
}
