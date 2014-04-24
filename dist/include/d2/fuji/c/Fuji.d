module fuji.c.Fuji;

public import fuji.c.MFTypes;
public import fuji.c.MFString;
// HACK: if we keep this, D's anti-hijacking falls over when we create overloads in fuji.dbg... :/
//public import fuji.c.MFDebug;

nothrow:

struct MFEngineInstance;

extern (C) MFEngineInstance* Fuji_CreateEngineInstance();
extern (C) void Fuji_DestroyEngineInstance(MFEngineInstance* pEngineInstance = null);

extern (C) MFEngineInstance* Fuji_GetCurrentEngineInstance();
extern (C) MFEngineInstance* Fuji_SetCurrentEngineInstance(MFEngineInstance* pEngineInstance);

enum MFDeg2Rad(alias a) = 0.017453292519943295769236907684886 * a;
enum MFRad2Deg(alias a) = 57.295779513082320876798154814105 * a;
enum MFAlign(alias x, alias bytes) = (x + (bytes-1)) & ~(bytes-1);
enum MFAlign16(alias x) = MFAlign!(x, 16);
enum MFUnflag(alias x, alias y) = x & ~y;
enum MFFlag(alias x, alias y) = x | y;
enum MFBit(alias x) = 1 << x;
version(LittleEndian)
	enum MFMakeFourCC(alias ch0, alias ch1, alias ch2, alias ch3) = (cast(uint)cast(ubyte)ch0 | (cast(uint)cast(ubyte)ch1 << 8) | (cast(uint)cast(ubyte)ch2 << 16) | (cast(uint)cast(ubyte)ch3 << 24 ));
else version(BigEndian)
	enum MFMakeFourCC(alias ch0, alias ch1, alias ch2, alias ch3) = (cast(uint)cast(ubyte)ch3 | (cast(uint)cast(ubyte)ch2 << 8) | (cast(uint)cast(ubyte)ch1 << 16) | (cast(uint)cast(ubyte)ch0 << 24 ));
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
