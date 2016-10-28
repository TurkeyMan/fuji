module fuji.midi;

public import fuji.c.MFMidi;
import fuji.device;

nothrow @nogc:

alias MFMidi_GetEvents = fuji.c.MFMidi.MFMidi_GetEvents;
MFMidiEvent[] MFMidi_GetEvents(MFDevice* pMidiInput, MFMidiEvent[] events, bool bPeek = false) nothrow
{
	size_t count = MFMidi_GetEvents(pMidiInput, events.ptr, events.length, bPeek);
	return events[0 .. count];
}

struct MidiInputDevice
{
	Device device;
	alias device this;

nothrow @nogc:
	this(size_t index)
	{
		device = Device(MFDeviceType.MidiInput, index);
	}
	this(const(char)[] id)
	{
		device = Device(id);
	}
	this(MFDefaultDeviceType defaultType)
	{
		device = Device(MFDeviceType.MidiInput, defaultType);
	}
	this(Device device)
	{
		assert(device.type == MFDeviceType.MidiInput);
		this.device = device;
	}

	bool open(bool bBuffered = false, MFMidiEventCallback* pEventCallback = null)
	{
		return MFMidi_OpenInput(device.handle, bBuffered, pEventCallback);
	}
	void close()
	{
		MFMidi_CloseInput(device.handle);
	}

	void start()
	{
		MFMidi_Start(device.handle);
	}
	void stop()
	{
		MFMidi_Stop(device.handle);
	}

	@property size_t numEvents()
	{
		return MFMidi_GetEvents(device.handle, null, 0, false);
	}

	MFMidiEvent[] getEvents(MFMidiEvent[] eventBuffer, bool bPeek = false)
	{
		return MFMidi_GetEvents(device.handle, eventBuffer, bPeek);
	}

	uint read(MFMidiDataType type, int channel, int note = 0)
	{
		return MFMidi_GetState(device.handle, type, channel, note);
	}
	bool pressed(int channel, int note)
	{
		return MFMidi_WasPressed(device.handle, channel, note);
	}
	bool released(int channel, int note)
	{
		return MFMidi_WasReleased(device.handle, channel, note);
	}
}

struct MidiOutputDevice
{
	Device device;
	alias device this;

nothrow @nogc:
	this(size_t index)
	{
		device = Device(MFDeviceType.MidiOutput, index);
	}
	this(const(char)[] id)
	{
		device = Device(id);
	}
	this(MFDefaultDeviceType defaultType)
	{
		device = Device(MFDeviceType.MidiOutput, defaultType);
	}
	this(Device device)
	{
		assert(device.type == MFDeviceType.MidiOutput);
		this.device = device;
	}

	bool open()
	{
		return MFMidi_OpenOutput(device.handle);
	}
	void close()
	{
		MFMidi_CloseOutput(device.handle);
	}

	void sendPacket(const(ubyte)[] bytes)
	{
		MFMidi_SendPacket(device.handle, bytes.ptr, bytes.length);
	}
	void sendShortMessage(uint message)
	{
		MFMidi_SendShortMessage(device.handle, message);
	}
	void sendEvent(ref MFMidiEvent event)
	{
		MFMidi_SendEvent(device.handle, &event);
	}

	void setControl(ubyte channel, MFMidiControl control, ushort value)
	{
		MFMidi_SetControlWord(device.handle, channel, control, value);
	}
	void setControl(ubyte channel, MFMidiControl control, ubyte value)
	{
		MFMidi_SetControlByte(device.handle, channel, control, value);
	}
	void setControl(ubyte channel, MFMidiControl control, bool value)
	{
		MFMidi_SetControlBool(device.handle, channel, control, value);
	}

	void programChange(ubyte channel, ubyte program, ushort bank = 0xFFFF)
	{
		MFMidi_ProgramChange(device.handle, channel, program, bank);
	}

	void setParameter(ubyte channel, bool bRegistered, ushort parameter, ubyte data)
	{
		MFMidi_SetParameterB(device.handle, channel, bRegistered, parameter, data);
	}
	void setParameter(ubyte channel, bool bRegistered, ushort parameter, ubyte msb, ubyte lsb)
	{
		MFMidi_SetParameter2B(device.handle, channel, bRegistered, parameter, msb, lsb);
	}
	void setParameter(ubyte channel, bool bRegistered, ushort parameter, ushort data)
	{
		MFMidi_SetParameterW(device.handle, channel, bRegistered, parameter, data);
	}

	void incParameter(ubyte channel, bool bRegistered, ushort parameter)
	{
		MFMidi_IncParameter(device.handle, channel, bRegistered, parameter);
	}
	void decParameter(ubyte channel, bool bRegistered, ushort parameter)
	{
		MFMidi_DecParameter(device.handle, channel, bRegistered, parameter);
	}
}
