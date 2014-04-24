module fuji.filesystem;

public import fuji.c.MFFileSystem;

import fuji.string;

import std.file;
import std.typecons;
import std.array;
import std.range;
import std.algorithm;

alias MFFile_SystemPath = fuji.c.MFFileSystem.MFFile_SystemPath;
const(char)[] MFFile_SystemPath(const(char)[] filename = null) nothrow
{
	return MFFile_SystemPath(filename ? filename.toStringz : null).toDStr;
}

alias MFFile_HomePath = fuji.c.MFFileSystem.MFFile_HomePath;
const(char)[] MFFile_HomePath(const(char)[] filename = null) nothrow
{
	return MFFile_HomePath(filename ? filename.toStringz : null).toDStr;
}

alias MFFileSystem_Load = fuji.c.MFFileSystem.MFFileSystem_Load;
ubyte[] MFFileSystem_Load(const(char)[] filename, size_t extraBytes = 0) nothrow
{
	size_t size;
	ubyte* file = MFFileSystem_Load(filename.toStringz, &size, extraBytes);
	return file[0..size];
}

alias MFFileSystem_Save = fuji.c.MFFileSystem.MFFileSystem_Save;
int MFFileSystem_Save(const(char)[] filename, const(ubyte)[] buffer) nothrow
{
	return MFFileSystem_Save(filename.toStringz, buffer.ptr, buffer.length);
}

alias MFFileSystem_GetSize = fuji.c.MFFileSystem.MFFileSystem_GetSize;
long MFFileSystem_GetSize(const(char)[] filename) nothrow
{
	return MFFileSystem_GetSize(filename.toStringz);
}

alias MFFileSystem_Exists = fuji.c.MFFileSystem.MFFileSystem_Exists;
bool MFFileSystem_Exists(const(char)[] filename) nothrow
{
	return MFFileSystem_Exists(filename.toStringz);
}

alias MFFileSystem_FindFirst = fuji.c.MFFileSystem.MFFileSystem_FindFirst;
MFFind* MFFileSystem_FindFirst(const(char)[] searchPattern, MFFindData *pFindData) nothrow
{
	return MFFileSystem_FindFirst(searchPattern.toStringz, pFindData);
}


// offer a directory iterator similar to the one in std.file

enum SpanMode
{
	shallow,	// Only spans one directory.
	depth,		// Depth first. contents is scanned before directory its self.
	breadth,	// Bredth first.
}

private string joinPath(const(char)[] s1, const(char)[] s2)
{
	return (s1[$-1] == ':' || s1[$-1] == '/' ? s1 ~ s2 : s1 ~ "/" ~ s2).idup;
}

struct DirEntry
{
	alias filepath this;

	private this(string path, in MFFindData* fd)
	{
		filename = fd.filename.idup;
		directory = path;
		if(directory.length > 0 && directory[$-1] == '/')
			directory = directory[0..$-1];

		filepath = joinPath(path, filename);

		systemPath = fd.systemPath;

		size = fd.fileSize;
		attributes = fd.attributes;
	}

	@property bool isDir() const pure nothrow { return (attributes & MFFileAttributes.Directory) != 0; }
	@property bool isFile() const pure nothrow { return !isDir; }
	@property bool isSymlink() const pure nothrow { return (attributes & MFFileAttributes.SymLink) != 0; }

	string filepath;	// the file or directory represented by this DirEntry
	string systemPath;	// the system path to this file.
	string filename;	// the filename part
	string directory;	// the directory part
	ulong size;			// file size in bytes
	uint  attributes;	// file attributes
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

	void pushExtra(DirEntry de){ _stashed.put(de); }
	bool hasExtra(){ return !_stashed.data.empty; }
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
		MFFind* h = MFFileSystem_FindFirst(search_pattern, &findinfo);
		if (!h)
			throw new FileException(search_pattern, "Invalid find path");
		_stack.put(DirHandle(directory, h));
		return toNext(false, &findinfo);
	}

	bool next()
	{
		if(_stack.data.empty)
			return false;
		MFFindData findinfo;
		return toNext(true, &findinfo);
	}

	bool toNext(bool fetch, MFFindData* findinfo)
	{
		if(fetch)
		{
			if(MFFileSystem_FindNext(_stack.data[$-1].h, findinfo) == false)
			{
				popDirStack();
				return false;
			}
		}
		while(std.string.cmp(findinfo.filename, ".") == 0 || std.string.cmp(findinfo.filename, "..") == 0)
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

	void releaseDirStack()
	{
		foreach(d; _stack.data)
			MFFileSystem_FindClose(d.h);
	}

	bool mayStepIn()
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
	@property bool empty() { return _stashed.data.empty && _stack.data.empty; }
	@property DirEntry front() { return _cur; }
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

	~this()
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
	@property bool empty()		{ return impl.empty; }
	@property DirEntry front()	{ return impl.front; }
	void popFront()				{ impl.popFront(); }
}

auto dirEntries(string path, SpanMode mode, bool followSymlink = true)
{
	return DirIterator(path, mode, followSymlink);
}
