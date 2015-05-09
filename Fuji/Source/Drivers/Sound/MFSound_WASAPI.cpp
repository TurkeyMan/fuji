#include "Fuji_Internal.h"

#if MF_SOUND == MF_DRIVER_DSOUND || defined(MF_SOUNDPLUGIN_DSOUND)

#include "MFSound_Internal.h"

#if defined(MF_WINDOWS)
	#include <Mmdeviceapi.h>
	#include <Audioclient.h>
	#include <Audiopolicy.h>
	#include <Functiondiscoverykeys_devpkey.h>
#endif

// TODO:...
class MFAudioDeviceNotification : public IMMNotificationClient
{
	ULONG rc;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return ++rc;
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		return --rc;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
	{
		MFDebug_Log(0, MFStr("MM State changed: %d (%S)", dwNewState, pwstrDeviceId));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId)
	{
		MFDebug_Log(0, MFStr("MM device added: %S", pwstrDeviceId));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId)
	{
		MFDebug_Log(0, MFStr("MM device removed: %S", pwstrDeviceId));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
	{
		MFDebug_Log(0, MFStr("MM default device changed: %S", pwstrDefaultDeviceId));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
	{
//		MFDebug_Log(0, MFStr("MM Prop changed: %S", pwstrDeviceId));
		return S_OK;
	}
};

struct MFAudioDevice
{
	DWORD state;
	bool bActive;

	MFWaveFormat format;
	uint32 bitsPerSample;
	uint32 channels;
	uint32 channelMask;

	char id[64];
	char deviceInterface_FriendlyName[128];
	char device_Desc[128];
	char device_FriendlyName[128];
	char device_Manufacturer[128];

	IMMDevice *pDevice;
	IAudioClient *pAudioClient;
};

struct MFAudioCaptureDevice
{
	DWORD state;
	bool bActive;

	MFWaveFormat format;
	uint32 bitsPerSample;
	uint32 channels;
	uint32 channelMask;

	char id[64];
	char deviceInterface_FriendlyName[128];
	char device_Desc[128];
	char device_FriendlyName[128];
	char device_Manufacturer[128];

	MFAudioCaptureCallback *pSampleCallback;
	void *pUserData;

	IMMDevice *pDevice;
	IAudioClient *pAudioClient;
    IAudioCaptureClient *pCaptureClient;
};

static MFAudioDevice *gpDevices;
static MFAudioCaptureDevice *gpCaptureDevices;
static size_t gNumDevices, gNumCaptureDevices;

static IMMDeviceEnumerator *gpEnumerator;
static MFAudioDeviceNotification *gpNotification;

void MFSound_InitWASAPI()
{
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
	//...

	// enumerate capture devices
	IMMDeviceCollection *pDevices;
	gpEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &pDevices);
	if(pDevices)
	{
		UINT count;
		pDevices->GetCount(&count);

		gpDevices = (MFAudioDevice*)MFHeap_AllocAndZero(sizeof(MFAudioDevice)*count);
		gNumDevices = count;

		for(UINT i=0; i<count; ++i)
		{
			IMMDevice *pDevice;
			pDevices->Item(i, &pDevice);

			MFAudioDevice &device = gpDevices[i];

			wchar_t *pWC;
			pDevice->GetId(&pWC);
			MFString_CopyUTF16ToUTF8(device.id, pWC);
			CoTaskMemFree(pWC);

			pDevice->GetState(&device.state);

			IPropertyStore *pProps;
			pDevice->OpenPropertyStore(STGM_READ, &pProps);

			PROPVARIANT v;
			PropVariantInit(&v);
			pProps->GetValue(PKEY_DeviceInterface_FriendlyName, &v);
			MFString_CopyUTF16ToUTF8(device.deviceInterface_FriendlyName, v.pwszVal);
			PropVariantClear(&v);

			PropVariantInit(&v);
			pProps->GetValue(PKEY_Device_DeviceDesc, &v);
			MFString_CopyUTF16ToUTF8(device.device_Desc, v.pwszVal);
			PropVariantClear(&v);

			PropVariantInit(&v);
			pProps->GetValue(PKEY_Device_FriendlyName, &v);
			MFString_CopyUTF16ToUTF8(device.device_FriendlyName, v.pwszVal);
			PropVariantClear(&v);

			PropVariantInit(&v);
			pProps->GetValue(PKEY_Device_Manufacturer, &v);
			MFString_CopyUTF16ToUTF8(device.device_Manufacturer, v.pwszVal ? v.pwszVal : L"");
			PropVariantClear(&v);

			MFDebug_Log(0, MFStr("Found audio device: %s, %s - state: %d (%s)", device.device_FriendlyName, device.device_Manufacturer, device.state, device.id));

			pProps->Release();
			pDevice->Release();
		}
		pDevices->Release();
	}

	IMMDevice *pDevice;
	gpEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if(pDevice)
	{
		wchar_t *pDefaultId;
		pDevice->GetId(&pDefaultId);
		CoTaskMemFree(pDefaultId);
		pDevice->Release();
	}
	gpEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
	if(pDevice)
	{
		pDevice->Release();
	}
	gpEnumerator->GetDefaultAudioEndpoint(eRender, eCommunications, &pDevice);
	if(pDevice)
	{
		pDevice->Release();
	}

	gpEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &pDevices);
	if(pDevices)
	{
		UINT count;
		pDevices->GetCount(&count);

		gpCaptureDevices = (MFAudioCaptureDevice*)MFHeap_AllocAndZero(sizeof(MFAudioCaptureDevice)*count);
		gNumCaptureDevices = count;

		for(UINT i=0; i<count; ++i)
		{
			IMMDevice *pDevice;
			pDevices->Item(i, &pDevice);

			MFAudioCaptureDevice &device = gpCaptureDevices[i];

			wchar_t *pWC;
			pDevice->GetId(&pWC);
			MFString_CopyUTF16ToUTF8(device.id, pWC);
			CoTaskMemFree(pWC);

			pDevice->GetState(&device.state);

			IPropertyStore *pProps;
			pDevice->OpenPropertyStore(STGM_READ, &pProps);

			PROPVARIANT v;
			PropVariantInit(&v);
			pProps->GetValue(PKEY_DeviceInterface_FriendlyName, &v);
			MFString_CopyUTF16ToUTF8(device.deviceInterface_FriendlyName, v.pwszVal);
			PropVariantClear(&v);

			PropVariantInit(&v);
			pProps->GetValue(PKEY_Device_DeviceDesc, &v);
			MFString_CopyUTF16ToUTF8(device.device_Desc, v.pwszVal);
			PropVariantClear(&v);

			PropVariantInit(&v);
			pProps->GetValue(PKEY_Device_FriendlyName, &v);
			MFString_CopyUTF16ToUTF8(device.device_FriendlyName, v.pwszVal);
			PropVariantClear(&v);

			PropVariantInit(&v);
			pProps->GetValue(PKEY_Device_Manufacturer, &v);
			MFString_CopyUTF16ToUTF8(device.device_Manufacturer, v.pwszVal ? v.pwszVal : L"");
			PropVariantClear(&v);

			MFDebug_Log(0, MFStr("Found audio capture device: %s, %s - state: %d (%s)", device.device_FriendlyName, device.device_Manufacturer, device.state, device.id));

			pProps->Release();
			pDevice->Release();
		}
		pDevices->Release();
	}

	gpEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
	if(pDevice) pDevice->Release();
	gpEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDevice);
	if(pDevice) pDevice->Release();
	gpEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pDevice);
	if(pDevice) pDevice->Release();

}

void MFSound_DeinitWASAPI()
{
	MFHeap_Free(gpCaptureDevices);

	gpEnumerator->Release();
	delete gpNotification;
}

void MFSound_UpdateWASAPI()
{
	const int NumSamples = 4096;
	float buffer[NumSamples];

	for(size_t i=0; i<gNumCaptureDevices; ++i)
	{
		if(gpCaptureDevices[i].pDevice && gpCaptureDevices[i].bActive)
		{
			MFAudioCaptureDevice &device = gpCaptureDevices[i];

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
		}
	}
}

MF_API size_t MFSound_GetNumCaptureDevices()
{
	return gNumCaptureDevices;
}

MF_API const char *MFSound_GetCaptureDeviceId(size_t index)
{
	MFDebug_Assert(index < gNumCaptureDevices, "Invalid device index!");
	return gpCaptureDevices[index].id;
}

MF_API MFAudioCaptureDevice* MFSound_OpenCaptureDevice(const char *pDeviceName)
{
	for(size_t i=0; i<gNumCaptureDevices; ++i)
	{
		if(MFString_Compare(gpCaptureDevices[i].id, pDeviceName) == 0)
		{
			MFAudioCaptureDevice &device = gpCaptureDevices[i];

			if(device.state == 1)
			{
				// get a handle to the device
				wchar_t id[256];
				MFWString_CopyUTF8ToUTF16(id, device.id);
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
				return &device;
			}
			else
				break;
		}
	}
	return nullptr;
}

MF_API void MFSound_CloseCaptureDevice(MFAudioCaptureDevice *pDevice)
{
	MFSound_StopCapture(pDevice);

	pDevice->pCaptureClient->Release();
	pDevice->pAudioClient->Release();
	pDevice->pDevice->Release();
}

MF_API void MFSound_StartCapture(MFAudioCaptureDevice *pDevice, MFAudioCaptureCallback *pCallback, void *pUserData)
{
	pDevice->pSampleCallback = pCallback;
	pDevice->pUserData = pUserData;

	HRESULT hr = pDevice->pAudioClient->Start();  // Start recording.
	if(FAILED(hr))
		MFDebug_Warn(2, "Couldn't start capture");

	pDevice->bActive = true;
}

MF_API void MFSound_StopCapture(MFAudioCaptureDevice *pDevice)
{
	HRESULT hr = pDevice->pAudioClient->Stop();  // Stop recording.
	pDevice->bActive = false;
}

MF_API const char *MFSound_GetCaptureDeviceString(const MFAudioCaptureDevice *pDevice, MFSoundDeviceString string)
{
	switch(string)
	{
		case MFSDS_ID:
			return pDevice->id;
		case MFSDS_InterfaceName:
			return pDevice->deviceInterface_FriendlyName;
		case MFSDS_DeviceName:
			return pDevice->device_FriendlyName;
		case MFSDS_Description:
			return pDevice->device_Desc;
		case MFSDS_Manufacturer:
			return pDevice->device_Manufacturer;
	}
	return NULL;
}

#endif
