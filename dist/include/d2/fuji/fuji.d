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

