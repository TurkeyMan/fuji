module fuji.dbg;

public import fuji.c.MFDebug;
import fuji.string;


// register fuji assert handler...
static this()
{
	import core.exception;

	static void fujiAssert(string file, size_t line, string msg) nothrow
	{
		auto _msg = Stringz!()(msg);
		auto _file = Stringz!()(file);
		MFDebug_DebugAssert("assert()", _msg, _file, cast(int)line);
	}

	assertHandler = &fujiAssert;
}

void logError(Args...)(const(char)[] message, Args args)
{
	import std.format : format;
	MFDebug_Error(format(message, args));
}

void logWarning(Args...)(int level, const(char)[] message, Args args)
{
	import std.format : format;
	MFDebug_Warn(level, format(message, args));
}

void logMessage(Args...)(const(char)[] message, Args args)
{
	import std.format : format;
	MFDebug_Message(format(message, args));
}

void debugLog(Args...)(int level, const(char)[] message, Args args)
{
	import std.format : format;
	MFDebug_Log(level, format(message, args));
}

nothrow:
@nogc:

void MFDebug_Assert(alias reason = false)(in char[] message, string file = __FILE__, int line = __LINE__) nothrow
{
	if(!reason)
		MFDebug_DebugAssert(reason.stringof.ptr, message ? message.ptr : ("Failed: " ~ reason.stringof).ptr, file.ptr, line);
}

alias MFDebug_Message = fuji.c.MFDebug.MFDebug_Message;
void MFDebug_Message(const(char)[] message)
{
	auto s = Stringz!()(message);
	MFDebug_Message(s);
}

alias MFDebug_Error = fuji.c.MFDebug.MFDebug_Error;
void MFDebug_Error(const(char)[] errorMessage)
{
	auto s = Stringz!()(errorMessage);
	MFDebug_Error(s);
}

alias MFDebug_Warn = fuji.c.MFDebug.MFDebug_Warn;
void MFDebug_Warn(int level, const(char)[] warningMessage)
{
	auto s = Stringz!()(warningMessage);
	MFDebug_Warn(level, s);
}

alias MFDebug_Log = fuji.c.MFDebug.MFDebug_Log;
void MFDebug_Log(int level, const(char)[] message)
{
	auto s = Stringz!()(message);
	MFDebug_Log(level, s);
}
