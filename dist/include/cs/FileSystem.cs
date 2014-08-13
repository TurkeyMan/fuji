using System;
using System.Runtime.InteropServices;

namespace Fuji
{
	struct MFFileTime
	{
		ulong ticks;
	}

	struct MFFileInfo
	{
		ulong size;
		uint attributes;
		MFFileTime writeTime;
		MFFileTime accessTime;
	}
}
