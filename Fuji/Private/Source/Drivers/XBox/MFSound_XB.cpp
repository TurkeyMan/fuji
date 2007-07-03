#include "Fuji.h"

#if MF_SOUND == XBOX

//#define _USE_LIBVORBIS

#include "MFFileSystem.h"
#include "MFPtrList.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "DebugMenu.h"
#include "MFSound_Internal.h"
#include "MFSystem.h"

#if defined(_USE_LIBVORBIS)
#include <vorbis/vorbisfile.h>
#endif

struct MFSoundMusic
{
	char name[256];

#if defined(_USE_LIBVORBIS)
	OggVorbis_File vorbisFile;

	vorbis_info *pInfo;
#endif

	IDirectSoundBuffer *pDSMusicBuffer;

	uint32 bufferSize;
	uint32 playBackOffset;

	float trackLength;
	float currentTime;

	bool playing;
};

void MFSound_ServiceMusicBuffer(int track);


IDirectSound8 *pDirectSound;

MFSoundMusic *gMusicTracks;

#if !defined(_RETAIL)
MenuItemBool showSoundStats;
#endif


void MFSound_InitModule()
{
	MFCALLSTACK;

	DirectSoundCreate(NULL, &pDirectSound, NULL);

	gMusicTracks = (MFSoundMusic*)MFHeap_Alloc(sizeof(MFSoundMusic) * gDefaults.sound.maxMusicTracks);
	MFZeroMemory(gMusicTracks, sizeof(MFSoundMusic)*gDefaults.sound.maxMusicTracks);

#if !defined(_RETAIL)
	DebugMenu_AddMenu("Sound Options", "Fuji Options");
	DebugMenu_AddItem("Show Sound Stats", "Sound Options", &showSoundStats);
#endif
}

void MFSound_DeinitModule()
{
	MFCALLSTACK;

	pDirectSound->Release();
}

void MFSound_Update()
{
	MFCALLSTACKc;

	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		if(gMusicTracks[a].pDSMusicBuffer && gMusicTracks[a].playing)
		{
			MFSound_ServiceMusicBuffer(a);
		}
	}
}

void MFSound_Draw()
{
	MFCALLSTACK;

#if !defined(_RETAIL)
	if(!showSoundStats) return;

	float y = 20.0f;

#if defined(_USE_LIBVORBIS)
	for(int a=0; a<gDefaults.sound.maxMusicTracks; a++)
	{
		if(gMusicTracks[a].pDSMusicBuffer)
		{
			Font_DrawTextf(gpDebugFont, 20.0f, y, 20.0f, 0xFFFFFF00, "Track %d: %s", a, gMusicTracks[a].name);
			y += 20.0f;

			Font_DrawTextf(gpDebugFont, 30.0f, y, 20.0f, 0xFFFFFFFF, "Channels: %d, Samplerate: %d, AvgBitrate: %dkbps, Version: %d", gMusicTracks[a].pInfo->channels, gMusicTracks[a].pInfo->rate, gMusicTracks[a].pInfo->bitrate_nominal/1000, gMusicTracks[a].pInfo->version);
			y += 20.0f;

			static float bitrate = 0.0f;
			long br = ov_bitrate_instant(&gMusicTracks[a].vorbisFile);

			if(br)
				bitrate = (float)br*0.02f + bitrate * 0.98f;

			Font_DrawTextf(gpDebugFont, 30.0f, y, 20.0f, 0xFFFFFFFF, "CurrentBitrate: %dkbps", ((int)bitrate)/1000);
			y += 20.0f;

			Font_DrawTextf(gpDebugFont, 30.0f, y, 20.0f, 0xFFFFFFFF, "TrackLength: %d:%02d, CurrentTime: %d:%02d", ((int)gMusicTracks[a].trackLength) / 60, ((int)gMusicTracks[a].trackLength) % 60, ((int)gMusicTracks[a].currentTime) / 60, ((int)gMusicTracks[a].currentTime) % 60);
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

			MFSetColour(0xFF8080FF);
			MFSetPosition(xPlayback-1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayback+1.0f, y-1.0f, 0.0f);
			MFSetPosition(xPlayback-1.0f, y+21.0f, 0.0f);
			MFSetPosition(xPlayback+1.0f, y+21.0f, 0.0f);
			MFEnd();

			y += 30.0f;

			Font_DrawTextf(gpDebugFont, 30.0f, y, 20.0f, 0xFFFFFFFF, "Buffer:");

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
#endif
}


int MFSound_LoadBank(const char *pFilename)
{
	MFCALLSTACK;

	return -1;
}

void MFSound_UnloadBank(int bankID)
{
	MFCALLSTACK;

}

int MFSound_FindSound(const char *pSoundName, int searchBankID)
{
	MFCALLSTACK;

	return -1;
}

int MFSound_Play(int soundID)
{
	MFCALLSTACK;

	return 0;
}

int MFSound_Play3D(int soundID)
{
	MFCALLSTACK;

	return 0;
}

void MFSound_Stop(int soundID)
{
	MFCALLSTACK;

}

void MFSound_SetListenerPos(const MFVector& listenerPos)
{
	MFCALLSTACK;

}


void MFSound_SetVolume(int soundID, float volume)
{
	MFCALLSTACK;

}

void MFSound_SetMasterVolume(int soundID, float volume)
{
	MFCALLSTACK;

}

void MFSound_SetPlaybackRate(int soundID, float rate)
{
	MFCALLSTACK;

}

#if defined(_USE_LIBVORBIS)
int MFSoundXB_VorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	return MFFile_StdSeek(datasource, (long)offset, whence);
}
#endif

//
// Vorbis Music Functions
//
int MFSound_MusicPlay(const char *pFilename, bool pause)
{
	MFCALLSTACK;

	int t = 0;

#if defined(_USE_LIBVORBIS)
	// fine free music track
	while(gMusicTracks[t].pDSMusicBuffer && t < gDefaults.sound.maxMusicTracks) t++;
	if(t == gDefaults.sound.maxMusicTracks) return -1;

	SoundMusic& track = gMusicTracks[t];

	// open vorbis file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return -1;

	// setup vorbis read callbacks
	ov_callbacks callbacks;
	callbacks.read_func = MFFile_StdRead;
	callbacks.seek_func = SoundXB_VorbisSeek;
	callbacks.close_func = MFFile_StdClose;
	callbacks.tell_func = MFFile_StdTell;

	// open vorbis file
	if(ov_test_callbacks(hFile, &track.vorbisFile, NULL, 0, callbacks))
	{
		MFDebug_Assert(false, "Not a vorbis file.");
		return -1;
	}

	ov_test_open(&track.vorbisFile);

	// copy the filename
	MFString_Copy(track.name, pFilename);

	// get vorbis file info
	track.pInfo = ov_info(&track.vorbisFile, -1);

	track.trackLength = (float)ov_time_total(&track.vorbisFile, -1);
	track.currentTime = 0.0f;

	// fill out DSBuffer creation data
	DSBUFFERDESC desc;
	WAVEFORMATEX wfx;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)track.pInfo->channels;
	wfx.nSamplesPerSec = track.pInfo->rate;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample/8);
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = 0;

	track.bufferSize = wfx.nAvgBytesPerSec;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = 0;
	desc.dwBufferBytes = track.bufferSize;
	desc.lpwfxFormat = &wfx;
	desc.lpMixBins = NULL;
	desc.dwInputMixBin = 0;

	// create the DSBuffer
	DirectSoundCreateBuffer(&desc, &track.pDSMusicBuffer);

	track.playBackOffset = 0;

	void *pData1, *pData2;
	DWORD bytes1, bytes2;
	int currentBitstream;
	uint32 bufferFed = 0;

	// fill buffer
	track.pDSMusicBuffer->Lock(0, track.bufferSize, &pData1, &bytes1, &pData2, &bytes2, DSBLOCK_ENTIREBUFFER);

	char *pData = (char*)pData1;

	while(bufferFed < track.bufferSize)
	{
		int r = ov_read(&track.vorbisFile, pData, track.bufferSize-bufferFed, 0, 2, 1, &currentBitstream);

		if(!r)
		{
			ov_time_seek(&track.vorbisFile, 0.0);
		}

		pData += r;
		bufferFed += r;
	}

	track.pDSMusicBuffer->Unlock(pData1, bytes1, pData2, bytes2);

	track.pDSMusicBuffer->Play(0, 0, DSBPLAY_FROMSTART|DSBPLAY_LOOPING);

	// dont begin playback is we start paused
	if(pause)
	{
		track.pDSMusicBuffer->Pause(DSBPAUSE_PAUSE);
		track.playing = false;
		return t;
	}

	// play buffer
	track.playing = true;
#endif

	return t;
}

void MFSound_ServiceMusicBuffer(int trackID)
{
	MFCALLSTACK;

#if defined(_USE_LIBVORBIS)
	SoundMusic& track = gMusicTracks[trackID];

	void *pData1, *pData2;
	DWORD bytes1, bytes2;
	int currentBitstream;
	uint32 bufferFed = 0;

	DWORD playCursor;

	// get cursor pos
	track.pDSMusicBuffer->GetCurrentPosition(&playCursor, NULL);

	// calculate lock size
	int lockSize;

	if(playCursor < track.playBackOffset)
	{
		lockSize = playCursor + (track.bufferSize - track.playBackOffset);
	}
	else
	{
		lockSize = playCursor - track.playBackOffset;
	}

	// fill buffer
	track.pDSMusicBuffer->Lock(track.playBackOffset, lockSize, &pData1, &bytes1, &pData2, &bytes2, NULL);

	char *pData = (char*)pData1;
	uint32 bytesToWrite = bytes1;
	bool wrapped = false;

	while(bufferFed < bytesToWrite)
	{
		int r = ov_read(&track.vorbisFile, pData, bytesToWrite-bufferFed, 0, 2, 1, &currentBitstream);

		if(!r)
		{
			ov_time_seek(&track.vorbisFile, 0.0);
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
	track.pDSMusicBuffer->Unlock(pData1, bytes1, pData2, bytes2);

	// increment playback cursor
	track.playBackOffset = playCursor;

	// update playback time
	track.currentTime = (float)ov_time_tell(&track.vorbisFile);
#endif
}

void MFSound_MusicUnload(int track)
{
	MFCALLSTACK;

#if defined(_USE_LIBVORBIS)
	if(gMusicTracks[track].playing) gMusicTracks[track].pDSMusicBuffer->Stop();

	gMusicTracks[track].pInfo = NULL;
	ov_clear(&gMusicTracks[track].vorbisFile);

	gMusicTracks[track].pDSMusicBuffer->Release();
	gMusicTracks[track].pDSMusicBuffer = NULL;
#endif
}

void MFSound_MusicSeek(int track, float seconds)
{
	MFCALLSTACK;

#if defined(_USE_LIBVORBIS)
	ov_time_seek(&gMusicTracks[track].vorbisFile, seconds);
#endif
}

void MFSound_MusicPause(int track, bool pause)
{
	MFCALLSTACK;

	if(pause)
	{
		if(gMusicTracks[track].playing)
			gMusicTracks[track].pDSMusicBuffer->Pause(DSBPAUSE_PAUSE);
	}
	else
	{
		if(!gMusicTracks[track].playing)
			gMusicTracks[track].pDSMusicBuffer->Pause(DSBPAUSE_RESUME);
	}
}

void MFSound_MusicSetVolume(int track, float volume)
{
	MFCALLSTACK;

//	gMusicTracks[track].pDSMusicBuffer->SetVolume();
}


#endif
