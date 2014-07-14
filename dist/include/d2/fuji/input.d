module fuji.input;

public import fuji.c.MFInput;

nothrow:
@nogc:

alias MFInput_GetEvents = fuji.c.MFInput.MFInput_GetEvents;
MFInputEvent[] MFInput_GetEvents(int device, int deviceID, MFInputEvent[] events, bool bPeek = false) nothrow
{
	size_t count = fuji.c.MFInput.MFInput_GetEvents(device, deviceID, events.ptr, events.length, bPeek);
	return events[0 .. count];
}
