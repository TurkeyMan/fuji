module fuji.string;

public import fuji.c.MFString;
import fuji.dbg;

nothrow:

string makePath(const(char)[][] parts ...)
{
	string path;
	foreach(part; parts)
	{
		if(part.isRooted)
			path = part.idup;
		else
		{
			if(path.length == 0 || (part.length > 0 && (path[$-1].isPathSeparator || part[0].isPathSeparator)))
				path = path ~ part.idup;
			else
				path = path ~ '/' ~ part.idup;
		}
	}
	return path;
}


@nogc:

bool isPathSeparator(dchar c)
{
	return c == '/' || c == '\\';
}

bool isRooted(const(char)[] path)
{
	foreach(c; path)
	{
		if(c == ':')
			return true;
	}
	return false;
}


inout(char)[] fileExtension(inout(char)[] filename)
{
	for(size_t i = filename.length - 1; i > 0; --i)
	{
		char c = filename[i];
		if(c == '.')
			return !filename[i-1].isPathSeparator ? filename[i..$] : filename[$..$];
		else if(c.isPathSeparator)
			return filename[$..$];
	}
	return filename[$..$];
}

inout(char)[] truncateExtension(inout(char)[] filename)
{
	for(size_t i = filename.length - 1; i > 0; --i)
	{
		char c = filename[i];
		if(c == '.')
			return !filename[i-1].isPathSeparator ? filename[0..i] : filename;
		else if(c.isPathSeparator)
			return filename;
	}
	return filename;
}

inout(char)[] fileName(inout(char)[] filename)
{
	for(ptrdiff_t i = filename.length - 1; i >= 0; --i)
	{
		char c = filename[i];
		if(c.isPathSeparator)
			return filename[i+1..$];
	}
	return filename;
}

inout(char)[] fileNameWithoutExtension(inout(char)[] filename)
{
	size_t dot = filename.length;
	for(ptrdiff_t i = filename.length - 1; i >= 0; --i)
	{
		char c = filename[i];
		if(c == '.')
		{
			if(dot == filename.length && i > 0)
				dot = i;
		}
		else if(c.isPathSeparator)
			return dot == i + 1 ? filename[i+1..$] : filename[i+1..dot];
	}
	return filename[0..dot];
}

inout(char)[] filePath(inout(char)[] filename, bool withSeparator = false)
{
	for(ptrdiff_t i = filename.length - 1; i >= 0; --i)
	{
		char c = filename[i];
		if(c.isPathSeparator)
			return filename[0..i + (withSeparator ? 1 : 0)];
	}
	return filename[0..0];
}


// quick and dirty stack string for passing to C code
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
