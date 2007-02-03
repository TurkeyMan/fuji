/**
 * @addtogroup MFFileSystem
 * @{
 */

#if !defined(_MFFILESYSTEM_ZIPFILE_H)
#define _MFFILESYSTEM_ZIPFILE_H

#include "MFFileSystem.h"

/**
 * Open a zip archive file structure.
 * Information required to open a file within a zip archive.
 */
struct MFOpenDataZipFile : public MFOpenData
{
	MFFileHandle zipArchiveHandle;	/**< The MFFile handle to an open zip file. This handle can exist in any other filesystem that supports seeking. */
	const char *pFilename;			/**< Filename of the file within the zip file to open. */
};

/**
 * Mount zip archive filesystem structure.
 * Information required to mount a zip archive filesystem.
 */
struct MFMountDataZipFile : public MFMountData
{
	MFFileHandle zipArchiveHandle;	/**< The MFFile handle to an open zip file. This handle can exist in any other filesystem that supports seeking. */
};

/**
 * Open zip file flags enum.
 * Open file flags relating to zip files.
 */
enum MFOpenZipFlags
{
	MFOF_Zip_AlreadyMounted = MFOF_User	/**< Tells the MFFileZipFile_Open() function that the zip filesystem has already been mounted. */
};

/** @} */

#endif
