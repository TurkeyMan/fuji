/**
 * @file Fuji.h
 * @brief Mount Fuji Engine main include file.
 * This is the main include file for the Mount Fuji Engine.
 * This file MUST be included before any other includes.
 * @author Manu Evans
 * @defgroup Fuji General
 * @{
 */

#if !defined(_FUJI_H)
#define _FUJI_H

/*
	We'll define a few #define's to tell us about the target architecture:

	Compilers:
		MF_COMPILER_VISUALC
		MF_COMPILER_GCC
		MF_COMPILER_SN
		MF_COMPILER_CODEWARRIOR

	Assembler Syntax:
		MF_ASM_INTEL
		MF_ASM_ATNT

	Architectures:
		MF_ARCH_X86
		MF_ARCH_X64
		MF_ARCH_ITANIUM
		MF_ARCH_PPC
		MF_ARCH_MIPS
		MF_ARCH_SH4
		MF_ARCH_SPU
		MF_ARCH_ARM
		MF_ARCH_68K

	Endian:
		MF_ENDIAN_LITTLE
		MF_ENDIAN_BIG

	Data word:
		MF_32BIT
		MF_64BIT

	Platform:
		MF_WINDOWS
		MF_LINUX
		MF_OSX
		MF_XBOX
		MF_X360
		MF_PSP
		MF_PS2
		MF_PS3
		MF_GC
		MF_WII
		MF_DC
		MF_AMIGA
		MF_SYMBIAN
		MF_IPHONE
		MF_ANDROID
*/

// detect compiler
#if defined(_MSC_VER)
	#define MF_COMPILER_VISUALC
#elif defined(__GNUC__)
	#define MF_COMPILER_GCC
#else
	#error Unrecognised compiler. Contact Fuji dev team or add an entry here...
#endif

// detect architecture/platform
#if defined(MF_COMPILER_VISUALC)
	// if we're using an MS compiler, we must be building windows or xbox
	#if defined(_XBOX)
		// detect xbox version
		#if _XBOX_VER < 200
			#define MF_XBOX
			#define MF_PLATFORM XBOX
			#define MF_ARCH_X86
			#define MF_32BIT
		#elif _XBOX_VER >= 200
			#define MF_X360
			#define MF_PLATFORM X360
			#define MF_ARCH_PPC
			#define MF_64BIT
		#else
			#error XBox version undefined...
		#endif
	#else
		#define MF_WINDOWS
		#define MF_PLATFORM WINDOWS

		// detect the architecture
		#if defined(_M_IA64)
			#define MF_64BIT
			#define MF_ARCH_ITANIUM
		#elif defined(_M_X64) || defined(_M_AMD64)
			#define MF_64BIT
			#define MF_ARCH_X64
		#else
			#define MF_32BIT
			#define MF_ARCH_X86
		#endif
	#endif
#elif defined(PSP) || defined(__psp__) || defined(__PSP__) || defined(_PSP)
	#define MF_PSP
	#define MF_PLATFORM PSP
	#define MF_ARCH_MIPS
	#define MF_32BIT
#elif defined(_EE_) || defined(_EE) || defined(_R5900) || defined(__R5900)
	#define MF_PS2
	#define MF_PLATFORM PS2
	#define MF_ARCH_MIPS
	#define MF_32BIT
#elif defined(_GC)
	#define MF_GC
	#define MF_PLATFORM GC
	#define MF_ARCH_PPC
#elif defined(__wii__) || defined(_WII)
	#define MF_WII
	#define MF_PLATFORM WII
	#define MF_ARCH_PPC
#elif defined(_DC)
	#define MF_DC
	#define MF_PLATFORM DC
	#define MF_ARCH_SH4
#elif defined(__PPU__)
	#define MF_PS3
	#define MF_PLATFORM PS3
	#define MF_ARCH_PPC
	#define MF_64BIT
#elif defined(TARGET_OS_IPHONE)
	#include <TargetConditionals.h>
	#define MF_IPHONE
	#define MF_PLATFORM IPHONE
	#if TARGET_IPHONE_SIMULATOR == 1
		#define MF_ARCH_X86
		#define MF_ENDIAN_LITTLE
		#define MF_32BIT
	#else
		#define MF_ARCH_ARM
		#define MF_ENDIAN_LITTLE
		#define MF_32BIT
	#endif
#elif defined(ANDROID_NDK) || defined(__ANDROID__) || defined(ANDROID)
	#define MF_ANDROID
	#define MF_PLATFORM ANDROID
	#if defined(__arm__)
		#define MF_ARCH_ARM
		#define MF_ENDIAN_LITTLE
		#define MF_32BIT
	#else
		#error "Unsupported Android Architecture!"
	#endif
#elif defined(_SYMBIAN)
	#define MF_SYMBIAN
	#define MF_PLATFORM SYMBIAN
	#define MF_ARCH_ARM
	#define MF_ENDIAN_LITTLE
	#define MF_32BIT
#elif defined(__SH4__) || defined(__SH4_SINGLE_ONLY__)
	#define MF_ARCH_SH4
	// shall we assume DC here? yeah, why not... i've never heard of an SH4 in anything else..
	#define MF_DC
	#define MF_PLATFORM DC
	#define MF_32BIT
#elif defined(_NACL)
	#define MF_NACL
	#define MF_PLATFORM NACL
	#if defined(__x86_64__)
		#define MF_ARCH_X64
		#define MF_64BIT
	#elif defined(__i386__)
		#define MF_ARCH_X86
		#define MF_32BIT
	#else
		#error "Unknown architecture?!"
	#endif
#elif defined(__arm__)
	#define MF_ARCH_ARM
#elif defined(_MIPS_ARCH) || defined(_mips) || defined(__mips) || defined(__mips__) || defined(__MIPSEL__) || defined(_MIPSEL) || defined(__MIPSEL)
	#define MF_ARCH_MIPS
#elif defined(__ppc64__)
	#define MF_ARCH_PPC
	#define MF_64BIT
#elif defined(__ppc) || defined(__powerpc__) || defined(__PowerPC__) || defined(__PPC__) || defined(__ppc__)
	#define MF_ARCH_PPC
#elif defined(__x86_64__)
	#define MF_ARCH_X64
	#define MF_64BIT
#elif defined(__i386__) || defined(_M_IX86)
	#define MF_ARCH_X86
#else
	// assume x86 if we couldnt identify an architecture, since its the most likely
	#define MF_ARCH_X86
#endif

// if the architecture or platform didn't specify a data word size, try and detect one
#if !defined(MF_32BIT) && !defined(MF_64BIT)
	// detect 64bit
	#define MF_32BIT
#endif

// if the architecture didn't specify a platform, try and detect one
#if !defined(MF_PLATFORM)
	// check OSX, assume linux for now
	#define MF_LINUX
	#define MF_PLATFORM LINUX
#endif

// select architecture endian
#if !defined(MF_ENDIAN_BIG) && !defined(MF_ENDIAN_LITTLE)
	#if defined(MF_ARCH_PPC) || defined(MF_ARCH_SPU) || defined(MF_ARCH_68K)
		#define MF_ENDIAN_BIG
	#else
		#define MF_ENDIAN_LITTLE
	#endif
#endif

// select asm format
#if defined(MF_COMPILER_VISUALC)
	#define MF_ASM_INTEL
#else
	#define MF_ASM_ATNT
#endif

/*** Compiler definitions ***/

#if defined(MF_COMPILER_GCC)
	#define __cdecl __attribute__((__cdecl__))
	#define _cdecl __attribute__((__cdecl__))
#endif

#if defined(MF_COMPILER_VISUALC)
	#pragma warning(disable: 4201)

	// Stop it bitching about the string functions not being secure
	#if !defined(_CRT_SECURE_NO_DEPRECATE)
		#define _CRT_SECURE_NO_DEPRECATE
	#endif

	// disable 'unreferenced formal parameter'
	#pragma warning(disable:4100)

	// Disable depreciated bullshit
	#pragma warning(disable:4996)

	// Disable C-linkage returning UDT (user data type)
	#pragma warning(disable:4190)
#endif


/*** Shared object/DLL support ***/

#if defined(MF_COMPILER_VISUALC)
	#define MF_EXPORT __declspec(dllexport)
	#define MF_EXPORT_VARIABLE __declspec(dllexport)
#else
	#define MF_EXPORT
	#define MF_EXPORT_VARIABLE
#endif

#if defined(MF_SHAREDLIB)
	#define MF_API extern "C" MF_EXPORT
#else
	#define MF_API extern "C"
#endif


/**
 * Defines a Fuji platform at runtime.
 * These are generally used to communicate current or target platform at runtime.
 */
enum MFPlatform
{
	FP_Unknown = -1, /**< Unknown platform */

	FP_PC = 0,			/**< PC */
	FP_XBox,			/**< XBox */
	FP_Linux,			/**< Linux */
	FP_PSP,				/**< Playstation Portable */
	FP_PS2,				/**< Playstation 2 */
	FP_DC,				/**< Dreamcast */
	FP_GC,				/**< Gamecube */
	FP_OSX,				/**< MacOSX */
	FP_Amiga,			/**< Amiga */
	FP_XBox360,			/**< XBox360 */
	FP_PS3,				/**< Playstation 3 */
	FP_Wii,				/**< Nintendo Wii */
	FP_Symbian,			/**< Symbian OS */
	FP_IPhone,			/**< IPhone OS */
	FP_Android,			/**< Android */
	FP_WindowsMobile,	/**< Windows Mobile */
	FP_NativeClient,	/**< Native Client (NaCL) */

	FP_Max,		/**< Max platform */
	FP_ForceInt = 0x7FFFFFFF /**< Force the enum to an int */
};

/**
 * Defines a platform endian.
 * Generally used to communicate current or target platform endian at runtime.
 */
enum MFEndian
{
	MFEndian_Unknown = -1,		/**< Unknown endian */

	MFEndian_LittleEndian = 0,	/**< Little Endian */
	MFEndian_BigEndian,			/**< Big Endian */

	MFEndian_ForceInt = 0x7FFFFFFF	/**< Force the enum to an int */
};

/*** Driver selection and platform definitions ***/

#include "MFDriver.h"

/*** Locale Options ***/

// these defines alter the rhobustness vs speed trade off of the string functions.
#if !defined(MFLOCALE_ENGLISH_ONLY) && !defined(MFLOCALE_BASIC_LATIN) && !defined(MFLOCALE_BASIC_LATIN_CRYLLIC)
	// choose one below
//	#define MFLOCALE_ENGLISH_ONLY
//	#define MFLOCALE_BASIC_LATIN
	#define MFLOCALE_BASIC_LATIN_GREEK_CRYLLIC
#endif

#define MFLOCALE_UTF8_SUPPORT
#define MFLOCALE_UNICODE_SUPPORT


/*** Compiler compatibility macros ***/

#if defined(MF_COMPILER_VISUALC)
	#define MFALIGN_BEGIN(n) __declspec(align(n))
	#define MFALIGN_END(n)
#elif defined(MF_COMPILER_GCC)
	#define MFALIGN_BEGIN(n)
	#define MFALIGN_END(n) __attribute__((aligned(n)))
#else
	#define MFALIGN_BEGIN(n)
	#define MFALIGN_END(n)
#endif

#if defined(MF_COMPILER_GCC)
	#define MFPACKED __attribute__((packed))
#else
	#define MFPACKED
#endif


/*** Fuji Types ***/

#include "MFTypes.h"


/*** Defines and macros ***/

#if !defined(NULL) /* In case stdlib.h hasn't been included */
#define NULL 0
#endif

#define MFPI 3.141592653589f
#define MFALMOST_ZERO 0.000001f
#define MFDEGREES(a) (0.017453292519943295769236907684886f * (a))
#define MFRADIANS(a) (57.295779513082320876798154814105f * (a))
#define MFALIGN(x, bytes) ((((size_t)(x))+((bytes)-1)) & ~((bytes)-1))
#define MFALIGN16(x) MFALIGN(x, 16)
#define MFUNFLAG(x, y) ((x)&=~(y))
#define MFFLAG(x, y) ((x)|=(y))
#define MFBIT(x) (1<<(x))
#if defined(MF_ENDIAN_BIG)
#define MFMAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32)(uint8)(ch3) | ((uint32)(uint8)(ch2) << 8) | ((uint32)(uint8)(ch1) << 16) | ((uint32)(uint8)(ch0) << 24 ))
#else
#define MFMAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) | ((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))
#endif


/*** Additional includes ***/

#include "MFModule.h"

#include "MFString.h"
#include "MFMath.h"
#include "MFVector.h"
#include "MFDebug.h"

#include "Util.h"


/*** Callstack profiling ***/

#define _ENABLE_CALLSTACK_PROFILING

#include "MFCallstack.h"


#endif // _FUJI_H

/** @} */

/**
 * @mainpage Mount Fuji Engine Users Manual
 *
 * @section intro_sec Introduction
 *
 * Mount Fuji is a cross platform game engine which aims to provide a set of features and tools which can be used to radically simplify the development process of games on modern hardware.\n
 * Mount Fuji provides a rich API which supports most flavours of PC and modern Consoles. While Mount Fuji is internally developed in C/C++ and Assembly, it is accessible by a variety of languages as a statically and dynamically linkable library.\n
 * The Mount Fuji engine also provides a set of data and asset management tools designed to take game assets developed in a wide variety of art and asset packages in game. Assets are converted to internal platform specific data formats to maximise efficiency and performance on all available platforms.\n
 * The Mount Fuji engine is currently only available to internal developers and beta testers, but a public release is planned when development reaches a stable state.\n
 * For related questions or discussion, please refer to the Mount Fuji Forum.
 *
 * @section faq_sec FAQ
 *
 * @subsection question1 What platforms are currently supported by the Mount Fuji Engine?
 *
 * The Mount Fuji Engine has been successfully compiled and tested on the following platforms: Windows, Linux, PSP and XBox.\n
 * Other platforms in the works at various degrees of functionality include OSX, PS2, Gamecube and Dreamcast. (I'd appreciate more people to help me test these system)\n
 * System planned for support include PS3 (when it releases in australia using the OpenPlatform technology) and Wii (also via gamecube support).
 *
 * @subsection question2 What support is offered for internationalisation?
 *
 * The Mount Fuji Engine provides extensive support for internationalisation. The font functions are UTF8 and Unicode aware, and the engine has internal support for Unicode string, or more conveniently UTF8 encoding when using the standard string functions.\n
 * Tools are provided for managing translations and string tables.
 *
 * @subsection question3 What asset formats are supported?
 *
 * Asset files in typical formats are not directly loaded by the engine.\n
 * The engine is designed to load raw optimised platform specific asset formats. This is for efficiency and performance.\n
 * Tools are provided to convert many common asset formats into 'data archives' for consumption by the engine.\n
 * Supported formats include:\n
 * DAE (Collada), X, ASE, OBJ, MD2, MD3. Note: Animation is currently only supported through the DAE and X formats.\n
 * PNG, TGA, BMP.\n
 * WAV, OGG.
 *
 * @subsection question4 Are sample applications provided?
 *
 * Yes, samples are provided for some typical tasks.\n
 * Further questions/support can be brought to me directly.
 *
 * @subsection question5 Is this gonna be able to make pron games on PSP?
 *
 * Sure, if thats what does it for you ;)
 *
 * @section credits_sec Special Thanks
 *
 * Special thanks go to all the console homebrew communities who have produced fantastic tools for many platforms and found numerous exploits to make any of it even possible!\n
 * The zlib and libPNG teams, yes i use zlib :)\n
 * Andreas Jonsson from AngelCode (http://www.angelcode.com) for providing AngelScript, an awesome cross platform scripting language and his BMFont tool which is used to generate Fuji Fonts.
 *
 */
