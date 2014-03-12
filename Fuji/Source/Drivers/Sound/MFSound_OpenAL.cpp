#include "Fuji_Internal.h"

#if MF_SOUND == MF_DRIVER_OPENAL || defined(MF_SOUNDPLUGIN_OPENAL)

#include "MFSystem.h"
#include "MFSound_Internal.h"
#include "MFHeap.h"

#ifdef __APPLE__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
	#include <AL/alext.h>
#endif


/**** Structures ****/

struct MFSoundDataInternal
{
	ALuint buffer;
	ALenum format;
};

struct MFVoiceDataInternal
{
	ALuint source;
};

struct OutputDevice
{
	char name[64];
	ALCdevice *pDevice;
};

struct CaptureDevice
{
	char name[64];
	ALCdevice *pDevice;
};

struct Context
{
	struct OpenALExtensions
	{
		bool static_buffer;
		bool offset;
		bool float32;
		bool source_radius;
		bool buffer_sub_data;
		bool buffer_samples;
	};

	ALCcontext *pContext;
	OutputDevice *pDevice;
	OpenALExtensions ext;
};

static OutputDevice gOutputDevices[16];
static CaptureDevice gCaptureDevices[16];
static int gNumOutputDevices = 0;
static int gNumCaptureDevices = 0;
static int gDefaultOutputDevice = -1;
static int gDefaultCaptureDevice = -1;


/**** Globals ****/

static uint32 gAPIVersion;

static Context *gpCurrentContext = NULL;

static PFNALBUFFERDATASTATICPROC alBufferDataStatic = NULL;
static PFNALBUFFERSUBDATASOFTPROC alBufferSubDataSOFT = NULL;


/**** Functions ****/

static OutputDevice *CreateDevice(int deviceId)
{
	OutputDevice &device = gOutputDevices[deviceId];

	if(!device.pDevice)
		device.pDevice = alcOpenDevice(gOutputDevices[deviceId].name);

	return &device;
}

static Context *MakeCurrent(Context *pContext)
{
	Context *pOld = gpCurrentContext;
	alcMakeContextCurrent(pContext ? pContext->pContext : NULL);
	gpCurrentContext = pContext;
	return pOld;
}

static Context *CreateContext(int deviceId)
{
	OutputDevice *pDevice = CreateDevice(deviceId);
	if(!pDevice)
		return NULL;

	Context *pContext = (Context*)MFHeap_Alloc(sizeof(Context));
	pContext->pContext = alcCreateContext(pDevice->pDevice, NULL);
	pContext->pDevice = pDevice;

	Context *pOld = MakeCurrent(pContext);

	const char *pVersion = alGetString(AL_VERSION);
	const char *pExtensions = alGetString(AL_EXTENSIONS);
	MFDebug_Log(0, MFStr("OpenAL Version: %s", pVersion));
	MFDebug_Log(0, MFStr("OpenAL Extensions: %s", pExtensions));

	pContext->ext.static_buffer = alIsExtensionPresent("ALC_EXT_STATIC_BUFFER") == AL_TRUE;
	pContext->ext.offset = alIsExtensionPresent("AL_EXT_OFFSET") == AL_TRUE;	
	pContext->ext.float32 = alIsExtensionPresent("AL_EXT_float32") == AL_TRUE;
	pContext->ext.source_radius = alIsExtensionPresent("AL_EXT_SOURCE_RADIUS") == AL_TRUE;
	pContext->ext.buffer_sub_data = alIsExtensionPresent("AL_SOFT_buffer_sub_data") == AL_TRUE;
	pContext->ext.buffer_samples = alIsExtensionPresent("AL_SOFT_buffer_samples") == AL_TRUE;

	if(pContext->ext.static_buffer)
		alBufferDataStatic = (PFNALBUFFERDATASTATICPROC)alGetProcAddress("alBufferDataStatic");
	if(pContext->ext.buffer_sub_data)
		alBufferSubDataSOFT = (PFNALBUFFERSUBDATASOFTPROC)alGetProcAddress("alBufferSubDataSOFT");

	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListener3f(AL_ORIENTATION, 0, 0, -1);

	MakeCurrent(pOld);

	return pContext;
}

static void DestroyContext(Context *pContext)
{
	alcDestroyContext(pContext->pContext);
	MFHeap_Free(pContext);
}

void MFSound_InitModulePlatformSpecific(int *pSoundDataSize, int *pVoiceDataSize)
{
	MFCALLSTACK;
	
	ALCint minor, major;
	alcGetIntegerv(NULL, ALC_MAJOR_VERSION, 1, &major);
	alcGetIntegerv(NULL, ALC_MINOR_VERSION, 1, &minor);
	gAPIVersion = major*100 + minor;

	bool bCanEnumerate, bHasCapture;
	if(gAPIVersion >= 101)
	{
		bCanEnumerate = true;
		bHasCapture = true;		
	}
	else
	{
		bCanEnumerate = alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == AL_TRUE;
		bHasCapture = alcIsExtensionPresent(NULL, "ALC_EXT_CAPTURE") == AL_TRUE;
	}

	if(bCanEnumerate)
	{
		const char *pDevices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
		const char *pDefault = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		while(pDevices && *pDevices)
		{
			if(!MFString_Compare(pDevices, pDefault))
				gDefaultOutputDevice = gNumOutputDevices;

			MFDebug_Log(2, MFStr("OpenAL: found output device '%s'%s", pDevices, gDefaultOutputDevice == gNumOutputDevices ? " (default)" : ""));

			OutputDevice &device = gOutputDevices[gNumOutputDevices++];
			MFString_CopyN(device.name, pDevices, sizeof(device.name)-1);
			device.name[sizeof(device.name)-1] = 0;
			device.pDevice = NULL;

			pDevices += MFString_Length(pDevices) + 1;
		}

//		MFDebug_Assert(gDefaultOutputDevice >= 0, "OpenAL: No default output device?");
		if(gDefaultOutputDevice >= 0)
			MFDebug_Warn(2, "OpenAL: No default output device?");
		
		if(bHasCapture)
		{
			pDevices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
			pDefault = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
			while(pDevices && *pDevices)
			{
				if(!MFString_Compare(pDevices, pDefault))
					gDefaultCaptureDevice = gNumCaptureDevices;

				MFDebug_Log(2, MFStr("OpenAL: found capture device '%s'%s", pDevices, gDefaultCaptureDevice == gNumCaptureDevices ? " (default)" : ""));

				CaptureDevice &device = gCaptureDevices[gNumCaptureDevices++];
				MFString_CopyN(device.name, pDevices, sizeof(device.name)-1);
				device.name[sizeof(device.name)-1] = 0;
				device.pDevice = NULL;

				pDevices += MFString_Length(pDevices) + 1;
			}

			MFDebug_Assert(gDefaultCaptureDevice >= 0, "OpenAL: No default capture device?");
		}
	}

	// create a context
	Context *pContext = CreateContext(gDefaultOutputDevice);
	MakeCurrent(pContext);

	// we need to return the size of the internal structures so the platform independant
	// code can make the correct allocations..
	*pSoundDataSize = sizeof(MFSoundDataInternal);
	*pVoiceDataSize = sizeof(MFVoiceDataInternal);
}

void MFSound_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	// TODO: remove this, user will control contexts
	DestroyContext(gpCurrentContext);

	for(int i=0; i<gNumCaptureDevices; ++i)
	{
		if(gCaptureDevices[i].pDevice)
			alcCloseDevice(gCaptureDevices[i].pDevice);
	}

	for(int i=0; i<gNumOutputDevices; ++i)
	{
		if(gOutputDevices[i].pDevice)
			alcCloseDevice(gOutputDevices[i].pDevice);
	}
}

void MFSound_UpdateInternal()
{
}

bool MFSound_UpdateVoiceInternal(MFVoice *pVoice)
{
	if(pVoice->flags & MFPF_Paused)
		return false;

	ALint playing;
	alGetSourcei(pVoice->pInternal->source, AL_SOURCE_STATE, &playing);
	if(playing == AL_PLAYING)
		return false;

	// destroy voice
	alDeleteSources(1, &pVoice->pInternal->source);
	return true;
}

void MFSound_CreateInternal(MFSound *pSound)
{
	MFSoundTemplate *pTemplate = pSound->pTemplate;
	MFSoundDataInternal *pInternal = pSound->pInternal;

	alGenBuffers(1, &pInternal->buffer);

	// if dynamic, allocate buffer
	if(pTemplate->flags & MFSF_Dynamic)
	{
		long bufferSize = ((pTemplate->numChannels * pTemplate->bitsPerSample) >> 3) * pTemplate->numSamples;

		if(gpCurrentContext->ext.static_buffer)
		{
			pTemplate->ppStreams = (char**)MFHeap_Alloc(sizeof(char*) + bufferSize);
			pTemplate->ppStreams[0] = (char*)&pTemplate->ppStreams[1];

			alBufferDataStatic(pInternal->buffer, pInternal->format, pTemplate->ppStreams[0], bufferSize, pTemplate->sampleRate);
		}
		else if(gpCurrentContext->ext.buffer_sub_data)
		{
			switch((pTemplate->numChannels << 16) | pTemplate->bitsPerSample)
			{
				case 0x10000 | 8:	pInternal->format = AL_FORMAT_MONO8;	break;
				case 0x20000 | 8:	pInternal->format = AL_FORMAT_STEREO8;	break;
				case 0x10000 | 16:	pInternal->format = AL_FORMAT_MONO16;	break;
				case 0x20000 | 16:	pInternal->format = AL_FORMAT_STEREO16;	break;
				case 0x10000 | 32:
					if(gpCurrentContext->ext.float32)
						pInternal->format = AL_FORMAT_MONO_FLOAT32;
					else
						MFDebug_Assert(false, "Unsupported sample format!");
					break;
				case 0x20000 | 32:
					if(gpCurrentContext->ext.float32)
						pInternal->format = AL_FORMAT_STEREO_FLOAT32;
					else
						MFDebug_Assert(false, "Unsupported sample format!");
					break;
				default:
					MFDebug_Assert(false, "Unsupported sample format!");
					break;
			}

			// set buffer data with null allocates a buffer filled with undefined junk
			alBufferData(pInternal->buffer, pInternal->format, NULL, bufferSize, pTemplate->sampleRate);
		}
	}
}

void MFSound_DestroyInternal(MFSound *pSound)
{
	alDeleteBuffers(1, &pSound->pInternal->buffer);
	
	// if dynamic, free buffer
	if(pSound->pTemplate->flags & MFSF_Dynamic)
	{
		if(gpCurrentContext->ext.static_buffer)
			MFHeap_Free(pSound->pTemplate->ppStreams);
	}
}

MF_API size_t MFSound_SetBufferData(MFSound *pSound, const void *pData, size_t size)
{
	MFSoundTemplate *pTemplate = pSound->pTemplate;
	MFSoundDataInternal *pInternal = pSound->pInternal;

	bool bTranscode = false;
	switch((pTemplate->numChannels << 16) | pTemplate->bitsPerSample)
	{
		case 0x10000 | 8:	pInternal->format = AL_FORMAT_MONO8;	break;
		case 0x20000 | 8:	pInternal->format = AL_FORMAT_STEREO8;	break;
		case 0x10000 | 16:	pInternal->format = AL_FORMAT_MONO16;	break;
		case 0x20000 | 16:	pInternal->format = AL_FORMAT_STEREO16;	break;
		case 0x10000 | 24:
			pInternal->format = AL_FORMAT_MONO16;
			bTranscode = true;
			break;
		case 0x20000 | 24:
			pInternal->format = AL_FORMAT_STEREO16;
			bTranscode = true;
			break;
		case 0x10000 | 32:
			if(gpCurrentContext->ext.float32)
				pInternal->format = AL_FORMAT_MONO_FLOAT32;
			else
			{
				pInternal->format = AL_FORMAT_MONO16;
				bTranscode = true;
			}
			break;
		case 0x20000 | 32:
			if(gpCurrentContext->ext.float32)
				pInternal->format = AL_FORMAT_STEREO_FLOAT32;
			else
			{
				pInternal->format = AL_FORMAT_STEREO16;
				bTranscode = true;
			}
			break;
		default:
			MFDebug_Assert(false, "Unsupported sample format!");
			return 0;
	}

	if(!bTranscode)
	{
		if(gpCurrentContext->ext.static_buffer)
			alBufferDataStatic(pInternal->buffer, pInternal->format, (ALvoid*)pData, size, pTemplate->sampleRate);
		else
			alBufferData(pInternal->buffer, pInternal->format, pData, size, pTemplate->sampleRate);
	}
	else
	{
		int numSamples = size*8 / pTemplate->bitsPerSample;

		size = sizeof(int16)*numSamples;
		int16 *pTranscodeBuffer = (int16*)MFHeap_TAlloc(size);

		switch(pTemplate->bitsPerSample)
		{
			case 24:
			{
				const uint8 *pSource = (const uint8*)pData;
				for(int i=0; i<numSamples; ++i)
				{
#if defined(MFBIG_ENDIAN)
					pTranscodeBuffer[i] = (int16)(((uint16)pSource[0] << 8) | pSource[1]);
#else
					pTranscodeBuffer[i] = (int16)(((uint16)pSource[2] << 8) | pSource[1]);
#endif
					pSource += 3;
				}
				break;
			}
			case 32:
			{
				const float *pSource = (const float*)pData;
				for(int i=0; i<numSamples; ++i)
					pTranscodeBuffer[i] = MFClamp((int16)-32768, (int16)(pSource[i] * 32768.f), (int16)32767);
				break;
			}
		}

		alBufferData(pInternal->buffer, pInternal->format, pTranscodeBuffer, size, pTemplate->sampleRate);

		MFHeap_Free(pTranscodeBuffer);
	}

	return size;
}

MF_API int MFSound_Lock(MFSound *pSound, size_t offset, size_t bytes, void **ppData, size_t *pSize, void **ppData2, size_t *pSize2)
{
	MFDebug_Assert(!(pSound->flags & MFPF_Locked), MFStr("Dynamic sound '%s' is already locked.", pSound->name));

	MFSoundTemplate *pTemplate = pSound->pTemplate;

	size_t bufferSize = ((pTemplate->numChannels * pTemplate->bitsPerSample) >> 3) * pTemplate->numSamples;
	MFDebug_Assert(offset < bufferSize, "MFSound_Lock: Invalid buffer offset.");

	if(bytes > bufferSize)
		bytes = bufferSize;

	if(gpCurrentContext->ext.buffer_sub_data)
	{
		// return some temp memory for the user to write to (TODO: do we want to allocate this better?)
		pSound->pLock1 = MFHeap_TAlloc(bytes);
		pSound->lockSize1 = bytes;
		pSound->pLock2 = NULL;
		pSound->lockSize2 = 0;
	}
	else if(gpCurrentContext->ext.static_buffer)
	{
		// we own the buffer under apple's implementation, just return the direct pointer
		pSound->pLock1 = pTemplate->ppStreams[0] + offset;

		if(offset + bytes > bufferSize)
		{
			pSound->lockSize1 = bufferSize - offset;
			pSound->pLock2 = pTemplate->ppStreams[0];
			pSound->lockSize2 = bytes - pSound->lockSize1;
		}
		else
		{
			pSound->lockSize1 = bytes;
			pSound->pLock2 = NULL;
			pSound->lockSize2 = 0;
		}
	}
	else
	{
		MFDebug_Warn(1, "No direct buffer management extension available!");

		pSound->pLock1 = NULL;
		pSound->lockSize1 = 0;
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
	MFDebug_Assert(pSound->flags & MFPF_Locked, MFStr("Dynamic sound '%s' is not locked.", pSound->name));

	MFSoundTemplate *pTemplate = pSound->pTemplate;
	MFSoundDataInternal *pInternal = pSound->pInternal;

	if(gpCurrentContext->ext.buffer_sub_data)
	{
		// write data to the buffer
		size_t bufferSize = ((pTemplate->numChannels * pTemplate->bitsPerSample) >> 3) * pTemplate->numSamples;
		size_t untilEnd = bufferSize - pSound->lockOffset;
		
		if(pSound->lockBytes > untilEnd)
		{
			alBufferSubDataSOFT(pInternal->buffer, pInternal->format, pSound->pLock1, pSound->lockOffset, untilEnd);
			alBufferSubDataSOFT(pInternal->buffer, pInternal->format, (char*)pSound->pLock1 + untilEnd, 0, pSound->lockBytes - untilEnd);
		}
		else
		{
			alBufferSubDataSOFT(pInternal->buffer, pInternal->format, pSound->pLock1, pSound->lockOffset, pSound->lockBytes);
		}

		MFHeap_Free(pSound->pLock1);
	}

	pSound->pLock1 = NULL;
	pSound->lockSize1 = 0;
	pSound->pLock2 = NULL;
	pSound->lockSize2 = 0;

	pSound->flags = pSound->flags & ~MFPF_Locked;
}

void MFSound_PlayInternal(MFVoice *pVoice)
{
	MFCALLSTACK;

	ALuint source;
	alGenSources(1, &source);
	pVoice->pInternal->source = source;

	MFSound *pSound = pVoice->pSound;
	alSourcei(source, AL_BUFFER, pSound->pInternal->buffer);

	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, (pVoice->flags & MFPF_Looping) ? AL_TRUE : AL_FALSE);

	if(!(pVoice->flags & MFPF_BeginPaused))
		alSourcePlay(pVoice->pInternal->source);
	else
		pVoice->flags |= MFPF_Paused;
}

MF_API void MFSound_Pause(MFVoice *pVoice, bool pause)
{
	MFCALLSTACK;

	if(pause && !(pVoice->flags & MFPF_Paused))
	{
		alSourcePause(pVoice->pInternal->source);
		pVoice->flags |= MFPF_Paused;
	}
	else if(!pause && (pVoice->flags & MFPF_Paused))
	{
		alSourcePlay(pVoice->pInternal->source);
		pVoice->flags &= ~MFPF_Paused;
	}
}

MF_API void MFSound_Stop(MFVoice *pVoice)
{
	MFCALLSTACK;

	alSourceStop(pVoice->pInternal->source);
}

MF_API void MFSound_SetListenerPos(const MFMatrix& listenerPos)
{
	MFCALLSTACK;

	// TODO: do this!
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListener3f(AL_ORIENTATION, 0, 0, -1);
}

MF_API void MFSound_SetVolume(MFVoice *pVoice, float volume)
{
	MFCALLSTACK;

	alSourcef(pVoice->pInternal->source, AL_GAIN, volume);
}

MF_API void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate)
{
	MFCALLSTACK;

	alSourcef(pVoice->pInternal->source, AL_PITCH, rate);
}

MF_API void MFSound_SetPan(MFVoice *pVoice, float pan)
{
	if(pVoice->pSound->pTemplate->numChannels == 2)
	{
		// TODO: do this...
	}
}

MF_API void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds)
{
	MFCALLSTACK;

	alSourcef(pVoice->pInternal->source, AL_SEC_OFFSET, seconds);
}

MF_API void MFSound_SetMasterVolume(float volume)
{
	MFCALLSTACK;

	alListenerf(AL_GAIN, volume);
}

size_t MFSound_GetPlayCursorInternal(MFVoice *pVoice, size_t *pWriteCursor)
{
	MFCALLSTACK;

	ALint offset[2];
	if(gpCurrentContext->ext.buffer_sub_data)
	{
		alGetSourceiv(pVoice->pInternal->source, AL_BYTE_RW_OFFSETS_SOFT, offset);
	}
	else
	{
		alGetSourcei(pVoice->pInternal->source, AL_BYTE_OFFSET, &offset[0]);
		offset[1] = offset[0];
	}

	if(pWriteCursor)
		*pWriteCursor = (uint32)offset[1];

	return (uint32)offset[0];
}

#endif // MF_SOUND
