#include "Fuji_Internal.h"

#if MF_SOUND == MF_DRIVER_DSOUND || defined(MF_SOUNDPLUGIN_DSOUND)

#include "MFDisplay_Internal.h"
#include "MFWindow.h"
#include "MFSystem.h"
#include "MFSound_Internal.h"

#if defined(MF_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <Mmreg.h>
	#include <dsound.h>

	#pragma comment(lib, "Dsound")
	#pragma comment(lib, "dxguid")
#elif defined(MF_XBOX)
	#include <xtl.h>
#endif

/**** Structures ****/

struct MFSoundDataInternal
{
	IDirectSoundBuffer *pBuffer;
	IDirectSoundBuffer *p3DBuffer;
};

struct MFVoiceDataInternal
{
	IDirectSoundBuffer *pBuffer;
	IDirectSoundBuffer8 *pBuffer8;
#if !defined(MF_XBOX)
	IDirectSound3DBuffer8 *p3DBuffer8;
#endif
};


/**** Globals ****/

IDirectSound8 *pDirectSound;
static IDirectSoundBuffer *pDSPrimaryBuffer;


/**** Functions ****/

int32 MFSoundInternal_GetDecibels(float level)
{
	return DSBVOLUME_MIN + (uint32)((DSBVOLUME_MAX - DSBVOLUME_MIN) * MFPow(level, 0.15f));
//	return (int32)(level < 0.0001f ? -10000 : 20*log10(level));
}

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;

	void MFSound_InitWASAPI();
	MFSound_InitWASAPI();

#if defined(MF_XBOX)
	DirectSoundCreate(NULL, &pDirectSound, NULL);
#else
	HRESULT hr = DirectSoundCreate8(NULL, &pDirectSound, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create DirectSound instance.");
	if(FAILED(hr))
		return;

	// create the primary sound buffer
	// fill out DSBuffer creation data
	DSBUFFERDESC desc;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
	desc.dwBufferBytes = 0;
	desc.lpwfxFormat = NULL;
	desc.dwReserved = 0; 
	desc.guid3DAlgorithm = DS3DALG_DEFAULT; 

	// create the DSBuffer
	hr = pDirectSound->CreateSoundBuffer(&desc, &pDSPrimaryBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create the Primary Sound Buffer");

	HWND hWnd = (HWND)MFWindow_GetSystemWindowHandle(MFDisplay_GetCurrent()->settings.pWindow);
	hr = pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to set the DirectSound cooperative level");
#endif

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	if(pDSPrimaryBuffer)
		pDSPrimaryBuffer->Release();
	if(pDirectSound)
		pDirectSound->Release();

	void MFSound_DeinitWASAPI();
	MFSound_DeinitWASAPI();
}

void MFSound_UpdateInternal()
{
	void MFSound_UpdateWASAPI();
	MFSound_UpdateWASAPI();
}

bool MFSound_UpdateVoiceInternal(MFVoice *pVoice)
{
	if(pVoice->flags & MFPF_Paused)
		return false;

	DWORD stat;
	pVoice->pInternal->pBuffer8->GetStatus(&stat);

	if(stat & DSBSTATUS_PLAYING)
		return false;

	// destroy voice
#if !defined(MF_XBOX)
	if(pVoice->pInternal->p3DBuffer8)
		pVoice->pInternal->p3DBuffer8->Release();
#endif
	pVoice->pInternal->pBuffer8->Release();
	pVoice->pInternal->pBuffer->Release();

	return true;
}

void MFSound_CreateInternal(MFSound *pSound)
{
	MFSoundTemplate *pTemplate = pSound->pTemplate;

	int bytesPerSample = (pTemplate->numChannels * pTemplate->bitsPerSample)>>3;

	DSBUFFERDESC desc;
	WAVEFORMATEX wfx;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwBufferBytes = pTemplate->numSamples * bytesPerSample;
	desc.lpwfxFormat = &wfx;
#if !defined(MF_XBOX)
	desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	desc.dwReserved = 0;
	desc.guid3DAlgorithm = DS3DALG_DEFAULT;

	// if we're not creating a dynamic buffer we should probably make it static..
	if(!(pTemplate->flags & MFSF_Dynamic))
		desc.dwFlags |= DSBCAPS_STATIC;

	// we can only pan 2 stereo buffers
	if(pTemplate->numChannels == 2)
		desc.dwFlags |= DSBCAPS_CTRLPAN;

	// if we want global focus..
	if(gDefaults.sound.useGlobalFocus)
		desc.dwFlags |= DSBCAPS_GLOBALFOCUS;
#else
	desc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCDEFER | DSBCAPS_MUTE3DATMAXDISTANCE;
    desc.lpMixBins = NULL;
    desc.dwInputMixBin = 0;
#endif

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = pTemplate->numChannels;
	wfx.nSamplesPerSec = pTemplate->sampleRate;
	wfx.nAvgBytesPerSec = pTemplate->sampleRate * bytesPerSample;
	wfx.nBlockAlign = (WORD)bytesPerSample;
	wfx.wBitsPerSample = (WORD)pTemplate->bitsPerSample;
	wfx.cbSize = 0;

	// create the 2d buffer
	pDirectSound->CreateSoundBuffer(&desc, &pSound->pInternal->pBuffer, NULL);

#if !defined(MF_XBOX)
	if(wfx.nChannels == 1)
	{
		// and create the 3d buffer (but only for 1 channel sounds)
		desc.dwFlags |= DSBCAPS_CTRL3D;
		pDirectSound->CreateSoundBuffer(&desc, &pSound->pInternal->p3DBuffer, NULL);
	}
#endif
}

void MFSound_DestroyInternal(MFSound *pSound)
{
	MFCALLSTACK;

	pSound->pInternal->pBuffer->Release();
	if(pSound->pInternal->p3DBuffer)
		pSound->pInternal->p3DBuffer->Release();
}

MF_API size_t MFSound_SetBufferData(MFSound *pSound, const void *pData, size_t size)
{
	// lock the buffers and copy in the data
	void *pBuffer;
	size_t len;
	
	MFSound_Lock(pSound, 0, 0, &pBuffer, &len);
	MFDebug_Assert(len == size, "Incorrect number of bytes in source buffer.");

	MFCopyMemory(pBuffer, pData, len);

	MFSound_Unlock(pSound);

	return len;
}

MF_API int MFSound_Lock(MFSound *pSound, size_t offset, size_t bytes, void **ppData, size_t *pSize, void **ppData2, size_t *pSize2)
{
	MFDebug_Assert(!(pSound->flags & MFPF_Locked), MFStr("Dynamic sound '%s' is already locked.", pSound->name));

	pSound->pInternal->pBuffer->Lock((DWORD)offset, (DWORD)bytes, &pSound->pLock1, (DWORD*)&pSound->lockSize1, &pSound->pLock2, (DWORD*)&pSound->lockSize2, bytes == 0 ? DSBLOCK_ENTIREBUFFER : 0);

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
	MFDebug_Assert(pSound->flags & MFPF_Locked, MFStr("Dynamic sound '%s' is not locked.", pSound->name));

	if(pSound->pInternal->p3DBuffer)
	{
		// copy new data into 3d buffer...
		VOID *pBuffer, *pB2;
		DWORD bufferLen, l2;

		pSound->pInternal->p3DBuffer->Lock((DWORD)pSound->lockOffset, (DWORD)pSound->lockBytes, &pBuffer, &bufferLen, &pB2, &l2, pSound->lockBytes == 0 ? DSBLOCK_ENTIREBUFFER : 0);
		MFCopyMemory(pBuffer, pSound->pLock1, pSound->lockSize1);
		if(pB2)
			MFCopyMemory(pB2, pSound->pLock2, pSound->lockSize2);
		pSound->pInternal->p3DBuffer->Unlock(pBuffer, bufferLen, pB2, l2);
	}

	// and unlock the main buffer
	pSound->pInternal->pBuffer->Unlock(pSound->pLock1, (DWORD)pSound->lockSize1, pSound->pLock2, (DWORD)pSound->lockSize2);

	pSound->pLock1 = NULL;
	pSound->lockSize1 = 0;
	pSound->pLock2 = NULL;
	pSound->lockSize2 = 0;

	pSound->flags = pSound->flags & ~MFPF_Locked;
}

void MFSound_PlayInternal(MFVoice *pVoice)
{
	MFCALLSTACK;

	MFSound *pSound = pVoice->pSound;

#if !defined(MF_XBOX)
	if(pVoice->flags & MFPF_3D && pSound->pInternal->p3DBuffer)
	{
		pDirectSound->DuplicateSoundBuffer(pSound->pInternal->p3DBuffer, &pVoice->pInternal->pBuffer);
		pVoice->pInternal->pBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pVoice->pInternal->p3DBuffer8);
	}
	else
#endif
	{
		pDirectSound->DuplicateSoundBuffer(pSound->pInternal->pBuffer, &pVoice->pInternal->pBuffer);
		pVoice->pInternal->p3DBuffer8 = NULL;
	}

	pVoice->pInternal->pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (VOID**)&pVoice->pInternal->pBuffer8);

	if(!(pVoice->flags & MFPF_BeginPaused))
	{
		DWORD playbackFlags = (pVoice->flags & MFPF_Looping) ? DSBPLAY_LOOPING : 0;
		pVoice->pInternal->pBuffer8->Play(0, 0, playbackFlags);
	}
	else
		pVoice->flags |= MFPF_Paused;
}

MF_API void MFSound_Pause(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;

	if(pause && !(pVoice->flags & MFPF_Paused))
	{
		pVoice->pInternal->pBuffer8->Stop();
		pVoice->flags |= MFPF_Paused;
	}
	else if(!pause && (pVoice->flags & MFPF_Paused))
	{
		DWORD playbackFlags = (pVoice->flags & MFPF_Looping) ? DSBPLAY_LOOPING : 0;
		pVoice->pInternal->pBuffer8->Play(0, 0, playbackFlags);
		pVoice->flags &= ~MFPF_Paused;
	}
}

MF_API void MFSound_Stop(MFVoice *pVoice)
{
	MFCALLSTACK;

	pVoice->pInternal->pBuffer8->Stop();
}

MF_API void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
	MFCALLSTACK;

}

MF_API void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
	MFCALLSTACK;

	pVoice->pInternal->pBuffer8->SetVolume(MFSoundInternal_GetDecibels(volume));
}

MF_API void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;

	pVoice->pInternal->pBuffer8->SetFrequency((DWORD)((float)pVoice->pSound->pTemplate->sampleRate * rate));
}

MF_API void MFSound_SetPan(MFVoice *pVoice, float pan)
{
	if(pVoice->pSound->pTemplate->numChannels == 2)
	{
		if(pan >= 0)
		{
			float level = 1-pan;
			pVoice->pInternal->pBuffer8->SetPan(-MFSoundInternal_GetDecibels(level));
		}
		else
		{
			float level = 1+pan;
			pVoice->pInternal->pBuffer8->SetPan(MFSoundInternal_GetDecibels(level));
		}
	}
}

MF_API void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
	MFCALLSTACK;

	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	DWORD offset = (DWORD)((float)((pT->sampleRate * pT->numChannels * pT->bitsPerSample) >> 3) * seconds);
	pVoice->pInternal->pBuffer8->SetCurrentPosition(offset);
}

MF_API void MFSound_SetMasterVolume(float volume)
{
	MFCALLSTACK;

	pDSPrimaryBuffer->SetVolume(MFSoundInternal_GetDecibels(volume));
}

size_t MFSound_GetPlayCursorInternal(MFVoice *pVoice, size_t *pWriteCursor)
{
	MFCALLSTACK;

	DWORD play, write;
	pVoice->pInternal->pBuffer8->GetCurrentPosition(&play, &write);

	if(pWriteCursor)
		*pWriteCursor = write;

	return play;
}

#endif // MF_SOUND
