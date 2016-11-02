module fuji.input;

public import fuji.c.MFInput;

import fuji.fuji : toDStr;

nothrow:

struct InputDevice
{
nothrow:
	this(MFInputDevice device, int deviceID) @nogc
	{
		this.device = cast(int)device;
		this.deviceID = deviceID;
	}

	@property bool available() const @nogc { return MFInput_IsAvailable(device, deviceID); }
	@property bool connected() const @nogc { return MFInput_IsConnected(device, deviceID); }
	@property bool ready() const @nogc { return MFInput_IsReady(device, deviceID); }

	@property uint deviceFlags() const @nogc { return MFInput_GetDeviceFlags(device, deviceID); }

	@property const(char)[] name() const @nogc { return MFInput_GetDeviceName(device, deviceID); }

	const(char)[] enumerateString(int control, bool includeDevice = false, bool includeDeviceID = false) const @nogc { return MFInput_EnumerateString(control, device, deviceID, includeDevice, includeDeviceID).toDStr; }

	float read(int button, float *pPrevState = null) const @nogc { return MFInput_Read(button, device, deviceID, pPrevState); }
	float pressed(int button) const @nogc { return MFInput_WasPressed(button, device, deviceID); }
	float released(int button) const @nogc { return MFInput_WasReleased(button, device, deviceID); }

	size_t numPendingEvents() const @nogc { return fuji.c.MFInput.MFInput_GetEvents(device, deviceID, null, size_t.max, true); }

	MFInputEvent[] inputEvents(bool bPeek = false)
	{
		size_t numEvents = numPendingEvents();
		if (numEvents == 0)
			return null;
		MFInputEvent[] events = new MFInputEvent[numEvents];
		numEvents = fuji.c.MFInput.MFInput_GetEvents(device, deviceID, events.ptr, events.length, bPeek);
		return events[0 .. numEvents];
	}

	MFInputEvent[] getInputEvents(MFInputEvent[] eventsBuffer, bool bPeek = false) @nogc
	{
		size_t count = fuji.c.MFInput.MFInput_GetEvents(device, deviceID, eventsBuffer.ptr, eventsBuffer.length, bPeek);
		return eventsBuffer[0 .. count];
	}

	bool keyboardStatusState(MFKeyboardStatusState keyboardState)
	{
		assert(device == MFInputDevice.Keyboard);
		return MFInput_GetKeyboardStatusState(cast(int)keyboardState, deviceID);
	}

	const(char)[] gamepadButtonName(int button) const @nogc
	{
		assert(device == MFInputDevice.Gamepad);
		return MFInput_GetGamepadButtonName(button, deviceID);
	}

	int device;
	int deviceID;
}

@nogc:

alias MFInput_GetEvents = fuji.c.MFInput.MFInput_GetDeviceName;
const(char)[] MFInput_GetDeviceName(int device, int deviceID)
{
	return fuji.c.MFInput.MFInput_GetDeviceName(device, deviceID).toDStr;
}

alias MFInput_GetEvents = fuji.c.MFInput.MFInput_GetGamepadButtonName;
const(char)[] MFInput_GetGamepadButtonName(int button, int deviceID)
{
	return fuji.c.MFInput.MFInput_GetGamepadButtonName(button, deviceID).toDStr;
}

alias MFInput_GetEvents = fuji.c.MFInput.MFInput_GetEvents;
MFInputEvent[] MFInput_GetEvents(int device, int deviceID, MFInputEvent[] events, bool bPeek = false) nothrow
{
	size_t count = fuji.c.MFInput.MFInput_GetEvents(device, deviceID, events.ptr, events.length, bPeek);
	return events[0 .. count];
}
