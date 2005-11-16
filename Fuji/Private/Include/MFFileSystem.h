/**
 * @file MFFileSystem.h
 * @brief Mount Fuji Engine FileSystem module.
 * Provides functions to interact with the filesystem.
 * @author Manu Evans
 * @defgroup MFFileSystem FileSystem Functions
 * @{
 */

#if !defined(_MFFILESYSTEM_H)
#define _MFFILESYSTEM_H

struct MFFile;
struct MFMount;

/**
 * Represents a Fuji File handle.
 * Represents a Fuji File handle.
 */
typedef MFFile* MFFileHandle;

/**
 * Represents a Fuji FileSystem.
 * Represents a Fuji FileSystem.
 */
typedef int MFFileSystemHandle;

/**
 * Callback function type for file op completion notification.
 * Callback function type for file op completion notification.
 */
typedef void (*MFAsyncOperationCompletedCallback)(MFFile *);

/**
 * File open flags.
 * Flags to use when opening a file.
 */
enum MFOpenFlags
{
	MFOF_Read	= 1,	/**< File has read access */
	MFOF_Write	= 2,	/**< File has write access */
	MFOF_Text	= 4,	/**< Open file in text mode */
	MFOF_Binary	= 8,	/**< Open file in binary mode */
	MFOF_Async	= 16,	/**< Open file in asyncrenous mode */

	MFOF_User	= 256,	/**< User flags begin here (for use by other file systems) */

	MFOF_ForceInt = 0x7FFFFFFF /**< Force open flags to an int type */
};

/**
 * File seek origin.
 * File seek origin.
 */
enum MFFileSeek
{
	MFSeek_Begin,	/**< Seek from the start of the file */
	MFSeek_Current,	/**< Seek from the current file poition */
	MFSeek_End		/**< Seek from the end of the file */
};

/**
 * Represents asyncrenous file operation state.
 * Represents asyncrenous file operation state.
 */
enum MFFileState
{
	MFFS_Unavailable = -1,	/**< File is unavailable */
	MFFS_Ready = 0,			/**< File is ready */
	MFFS_Busy,				/**< File is busy */
	MFFS_Waiting			/**< File is waiting */
};

/**
 * Gets the system file path.
 * Gets the default game data path and optionally appends a filename.
 * @param filename Filename to append to the path. This may be set to NULL.
 * @return Returns the path to the game data directory.
 */
char* MFFile_SystemPath(const char *filename = NULL);

/**
 * Gets the system home path.
 * Gets the default game home path and optionally appends a filename.
 * @param filename Filename to append to the path. This may be set to NULL.
 * @return Returns the path to the game home directory.
 * @remarks The home path is typically used for debugging and development purposes (writing logs, screenshots, etc). The game should not require access to the home path.
 */
char* MFFile_HomePath(const char *filename = NULL);


///////////////////////////
// file access functions

/**
 * Open file data base structure.
 * Base structure for open file data.
 */
struct MFOpenData
{
	int cbSize;			/**< Size of the structure */
	uint32 openFlags;	/**< Open file flags, this can be values from the MFOpenFlags enum */
};

/**
 * Open a file.
 * Opens a file.
 * @param fileSystem Filesystem which provides the file.
 * @param pOpenData Pointer to an MFOpenData structure describing the file to open.
 * @return Returns an MFFileHandle to the newly opened file, returns NULL if the file open failed.
 */
MFFileHandle MFFile_Open(MFFileSystemHandle fileSystem, MFOpenData *pOpenData);

/**
 * Close a file.
 * Closes an open file.
 * @param fileHandle Handle to an open file.
 * @return Returns 0 if the file was successfully closed.
 */
int MFFile_Close(MFFileHandle fileHandle);

/**
 * Read data from a file.
 * Reads data from a file.
 * @param fileHandle Handle to an open file.
 * @param pBuffer Pointer to a buffer where the read data will be stored.
 * @param bytes Number of bytes to read.
 * @param async If true, the read will be performed asyncrenously, putting the file into a 'busy' state.
 * @return Returns the number of bytes read.
 */
int MFFile_Read(MFFileHandle fileHandle, void *pBuffer, uint32 bytes, bool async = false);

/**
 * Write to a file.
 * Writes data to a file.
 * @param fileHandle Handle to an open file.
 * @param pBuffer Pointer to the data to be written.
 * @param bytes Number of bytes to write.
 * @param async If true, the write will be performed asyncrenously, putting the file into a 'busy' state.
 * @return Returns the number of bytes written.
 */
int MFFile_Write(MFFileHandle fileHandle, const void *pBuffer, uint32 bytes, bool async = false);

/**
 * Seek the file.
 * Seek to a soecified file offset.
 * @param fileHandle Handle to an open file.
 * @param bytes Number of bytes to seek.
 * @param relativity Member of the MFFileSeek enumerated type where to begin the seek.
 * @return Returns the new file offset in bytes.
 */
int MFFile_Seek(MFFileHandle fileHandle, int bytes, MFFileSeek relativity);

/**
 * Tell the file position.
 * Tells the current file position.
 * @param fileHandle Handle to an open file.
 * @return Returns the file pointer offset in bytes.
 */
int MFFile_Tell(MFFileHandle fileHandle);

/**
 * Get the state of a file.
 * Get the activity state of a file.
 * @param fileHandle Handle to an open file.
 * @return Returns the current file activity state.
 */
MFFileState MFFile_Query(MFFileHandle fileHandle);

/**
 * Get the size of a file.
 * Gets the size of a file.
 * @param fileHandle Handle to an open file.
 * @return Returns the size of the file in bytes. Returns -1 for a file stream with an undefined length. Returns 0 if the file does not exist.
 */
int MFFile_GetSize(MFFileHandle fileHandle);


// stdio signiture functions (these can be used as callbacks to many libs and API's)

/**
 * Close a file.
 * Closes an open file.
 * @param stream Handle to an open file.
 * @return Returns 0 if the file was successfully closed.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
int MFFile_StdClose(void* stream);

/**
 * Read data from a file.
 * Reads data from a file.
 * @param buffer Pointer to a buffer where the read data will be stored.
 * @param size Number of bytes in a block.
 * @param count Number of blocks to read.
 * @param stream Handle to an open file.
 * @return Returns the number of bytes read.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
uint32 MFFile_StdRead(void *buffer, uint32 size, uint32 count, void* stream);

/**
 * Write to a file.
 * Writes data to a file.
 * @param buffer Pointer to the data to be written.
 * @param size Number of bytes in a block.
 * @param count Number of blocks to write.
 * @param stream Handle to an open file.
 * @return Returns the number of bytes written.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
uint32 MFFile_StdWrite(void *buffer, uint32 size, uint32 count, void* stream);

/**
 * Seek the file.
 * Seek to a soecified file offset.
 * @param stream Handle to an open file.
 * @param offset Number of bytes to seek.
 * @param whence Member of the MFFileSeek enumerated type where to begin the seek.
 * @return Returns the new file offset in bytes.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
int MFFile_StdSeek(void* stream, long offset, int whence);

/**
 * Tell the file position.
 * Tells the current file position.
 * @param stream Handle to an open file.
 * @return Returns the file pointer offset in bytes.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
long MFFile_StdTell(void* stream);


//////////////////////////////
// mounted filesystem access

/**
 * Mount flags.
 * General mount flags. These can be used to control the way a filesystem is mounted.
 */
enum MFMountFlags
{
	MFMF_FlattenDirectoryStructure = 1,	/**< Flattens the directory heirarchy */
	MFMF_Recursive = 2,					/**< Recurse into subdirectories when building TOC */
	MFMF_DontCacheTOC = 4,				/**< Doesn't take a local memory copy of the TOC (useful for filesystems read from memory) */
	MFMF_OnlyAllowExclusiveAccess = 8,	/**< This will exclude this mount from any non-specific filesystem operations (filenames explicitly directed to this mount using 'device:') */

	MFMF_ForceUInt = 0xFFFFFFFF			/**< Force MountFlags to an int type */
};

/**
 * Mount priority enum.
 * These are some guideline mount priorities. Priorities can really be any number.
 */
enum MFMountPriority
{
	MFMP_VeryHigh = 0,		/**< Very high priority filesystem (first in the search queue) */
	MFMP_AboveNormal = 5,	/**< Above normal priority filesystem */
	MFMP_Normal = 10,		/**< Normal priority filesystem */
	MFMP_BelowNormal = 15,	/**< Below normal priority filesystem */
	MFMP_VeryLow = 20		/**< Very low priority filesystem (last in the search queue) */
};

/**
 * Mount data base structure.
 * Base structure for mount data.
 */
struct MFMountData
{
	int cbSize;					/**< Size of the structure */
	uint32 flags;				/**< Mount flags, this can be values from the MFMountFlags enum */
	const char *pMountpoint;	/**< The mountpoint string (the volume name) */
	int priority;				/**< Filsystem priority when searching for files */
};

/**
 * FileSystem handle enums.
 * These provide direct access to the various available file systems.
 */
enum MFFileSystemHandles
{
	MFFSH_Unknown = -1,			/**< Unknown FileSystem */

	MFFSH_NativeFileSystem = 0,	/**< The native operating system FileSystem */
	MFFSH_MemoryFileSystem,		/**< Memory file FileSystem */
	MFFSH_ZipFileSystem,		/**< Zip file FileSystem */

	MFFSH_Max,					/**< Maximum FileSystem */
	MFFSH_ForceInt = 0x7FFFFFFF	/**< Force FileSystem to an int type */
};

/**
 * Get a handle to a specific filesystem.
 * Gets a handle to a specific filesystem.
 * @param fileSystemHandle Enum of the filesystem to retrieve.
 * @return Returns the FileSystemHandle for the specified filesystem.
 */
MFFileSystemHandle MFFileSystem_GetInternalFileSystemHandle(MFFileSystemHandles fileSystemHandle);

/**
 * Mounts a filesystem.
 * Mounts a filesystem which provides files to Fuji.
 * @param fileSystem Handle to the filesystem which provides access to the data.
 * @param pMountData Pointer to a MFMountData structure filled with all the mount paramaters.
 * @return Returns 0 if filesystem was successfully mounted.
 */
int MFFileSystem_Mount(MFFileSystemHandle fileSystem, MFMountData *pMountData);

/**
 * Dismount a filesystem.
 * Dismounts a mounted filesystem.
 * @param pMountpoint The name of the mountpoint for the filesystem to dismount.
 * @return Returns 0 if the filesystem was successfully dismounted.
 */
int MFFileSystem_Dismount(const char *pMountpoint);

/**
 * Open a file from the mounted filesystem stack.
 * Open a file from the mounted filesystem stack.
 * @param pFilename The name of the file to open.
 * @param openFlags Open file flags.
 * @return Returns a handle to the opened file. Returns NULL if open failed.
 */
MFFileHandle MFFileSystem_Open(const char *pFilename, uint32 openFlags = MFOF_Read|MFOF_Binary);

/**
 * Load a file from the filesystem.
 * Load a file from the filesystem.
 * @param pFilename The name of the file to load.
 * @param pBytesRead Optional pointer to a uint32 that will receive the size of the file loaded.
 * @return Returns a pointer to a new buffer containing the file that was loaded.
 */
char* MFFileSystem_Load(const char *pFilename, uint32 *pBytesRead = NULL);

/**
 * Write a file to a filesystem.
 * Write a file to a filesystem.
 * @param pFilename The name of the file to write. If the target file does not already exist, the filename must include the mountpoint to identify the target filesystem.
 * @param pBuffer Buffer to write to the file.
 * @param size Size of the buffer to write.
 * @return Returns 0 if the file was succesfully written.
 */
int MFFileSystem_Save(const char *pFilename, char *pBuffer, uint32 size);

/**
 * Get the size of a file.
 * Gets the size of a file.
 * @param pFilename The name of the file to find the size.
 * @return Returns the size of the file in bytes. If the file does not exist, MFFileSystem_GetSize returns 0, however, a zero length file will also return 0. Use MFFileSystem_Exists to correctly test if a file exists.
 */
int MFFileSystem_GetSize(const char *pFilename);

/**
 * See if a file is available to the filesystem.
 * Attempts to locate a file in the mounted filesystem stack.
 * @param pFilename The filename to search the filesystem for.
 * @return Returns true if the file can be found within the mounted filesystem stack.
 */
bool MFFileSystem_Exists(const char *pFilename);


/////////////////////////////////////
// implementing a custom filesystem

// this can be exposed to the game later on when the design is more settled

#endif

/** @} */
