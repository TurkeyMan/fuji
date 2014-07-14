module fuji.c.MFString;

nothrow:
@nogc:

extern (C) bool MFString_PatternMatch(const(char)* pPattern, const(char)* pFilename, const(char)** ppMatchDirectory = null, bool bCaseSensitive = false);


struct MFString
{
	alias dstr this;

nothrow:
	@property string idup() const pure { return dstr.idup; }

@nogc:
	this(this)
	{
		if(pData)
			++pData.refCount;
	}

	this(const(char)[] s)
	{
		if(s)
		{
			pData = MFStringData_Alloc();
			pData.bytes = s.length;

			pData.pMemory = MFStringHeap_Alloc(pData.bytes + 1, &pData.allocated);
			pData.pMemory[0..pData.bytes] = s[];
			pData.pMemory[pData.bytes] = 0;
		}
	}

	~this()
	{
		if(pData)
			pData.destroy();
	}

	@property bool empty() const pure { return !pData || pData.bytes == 0; }
	@property size_t length() const pure { return pData ? pData.bytes : 0; }

	@property inout(char*) cstr() inout pure { return pData ? pData.pMemory : null; }
	@property inout(char)[] dstr() inout pure { return pData ? pData.pMemory[0 .. pData.bytes] : null; }

	inout(char*) toStringz() inout pure { return pData ? pData.pMemory : null; }

	private	MFStringData *pData;
}

extern (C) MFString MFString_GetStats();
extern (C) void MFString_Dump();


inout(char)[] toDStr(inout(char)* cstr) pure
{
	import std.c.string : strlen;
	return cstr ? cstr[0 .. strlen(cstr)] : cstr[0 .. 0];
}


private:

struct MFStringData
{
	char* pMemory;
	size_t bytes;
	size_t allocated;
	int refCount;

	void destroy() nothrow @nogc
	{
		if(--refCount == 0)
			MFStringData_Destroy(&this);
	}
};

extern(C) MFStringData* MFStringData_Alloc();
extern(C) void MFStringData_Destroy(MFStringData* pStringData);

extern (C) char* MFStringHeap_Alloc(size_t bytes, size_t* pAllocated = null);
extern (C) void MFStringHeap_Free(char* pString);
