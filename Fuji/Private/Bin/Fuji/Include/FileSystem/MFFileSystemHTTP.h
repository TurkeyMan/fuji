/**
 * @addtogroup MFFileSystem
 * @{
 */

#if !defined(_MFFILESYSTEM_HTTP_H)
#define _MFFILESYSTEM_HTTP_H

#include "MFFileSystem.h"

/**
 * Open HTTP file structure.
 * Information required to open a HTTP file.
 */
struct MFOpenDataHTTP : public MFOpenData
{
	const char *pURL;	/**< URL to the file on a remote HTTP server. */
	int port;			/**< Port to use for HTTP communication. */
};

/**
 * Mount HTTP filesystem structure.
 * Information required to mount a HTTP filesystem.
 */
struct MFMountDataHTTP : public MFMountData
{
};

/** @} */

#endif
