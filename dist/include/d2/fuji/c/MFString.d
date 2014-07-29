module fuji.c.MFString;

nothrow:
@nogc:

/**
* Copy memory.
* Copy memory from one location to another.
* @param pDest Pointer to the destination buffer.
* @param pSrc Pointer to the source buffer.
* @param size Size of data to copy in bytes.
* @return Returns pDest for convenience.
*/
extern (C) void* MFCopyMemory(void* pDest, const(void)* pSrc, size_t size);

/**
* Zero memory.
* Initialise a region of memory to zero.
* @param pDest Pointer to the target buffer.
* @param size Size of data to initialise in bytes.
* @return Returns pDest for convenience.
*/
extern (C) void* MFZeroMemory(void* pDest, size_t size);

/**
* Initialise memory.
* Initialise a region of memory to a specified value.
* @param pDest Pointer to the target buffer.
* @param value Value written to each byte in the memory region.
* @param size Number of bytes to initialise.
* @return Returns pDest for convenience.
*/
extern (C) void* MFMemSet(void* pDest, int value, size_t size);

/**
* Compare memory.
* Compare bytes returning the difference.
* @param pBuf1 Pointer to the first buffer.
* @param pBuf2 Pointer to the second buffer.
* @param size Number of bytes to compate.
* @return Return the difference of the first encountered differing byte. Returns 0 if buffers are identical.
*/
extern (C) int MFMemCompare(const(void)* pBuf1, const(void)* pBuf2, size_t size);

/**
* String pattern match.
* Performs a string pattern match.
* @param pPattern String containing the filename pattern to test. May contain wild cards, etc.
* @param pFilename String containing the filename to compare against.
* @param ppMatchDirectory Cant remember what this is used for. Dont use it...
* @param bCaseSensitive Perform the pattern match with case sensitivity.
* @return True if the pattern matches the filename.
*/
extern (C) bool MFString_PatternMatch(const(char)* pPattern, const(char)* pFilename, const(char)** ppMatchDirectory = null, bool bCaseSensitive = false);

/**
* URL encode a string.
* Encodes a string for use in a URL or HTTP 'POST' content.
* @param pString String to encode.
* @param pExcludeChars A string containing characters to be excluded from URL encoding.
* @return The URL encoded string.
* @remarks The pointer returned is in the MFStr buffer. If persistence is desired, the client should take a copy.
*/
extern (C) const(char)* MFStr_URLEncodeString(const(char)* pString, const(char)* pExcludeChars = null);

/**
* URL encode a string.
* Encodes a string for use in a URL or HTTP 'POST' content.
* @param pDest Output buffer, may be null.
* @param pString String to encode.
* @param pExcludeChars A string containing characters to be excluded from URL encoding.
* @return Return the length of the output string, excluding the terminating null.
* @remarks If pDest is null, the output length will be calculated but no output will be written.
*/
extern (C) size_t MFString_URLEncode(char* pDest, const(char)* pString, const(char)* pExcludeChars = null);

/**
* Test if a string is a number.
* Tests to see if a given string contains a decimal or hexadecimal number (using '0x' hex notation).
* @param pString String to test.
* @param bAllowHex Test also if the string is a hexadecimal number.
* @return True if the string was a valid number.
*/
extern (C) bool MFString_IsNumber(const(char)* pString, bool bAllowHex = true);

/**
* Parse an integer from a string.
* Parses a decimal, hexadecimal or binary integer from a given string.
* @param pString String to parse.
* @param bDetectBase Detect the numeric base of the string via prefixes '0x' or '$' for hex, or 'b' for binary.
* @param base If \a bDetectBase is false, specifies the numeric base to use. Accepted values are 10, 16 and 2.
* @param ppNextChar Optional pointer that receives the following character in the string.
* @return The integer parsed from the string.
*/
extern (C) int MFString_AsciiToInteger(const(char)* pString, bool bDetectBase = true, int base = 10, const(char)** ppNextChar = null);

/**
* Parse a float from a string.
* Parses a float from a given string.
* @param pString String to parse.
* @param ppNextChar Optional pointer that receives the following character in the string.
* @return The float parsed from the string.
*/
extern (C) float MFString_AsciiToFloat(const(char)* pString, const(char)** ppNextChar = null);

/**
* Find a string within a list of strings.
* Finds a string within a list of strings and return the item index.
* @param pString String to find.
* @param ppKeys Array of keys to search.
* @param numKeys Number of items in the array. If -1 is given, ppKeys is assumed to be a null terminated array.
* @param bCaseSensitive Perform case sensitive string comparisons.
* @return The index of \a pString if it was present in the list, or -1 if it was not present.
*/
extern (C) int MFString_Enumerate(const(char)* pString, const(char)** ppKeys, size_t numKeys = -1, bool bCaseSensitive = false);



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

	private	MFStringData* pData;
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

nothrow:
@nogc:
	void destroy()
	{
		if(--refCount == 0)
			MFStringData_Destroy(&this);
	}
};

extern(C) MFStringData* MFStringData_Alloc();
extern(C) void MFStringData_Destroy(MFStringData* pStringData);

extern (C) char* MFStringHeap_Alloc(size_t bytes, size_t* pAllocated = null);
extern (C) void MFStringHeap_Free(char* pString);
