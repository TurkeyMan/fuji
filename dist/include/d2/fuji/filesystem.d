module fuji.filesystem;

public import fuji.fuji;

/**
* @struct MFFile
* Represents a Fuji file.
*/
struct MFFile;

/**
* Represents a Fuji FileSystem.
* Represents a Fuji FileSystem.
*/
alias int MFFileSystemHandle;

/**
* File open flags.
* Flags to use when opening a file.
*/
enum MFOpenFlags : uint
{
	Read	= 1,	/**< File has read access */
	Write	= 2,	/**< File has write access */
	Text	= 4,	/**< Open file in text mode */
	Binary	= 8,	/**< Open file in binary mode */

	CreateDirectory = 16,	/**< Create the directory if it doesn't already exist */

	User	= 256	/**< User flags begin here (for use by other file systems) */
}

/**
* File seek origin.
* File seek origin.
*/
enum MFFileSeek
{
	Begin,		/**< Seek from the start of the file */
	Current,	/**< Seek from the current file poition */
	End			/**< Seek from the end of the file */
}

/**
* Represents asyncrenous file operation state.
* Represents asyncrenous file operation state.
*/
enum MFJobState
{
	Ready = 0,		/**< Job is ready */
	Waiting,		/**< Job is waiting */
	Busy,			/**< Job is busy */
	Finished		/**< Job is busy */
}

/**
* Gets the system file path.
* Gets the default game data path and optionally appends a filename.
* @param filename Filename to append to the path. This may be set to null.
* @return The path to the game data directory.
*/
extern (C) const(char)* MFFile_SystemPath(const(char)* filename = null);
const(char)[] MFFile_SystemPath(const(char)[] filename = null)
{
	return MFFile_SystemPath(filename ? filename.toStringz : null).toDStr;
}

/**
* Gets the system home path.
* Gets the default game home path and optionally appends a filename.
* @param filename Filename to append to the path. This may be set to null.
* @return The path to the game home directory.
* @remarks The home path is typically used for debugging and development purposes (writing logs, screenshots, etc). The game should not require access to the home path.
*/
extern (C) const(char)* MFFile_HomePath(const(char)* filename = null);
const(char)[] MFFile_HomePath(const(char)[] filename = null)
{
	return MFFile_HomePath(filename ? filename.toStringz : null).toDStr;
}


///////////////////////////
// file access functions

/**
* Open file data base structure.
* Base structure for open file data.
*/
struct MFOpenData
{
	int cbSize = typeof(this).sizeof;	/**< Size of the structure */
	uint openFlags;						/**< Open file flags, this can be values from the MFOpenFlags enum */
}

/**
* Open a file.
* Opens a file.
* @param fileSystem Filesystem which provides the file.
* @param pOpenData Pointer to an MFOpenData structure describing the file to open.
* @return A pointer to the newly opened file, returns null if the file open failed.
*/
extern (C) MFFile* MFFile_Open(MFFileSystemHandle fileSystem, const ref MFOpenData openData);

/**
* Close a file.
* Closes an open file.
* @param pFile Pointer to an open file.
* @return Returns 0 if the file was successfully closed.
*/
extern (C) int MFFile_Close(MFFile *pFile);

/**
* Read data from a file.
* Reads data from a file.
* @param pFile Pointer to an open file.
* @param pBuffer Pointer to a buffer where the read data will be stored.
* @param bytes Number of bytes to read.
* @param async If true, the read will be performed asyncrenously, putting the file into a 'busy' state.
* @return The number of bytes read.
*/
extern (C) int MFFile_Read(MFFile *pFile, void *pBuffer, size_t bytes, bool async = false);

/**
* Write to a file.
* Writes data to a file.
* @param pFile Pointer to an open file.
* @param pBuffer Pointer to the data to be written.
* @param bytes Number of bytes to write.
* @param async If true, the write will be performed asyncrenously, putting the file into a 'busy' state.
* @return The number of bytes written.
*/
extern (C) int MFFile_Write(MFFile *pFile, const void *pBuffer, size_t bytes, bool async = false);

/**
* Seek the file.
* Seek to a soecified file offset.
* @param pFile Pointer to an open file.
* @param bytes Number of bytes to seek.
* @param relativity Member of the MFFileSeek enumerated type where to begin the seek.
* @return The new file offset in bytes.
*/
extern (C) int MFFile_Seek(MFFile *pFile, int bytes, MFFileSeek relativity);

/**
* Tell the file position.
* Tells the current file position.
* @param pFile Pointer to an open file.
* @return The file pointer offset in bytes.
*/
extern (C) int MFFile_Tell(MFFile *pFile);

/**
* Get the size of a file.
* Gets the size of a file.
* @param pFile Pointer to an open file.
* @return The size of the file in bytes. Returns -1 for a file stream with an undefined length. Returns 0 if the file does not exist.
*/
extern (C) long MFFile_GetSize(MFFile *pFile);

/**
* Check for end of file.
* Check to see if the file has reached its end.
* @param pFile Pointer to an open file.
* @return True if the file pointer has reached the end of the file, otherwise false.
*/
extern (C) bool MFFile_IsEOF(MFFile *pFile);

// stdio signiture functions (these can be used as callbacks to many libs and API's)

/**
* Close a file.
* Closes an open file.
* @param stream Handle to an open file.
* @return Returns 0 if the file was successfully closed.
* @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
*/
extern (C) int MFFile_StdClose(void* stream);

/**
* Read data from a file.
* Reads data from a file.
* @param buffer Pointer to a buffer where the read data will be stored.
* @param size Number of bytes in a block.
* @param count Number of blocks to read.
* @param stream Handle to an open file.
* @return The number of bytes read.
* @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
*/
extern (C) size_t MFFile_StdRead(void *buffer, size_t size, size_t count, void* stream);

/**
* Write to a file.
* Writes data to a file.
* @param buffer Pointer to the data to be written.
* @param size Number of bytes in a block.
* @param count Number of blocks to write.
* @param stream Handle to an open file.
* @return The number of bytes written.
* @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
*/
extern (C) size_t MFFile_StdWrite(const void *buffer, size_t size, size_t count, void* stream);

/**
* Seek the file.
* Seek to a soecified file offset.
* @param stream Handle to an open file.
* @param offset Number of bytes to seek.
* @param whence Member of the MFFileSeek enumerated type where to begin the seek.
* @return The new file offset in bytes.
* @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
*/
extern (C) long MFFile_StdSeek(void* stream, long offset, int whence);

/**
* Tell the file position.
* Tells the current file position.
* @param stream Handle to an open file.
* @return The file pointer offset in bytes.
* @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
*/
extern (C) long MFFile_StdTell(void* stream);


//////////////////////////////
// mounted filesystem access

/**
* @struct MFFind
* Represents a Fuji find handle.
*/
struct MFFind;

/**
* Mount flags.
* General mount flags. These can be used to control the way a filesystem is mounted.
*/
enum MFMountFlags
{
	FlattenDirectoryStructure = 1,	/**< Flattens the directory heirarchy */
	Recursive = 2,					/**< Recurse into subdirectories when building TOC */
	DontCacheTOC = 4,				/**< Doesn't take a local memory copy of the TOC (useful for filesystems read from memory) */
	OnlyAllowExclusiveAccess = 8	/**< This will exclude this mount from any non-specific filesystem operations (filenames explicitly directed to this mount using 'device:') */
}

/**
* Mount priority enum.
* These are some guideline mount priorities. Priorities can really be any number.
*/
enum MFMountPriority
{
	Highest = 0,			/**< Highest priority filesystem (first in the search queue) */
	VeryHigh = 1,			/**< Very high priority filesystem */
	AboveNormal = 5,		/**< Above normal priority filesystem */
	Normal = 10,			/**< Normal priority filesystem */
	BelowNormal = 15,		/**< Below normal priority filesystem */
	VeryLow = 20,			/**< Very low priority filesystem (last in the search queue) */
}

/**
* File attributes.
* These are a set of file attributes.
*/
enum MFFileAttributes : uint
{
	Directory = MFBit!0,	/**< File is a directory */
	SymLink = MFBit!1,		/**< File is a symbolic link */
	Hidden = MFBit!2,		/**< File is hidden */
	ReadOnly = MFBit!3,		/**< File is read only */
}

/**
* Mount data base structure.
* Base structure for mount data.
*/
struct MFMountData
{
	int cbSize = typeof(this).sizeof;		/**< Size of the structure */
	uint flags;								/**< Mount flags, this can be values from the MFMountFlags enum */
	const(char)* pMountpoint;				/**< The mountpoint string (the volume name) */
	int priority = MFMountPriority.Normal;	/**< Filsystem priority when searching for files */
}

/**
* FileSystem handle enums.
* These provide direct access to the various available file systems.
*/
enum MFFileSystemHandles
{
	Unknown = -1,			/**< Unknown FileSystem */

	NativeFileSystem = 0,	/**< The native operating system FileSystem */
	MemoryFileSystem,		/**< Memory file FileSystem */
	CachedFileSystem,		/**< Cached file FileSystem */
	ZipFileSystem,			/**< Zip file FileSystem */
	HTTPFileSystem,			/**< HTTP file FileSystem */
	FTPFileSystem			/**< FTP file FileSystem */
}

/**
* FileSystem find data.
* Structure used to return information about a file in the filesystem.
*/
struct MFFindData
{
	char pFilename[256] = void;		/**< The files filename */
	char pSystemPath[256] = void;	/**< The system path to the file */
	ulong fileSize;					/**< The files size */
	uint attributes;				/**< The files attributes */

	@property const(char)[] filename() const pure nothrow	{ return fuji.string.toDStr(pFilename.ptr); }
	@property const(char)[] systemPath() const pure nothrow	{ return fuji.string.toDStr(pSystemPath.ptr); }
}

/**
* FileSystem volume info.
* MFFileSystem volume information.
*/
struct MFVolumeInfo
{
	const(char)* pVolumeName;		/**< The name of the volume */
	MFFileSystemHandle fileSystem;	/**< The filesystem handle the volume is mounted on */
	uint flags;						/**< Volume flags */
	int priority;					/**< Mount priority */

	@property const(char)[] volumeName() const pure nothrow { return fuji.string.toDStr(pVolumeName); }
}

/**
* Get a handle to a specific filesystem.
* Gets a handle to a specific filesystem.
* @param fileSystemHandle Enum of the filesystem to retrieve.
* @return The FileSystemHandle for the specified filesystem.
*/
extern (C) MFFileSystemHandle MFFileSystem_GetInternalFileSystemHandle(MFFileSystemHandles fileSystemHandle);

/**
* Mounts a filesystem.
* Mounts a filesystem which provides files to Fuji.
* @param fileSystem Handle to the filesystem which provides access to the data.
* @param pMountData Pointer to a MFMountData structure filled with all the mount parameters.
* @return Returns 0 if filesystem was successfully mounted.
*/
extern (C) int MFFileSystem_Mount(MFFileSystemHandle fileSystem, const ref MFMountData mountData);

/**
* Mounts a filesystem from a fuji path.
* Mounts a filesystem which provides files to Fuji from a fully justified Fuji path.
* @param pMountpoint The volume name.
* @param pFujiPath Fully justified Fuji path to the volume root.
* @param priority Filsystem priority when searching for files.
* @param flags Mount flags, this can be any combination of values from the MFMountFlags enum.
* @return Returns 0 if filesystem was successfully mounted.
*/
extern (C) int MFFileSystem_MountFujiPath(const(char)* pMountpoint, const(char)* pFujiPath, int priority = MFMountPriority.Normal, uint flags = 0);

/**
* Dismount a filesystem.
* Dismounts a mounted filesystem.
* @param pMountpoint The name of the mountpoint for the filesystem to dismount.
* @return Returns 0 if the filesystem was successfully dismounted.
*/
extern (C) int MFFileSystem_Dismount(const(char)* pMountpoint);

/**
* Open a file from the mounted filesystem stack.
* Open a file from the mounted filesystem stack.
* @param pFilename The name of the file to open.
* @param openFlags Open file flags.
* @return A pointer to the opened file. Returns null if open failed.
*/
extern (C) MFFile* MFFileSystem_Open(const(char)* pFilename, uint openFlags = MFOpenFlags.Read | MFOpenFlags.Binary);

/**
* Load a file from the filesystem.
* Load a file from the filesystem.
* @param pFilename The name of the file to load.
* @param pBytesRead Optional pointer to a size_t that will receive the size of the file loaded.
* @param @param extraBytes Number of extra bytes to allocate at the end of the buffer. Initialises the first extra byte to 0 (useful for appending a NULL byte to the end of text files).
* @return A pointer to a new buffer containing the file that was loaded.
*/
extern (C) ubyte* MFFileSystem_Load(const(char)* pFilename, size_t *pBytesRead = null, size_t extraBytes = 0);
ubyte[] MFFileSystem_Load(const(char)[] filename, size_t extraBytes = 0)
{
	size_t size;
	ubyte* file = MFFileSystem_Load(filename.toStringz, &size, extraBytes);
	return file[0..size];
}

/**
* Write a file to a filesystem.
* Write a file to a filesystem.
* @param pFilename The name of the file to write. If the target file does not already exist, the filename must include the mountpoint to identify the target filesystem.
* @param pBuffer Buffer to write to the file.
* @param size Size of the buffer to write.
* @return Returns 0 if the file was succesfully written.
*/
extern (C) int MFFileSystem_Save(const(char)* pFilename, const(ubyte)* pBuffer, size_t size);
int MFFileSystem_Save(const(char)[] filename, const(ubyte)[] buffer)
{
	return MFFileSystem_Save(filename.toStringz, buffer.ptr, buffer.length);
}

/**
* Get the size of a file.
* Gets the size of a file.
* @param pFilename The name of the file to find the size.
* @return The size of the file in bytes. If the file does not exist, MFFileSystem_GetSize returns 0.
* @remarks If the file does not exist, MFFileSystem_GetSize returns 0, however, a zero length file will also return 0. Use MFFileSystem_Exists to correctly test if a file exists. MFFileSystem_GetSize may also return -1 if the files length is not known, for instance, an endless or unknown length network stream.
*/
extern (C) long MFFileSystem_GetSize(const(char)* pFilename);
long MFFileSystem_GetSize(const(char)[] filename)
{
	return MFFileSystem_GetSize(filename.toStringz);
}

/**
* See if a file is available to the filesystem.
* Attempts to locate a file in the mounted filesystem stack.
* @param pFilename The filename to search the filesystem for.
* @return True if the file can be found within the mounted filesystem stack.
*/
extern (C) bool MFFileSystem_Exists(const(char)* pFilename);
bool MFFileSystem_Exists(const(char)[] filename)
{
	return MFFileSystem_Exists(filename.toStringz);
}

/**
* Get number of available volumes.
* Gets the number of available volumes.
* @return The number of mounted volumes.
*/
extern (C) int MFFileSystem_GetNumVolumes();

/**
* Get volume mount details.
* Gets the details of a mounted volume.
* @param volumeID Target volume ID.
* @param pVolumeInfo Pointer to an MFVolumeInfo structre that receives the volumes mount details.
* @return None.
*/
extern (C) void MFFileSystem_GetVolumeInfo(int volumeID, MFVolumeInfo *pVolumeInfo);

/**
* Begin a find for files.
* Finds the first file matching a specified search pattern.
* @param pSearchPattern The search pattern. The search pattern MUST be a full justified fuji path begining with a volume, and ending with a filename pattern to match.
* @param pFindData Pointer to an MFFindData structure which receives details about the file.
* @return An MFFind handle that is passed to subsequent calls to MFFileSystem_FindNext and MFFileSystem_FindClose.
* @remarks Currently the only valid filename pattern is '*'. For example: "data:subdir/ *" is a valid search pattern.
* @see MFFileSystem_FindNext(), MFFileSystem_FindClose()
*/
extern (C) MFFind* MFFileSystem_FindFirst(const(char)* pSearchPattern, MFFindData *pFindData);
MFFind* MFFileSystem_FindFirst(const(char)[] searchPattern, MFFindData *pFindData)
{
	return MFFileSystem_FindFirst(searchPattern.toStringz, pFindData);
}

/**
* Find the next file.
* Finds the next file in the directory matching the search pattern specified in MFFileSystem_FindFirst().
* @param pFind MFFind handle returned from a previous call to MFFileSystem_FindFirst().
* @param pFindData Pointer to an MFFindData structure which receives details about the file.
* @return True on success or false if there are no more files in the directory.
* @see MFFileSystem_FindFirst(), MFFileSystem_FindClose()
*/
extern (C) bool MFFileSystem_FindNext(MFFind *pFind, MFFindData *pFindData);

/**
* Close an open find.
* Closes an open find file process.
* @param pFind MFFind handle returned from a previous call to MFFileSystem_FindFirst().
* @return None.
* @see MFFileSystem_FindFirst()
*/
extern (C) void MFFileSystem_FindClose(MFFind *pFind);


/////////////////////////////////////
// helper functions, to make life easier

extern (C) MFFile* MFFile_CreateMemoryFile(const void *pMemory, size_t size, bool writable = false, bool ownMemory = false);


// D wrappers

import fuji.string;

import std.file;
import std.typecons;
import std.string;
import std.array;
import std.range;
import std.algorithm;

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
