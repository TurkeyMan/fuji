module fuji.sound;

public import fuji.c.MFSound;
import fuji.device;

nothrow: @nogc:

struct AudioCaptureDevice
{
	Device device;

	alias device this;

	this(Device device)
	{
		this.device = device;
	}

nothrow: @nogc:
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
}
