module fuji.fs.native;

public import fuji.filesystem;

/**
 * Open native file structure.
 * Information required to open a native file.
 */
struct MFOpenDataNative
{
	MFOpenData base = MFOpenData(typeof(this).sizeof, MFOpenData.init.openFlags);
	alias base this;

	const(char)* pFilename;	/**< Filename of the file in the native filesystem. */
}

/**
 * Mount native filesystem structure.
 * Information required to mount a native filesystem.
 */
struct MFMountDataNative
{
	MFMountData base = MFMountData(typeof(this).sizeof, MFMountData.init.flags, MFMountData.init.pMountpoint, MFMountData.init.priority);
	alias base this;

	const(char)* pPath;	/**< Path in the native filsystem to mount. */
}

