module fuji.fs.cached;

public import fuji.filesystem;

/**
 * Open cached file structure.
 * Information required to open cached file.
 */
struct MFOpenDataCachedFile
{
	MFOpenData base = MFOpenData(typeof(this).sizeof, MFOpenData.init.openFlags);
	alias base this;

	MFFile* pBaseFile;		/**< The MFFile handle to an open file. This handle can exist in any other filesystem. */
	int maxCacheSize;		/**< Maximum size of the file cache. */
}

/**
 * Open cached file flags enum.
 * Open file flags relating to cached files.
 */
enum MFOpenCachedFlags : uint
{
	CleanupBaseFile = MFOpenFlags.User	/**< Tells the MFFileCachedFile_Open() function that the cached file owns the pointer to the base file and it should clean it up when its done. */
}

