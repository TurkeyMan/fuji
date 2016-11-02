module fuji.filesystem;

public import fuji.c.MFFileSystem;

import fuji.string;
import fuji.dbg;

import std.file;
import std.typecons;
import std.array;
import std.range;
import std.algorithm;
import std.string : cmp;

public import std.exception : assumeUnique;

// offer a directory iterator similar to the one in std.file

enum SpanMode
{
	shallow,	// Only spans one directory.
	depth,		// Depth first. contents is scanned before directory its self.
	breadth,	// Bredth first.
}

private string joinPath(const(char)[] s1, const(char)[] s2) pure nothrow
{
	return (s1[$-1] == ':' || s1[$-1] == '/' ? s1 ~ s2 : s1 ~ "/" ~ s2).idup;
}

struct DirEntry
{
	alias filepath this;

	string filepath;		// the file or directory represented by this DirEntry
	string systemPath;		// the system path to this file.
	string filename;		// the filename part
	string directory;		// the directory part
	ulong size;				// file size in bytes
	uint  attributes;		// file attributes
	MFFileTime createTime;	// creation time
	MFFileTime writeTime;	// last write time
	MFFileTime accessTime;	// last access time

pure:
nothrow:
	private this(string path, ref in MFFindData fd)
	{
		filename = fd.filename.idup;
		directory = path;
		if(directory.length > 0 && directory[$-1] == '/')
			directory = directory[0..$-1];

		filepath = joinPath(path, filename);

		systemPath = fd.systemPath.idup;

		size = fd.fileSize;
		attributes = fd.attributes;

		createTime = fd.createTime;
		writeTime = fd.writeTime;
		accessTime = fd.accessTime;
	}

@nogc:
	@property bool isDir() const { return (attributes & MFFileAttributes.Directory) != 0; }
	@property bool isFile() const { return !isDir; }
	@property bool isSymlink() const { return (attributes & MFFileAttributes.SymLink) != 0; }
}

private struct DirIteratorImpl
{
	struct DirHandle
	{
		string dirpath;
		MFFind* h;
	}

	SpanMode _mode;
	string _pattern;
	bool _followSymlink;
	DirEntry _cur;
	Appender!(DirHandle[]) _stack;
	Appender!(DirEntry[]) _stashed; //used in depth first mode

	void pushExtra(DirEntry de) nothrow { _stashed.put(de); }
	bool hasExtra() nothrow @nogc { return !_stashed.data.empty; }
	DirEntry popExtra()
	{
		DirEntry de;
		de = _stashed.data[$-1];
		_stashed.shrinkTo(_stashed.data.length - 1);
		return de;
	}

	bool stepIn(string directory)
	{
		string search_pattern = joinPath(directory, "*");
		MFFindData findinfo = void;
		MFFind* h = MFFileSystem_FindFirst(search_pattern, findinfo);
		if(!h)
			throw new FileException(search_pattern, "Invalid find path");
		_stack.put(DirHandle(directory, h));
		return toNext(false, findinfo);
	}

	bool next()
	{
		if(_stack.data.empty)
			return false;
		MFFindData findinfo = void;
		return toNext(true, findinfo);
	}

	bool toNext(bool fetch, ref MFFindData findinfo)
	{
		if(fetch)
		{
			if(MFFileSystem_FindNext(_stack.data[$-1].h, findinfo) == false)
			{
				popDirStack();
				return false;
			}
		}
		while(cmp(findinfo.filename, ".") == 0 || cmp(findinfo.filename, "..") == 0)
//			  || (!(findinfo.attributes & MFFileAttributes.Directory) && !MFString_PatternMatch(_pattern.ptr, findinfo.filename.ptr)))
		{
			if(MFFileSystem_FindNext(_stack.data[$-1].h, findinfo) == false)
			{
				popDirStack();
				return false;
			}
		}
		_cur = DirEntry(_stack.data[$-1].dirpath, findinfo);
		return true;
	}

	void popDirStack()
	{
		assert(!_stack.data.empty);
		MFFileSystem_FindClose(_stack.data[$-1].h);
		_stack.shrinkTo(_stack.data.length-1);
	}

	void releaseDirStack() nothrow @nogc
	{
		foreach(d; _stack.data)
			MFFileSystem_FindClose(d.h);
	}

	bool mayStepIn() const pure nothrow @nogc
	{
		return _cur.isDir && (_followSymlink || !_cur.isSymlink);
	}

	this(string pathname, SpanMode mode, bool followSymlink)
	{
		_mode = mode;
		_followSymlink = followSymlink;
		_stack = appender(cast(DirHandle[])[]);
		if(_mode == SpanMode.depth)
			_stashed = appender(cast(DirEntry[])[]);

		_pattern = pathname.find('*');
		pathname = pathname[0 .. $ - _pattern.length];

		if(stepIn(pathname))
		{
			if(_mode == SpanMode.depth)
				while(mayStepIn())
				{
					auto thisDir = _cur;
					if(stepIn(_cur.filepath))
					{
						pushExtra(thisDir);
					}
					else
						break;
				}
		}
	}
	@property bool empty() const pure nothrow @nogc { return _stashed.data.empty && _stack.data.empty; }
	@property DirEntry front() const pure nothrow @nogc { return _cur; }
	void popFront()
	{
		switch(_mode)
		{
			case SpanMode.depth:
				if(next())
				{
					while(mayStepIn())
					{
						auto thisDir = _cur;
						if(stepIn(_cur.filepath))
						{
							pushExtra(thisDir);
						}
						else
							break;
					}
				}
				else if(hasExtra())
					_cur = popExtra();
				break;
			case SpanMode.breadth:
				if(mayStepIn())
				{
					if(!stepIn(_cur.filepath))
						while(!empty && !next()){}
				}
				else
					while(!empty && !next()){}
				break;
			default:
				next();
		}
	}

	~this() nothrow @nogc
	{
		releaseDirStack();
	}
}

struct DirIterator
{
private:
	RefCounted!(DirIteratorImpl, RefCountedAutoInitialize.no) impl;
	this(string pathname, SpanMode mode, bool followSymlink)
	{
		impl = typeof(impl)(pathname, mode, followSymlink);
	}

public:
	@property bool empty() const pure nothrow @nogc		{ return impl.empty; }
	@property DirEntry front() const pure nothrow @nogc	{ return impl.front; }
	void popFront()										{ impl.popFront(); }
}

auto dirEntries(string path, SpanMode mode, bool followSymlink = true)
{
	return DirIterator(path, mode, followSymlink);
}


nothrow:

ubyte[] MFFileSystem_Load(const(char)[] filename, size_t extraBytes = 0)
{
	auto s = Stringz!()(filename);
	MFFile *file = MFFileSystem_Open(s, MFOpenFlags.Read | MFOpenFlags.Binary);
	if(file)
	{
		scope(exit) MFFile_Close(file);

		ulong size = MFFile_GetSize(file);
		if(size > 0)
		{
			static if(size_t.sizeof == 4)
			{
				if(size >= 1UL << 32)
				{
					MFDebug_Warn(1, "File is larger than the available address space: " ~ filename);
					return null;
				}
			}

			ubyte[] buffer = new ubyte[cast(size_t)size + extraBytes];
			size_t bytesRead = MFFile_Read(file, buffer.ptr, cast(size_t)size);
			assert(bytesRead == size, "bytesRead different from file size!(?)");

			if(extraBytes > 0)
				buffer[cast(size_t)size] = 0;

			return buffer;
		}
	}
	return null;
}

char[] MFFileSystem_LoadText(const(char)[] filename, size_t extraBytes = 0)
{
	return cast(char[])MFFileSystem_Load(filename, extraBytes);
}

size_t MFFileSystem_Save(const(char)[] filename, const(ubyte)[] buffer)
{
	MFDebug_Log(5, "Call: MFFileSystem_Save(\"" ~ filename ~ "\")");

	auto s = Stringz!()(filename);
	MFFile *file = MFFileSystem_Open(s, MFOpenFlags.Write | MFOpenFlags.Truncate | MFOpenFlags.Binary | MFOpenFlags.CreateDirectory);
	if(file)
	{
		scope(exit) MFFile_Close(file);
		return MFFile_Write(file, buffer.ptr, buffer.length, false);
	}
	return 0;
}

size_t MFFileSystem_SaveText(const(char)[] filename, const(char)[] buffer)
{
	return MFFileSystem_Save(filename, cast(const(ubyte)[])buffer);
}

@nogc:

alias MFFile_Open = fuji.c.MFFileSystem.MFFile_Open;
MFFile* MFFile_Open(MFFileSystemHandles fs, ref const(MFOpenData) openData)
{
	MFFileSystemHandle h = MFFileSystem_GetInternalFileSystemHandle(fs);
	return fuji.c.MFFileSystem.MFFile_Open(h, openData);
}

alias MFFile_SystemPath = fuji.c.MFFileSystem.MFFile_SystemPath;
const(char)[] MFFile_SystemPath(const(char)[] filename)
{
	auto s = Stringz!()(filename);
	return MFFile_SystemPath(s).toDStr;
}

alias MFFile_HomePath = fuji.c.MFFileSystem.MFFile_HomePath;
const(char)[] MFFile_HomePath(const(char)[] filename)
{
	auto s = Stringz!()(filename);
	return MFFile_HomePath(s).toDStr;
}

alias MFFileSystem_Open = fuji.c.MFFileSystem.MFFileSystem_Open;
MFFile* MFFileSystem_Open(const(char)[] filename, uint openFlags = MFOpenFlags.Read | MFOpenFlags.Binary)
{
	auto s = Stringz!()(filename);
	return MFFileSystem_Open(s, openFlags);
}

alias MFFileSystem_Load = fuji.c.MFFileSystem.MFFileSystem_Load;
alias MFFileSystem_Save = fuji.c.MFFileSystem.MFFileSystem_Save;

alias MFFileSystem_GetSize = fuji.c.MFFileSystem.MFFileSystem_GetSize;
ulong MFFileSystem_GetSize(const(char)[] filename)
{
	auto s = Stringz!()(filename);
	return MFFileSystem_GetSize(s);
}

alias MFFileSystem_Exists = fuji.c.MFFileSystem.MFFileSystem_Exists;
bool MFFileSystem_Exists(const(char)[] filename)
{
	auto s = Stringz!()(filename);
	return MFFileSystem_Exists(s);
}

alias MFFileSystem_FindFirst = fuji.c.MFFileSystem.MFFileSystem_FindFirst;
MFFind* MFFileSystem_FindFirst(const(char)[] searchPattern, out MFFindData findData)
{
	auto s = Stringz!()(searchPattern);
	return MFFileSystem_FindFirst(s, &findData);
}

alias MFFileSystem_FindNext = fuji.c.MFFileSystem.MFFileSystem_FindNext;
bool MFFileSystem_FindNext(MFFind* pFind, out MFFindData findData)
{
	return MFFileSystem_FindNext(pFind, &findData);
}
