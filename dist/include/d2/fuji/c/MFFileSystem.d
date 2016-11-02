module fuji.c.MFFileSystem;

import fuji.c.Fuji;
import core.stdc.config;

nothrow:
@nogc:

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
	Read		= 0x1,	/**< File has read access */
	Write		= 0x2,	/**< File has write access */
	Truncate	= 0x4,	/**< Truncate existing file */
	Append		= 0x8,	/**< Append to existing file */
	Text		= 0x10,	/**< Open file in text mode */
	Binary		= 0x20,	/**< Open file in binary mode */

	CreateDirectory = 0x40,	/**< Create the directory if it doesn't already exist */

	TryOpen		= 0x80,	/**< Try and open the file, but doesn't complain if the open operation fails */

	User		= 0x100	/**< User flags begin here (for use by other file systems) */
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

/**
* Gets the system home path.
* Gets the default game home path and optionally appends a filename.
* @param filename Filename to append to the path. This may be set to null.
* @return The path to the game home directory.
* @remarks The home path is typically used for debugging and development purposes (writing logs, screenshots, etc). The game should not require access to the home path.
*/
extern (C) const(char)* MFFile_HomePath(const(char)* filename = null);


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

struct MFFileTime
{
	ulong ticks;

nothrow:
@nogc:
	bool opEquals(MFFileTime other) const pure	{ return ticks == other.ticks; }
	long opCmp(MFFileTime other) const pure		{ return cast(long)ticks - cast(long)other.ticks; }
}

struct MFFileInfo
{
	ulong size;
	uint attributes;
	MFFileTime createTime;
	MFFileTime writeTime;
	MFFileTime accessTime;
}

/**
* Open a file.
* Opens a file.
* @param fileSystem Filesystem which provides the file.
* @param pOpenData Pointer to an MFOpenData structure describing the file to open.
* @return A pointer to the newly opened file, returns null if the file open failed.
*/
extern (C) MFFile* MFFile_Open(MFFileSystemHandle fileSystem, ref const(MFOpenData) openData);

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
extern (C) size_t MFFile_Read(MFFile *pFile, void *pBuffer, size_t bytes, bool async = false);

/**
* Write to a file.
* Writes data to a file.
* @param pFile Pointer to an open file.
* @param pBuffer Pointer to the data to be written.
* @param bytes Number of bytes to write.
* @param async If true, the write will be performed asyncrenously, putting the file into a 'busy' state.
* @return The number of bytes written.
*/
extern (C) size_t MFFile_Write(MFFile *pFile, const void *pBuffer, size_t bytes, bool async = false);

/**
* Seek the file.
* Seek to a soecified file offset.
* @param pFile Pointer to an open file.
* @param bytes Number of bytes to seek.
* @param relativity Member of the MFFileSeek enumerated type where to begin the seek.
* @return The new file offset in bytes.
*/
extern (C) ulong MFFile_Seek(MFFile *pFile, long bytes, MFFileSeek relativity);

/**
* Tell the file position.
* Tells the current file position.
* @param pFile Pointer to an open file.
* @return The file pointer offset in bytes.
*/
extern (C) ulong MFFile_Tell(MFFile *pFile);

/**
* Get the size of a file.
* Gets the size of a file.
* @param pFile Pointer to an open file.
* @return The size of the file in bytes. Returns -1 for a file stream with an undefined length. Returns 0 if the file does not exist.
*/
extern (C) ulong MFFile_GetSize(MFFile *pFile);

/**
* Check for end of file.
* Check to see if the file has reached its end.
* @param pFile Pointer to an open file.
* @return True if the file pointer has reached the end of the file, otherwise false.
*/
extern (C) bool MFFile_IsEOF(MFFile *pFile);

extern (C) bool MFFile_Stat(MFFileSystemHandle fileSystem, const(char)* pPath, MFFileInfo* pFileInfo);
extern (C) bool MFFile_CreateDirectory(MFFileSystemHandle fileSystem, const(char)* pPath);
extern (C) bool MFFile_Delete(MFFileSystemHandle fileSystem, const(char)* pPath, bool bRecursive);


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
extern (C) c_long MFFile_StdSeek(void* stream, c_long offset, int whence);

/**
* Tell the file position.
* Tells the current file position.
* @param stream Handle to an open file.
* @return The file pointer offset in bytes.
* @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
*/
extern (C) c_long MFFile_StdTell(void* stream);


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
	char[224] pFilename = void;		/**< The files filename */
	char[260] pSystemPath = void;	/**< The system path to the file */
	uint attributes;				/**< The files attributes */
	ulong fileSize;					/**< The files size */
	MFFileTime createTime;			/**< Time the file was created */
	MFFileTime writeTime;			/**< Last time the file was written */
	MFFileTime accessTime;			/**< Last time the file was accessed */

nothrow: @nogc:
	@property const(char)[] filename() const pure	{ return pFilename.ptr.toDStr; }
	@property const(char)[] systemPath() const pure	{ return pSystemPath.ptr.toDStr; }
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

	@property const(char)[] volumeName() const pure nothrow { return pVolumeName.toDStr; }
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
extern (C) int MFFileSystem_Mount(MFFileSystemHandle fileSystem, ref const(MFMountData) mountData);

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
extern (C) ubyte* MFFileSystem_Load(const(char)* pFilename, size_t* pBytesRead = null, size_t extraBytes = 0);

/**
* Write a file to a filesystem.
* Write a file to a filesystem.
* @param pFilename The name of the file to write. If the target file does not already exist, the filename must include the mountpoint to identify the target filesystem.
* @param pBuffer Buffer to write to the file.
* @param size Size of the buffer to write.
* @return Returns the number of bytes written.
*/
extern (C) size_t MFFileSystem_Save(const(char)* pFilename, const(ubyte)* pBuffer, size_t size);

/**
* Get the size of a file.
* Gets the size of a file.
* @param pFilename The name of the file to find the size.
* @return The size of the file in bytes. If the file does not exist, MFFileSystem_GetSize returns 0.
* @remarks If the file does not exist, MFFileSystem_GetSize returns 0, however, a zero length file will also return 0. Use MFFileSystem_Exists to correctly test if a file exists. MFFileSystem_GetSize may also return -1 if the files length is not known, for instance, an endless or unknown length network stream.
*/
extern (C) ulong MFFileSystem_GetSize(const(char)* pFilename);

/**
* See if a file is available to the filesystem.
* Attempts to locate a file in the mounted filesystem stack.
* @param pFilename The filename to search the filesystem for.
* @return True if the file can be found within the mounted filesystem stack.
*/
extern (C) bool MFFileSystem_Exists(const(char)* pFilename);

extern (C) bool MFFileSystem_Stat(const(char)* pPath, MFFileInfo* pFileInfo);
extern (C) bool MFFileSystem_CreateDirectory(const(char)* pPath);
extern (C) bool MFFileSystem_Delete(const(char)* pPath, bool bRecursive);

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
extern (C) void MFFileSystem_GetVolumeInfo(int volumeID, MFVolumeInfo* pVolumeInfo);

/**
* Begin a find for files.
* Finds the first file matching a specified search pattern.
* @param pSearchPattern The search pattern. The search pattern MUST be a full justified fuji path begining with a volume, and ending with a filename pattern to match.
* @param pFindData Pointer to an MFFindData structure which receives details about the file.
* @return An MFFind handle that is passed to subsequent calls to MFFileSystem_FindNext and MFFileSystem_FindClose.
* @remarks Currently the only valid filename pattern is '*'. For example: "data:subdir/ *" is a valid search pattern.
* @see MFFileSystem_FindNext(), MFFileSystem_FindClose()
*/
extern (C) MFFind* MFFileSystem_FindFirst(const(char)* pSearchPattern, MFFindData* pFindData);

/**
* Find the next file.
* Finds the next file in the directory matching the search pattern specified in MFFileSystem_FindFirst().
* @param pFind MFFind handle returned from a previous call to MFFileSystem_FindFirst().
* @param pFindData Pointer to an MFFindData structure which receives details about the file.
* @return True on success or false if there are no more files in the directory.
* @see MFFileSystem_FindFirst(), MFFileSystem_FindClose()
*/
extern (C) bool MFFileSystem_FindNext(MFFind* pFind, MFFindData* pFindData);

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

extern (C) MFFile* MFFile_CreateMemoryFile(const(void)* pMemory, size_t size, bool writable = false, bool ownMemory = false);
