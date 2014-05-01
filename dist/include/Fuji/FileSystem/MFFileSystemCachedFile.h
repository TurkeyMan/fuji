/**
 * @addtogroup MFFileSystem
 * @{
 */

#if !defined(_MFFILESYSTEM_CACHEDFILE_H)
#define _MFFILESYSTEM_CACHEDFILE_H

#include "Fuji/MFFileSystem.h"

/**
 * Open cached file structure.
 * Information required to open cached file.
 */
struct MFOpenDataCachedFile : public MFOpenData
{
	MFFile *pBaseFile;		/**< The MFFile handle to an open file. This handle can exist in any other filesystem. */
	size_t maxCacheSize;	/**< Maximum size of the file cache. */
};

/**
 * Open cached file flags enum.
 * Open file flags relating to cached files.
 */
enum MFOpenCachedFlags
{
	MFOF_Cached_CleanupBaseFile = MFOF_User	/**< Tells the MFFileCachedFile_Open() function that the cached file owns the pointer to the base file and it should clean it up when its done. */
};

/** @} */

#endif
