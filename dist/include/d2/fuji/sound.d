module fuji.sound;

public import fuji.c.MFSound;
import fuji.device;

nothrow @nogc:

struct AudioCaptureDevice
{
	Device device;
	alias device this;

nothrow @nogc:
	this(size_t index)
	{
		device = Device(MFDeviceType.AudioCapture, index);
	}
	this(const(char)[] id)
	{
		device = Device(id);
	}
	this(MFDefaultDeviceType defaultType)
	{
		device = Device(MFDeviceType.AudioCapture, defaultType);
	}
	this(Device device)
	{
		assert(device.type == MFDeviceType.AudioCapture);
		this.device = device;
	}

	bool open()
	{
		return MFSound_OpenCaptureDevice(device.handle);
	}

	void close()
	{
		MFSound_CloseCaptureDevice(device.handle);
	}

	void start(MFAudioCaptureCallback callback, void* pUserData = null)
	{
		MFSound_StartCapture(device.handle, callback, pUserData);
	}

	void stop()
	{
		MFSound_StopCapture(device.handle);
	}

	@property MFSoundDeviceInfo info() const pure
	{
		MFSoundDeviceInfo info;
		MFSound_GetDeviceInfo(device.handle, &info);
		return info;
	}
}
