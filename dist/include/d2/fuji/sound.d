module fuji.sound;

public import fuji.c.MFSound;

nothrow: @nogc:

const(char)[] MFSound_GetCaptureDeviceId(size_t index)
{
	import fuji.c.MFString: toDStr;
	return fuji.c.MFSound.MFSound_GetCaptureDeviceId(index).toDStr;
}

const(char)[] MFSound_GetCaptureDeviceString(const(MFAudioCaptureDevice)* pDevice, MFSoundDeviceString stringType) pure
{
	import fuji.c.MFString: toDStr;
	return fuji.c.MFSound.MFSound_GetCaptureDeviceString(pDevice, stringType).toDStr;
}

struct AudioCaptureDevice
{
	MFAudioCaptureDevice *pDevice;

nothrow: @nogc:
	bool open(size_t index)
	{
		const(char)* pId = fuji.c.MFSound.MFSound_GetCaptureDeviceId(index);
		if(pId)
			pDevice = MFSound_OpenCaptureDevice(pId);

		return pDevice != null;
	}
	bool open(const(char)[] id)
	{
		import fuji.string: Stringz;
		auto s = Stringz!128(id);

		pDevice = MFSound_OpenCaptureDevice(s);
		return pDevice != null;
	}

	void close()
	{
		MFSound_CloseCaptureDevice(pDevice);
	}

	void start(MFAudioCaptureCallback callback, void* pUserData = null)
	{
		MFSound_StartCapture(pDevice, callback, pUserData);
	}

	void stop()
	{
		MFSound_StopCapture(pDevice);
	}

pure:
	@property const(char)[] id() const { return MFSound_GetCaptureDeviceString(pDevice, MFSoundDeviceString.ID); }
	@property const(char)[] name() const { return MFSound_GetCaptureDeviceString(pDevice, MFSoundDeviceString.DeviceName); }
	@property const(char)[] interfaceName() const { return MFSound_GetCaptureDeviceString(pDevice, MFSoundDeviceString.InterfaceName); }
	@property const(char)[] description() const { return MFSound_GetCaptureDeviceString(pDevice, MFSoundDeviceString.Description); }
	@property const(char)[] manufacturer() const { return MFSound_GetCaptureDeviceString(pDevice, MFSoundDeviceString.Manufacturer); }
}
