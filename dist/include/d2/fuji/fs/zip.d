module fuji.fs.zip;

public import fuji.filesystem;

/**
 * Open a zip archive file structure.
 * Information required to open a file within a zip archive.
 */
struct MFOpenDataZipFile
{
	MFOpenData base = MFOpenData(typeof(this).sizeof, MFOpenData.init.openFlags);
	alias base this;

	MFFile* pZipArchive;		/**< The MFFile handle to an open zip file. This handle can exist in any other filesystem that supports seeking. */
	const(char*) pFilename;	/**< Filename of the file within the zip file to open. */
}

/**
 * Mount zip archive filesystem structure.
 * Information required to mount a zip archive filesystem.
 */
struct MFMountDataZipFile
{
	MFMountData base = MFMountData(typeof(this).sizeof, MFMountData.init.flags, MFMountData.init.pMountpoint, MFMountData.init.priority);
	alias base this;

	MFFile* pZipArchive;	/**< The MFFile handle to an open zip file. This handle can exist in any other filesystem that supports seeking. */
}

/**
 * Open zip file flags enum.
 * Open file flags relating to zip files.
 */
enum MFOpenZipFlags : uint
{
	AlreadyMounted = MFOpenFlags.User	/**< Tells the MFFileZipFile_Open() function that the zip filesystem has already been mounted. */
}

