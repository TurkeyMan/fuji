module fuji.sound;

public import fuji.c.MFSound;
import fuji.device;

nothrow: @nogc:

struct AudioCaptureDevice
{
	Device device;
	alias device this;

nothrow: @nogc:
	this(Device device)
	{
		this.device = device;
	}

	bool open()
	{
		return MFSound_OpenCaptureDevice(device.pDevice);
	}

	void close()
	{
		MFSound_CloseCaptureDevice(device.pDevice);
	}

	void start(MFAudioCaptureCallback callback, void* pUserData = null)
	{
		MFSound_StartCapture(device.pDevice, callback, pUserData);
	}

	void stop()
	{
		MFSound_StopCapture(device.pDevice);
	}

	@property MFSoundDeviceInfo info() const pure
	{
		MFSoundDeviceInfo info;
		MFSound_GetDeviceInfo(device.pDevice, &info);
		return info;
	}
}
