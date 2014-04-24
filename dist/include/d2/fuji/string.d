module fuji.string;

public import fuji.c.MFString;

public import std.string: toStringz;


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
	inout(char*) toStringz() inout pure nothrow { return pData ? pData.pMemory : null; }

	@property inout(char)[] asDString() inout pure nothrow { return pData ? pData.pMemory[0 .. pData.bytes] : null; }

	private	MFStringData *pData;
}
