/**
 * @addtogroup MFFileSystem
 * @{
 */

#if !defined(_MFFILESYSTEM_NATIVE_H)
#define _MFFILESYSTEM_NATIVE_H

#include "Fuji/MFFileSystem.h"

/**
 * Open native file structure.
 * Information required to open a native file.
 */
struct MFOpenDataNative : public MFOpenData
{
	const char *pFilename;	/**< Filename of the file in the native filesystem. */
};

/**
 * Mount native filesystem structure.
 * Information required to mount a native filesystem.
 */
struct MFMountDataNative : public MFMountData
{
	const char *pPath;	/**< Path in the native filsystem to mount. */
};

/** @} */

#endif
