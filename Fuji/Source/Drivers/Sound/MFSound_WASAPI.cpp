#include "Fuji_Internal.h"

#if MF_SOUND == MF_DRIVER_DSOUND || defined(MF_SOUNDPLUGIN_DSOUND)

#include "MFSound_Internal.h"
#include "MFDevice_Internal.h"
#include "MFObjectPool.h"
#include "MFThread.h"

#if defined(MF_WINDOWS)
	#include <Mmdeviceapi.h>
	#include <Audioclient.h>
	#include <Audiopolicy.h>
	#include <Functiondiscoverykeys_devpkey.h>
#endif


static MFDevice *NewDevice(LPCWSTR pwstrDeviceId);

struct MFAudioDevice
{
	DWORD state;

	MFWaveFormat format;
	uint32 bitsPerSample;
	uint32 channels;
	uint32 channelMask;

	IMMDevice *pDevice;
	IAudioClient *pAudioClient;
};

struct MFAudioCaptureDevice : MFAudioDevice
{
    IAudioCaptureClient *pCaptureClient;

	MFAudioCaptureCallback *pSampleCallback;
	void *pUserData;

	MFThread thread;

	volatile bool bActive;
	volatile bool bTerminate;
};


MFObjectPool gDevices;
MFObjectPool gCaptureDevices;

static IMMDeviceEnumerator *gpEnumerator;
static class MFAudioDeviceNotification *gpNotification;

static EDataFlow direction[2] = { eRender, eCapture };
static MFDeviceType dt[2] = { MFDT_AudioRender, MFDT_AudioCapture };
static ERole role[3] = { eConsole, eMultimedia, eCommunications };
static MFDefaultDeviceType def[3] = { MFDDT_Default, MFDDT_Multimedia, MFDDT_Communication };

void UpdateState(MFDevice *pDevice, DWORD state)
{
	MFAudioDevice &device = *(MFAudioDevice*)pDevice->pInternal;
	device.state = state;
	if(state == DEVICE_STATE_UNPLUGGED)
		pDevice->state = MFDevState_Disconnected;
	else if(state == DEVICE_STATE_NOTPRESENT)
		pDevice->state = MFDevState_Disconnected;
	else if(state == DEVICE_STATE_DISABLED)
		pDevice->state = MFDevState_Unavailable;
	else if(state == DEVICE_STATE_ACTIVE)
		pDevice->state = MFDevState_Ready;
	else
		pDevice->state = MFDevState_Unknown;
}


class MFAudioDeviceNotification : public IMMNotificationClient
{
	LONG rc;

public:
	MFAudioDeviceNotification() : rc(1) {}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		if(riid == IID_IUnknown)
		{
			AddRef();
			*ppvObject = (IUnknown*)this;
		}
		else if(riid == __uuidof(IMMNotificationClient))
		{
			AddRef();
			*ppvObject = (IMMNotificationClient*)this;
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&rc);
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&rc);
		if(ulRef == 0)
			delete this;
		return ulRef;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
	{
		MFDebug_Log(2, MFStr("WASAPI: State changed: %d (%S)", dwNewState, pwstrDeviceId));
		char temp[128];
		MFString_CopyUTF16ToUTF8(temp, pwstrDeviceId);
		MFDevice *pDev = MFDevice_GetDeviceById(temp);
		if(!pDev)
			pDev = NewDevice(pwstrDeviceId);
		if(pDev)
			UpdateState(pDev, dwNewState);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId)
	{
		MFDebug_Log(2, MFStr("WASAPI: Device added: %S", pwstrDeviceId));
		char temp[128];
		MFString_CopyUTF16ToUTF8(temp, pwstrDeviceId);
		MFDevice *pDev = MFDevice_GetDeviceById(temp);
		if(!pDev)
			pDev = NewDevice(pwstrDeviceId);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId)
	{
		MFDebug_Log(2, MFStr("WASAPI: Device removed: %S", pwstrDeviceId));
		char temp[128];
		MFString_CopyUTF16ToUTF8(temp, pwstrDeviceId);
		MFDevice *pDev = MFDevice_GetDeviceById(temp);
		if(!pDev)
			pDev = NewDevice(pwstrDeviceId);
		if(pDev)
			UpdateState(pDev, DEVICE_STATE_UNPLUGGED);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
	{
		MFDebug_Log(2, MFStr("WASAPI: Default device changed: %S", pwstrDefaultDeviceId));
		char temp[128];
		MFString_CopyUTF16ToUTF8(temp, pwstrDefaultDeviceId);
		MFDevice *pDev = MFDevice_GetDeviceById(temp);
		if(!pDev)
			pDev = NewDevice(pwstrDefaultDeviceId);
		if(pDev)
			MFDevice_SetDefaultDevice(dt[flow], def[role], pDev);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
	{
		MFDebug_Log(3, MFStr("WASAPI: Property changed: %S", pwstrDeviceId));
		return S_OK;
	}
};

static void GetDeviceInfo(IMMDevice *pDevice, MFDevice *pDev)
{
	wchar_t *pWC;
	pDevice->GetId(&pWC);
	MFString_CopyUTF16ToUTF8(pDev->strings[MFDS_ID], pWC);
	CoTaskMemFree(pWC);

	DWORD state;
	pDevice->GetState(&state);
	UpdateState(pDev, state);

	IPropertyStore *pProps;
	pDevice->OpenPropertyStore(STGM_READ, &pProps);

	PROPVARIANT v;
	PropVariantInit(&v);
	pProps->GetValue(PKEY_DeviceInterface_FriendlyName, &v);
	MFString_CopyUTF16ToUTF8(pDev->strings[MFDS_InterfaceName], v.pwszVal);
	PropVariantClear(&v);

	PropVariantInit(&v);
	pProps->GetValue(PKEY_Device_DeviceDesc, &v);
	MFString_CopyUTF16ToUTF8(pDev->strings[MFDS_Description], v.pwszVal);
	PropVariantClear(&v);

	PropVariantInit(&v);
	pProps->GetValue(PKEY_Device_FriendlyName, &v);
	MFString_CopyUTF16ToUTF8(pDev->strings[MFDS_DeviceName], v.pwszVal);
	PropVariantClear(&v);

	PropVariantInit(&v);
	pProps->GetValue(PKEY_Device_Manufacturer, &v);
	MFString_CopyUTF16ToUTF8(pDev->strings[MFDS_Manufacturer], v.pwszVal ? v.pwszVal : L"");
	PropVariantClear(&v);

	pProps->Release();
}

static MFDevice *NewDevice(LPCWSTR pwstrDeviceId)
{
	MFDevice *pDev = NULL;
	IMMDevice *pDevice;
	gpEnumerator->GetDevice(pwstrDeviceId, &pDevice);
	if(pDevice)
	{
		// TODO: don't know if it's a capture device or not!
		if(1)
		{
			pDev = MFDevice_AllocDevice(MFDT_AudioRender, NULL);
			pDev->pInternal = gDevices.AllocAndZero();
			GetDeviceInfo(pDevice, pDev);
		}
		else
		{
			pDev = MFDevice_AllocDevice(MFDT_AudioCapture, NULL);
			pDev->pInternal = gCaptureDevices.AllocAndZero();
			GetDeviceInfo(pDevice, pDev);
		}
		pDevice->Release();
	}
	return pDev;
}


void MFSound_InitWASAPI()
{
	gDevices.Init(sizeof(MFAudioDevice), 8, 8);
	gCaptureDevices.Init(sizeof(MFAudioCaptureDevice), 8, 8);

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&gpEnumerator);
	if(FAILED(hr))
	{
		MFDebug_Assert(false, "Couldn't create multimedia device enumerator!");
		return;
	}

	// enumerate audio devices...
	gpNotification = new MFAudioDeviceNotification;
	gpEnumerator->RegisterEndpointNotificationCallback(gpNotification);

	// enumerate render devices
	IMMDeviceCollection *pDevices;
	gpEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &pDevices);
	if(pDevices)
	{
		UINT count;
		pDevices->GetCount(&count);

		for(UINT i=0; i<count; ++i)
		{
			IMMDevice *pDevice;
			pDevices->Item(i, &pDevice);

			MFDevice *pDev = MFDevice_AllocDevice(MFDT_AudioRender, NULL);
			pDev->pInternal = gDevices.AllocAndZero();
			MFAudioDevice &device = *(MFAudioDevice*)pDev->pInternal;

			GetDeviceInfo(pDevice, pDev);

			pDevice->Release();

			MFDebug_Log(0, MFStr("Found audio device: %s, %s - state: %d (%s)", pDev->strings[MFDS_DeviceName], pDev->strings[MFDS_Manufacturer], device.state, pDev->strings[MFDS_ID]));
		}
		pDevices->Release();
	}

	// enumerate capture devices
	gpEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &pDevices);
	if(pDevices)
	{
		UINT count;
		pDevices->GetCount(&count);

		for(UINT i=0; i<count; ++i)
		{
			IMMDevice *pDevice;
			pDevices->Item(i, &pDevice);

			MFDevice *pDev = MFDevice_AllocDevice(MFDT_AudioCapture, NULL);
			pDev->pInternal = gCaptureDevices.AllocAndZero();
			MFAudioCaptureDevice &device = *(MFAudioCaptureDevice*)pDev->pInternal;

			GetDeviceInfo(pDevice, pDev);

			pDevice->Release();

			MFDebug_Log(0, MFStr("Found audio capture device: %s, %s - state: %d (%s)", pDev->strings[MFDS_DeviceName], pDev->strings[MFDS_Manufacturer], device.state, pDev->strings[MFDS_ID]));
		}
		pDevices->Release();
	}

	// set defaults (this is some awkward windows code!)
	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<3; ++j)
		{
			IMMDevice *pDevice;
			gpEnumerator->GetDefaultAudioEndpoint(direction[i], role[j], &pDevice);
			if(pDevice)
			{
				wchar_t *pDefaultId;
				pDevice->GetId(&pDefaultId);
				char temp[128];
				MFString_CopyUTF16ToUTF8(temp, pDefaultId);
				MFDevice *pDev = MFDevice_GetDeviceById(temp);
				MFDevice_SetDefaultDevice(dt[i], def[j], pDev);
				CoTaskMemFree(pDefaultId);
				pDevice->Release();
			}
		}
	}
}

void MFSound_DeinitWASAPI()
{
	gpEnumerator->Release();
	delete gpNotification;

	gDevices.Deinit();
	gCaptureDevices.Deinit();
}

void MFSound_UpdateWASAPI()
{
}

static int CaptureThread(void *pData)
{
	MFAudioCaptureDevice &device = *(MFAudioCaptureDevice*)pData;

	const int NumSamples = 4096;
	float buffer[NumSamples];

	while(1)
	{
		if(device.bActive)
		{
			while(device.bActive)
			{
				// get samples, and feed to callback
				UINT32 packetLength = 0;
				HRESULT hr = device.pCaptureClient->GetNextPacketSize(&packetLength);

				while(packetLength != 0)
				{
					// get the available data in the shared buffer.
					UINT32 numFramesAvailable;
					BYTE *pData;
					DWORD flags;
					UINT64 position, performanceCounter;
					hr = device.pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, &position, &performanceCounter);

					if(flags & AUDCLNT_BUFFERFLAGS_SILENT)
						pData = NULL;

					UINT32 numRemaining = numFramesAvailable;
					while(numRemaining)
					{
						UINT32 samplesToDeliver;
						float *pSamples;
						switch(device.format)
						{
							case MFWaveFmt_PCM_f32:
								pSamples = (float*)pData;
								samplesToDeliver = numRemaining;
								break;
							default:
								pSamples = buffer;
								samplesToDeliver = numRemaining;
								MFDebug_Assert(false, "TODO: Samples require conversion");
						}

						// feed samples to the callback
						device.pSampleCallback(pSamples, samplesToDeliver, device.channels, device.pUserData);
						numRemaining -= samplesToDeliver;
					}

					hr = device.pCaptureClient->ReleaseBuffer(numFramesAvailable);
					hr = device.pCaptureClient->GetNextPacketSize(&packetLength);
				}

				MFThread_Sleep(5);
			}

			HRESULT hr = device.pAudioClient->Stop();  // Stop recording.
		}

		if(device.bTerminate)
			break;

		MFThread_Sleep(64);
	}
	return 0;
}

MF_API bool MFSound_OpenCaptureDevice(MFDevice *pDevice)
{
	MFDebug_Assert(pDevice->type == MFDT_AudioCapture, "Incorrect device type!");
	MFAudioCaptureDevice &device = *(MFAudioCaptureDevice*)pDevice->pInternal;

	if(device.state == 1)
	{
		// get a handle to the device
		wchar_t id[256];
		MFWString_CopyUTF8ToUTF16(id, pDevice->strings[MFDS_ID]);
		HRESULT hr = gpEnumerator->GetDevice(id, &device.pDevice);

		// activate the capture device
		hr = device.pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&device.pAudioClient);

		// get format from input device
		WAVEFORMATEX *pwfx = NULL;
		hr = device.pAudioClient->GetMixFormat(&pwfx);

		device.format = MFWaveFmt_Unknown;
		device.bitsPerSample = pwfx->wBitsPerSample;
		device.channels = pwfx->nChannels;
		device.channelMask = 4;

		WORD wFormatTag = pwfx->wFormatTag;
		if(wFormatTag == WAVE_FORMAT_EXTENSIBLE)
		{
			WAVEFORMATEXTENSIBLE *pwfex = (WAVEFORMATEXTENSIBLE*)pwfx;
			device.channelMask = pwfex->dwChannelMask;
			if(pwfex->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
				wFormatTag = WAVE_FORMAT_PCM;
			else if(pwfex->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
				wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
		}
		switch(wFormatTag)
		{
			case WAVE_FORMAT_PCM:
			{
				switch(pwfx->wBitsPerSample)
				{
					case 8:
						device.format = MFWaveFmt_PCM_u8;
						break;
					case 16:
						device.format = MFWaveFmt_PCM_s16;
						break;
					case 24:
						device.format = MFWaveFmt_PCM_s24;
						break;
					default:
						MFDebug_Assert(false, "Invalid wave format!");
				}
				break;
			}
			case WAVE_FORMAT_IEEE_FLOAT:
			{
				MFDebug_Assert(pwfx->wBitsPerSample == 32, "Unknown float format!");
				device.format = MFWaveFmt_PCM_f32;
				break;
			}
			default:
				MFDebug_Warn(2, "Unsupported wave format!");
				break;
		}

		// initialise capture device
		REFERENCE_TIME hnsRequestedDuration = 10000000;
		hr = device.pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL);

		// get the size of the allocated buffer.
		UINT32 bufferFrameCount;
		hr = device.pAudioClient->GetBufferSize(&bufferFrameCount);

		hr = device.pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&device.pCaptureClient);

		// calculate the actual duration of the allocated buffer.
		REFERENCE_TIME hnsActualDuration = (REFERENCE_TIME)((double)10000000 * bufferFrameCount / pwfx->nSamplesPerSec);

		device.bActive = false;
		device.thread = MFThread_CreateThread(pDevice->strings[MFDS_ID], CaptureThread, &device, MFPriority_AboveNormal, MFTF_Joinable);

		return true;
	}

	return false;
}

MF_API void MFSound_CloseCaptureDevice(MFDevice *pDevice)
{
	MFDebug_Assert(pDevice->type == MFDT_AudioCapture, "Incorrect device type!");
	MFAudioCaptureDevice &device = *(MFAudioCaptureDevice*)pDevice->pInternal;

	MFSound_StopCapture(pDevice);

	device.bTerminate;
	MFThread_Join(device.thread);

	device.pCaptureClient->Release();
	device.pAudioClient->Release();
	device.pDevice->Release();
}

MF_API void MFSound_StartCapture(MFDevice *pDevice, MFAudioCaptureCallback *pCallback, void *pUserData)
{
	MFDebug_Assert(pDevice->type == MFDT_AudioCapture, "Incorrect device type!");
	MFAudioCaptureDevice &device = *(MFAudioCaptureDevice*)pDevice->pInternal;

	device.pSampleCallback = pCallback;
	device.pUserData = pUserData;

	HRESULT hr = device.pAudioClient->Start();  // Start recording.
	if(FAILED(hr))
		MFDebug_Warn(2, "Couldn't start capture");

	device.bActive = true;
}

MF_API void MFSound_StopCapture(MFDevice *pDevice)
{
	MFDebug_Assert(pDevice->type == MFDT_AudioCapture, "Incorrect device type!");
	MFAudioCaptureDevice &device = *(MFAudioCaptureDevice*)pDevice->pInternal;

	device.bActive = false;
}

#endif
