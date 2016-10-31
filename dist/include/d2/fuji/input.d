module fuji.input;

public import fuji.c.MFInput;

import fuji.fuji : toDStr;

nothrow:
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
