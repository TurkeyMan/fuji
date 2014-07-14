module fuji.string;

public import fuji.c.MFString;
import fuji.dbg;

nothrow:
@nogc:

struct Stringz(size_t stackSize = 256)
{
	alias cstr this;

nothrow:
@nogc:
	this(const(char)[] s)
	{
		if(!s)
			return;

		if(s.ptr[s.length] == 0)
		{
			str = s;
			stackStr[0] = 0;
		}
		else
		{
			char[] t;
			if(s.length < stackSize-1)
			{
				t = stackStr[0..s.length];
				stackStr[s.length] = 0;
			}
			else
			{
				debug MFDebug_Warn(4, "String is larger than stack buffer.".ptr);
				char* alloc = MFStringHeap_Alloc(s.length + 1);
				t = alloc[0..s.length];
				alloc[s.length] = 0;
				stackStr[0] = 1;
			}

			t[] = s[];
			str = t;
		}
	}

	~this()
	{
		if(str.ptr != stackStr.ptr && stackStr[0] == 1)
			MFStringHeap_Free(cast(char*)str.ptr);
		str = null;
	}

	@property const(char)* cstr() { return str.ptr; }
	@property const(char)[] dstr() { return str; }

private:
	const(char)[] str;
	char[stackSize] stackStr = void;
}


private:

extern (C) char* MFStringHeap_Alloc(size_t bytes, size_t* pAllocated = null);
extern (C) void MFStringHeap_Free(char* pString);
