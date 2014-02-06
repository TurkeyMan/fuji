module fuji.midi;

public import fuji.fuji;


struct MFMidiInput;

enum MFMidiDeviceStatus
{
	Unknown = -1,

	Disconnected = 0,
	Available,
	Ready,
	Active
}

enum MFMidiDataType
{
	Note = 0,
	Controller,
	Program,		// program/patch
	PitchWheel
}

struct MFMidiEvent
{
	uint timestamp;
	ubyte command;
	ubyte channel;
	ubyte data0;
	ubyte data1;
}

alias MFMidiEventCallback = void function(MFMidiInput* pMidiInput, const MFMidiEvent* pEvent);


extern (C) int MFMidi_GetNumDevices();
extern (C) const char* MFMidi_GetDeviceName(int deviceId);

extern (C) MFMidiDeviceStatus MFMidi_GetStatus(int deviceId);

extern (C) MFMidiInput* MFMidi_OpenInput(int deviceId, bool bBuffered = false, MFMidiEventCallback* pEventCallback = null);
extern (C) void MFMidi_CloseInput(MFMidiInput* pMidiInput);

extern (C) uint MFMidi_GetState(MFMidiInput* pMidiInput, MFMidiDataType type, int channel, int note = 0);
extern (C) uint MFMidi_WasPressed(MFMidiInput* pMidiInput, int channel, int note);
extern (C) uint MFMidi_WasReleased(MFMidiInput* pMidiInput, int channel, int note);

extern (C) bool MFMidi_Start(MFMidiInput* pMidiInput);
extern (C) void MFMidi_Stop(MFMidiInput* pMidiInput);
extern (C) size_t MFMidi_GetEvents(MFMidiInput* pMidiInput, MFMidiEvent* pEvents, size_t maxEvents, bool bPeek = false);
MFMidiEvent[] MFMidi_GetEvents(MFMidiInput* pMidiInput, MFMidiEvent[] events, bool bPeek = false)
{
	size_t count = MFMidi_GetEvents(pMidiInput, events.ptr, events.length, bPeek);
	return events[0 .. count];
}

enum MFMidiNote
{
	Unknown = 0xFF,

	C0 = 0,
	Cs0,
	D0,
	Ds0,
	E0,
	F0,
	Fs0,
	G0,
	Gs0,
	A0,
	As0,
	B0,
	C1,
	Cs1,
	D1,
	Ds1,
	E1,
	F1,
	Fs1,
	G1,
	Gs1,
	A1,
	As1,
	B1,
	C2,
	Cs2,
	D2,
	Ds2,
	E2,
	F2,
	Fs2,
	G2,
	Gs2,
	A2,
	As2,
	B2,
	C3,
	Cs3,
	D3,
	Ds3,
	E3,
	F3,
	Fs3,
	G3,
	Gs3,
	A3,
	As3,
	B3,
	C4,
	Cs4,
	D4,
	Ds4,
	E4,
	F4,
	Fs4,
	G4,
	Gs4,
	A4,
	As4,
	B4,
	C5,
	Cs5,
	D5,
	Ds5,
	E5,
	F5,
	Fs5,
	G5,
	Gs5,
	A5,
	As5,
	B5,
	C6,
	Cs6,
	D6,
	Ds6,
	E6,
	F6,
	Fs6,
	G6,
	Gs6,
	A6,
	As6,
	B6,
	C7,
	Cs7,
	D7,
	Ds7,
	E7,
	F7,
	Fs7,
	G7,
	Gs7,
	A7,
	As7,
	B7,
	C8,
	Cs8,
	D8,
	Ds8,
	E8,
	F8,
	Fs8,
	G8,
	Gs8,
	A8,
	As8,
	B8,
	C9,
	Cs9,
	D9,
	Ds9,
	E9,
	F9,
	Fs9,
	G9,
	Gs9,
	A9,
	As9,
	B9,
	C10,
	Cs10,
	D10,
	Ds10,
	E10,
	F10,
	Fs10,
	G10
}
