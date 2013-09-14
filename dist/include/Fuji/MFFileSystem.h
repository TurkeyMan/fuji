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

/**
 * @struct MFFile
 * Represents a Fuji file.
 */
struct MFFile;

/**
 * Represents a Fuji FileSystem.
 * Represents a Fuji FileSystem.
 */
typedef int MFFileSystemHandle;

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

	MFOF_CreateDirectory = 16,	/**< Create the directory if it doesn't already exist */

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
enum MFJobState
{
	MFJS_Ready = 0,		/**< Job is ready */
	MFJS_Waiting,		/**< Job is waiting */
	MFJS_Busy,			/**< Job is busy */
	MFJS_Finished,		/**< Job is busy */
};

/**
 * Gets the system file path.
 * Gets the default game data path and optionally appends a filename.
 * @param filename Filename to append to the path. This may be set to NULL.
 * @return Returns the path to the game data directory.
 */
MF_API const char* MFFile_SystemPath(const char *filename = NULL);

/**
 * Gets the system home path.
 * Gets the default game home path and optionally appends a filename.
 * @param filename Filename to append to the path. This may be set to NULL.
 * @return Returns the path to the game home directory.
 * @remarks The home path is typically used for debugging and development purposes (writing logs, screenshots, etc). The game should not require access to the home path.
 */
MF_API const char* MFFile_HomePath(const char *filename = NULL);


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
 * @return Returns a pointer to the newly opened file, returns NULL if the file open failed.
 */
MF_API MFFile* MFFile_Open(MFFileSystemHandle fileSystem, MFOpenData *pOpenData);

/**
 * Close a file.
 * Closes an open file.
 * @param pFile Pointer to an open file.
 * @return Returns 0 if the file was successfully closed.
 */
MF_API int MFFile_Close(MFFile *pFile);

/**
 * Read data from a file.
 * Reads data from a file.
 * @param pFile Pointer to an open file.
 * @param pBuffer Pointer to a buffer where the read data will be stored.
 * @param bytes Number of bytes to read.
 * @param async If true, the read will be performed asyncrenously, putting the file into a 'busy' state.
 * @return Returns the number of bytes read.
 */
MF_API int MFFile_Read(MFFile *pFile, void *pBuffer, size_t bytes, bool async = false);

/**
 * Write to a file.
 * Writes data to a file.
 * @param pFile Pointer to an open file.
 * @param pBuffer Pointer to the data to be written.
 * @param bytes Number of bytes to write.
 * @param async If true, the write will be performed asyncrenously, putting the file into a 'busy' state.
 * @return Returns the number of bytes written.
 */
MF_API int MFFile_Write(MFFile *pFile, const void *pBuffer, size_t bytes, bool async = false);

/**
 * Seek the file.
 * Seek to a soecified file offset.
 * @param pFile Pointer to an open file.
 * @param bytes Number of bytes to seek.
 * @param relativity Member of the MFFileSeek enumerated type where to begin the seek.
 * @return Returns the new file offset in bytes.
 */
MF_API int MFFile_Seek(MFFile *pFile, int bytes, MFFileSeek relativity);

/**
 * Tell the file position.
 * Tells the current file position.
 * @param pFile Pointer to an open file.
 * @return Returns the file pointer offset in bytes.
 */
MF_API int MFFile_Tell(MFFile *pFile);

/**
 * Get the size of a file.
 * Gets the size of a file.
 * @param pFile Pointer to an open file.
 * @return Returns the size of the file in bytes. Returns -1 for a file stream with an undefined length. Returns 0 if the file does not exist.
 */
MF_API int64 MFFile_GetSize(MFFile *pFile);

/**
 * Check for end of file.
 * Check to see if the file has reached its end.
 * @param pFile Pointer to an open file.
 * @return Returns true if the file pointer has reached the end of the file, otherwise false.
 */
MF_API bool MFFile_IsEOF(MFFile *pFile);

// stdio signiture functions (these can be used as callbacks to many libs and API's)

/**
 * Close a file.
 * Closes an open file.
 * @param stream Handle to an open file.
 * @return Returns 0 if the file was successfully closed.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
MF_API int MFFile_StdClose(void* stream);

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
MF_API size_t MFFile_StdRead(void *buffer, size_t size, size_t count, void* stream);

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
MF_API size_t MFFile_StdWrite(const void *buffer, size_t size, size_t count, void* stream);

/**
 * Seek the file.
 * Seek to a soecified file offset.
 * @param stream Handle to an open file.
 * @param offset Number of bytes to seek.
 * @param whence Member of the MFFileSeek enumerated type where to begin the seek.
 * @return Returns the new file offset in bytes.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
MF_API long MFFile_StdSeek(void* stream, long offset, int whence);

/**
 * Tell the file position.
 * Tells the current file position.
 * @param stream Handle to an open file.
 * @return Returns the file pointer offset in bytes.
 * @remarks This function complies with the stdio function signature (can be used as callbacks to many libs and API's).
 */
MF_API long MFFile_StdTell(void* stream);


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
	MFMF_FlattenDirectoryStructure = 1,	/**< Flattens the directory heirarchy */
	MFMF_Recursive = 2,					/**< Recurse into subdirectories when building TOC */
	MFMF_DontCacheTOC = 4,				/**< Doesn't take a local memory copy of the TOC (useful for filesystems read from memory) */
	MFMF_OnlyAllowExclusiveAccess = 8,	/**< This will exclude this mount from any non-specific filesystem operations (filenames explicitly directed to this mount using 'device:') */

	MFMF_ForceUInt = 0xFFFFFFFF			/**< Force MFMountFlags to a uint type */
};

/**
 * Mount priority enum.
 * These are some guideline mount priorities. Priorities can really be any number.
 */
enum MFMountPriority
{
	MFMP_Highest = 0,			/**< Highest priority filesystem (first in the search queue) */
	MFMP_VeryHigh = 1,			/**< Very high priority filesystem */
	MFMP_AboveNormal = 5,		/**< Above normal priority filesystem */
	MFMP_Normal = 10,			/**< Normal priority filesystem */
	MFMP_BelowNormal = 15,		/**< Below normal priority filesystem */
	MFMP_VeryLow = 20,			/**< Very low priority filesystem (last in the search queue) */

	MFMP_ForceInt = 0x7FFFFFFF	/**< Force MFMountPriority to an int type */
};

/**
 * File attributes.
 * These are a set of file attributes.
 */
enum MFFileAttributes
{
	MFFA_Directory = MFBIT(0),	/**< File is a directory */
	MFFA_SymLink = MFBIT(1),	/**< File is a symbolic link */
	MFFA_Hidden = MFBIT(2),		/**< File is hidden */
	MFFA_ReadOnly = MFBIT(3),	/**< File is read only */

	MFFA_ForceInt = 0x7FFFFFFF	/**< Force MFFileAttributes to an int type */
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
	MFFSH_CachedFileSystem,		/**< Cached file FileSystem */
	MFFSH_ZipFileSystem,		/**< Zip file FileSystem */
	MFFSH_HTTPFileSystem,		/**< HTTP file FileSystem */
	MFFSH_FTPFileSystem,		/**< FTP file FileSystem */

	MFFSH_Max,					/**< Maximum FileSystem */
	MFFSH_ForceInt = 0x7FFFFFFF	/**< Force FileSystem to an int type */
};

/**
 * FileSystem find data.
 * Structure used to return information about a file in the filesystem.
 */
struct MFFindData
{
	char pFilename[256];	/**< The files filename */
	char pSystemPath[256];	/**< The system path to the file */
	uint64 fileSize;		/**< The files size */
	uint32 attributes;		/**< The files attributes */
};

/**
 * FileSystem volume info.
 * MFFileSystem volume information.
 */
struct MFVolumeInfo
{
	const char *pVolumeName;		/**< The name of the volume */
	MFFileSystemHandle fileSystem;	/**< The filesystem handle the volume is mounted on */
	uint32 flags;					/**< Volume flags */
	int priority;					/**< Mount priority */
};

/**
 * Get a handle to a specific filesystem.
 * Gets a handle to a specific filesystem.
 * @param fileSystemHandle Enum of the filesystem to retrieve.
 * @return Returns the FileSystemHandle for the specified filesystem.
 */
MF_API MFFileSystemHandle MFFileSystem_GetInternalFileSystemHandle(MFFileSystemHandles fileSystemHandle);

/**
 * Mounts a filesystem.
 * Mounts a filesystem which provides files to Fuji.
 * @param fileSystem Handle to the filesystem which provides access to the data.
 * @param pMountData Pointer to a MFMountData structure filled with all the mount parameters.
 * @return Returns 0 if filesystem was successfully mounted.
 */
MF_API int MFFileSystem_Mount(MFFileSystemHandle fileSystem, MFMountData *pMountData);

/**
 * Mounts a filesystem from a fuji path.
 * Mounts a filesystem which provides files to Fuji from a fully justified Fuji path.
 * @param pMountpoint The volume name.
 * @param pFujiPath Fully justified Fuji path to the volume root.
 * @param priority Filsystem priority when searching for files.
 * @param flags Mount flags, this can be any combination of values from the MFMountFlags enum.
 * @return Returns 0 if filesystem was successfully mounted.
 */
MF_API int MFFileSystem_MountFujiPath(const char *pMountpoint, const char *pFujiPath, int priority = MFMP_Normal, uint32 flags = 0);

/**
 * Dismount a filesystem.
 * Dismounts a mounted filesystem.
 * @param pMountpoint The name of the mountpoint for the filesystem to dismount.
 * @return Returns 0 if the filesystem was successfully dismounted.
 */
MF_API int MFFileSystem_Dismount(const char *pMountpoint);

/**
 * Open a file from the mounted filesystem stack.
 * Open a file from the mounted filesystem stack.
 * @param pFilename The name of the file to open.
 * @param openFlags Open file flags.
 * @return Returns a pointer to the opened file. Returns NULL if open failed.
 */
MF_API MFFile* MFFileSystem_Open(const char *pFilename, uint32 openFlags = MFOF_Read|MFOF_Binary);

/**
 * Load a file from the filesystem.
 * Load a file from the filesystem.
 * @param pFilename The name of the file to load.
 * @param pBytesRead Optional pointer to a uint32 that will receive the size of the file loaded.
 * @param extraBytes Number of extra bytes to allocate at the end of the buffer. Initialises the first extra byte to 0 (useful for appending a NULL byte to the end of text files).
 * @return Returns a pointer to a new buffer containing the file that was loaded.
 */
MF_API char* MFFileSystem_Load(const char *pFilename, size_t *pBytesRead = NULL, size_t extraBytes = 0);

/**
 * Write a file to a filesystem.
 * Write a file to a filesystem.
 * @param pFilename The name of the file to write. If the target file does not already exist, the filename must include the mountpoint to identify the target filesystem.
 * @param pBuffer Buffer to write to the file.
 * @param size Size of the buffer to write.
 * @return Returns 0 if the file was succesfully written.
 */
MF_API int MFFileSystem_Save(const char *pFilename, const char *pBuffer, size_t size);

/**
 * Get the size of a file.
 * Gets the size of a file.
 * @param pFilename The name of the file to find the size.
 * @return Returns the size of the file in bytes. If the file does not exist, MFFileSystem_GetSize returns 0.
 * @remarks If the file does not exist, MFFileSystem_GetSize returns 0, however, a zero length file will also return 0. Use MFFileSystem_Exists to correctly test if a file exists. MFFileSystem_GetSize may also return -1 if the files length is not known, for instance, an endless or unknown length network stream.
 */
MF_API int64 MFFileSystem_GetSize(const char *pFilename);

/**
 * See if a file is available to the filesystem.
 * Attempts to locate a file in the mounted filesystem stack.
 * @param pFilename The filename to search the filesystem for.
 * @return Returns true if the file can be found within the mounted filesystem stack.
 */
MF_API bool MFFileSystem_Exists(const char *pFilename);

/**
 * Resolve the system path to a file.
 * Attempts to resolve the path to a given file on the native filesystem.
 * @param pFilename The filename to resolve.
 * @param bAbsolute Convert to an absolute path.
 * @return Returns the system path to the given file, or NULL if the file doesn't reside on the native filesystem.
 */
MF_API const char* MFFileSystem_ResolveSystemPath(const char *pFilename, bool bAbsolute = false);

/**
 * Get number of available volumes.
 * Gets the number of available volumes.
 * @return The number of mounted volumes.
 */
MF_API int MFFileSystem_GetNumVolumes();

/**
 * Get volume mount details.
 * Gets the details of a mounted volume.
 * @param volumeID Target volume ID.
 * @param pVolumeInfo Pointer to an MFVolumeInfo structre that receives the volumes mount details.
 * @return None.
 */
MF_API void MFFileSystem_GetVolumeInfo(int volumeID, MFVolumeInfo *pVolumeInfo);

/**
 * Begin a find for files.
 * Finds the first file matching a specified search pattern.
 * @param pSearchPattern The search pattern. The search pattern MUST be a full justified fuji path begining with a volume, and ending with a filename pattern to match.
 * @param pFindData Pointer to an MFFindData structure which receives details about the file.
 * @return An MFFind handle that is passed to subsequent calls to MFFileSystem_FindNext and MFFileSystem_FindClose.
 * @remarks Currently the only valid filename pattern is '*'. For example: "data:subdir/ *" is a valid search pattern.
 * @see MFFileSystem_FindNext(), MFFileSystem_FindClose()
 */
MF_API MFFind* MFFileSystem_FindFirst(const char *pSearchPattern, MFFindData *pFindData);

/**
 * Find the next file.
 * Finds the next file in the directory matching the search pattern specified in MFFileSystem_FindFirst().
 * @param pFind MFFind handle returned from a previous call to MFFileSystem_FindFirst().
 * @param pFindData Pointer to an MFFindData structure which receives details about the file.
 * @return Returns true on success or false if there are no more files in the directory.
 * @see MFFileSystem_FindFirst(), MFFileSystem_FindClose()
 */
MF_API bool MFFileSystem_FindNext(MFFind *pFind, MFFindData *pFindData);

/**
 * Close an open find.
 * Closes an open find file process.
 * @param pFind MFFind handle returned from a previous call to MFFileSystem_FindFirst().
 * @return None.
 * @see MFFileSystem_FindFirst()
 */
MF_API void MFFileSystem_FindClose(MFFind *pFind);


/////////////////////////////////////
// helper functions, to make life easier

MF_API MFFile* MFFile_CreateMemoryFile(const void *pMemory, size_t size, bool writable = false, bool ownMemory = false);


/////////////////////////////////////
// implementing a custom filesystem

// this can be exposed to the game later on when the design is more settled

#endif

/** @} */
