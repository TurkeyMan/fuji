#include "Common.h"
#include "FileSystem.h"
#include "Sound.h"
#include "PtrList.h"

#include <vorbis/vorbisfile.h>

struct SoundMusic
{
	IDirectSoundBuffer8 *pDSMusicBuffer;
	char *pVorbisBuffer;

	OggVorbis_File vorbisFile;

	uint32 size;
	uint32 offset;

	uint32 bufferSize;
	uint32 playBackOffset;

	bool playing;
};

void Sound_ServiceMusicBuffer(int track);


IDirectSound8 *pDirectSound;

SoundMusic gMusicTracks[4];


void Sound_InitModule()
{
	DirectSoundCreate(NULL, &pDirectSound, NULL);

}

void Sound_DeinitModule()
{
	pDirectSound->Release();
}

void Sound_Update()
{
	for(int a=0; a<sizeof(gMusicTracks)/sizeof(SoundMusic); a++)
	{
		if(gMusicTracks[a].pDSMusicBuffer && gMusicTracks[a].playing)
		{
			Sound_ServiceMusicBuffer(a);
		}
	}
}


int Sound_LoadBank(const char *pFilename)
{
	return -1;
}

void Sound_UnloadBank(int bankID)
{

}

int Sound_FindSound(const char *pSoundName, int searchBankID)
{
	return -1;
}

void Sound_Play(int soundID)
{

}

void Sound_Stop(int soundID)
{

}

void Sound_Play3D(int soundID)
{

}

void Sound_Stop3D(int soundID)
{

}

void Sound_SetListenerPos(const Vector3& listenerPos)
{

}


void Sound_SetVolume(int soundID, float volume)
{

}

void Sound_SetMasterVolume(int soundID, float volume)
{

}

void Sound_SetPlaybackRate(int soundID, float rate)
{

}


//
// VORBIS MEMORY READING FUNCTIONS
//
size_t readMemory_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	SoundMusic *pMusic = (SoundMusic*)datasource;

	uint32 readSize = Min(size*nmemb, pMusic->size-pMusic->offset);
	memcpy(ptr, &pMusic->pVorbisBuffer[pMusic->offset], readSize);
	pMusic->offset += readSize;

	return readSize;
}

int seekMemory_func(void *datasource, ogg_int64_t offset, int whence)
{
	SoundMusic *pMusic = (SoundMusic*)datasource;

	switch(whence)
	{
		case SEEK_SET:
			pMusic->offset = (int)offset;
			break;
		case SEEK_CUR:
			pMusic->offset += (int)offset;
			break;
		case SEEK_END:
			pMusic->offset = pMusic->size - (int)offset;
			break;
	}

	return pMusic->offset;
}

int closeMemory_func(void *datasource)
{
	SoundMusic *pMusic = (SoundMusic*)datasource;

	Heap_Free(pMusic->pVorbisBuffer);
	pMusic->pVorbisBuffer = NULL;

	return 0;
}

long tellMemory_func(void *datasource)
{
	SoundMusic *pMusic = (SoundMusic*)datasource;

	return pMusic->offset;
}

//
// Vorbis Music Functions
//
int Sound_MusicPlay(const char *pFilename, bool pause)
{
	int t = 0;

	// fine free music track
	while(gMusicTracks[t].pDSMusicBuffer && t < 4) t++;
	if(t == 4) return -1;

	SoundMusic& track = gMusicTracks[t];

	// load vorbis file
	track.pVorbisBuffer = File_Load(pFilename, &track.size);
	if(!track.pVorbisBuffer) return -1;

	track.offset = 0;

	// setup vorbis read callbacks
	ov_callbacks callbacks;
	callbacks.read_func = readMemory_func;
	callbacks.seek_func = seekMemory_func;
	callbacks.close_func = closeMemory_func;
	callbacks.tell_func = tellMemory_func;

	// open vorbis file
	if(ov_test_callbacks(&track, &track.vorbisFile, NULL, 0, callbacks))
	{
		Heap_Free(track.pVorbisBuffer);
		DBGASSERT(false, "Not a vorbis file.");
		return -1;
	}

	ov_test_open(&track.vorbisFile);

	// get vorbis file info
	vorbis_info *pInfo;
	pInfo = ov_info(&track.vorbisFile, -1);

	// fill out DSBuffer creation data
	DSBUFFERDESC desc;
	WAVEFORMATEX wfx;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = pInfo->channels;
	wfx.nSamplesPerSec = pInfo->rate;
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

	return t;
}

void Sound_ServiceMusicBuffer(int trackID)
{
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
}

void Sound_MusicUnload(int track)
{
	if(gMusicTracks[track].playing) gMusicTracks[track].pDSMusicBuffer->Stop();

	ov_clear(&gMusicTracks[track].vorbisFile);

	gMusicTracks[track].pDSMusicBuffer->Release();
	gMusicTracks[track].pDSMusicBuffer = NULL;
}

void Sound_MusicSeek(int track, float seconds)
{
	ov_time_seek(&gMusicTracks[track].vorbisFile, seconds);
}

void Sound_MusicPause(int track, bool pause)
{
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

void Sound_MusicSetVolume(int track, float volume)
{
//	gMusicTracks[track].pDSMusicBuffer->SetVolume();
}

