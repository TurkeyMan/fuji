#include "Fuji.h"

#if MF_SOUND == DSOUND

#include "MFSystem.h"
#include "MFSound_Internal.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Mmreg.h>
#include <dsound.h>


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
	IDirectSound3DBuffer8 *p3DBuffer8;
};


/**** Globals ****/

extern HWND apphWnd;

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

	HRESULT hr;

	DirectSoundCreate8(NULL, &pDirectSound, NULL);

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

	hr = pDirectSound->SetCooperativeLevel(apphWnd, DSSCL_PRIORITY);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to set the DirectSound cooperative level");

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	pDSPrimaryBuffer->Release();
	pDirectSound->Release();
}

void MFSound_UpdateInternal()
{
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
	if(pVoice->pInternal->p3DBuffer8)
		pVoice->pInternal->p3DBuffer8->Release();
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
	desc.dwReserved = 0;
	desc.guid3DAlgorithm = DS3DALG_DEFAULT;
	desc.lpwfxFormat = &wfx;
	desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;

	// if we're not creating a dynamic duffer we should probably make it static..
	if(!(pTemplate->flags & MFSF_Dynamic))
		desc.dwFlags |= DSBCAPS_STATIC;

	// we can only pan 2 stereo buffers
	if(pTemplate->numChannels == 2)
		desc.dwFlags |= DSBCAPS_CTRLPAN;

	// if we want global focus..
	if(gDefaults.sound.useGlobalFocus)
		desc.dwFlags |= DSBCAPS_GLOBALFOCUS;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = pTemplate->numChannels;
	wfx.nSamplesPerSec = pTemplate->sampleRate;
	wfx.nAvgBytesPerSec = pTemplate->sampleRate * bytesPerSample;
	wfx.nBlockAlign = (WORD)bytesPerSample;
	wfx.wBitsPerSample = (WORD)pTemplate->bitsPerSample;
	wfx.cbSize = 0;

	// create the 2d buffer
	pDirectSound->CreateSoundBuffer(&desc, &pSound->pInternal->pBuffer, NULL);

	if(wfx.nChannels == 1)
	{
		// and create the 3d buffer (but only for 1 channel sounds)
		desc.dwFlags |= DSBCAPS_CTRL3D;
		pDirectSound->CreateSoundBuffer(&desc, &pSound->pInternal->p3DBuffer, NULL);
	}
}

void MFSound_DestroyInternal(MFSound *pSound)
{
	MFCALLSTACK;

	pSound->pInternal->pBuffer->Release();
	if(pSound->pInternal->p3DBuffer)
		pSound->pInternal->p3DBuffer->Release();
}

int MFSound_Lock(MFSound *pSound, int offset, int bytes, void **ppData, uint32 *pSize, void **ppData2, uint32 *pSize2)
{
	MFDebug_Assert(!(pSound->flags & MFPF_Locked), MFStr("Dynamic sound '%s' is already locked.", pSound->name));

	pSound->pInternal->pBuffer->Lock(offset, bytes, &pSound->pLock1, (DWORD*)&pSound->lockSize1, &pSound->pLock2, (DWORD*)&pSound->lockSize2, bytes == 0 ? DSBLOCK_ENTIREBUFFER : 0);

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

void MFSound_Unlock(MFSound *pSound)
{
	MFDebug_Assert(pSound->flags & MFPF_Locked, MFStr("Dynamic sound '%s' is not locked.", pSound->name));

	if(pSound->pInternal->p3DBuffer)
	{
		// copy new data into 3d buffer...
		VOID *pBuffer, *pB2;
		DWORD bufferLen, l2;

		pSound->pInternal->p3DBuffer->Lock(pSound->lockOffset, pSound->lockBytes, &pBuffer, &bufferLen, &pB2, &l2, pSound->lockBytes == 0 ? DSBLOCK_ENTIREBUFFER : 0);
		MFCopyMemory(pBuffer, pSound->pLock1, pSound->lockSize1);
		if(pB2)
			MFCopyMemory(pB2, pSound->pLock2, pSound->lockSize2);
		pSound->pInternal->p3DBuffer->Unlock(pBuffer, bufferLen, pB2, l2);
	}

	// and unlock the main buffer
	pSound->pInternal->pBuffer->Unlock(pSound->pLock1, pSound->lockSize1, pSound->pLock2, pSound->lockSize2);

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

	if(pVoice->flags & MFPF_3D && pSound->pInternal->p3DBuffer)
	{
		pDirectSound->DuplicateSoundBuffer(pSound->pInternal->p3DBuffer, &pVoice->pInternal->pBuffer);
		pVoice->pInternal->pBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pVoice->pInternal->p3DBuffer8);
	}
	else
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

void MFSound_Pause(MFVoice *pVoice, bool pause)
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

void MFSound_Stop(MFVoice *pVoice)
{
	MFCALLSTACK;

	pVoice->pInternal->pBuffer8->Stop();
}

void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
	MFCALLSTACK;

}

void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
	MFCALLSTACK;

	pVoice->pInternal->pBuffer8->SetVolume(MFSoundInternal_GetDecibels(volume));
}

void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;

	pVoice->pInternal->pBuffer8->SetFrequency((DWORD)((float)pVoice->pSound->pTemplate->sampleRate * rate));
}

void MFSound_SetPan(MFVoice *pVoice, float pan)
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

void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
	MFCALLSTACK;

	MFSoundTemplate *pT = pVoice->pSound->pTemplate;

	DWORD offset = (DWORD)((float)((pT->sampleRate * pT->numChannels * pT->bitsPerSample) >> 3) * seconds);
	pVoice->pInternal->pBuffer8->SetCurrentPosition(offset);
}

void MFSound_SetMasterVolume(float volume)
{
	MFCALLSTACK;

	pDSPrimaryBuffer->SetVolume(MFSoundInternal_GetDecibels(volume));
}

uint32 MFSound_GetPlayCursor(MFVoice *pVoice, uint32 *pWriteCursor)
{
	MFCALLSTACK;

	DWORD play, write;

	pVoice->pInternal->pBuffer8->GetCurrentPosition(&play, &write);

	if(pWriteCursor)
		*pWriteCursor = write;

	return play;
}

#endif // MF_SOUND
