module fuji.sound;

public import fuji.c.MFSound;
import fuji.device;

nothrow: @nogc:

struct AudioCaptureDevice
{
	Device device;
	MFAudioCaptureDevice *pCapture;

	alias device this;

	this(Device device)
	{
		this.device = device;
	}

nothrow: @nogc:
	bool open()
	{
		pCapture = MFSound_OpenCaptureDevice(device.pDevice);
		return pCapture != null;
	}

	void close()
	{
		MFSound_CloseCaptureDevice(pCapture);
		pCapture = null;
	}

	void start(MFAudioCaptureCallback callback, void* pUserData = null)
	{
		MFSound_StartCapture(pCapture, callback, pUserData);
	}

	void stop()
	{
		MFSound_StopCapture(pCapture);
	}
}
