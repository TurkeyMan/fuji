module fuji.fs.http;

public import fuji.filesystem;

/**
 * HTTP 'POST' arguments.
 * Arguments passed to the HTTP server with 'POST' requests.
 */
struct MFFileHTTPRequestArg
{
	enum ArgType
	{
		Int,
		Float,
		String
	}

	const(char)* pArg;
	ArgType type;
	union
	{
		int iValue;
		float fValue;
		const(char)* pValue;
	}

	void SetInt(const(char*) pName, int i)
	{
		pArg = pName;
		type = ArgType.Int;
		iValue = i;
	}

	void SetFloat(const(char*) pName, float f)
	{
		pArg = pName;
		type = ArgType.Float;
		fValue = f;
	}

	void SetString(const(char*) pName, const(char*) pString)
	{
		pArg = pName;
		type = ArgType.String;
		pValue = pString;
	}
}

/**
 * Open HTTP file structure.
 * Information required to open a HTTP file.
 */
struct MFOpenDataHTTP
{
	MFOpenData base = MFOpenData(typeof(this).sizeof, MFOpenData.init.openFlags);
	alias base this;

	const(char*) pURL;					/**< URL to the file on a remote HTTP server. */
	int port = 80;						/**< Port to use for HTTP communication. */

	MFFileHTTPRequestArg* pPostArgs;	/**< Array of arguments for POST rewuests. */
	int numPostArgs;					/**< Number of POST arguments. */
}

/**
 * Mount HTTP filesystem structure.
 * Information required to mount a HTTP filesystem.
 */
struct MFMountDataHTTP
{
	MFMountData base = MFMountData(typeof(this).sizeof, MFMountData.init.flags, MFMountData.init.pMountpoint, MFMountData.init.priority);
	alias base this;
}

