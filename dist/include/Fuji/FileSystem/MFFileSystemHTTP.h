/**
 * @addtogroup MFFileSystem
 * @{
 */

#if !defined(_MFFILESYSTEM_HTTP_H)
#define _MFFILESYSTEM_HTTP_H

#include "Fuji/MFFileSystem.h"

/**
 * HTTP 'POST' arguments.
 * Arguments passed to the HTTP server with 'POST' requests.
 */
struct MFFileHTTPRequestArg
{
	enum ArgType
	{
		AT_Int,
		AT_Float,
		AT_String
	};

	const char *pArg;
	ArgType type;
	union
	{
		int iValue;
		float fValue;
		const char *pValue;
	};

	void SetInt(const char *pName, int i)
	{
		pArg = pName;
		type = AT_Int;
		iValue = i;
	}

	void SetFloat(const char *pName, float f)
	{
		pArg = pName;
		type = AT_Float;
		fValue = f;
	}

	void SetString(const char *pName, const char *pString)
	{
		pArg = pName;
		type = AT_String;
		pValue = pString;
	}
};

/**
 * Open HTTP file structure.
 * Information required to open a HTTP file.
 */
struct MFOpenDataHTTP : public MFOpenData
{
	const char *pURL;					/**< URL to the file on a remote HTTP server. */
	int port;							/**< Port to use for HTTP communication. */

	MFFileHTTPRequestArg *pPostArgs;	/**< Array of arguments for POST rewuests. */
	int numPostArgs;					/**< Number of POST arguments. */
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
