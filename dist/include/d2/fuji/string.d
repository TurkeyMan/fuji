module fuji.string;


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
	@property const(char)[] asDString() { return pData ? pData.pMemory[0 .. pData.bytes] : (cast(char*)null)[0..0]; }
	alias asDString this;

	string idup() { return asDString.idup; }

	private	MFStringData *pData;
}


// D helpers
import std.c.string;

inout(char)[] toDStr(inout(char)* cstr) pure nothrow
{
	return cstr ? cstr[0 .. strlen(cstr)] : cstr[0 .. 0];
}

