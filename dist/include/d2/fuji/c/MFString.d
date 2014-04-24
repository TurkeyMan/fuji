module fuji.c.MFString;

nothrow:

extern (C) bool MFString_PatternMatch(const(char)* pPattern, const(char)* pFilename, const(char)** ppMatchDirectory = null, bool bCaseSensitive = false);

struct MFString;


inout(char)[] toDStr(inout(char)* cstr) pure nothrow
{
	import std.c.string : strlen;
	return cstr ? cstr[0 .. strlen(cstr)] : cstr[0 .. 0];
}
