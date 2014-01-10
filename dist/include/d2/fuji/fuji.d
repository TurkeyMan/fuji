module fuji.fuji;

public import fuji.types;
public import fuji.dbg;

template MFDeg2Rad(alias a) { enum MFDeg2Rad = 0.017453292519943295769236907684886 * a; }
template MFRad2Deg(alias a) { enum MFRad2Deg = 57.295779513082320876798154814105 * a; }
template MFAlign(alias x, alias bytes) { enum MFAlign = (x + (bytes-1)) & ~(bytes-1); }
template MFAlign16(alias x) { enum MFAlign16 = MFAlign!(x, 16); }
template MFUnflag(alias x, alias y) { enum MFUnflag = x & ~y; }
template MFFlag(alias x, alias y) { enum MFFlag = x | y; }
template MFBit(alias x) { enum MFBit = 1 << x; }
version(LittleEndian)
{
	template MFMakeFourCC(alias ch0, alias ch1, alias ch2, alias ch3)
	{
		enum uint MFMakeFourCC = (cast(uint)cast(ubyte)ch0 | (cast(uint)cast(ubyte)ch1 << 8) | (cast(uint)cast(ubyte)ch2 << 16) | (cast(uint)cast(ubyte)ch3 << 24 ));
	}
}
else version(BigEndian)
{
	template MFMakeFourCC(alias ch0, alias ch1, alias ch2, alias ch3)
	{
		enum uint MFMakeFourCC = (cast(uint)cast(ubyte)ch3 | (cast(uint)cast(ubyte)ch2 << 8) | (cast(uint)cast(ubyte)ch1 << 16) | (cast(uint)cast(ubyte)ch0 << 24 ));
	}
}
else
	static assert("Unknown endian!");

/**
* Defines a Fuji platform at runtime.
* These are generally used to communicate current or target platform at runtime.
*/
enum MFPlatform
{
	Unknown = -1,	/**< Unknown platform */

	PC = 0,			/**< PC */
	XBox,			/**< XBox */
	Linux,			/**< Linux */
	PSP,			/**< Playstation Portable */
	PS2,			/**< Playstation 2 */
	DC,				/**< Dreamcast */
	GC,				/**< Gamecube */
	OSX,			/**< MacOSX */
	Amiga,			/**< Amiga */
	XBox360,		/**< XBox360 */
	PS3,			/**< Playstation 3 */
	Wii,			/**< Nintendo Wii */
	Symbian,		/**< Symbian OS */
	IPhone,			/**< IPhone OS */
	Android,		/**< Android */
	WindowsMobile,	/**< Windows Mobile */
	NativeClient	/**< Native Client (NaCL) */
}

/**
* Defines a platform endian.
* Generally used to communicate current or target platform endian at runtime.
*/
enum MFEndian
{
	Unknown = -1,		/**< Unknown endian */

	LittleEndian = 0,	/**< Little Endian */
	BigEndian			/**< Big Endian */
}

void MFEndian_Flip(T)(T* pData)
{
	static if(is(T == struct))
	{
		foreach(ref m; (*pData).tupleof)
		{
			alias M = typeof(m);
			static if(M.sizeof > 1 && (is(M == struct) || std.traits.isNumeric!M || std.traits.isSomeChar!M))
				MFEndian_Flip(&m);
		}
	}
	else
	{
		T copy = *pData;

		ubyte* pBytes = cast(ubyte*)pData;
		const(ubyte)* pCopy = cast(const(ubyte)*)&copy;
		foreach(a; 0 .. T.sizeof)
			pBytes[a] = pCopy[T.sizeof-1-a];
	}
}

template MFEndian_Flip(alias data)
{
	// TODO: this is tricky!
	static assert("TODO!");
}

version(LittleEndian)
{
	void MFEndian_HostToBig(T)(T* x) { MFEndian_Flip(x); }
	void MFEndian_HostToLittle(T)(T* x) {}
	void MFEndian_LittleToHost(T)(T* x) {}
	void MFEndian_BigToHost(T)(T* x) { MFEndian_Flip(x); }

	template MFEndian_HostToBig(alias x) { alias MFEndian_HostToBig = MFEndian_Flip!x; }
	template MFEndian_HostToLittle(alias x) { alias MFEndian_HostToLittle = x; }
	template MFEndian_LittleToHost(alias x) { alias MFEndian_LittleToHost = x; }
	template MFEndian_BigToHost(alias x) { alias MFEndian_BigToHost = MFEndian_Flip!x; }
}
else version(BigEndian)
{
	void MFEndian_HostToBig(T)(T* x) {}
	void MFEndian_HostToLittle(T)(T* x) { MFEndian_Flip(x); }
	void MFEndian_LittleToHost(T)(T* x) { MFEndian_Flip(x); }
	void MFEndian_BigToHost(T)(T* x) {}

	template MFEndian_HostToBig(alias x) { alias MFEndian_HostToBig = x; }
	template MFEndian_HostToLittle(alias x) { alias MFEndian_HostToLittle = MFEndian_Flip!x; }
	template MFEndian_LittleToHost(alias x) { alias MFEndian_LittleToHost = MFEndian_Flip!x; }
	template MFEndian_BigToHost(alias x) { alias MFEndian_BigToHost = x; }
}
else
	static assert("Unknown endian!");

void MFFixUp(ref void* pPointer, void* pBase, int fix)
{
	if(pPointer)
	{
		ptrdiff_t offset = cast(ptrdiff_t)pBase;

		if(!fix)
			offset = -offset;

		pPointer = cast(void*)(cast(ubyte*)pPointer + offset);
	}
}
