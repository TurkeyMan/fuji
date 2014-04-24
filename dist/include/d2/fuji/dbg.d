module fuji.dbg;

public import fuji.c.MFDebug;

import std.string;

void MFDebug_Assert(alias reason = false)(in char[] message, string file = __FILE__, int line = __LINE__) nothrow
{
	if(!reason)
		MFDebug_DebugAssert(reason.stringof.ptr, message ? message.ptr : ("Failed: " ~ reason.stringof).ptr, file.ptr, line);
}

alias MFDebug_Message = fuji.c.MFDebug.MFDebug_Message;
void MFDebug_Message(const(char)[] message) nothrow
{
	MFDebug_Message(message.toStringz);
}

alias MFDebug_Error = fuji.c.MFDebug.MFDebug_Error;
void MFDebug_Error(const(char)[] errorMessage) nothrow
{
	MFDebug_Error(errorMessage.toStringz);
}

alias MFDebug_Warn = fuji.c.MFDebug.MFDebug_Warn;
void MFDebug_Warn(int level, const(char)[] warningMessage) nothrow
{
	MFDebug_Warn(level, warningMessage.toStringz);
}

alias MFDebug_Log = fuji.c.MFDebug.MFDebug_Log;
void MFDebug_Log(int level, const(char)[] message) nothrow
{
	MFDebug_Log(level, message.toStringz);
}
