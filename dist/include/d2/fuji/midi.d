module fuji.midi;

public import fuji.c.MFMidi;

alias MFMidi_GetEvents = fuji.c.MFMidi.MFMidi_GetEvents;
MFMidiEvent[] MFMidi_GetEvents(MFMidiInput* pMidiInput, MFMidiEvent[] events, bool bPeek = false) nothrow
{
	size_t count = MFMidi_GetEvents(pMidiInput, events.ptr, events.length, bPeek);
	return events[0 .. count];
}
