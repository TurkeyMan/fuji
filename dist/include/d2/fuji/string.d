module fuji.string;

public import std.string: toStringz;


extern (C) bool MFString_PatternMatch(const(char)* pPattern, const(char)* pFilename, const(char)** ppMatchDirectory = null, bool bCaseSensitive = false);

// TODO: MFString compatibility needs major work...
private struct MFStringData
{
	char* pMemory;
	size_t bytes;
	size_t allocated;
	int refCount;
};

struct MFString
{
	alias asDString this;

	@property size_t length() const pure nothrow { return pData ? pData.bytes : 0; }
	@property string idup() const pure { return asDString.idup; }
	const(char*) toStringz() const pure nothrow { return pData ? pData.pMemory : null; }

	@property const(char)[] asDString() const pure nothrow { return pData ? pData.pMemory[0 .. pData.bytes] : null; }

	private	MFStringData *pData;
}


// D helpers
import std.c.string;

inout(char)[] toDStr(inout(char)* cstr) pure nothrow
{
	return cstr ? cstr[0 .. strlen(cstr)] : cstr[0 .. 0];
}

