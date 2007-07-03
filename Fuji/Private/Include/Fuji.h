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
		MF_ARCH_PPC
		MF_ARCH_MIPS
		MF_ARCH_SH4
		MF_ARCH_SPU

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


	We'll also define some #define's to tell us which drivers to use.
	At any time, these drivers may also be equal to the 
	Where any of these drivers is left undefined by a platform, NULL will be selected.

	MF_DISPLAY:
		NULL
		WIN32
		X11
		COCOA
		MF_PLATFORM

	MF_RENDERER:
		NULL
		D3D9
		D3D10
		OPENGL
		SDL
		SOFTWARE
		MF_PLATFORM

	MF_SOUND:
		NULL
		DSOUND
		FMOD
		ALSA
		MF_PLATFORM

	MF_HEAP:
		NULL
		WIN32
		CRT
		MF_PLATFORM

	MF_FILESYSTEM:
		NULL
		WIN32
		CRT
		MF_PLATFORM

	MF_THREAD:
		NULL
		WIN32
		CRT
		MF_PLATFORM

	MF_SOCKETS:
		NULL
		WINSOCK
		CRT
		MF_PLATFORM

	MF_INPUT:
	MF_AUXILLARYDISPLAY:
	MF_SYSTEM:
	MF_TRANSLATION:
	MF_DEBUG:
	MF_PARTICLESYSTEM:
		NULL
		MF_PLATFORM
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
		#define MF_ARCH_X86
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
#elif defined(__PPU__)
	#define MF_PS3
	#define MF_PLATFORM PS3
	#define MF_ARCH_PPC
	#define MF_64BIT
#elif defined(__SH4__) || defined(__SH4_SINGLE_ONLY__)
	#define MF_ARCH_SH4
	// shall we assume DC here? yeah, why not... i've never heard of an SH4 in anything else..
	#define MF_DC
	#define MF_PLATFORM DC
	#define MF_32BIT
#elif defined(_MIPS_ARCH) || defined(_mips) || defined(__mips) || defined(__mips__) || defined(__MIPSEL__) || defined(_MIPSEL) || defined(__MIPSEL)
	#define MF_ARCH_MIPS
#elif defined(__ppc) || defined(__powerpc__) || defined(__PowerPC__) || defined(__PPC__) || defined(__ppc__)
	#define MF_ARCH_PPC
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
#if defined(MF_ARCH_PPC)
	#define MF_ENDIAN_BIG
#else
	#define MF_ENDIAN_LITTLE
#endif

// select asm format
#if defined(MF_COMPILER_VISUALC)
	#define MF_ASM_INTEL
#else
	#define MF_ASM_ATNT
#endif


/*** Driver list ***/

#define MF_DRIVER_NULL		0
#define MF_DRIVER_WIN32		1
#define MF_DRIVER_X11		2
#define MF_DRIVER_COCOA		3
#define MF_DRIVER_D3D9		4
#define MF_DRIVER_D3D10		5
#define MF_DRIVER_OPENGL	6
#define MF_DRIVER_SDL		7
#define MF_DRIVER_SOFTWARE	8
#define MF_DRIVER_DSOUND	9
#define MF_DRIVER_FMOD		10
#define MF_DRIVER_ALSA		11
#define MF_DRIVER_CRT		12
#define MF_DRIVER_WINSOCK	13
#define MF_DRIVER_PC		14
#define MF_DRIVER_LINUX		15
#define MF_DRIVER_OSX		16
#define MF_DRIVER_XBOX		17
#define MF_DRIVER_X360		18
#define MF_DRIVER_PSP		19
#define MF_DRIVER_PS2		20
#define MF_DRIVER_PS3		21
#define MF_DRIVER_GC		22
#define MF_DRIVER_DC		23
#define MF_DRIVER_AMIGA		24
#define MF_DRIVER_WII		25


/*** Compiler definitions ***/

#if defined(MF_COMPILER_GCC)
	#define __cdecl __attribute__((__cdecl__))
	#define _cdecl __attribute__((__cdecl__))
#endif

#if defined(MF_COMPILER_VISUALC)
	#pragma warning(disable: 4201)

	// Stop it bitching about the string functions not being secure
	#define _CRT_SECURE_NO_DEPRECATE

	// disable 'unreferenced formal parameter'
	#pragma warning(disable:4100)

	// Disable depreciated bullshit
	#pragma warning(disable:4996)
#endif


/**
 * Defines a Fuji platform at runtime.
 * These are generally used to communicate current or target platform at runtime.
 */
enum MFPlatform
{
	FP_Unknown = -1, /**< Unknown platform */

	FP_PC = 0,	/**< PC */
	FP_XBox,	/**< XBox */
	FP_Linux,	/**< Linux */
	FP_PSP,		/**< Playstation Portable */
	FP_PS2,		/**< Playstation 2 */
	FP_DC,		/**< Dreamcast */
	FP_GC,		/**< Gamecube */
	FP_OSX,		/**< MacOSX */
	FP_Amiga,	/**< Amiga */
	FP_XBox360,	/**< XBox360 */
	FP_PS3,		/**< Playstation 3 */
	FP_Wii,		/**< Nintendo Wii */

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


/*** Platform independant defines and includes ***/

#if !defined(_PS2)
	#define _ENABLE_SCRIPTING
#endif


/*** Platform specific defines, includes and driver selection ***/

#if defined(MF_WINDOWS)

	#include <stdlib.h>

	// This forces ConvertASE and the shaders to use vertex data formats supported by D3D8
	#define SUPPORT_D3D8

	// this defines weather to take mouse[0]'s coords from the windows cursor or from
	// DirectInput's accumulated input events
	#define USE_WINDOWS_MOUSE_COORDS

	// this allows DirectInupt to manage the system mouse
	#define ALLOW_DI_MOUSE

	// this defines that rawinput will be used to recognise more than one mouse connected to the PC
	#define ALLOW_RAW_INPUT

	// add support for XInput (will reserve the first 4 controller slots and support hotswapping if any XInput devices are connected at startup)
	#define SUPPORT_XINPUT

	// add support for the G15 LCD screen
	#define SUPPORT_G15

	// specify drivers
	#define MF_DISPLAY			MF_DRIVER_WIN32
	#define MF_RENDERER			MF_DRIVER_D3D9
	#define MF_SOUND			MF_DRIVER_DSOUND
	#define MF_HEAP				MF_DRIVER_WIN32
	#define MF_FILESYSTEM		MF_DRIVER_WIN32
	#define MF_THREAD			MF_DRIVER_WIN32
	#define MF_SOCKETS			MF_DRIVER_WINSOCK
	#define MF_INPUT			MF_DRIVER_PC
	#define MF_AUXILLARYDISPLAY	MF_DRIVER_PC
	#define MF_SYSTEM			MF_DRIVER_PC
	#define MF_TRANSLATION		MF_DRIVER_PC
	#define MF_DEBUG			MF_DRIVER_PC

#elif defined(MF_XBOX)

	#define DEBUG_KEYBOARD
	#define DEBUG_MOUSE
	#include <xtl.h>

	// Use XG interface to create textures in place
//	#define XB_XGTEXTURES

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_XBOX
	#define MF_RENDERER		MF_DRIVER_XBOX
	#define MF_SOUND		MF_DRIVER_NULL //MF_DRIVER_DSOUND
	#define MF_HEAP			MF_DRIVER_XBOX
	#define MF_FILESYSTEM	MF_DRIVER_WIN32
	#define MF_THREAD		MF_DRIVER_WIN32
	#define MF_SOCKETS		MF_DRIVER_WINSOCK
	#define MF_INPUT		MF_DRIVER_XBOX
	#define MF_SYSTEM		MF_DRIVER_XBOX
	#define MF_TRANSLATION	MF_DRIVER_XBOX

#elif defined(MF_X360)

//	#error Not Supported...
	#include <xtl.h>

	#define MFBIG_ENDIAN
	#define MF64BIT

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_X360
	#define MF_RENDERER		MF_DRIVER_X360
	#define MF_SOUND		MF_DRIVER_X360
	#define MF_HEAP			MF_DRIVER_WIN32
	#define MF_FILESYSTEM	MF_DRIVER_WIN32
	#define MF_THREAD		MF_DRIVER_WIN32
	#define MF_SOCKETS		MF_DRIVER_WINSOCK
	#define MF_INPUT		MF_DRIVER_X360
	#define MF_SYSTEM		MF_DRIVER_X360
	#define MF_TRANSLATION	MF_DRIVER_X360

#elif defined(MF_LINUX)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

//	#define _OPENGL_CLIP_SPACE

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_X11
	#define MF_RENDERER		MF_DRIVER_OPENGL
	#define MF_INPUT		MF_DRIVER_LINUX
	#define MF_SYSTEM		MF_DRIVER_X11
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX

#elif defined(MF_OSX)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

//	#define _OPENGL_CLIP_SPACE

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_X11
	#define MF_RENDERER		MF_DRIVER_OPENGL
	#define MF_SYSTEM		MF_DRIVER_X11
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX

#elif defined(MF_PSP)

	#include <stdarg.h>
	#include <stdlib.h>

//	#define _USER_MODE
//	#define _PSP_GDB

	#if !defined(_USER_MODE)
//		#define _ENABLE_USB
	#endif

//	#define _OPENGL_CLIP_SPACE

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_PSP
	#define MF_RENDERER		MF_DRIVER_PSP
	#define MF_SOUND		MF_DRIVER_PSP
	#define MF_HEAP			MF_DRIVER_PSP
	#define MF_FILESYSTEM	MF_DRIVER_PSP
	#define MF_THREAD		MF_DRIVER_PSP
	#define MF_SOCKETS		MF_DRIVER_PSP
	#define MF_INPUT		MF_DRIVER_PSP
	#define MF_SYSTEM		MF_DRIVER_PSP
	#define MF_TRANSLATION	MF_DRIVER_PSP
	#define MF_DEBUG		MF_DRIVER_PSP

#elif defined(MF_PS2)

	#include <stdarg.h>
	#include <stdlib.h>

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_PS2
	#define MF_RENDERER		MF_DRIVER_PS2
	#define MF_INPUT		MF_DRIVER_PS2
	#define MF_SYSTEM		MF_DRIVER_PS2

#elif defined(MF_DC)

	#define _arch_dreamcast
	#include <kos.h>
	#include <stdarg.h> // For varargs
	#include <stdlib.h>

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_DC
	#define MF_RENDERER		MF_DRIVER_DC
	#define MF_INPUT		MF_DRIVER_DC
	#define MF_SYSTEM		MF_DRIVER_DC

#elif defined(MF_GC)

	#include <stdarg.h>
	#include <stdlib.h>

	// angel script needs max portability...
	#define MAX_PORTABILITY

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_GC
	#define MF_RENDERER		MF_DRIVER_GC
	#define MF_INPUT		MF_DRIVER_GC
	#define MF_SYSTEM		MF_DRIVER_GC

#endif

#if defined(_FUJI_UTIL)

	#include <stdio.h>
	#include <stdlib.h>
	#include <new>

#endif

// use NULL devices where specific devices aren't specified for a particular platform...
#if !defined(MF_DISPLAY)
	#define MF_DISPLAY NULL
#endif
#if !defined(MF_RENDERER)
	#define MF_RENDERER NULL
#endif
#if !defined(MF_SOUND)
	#define MF_SOUND NULL
#endif
#if !defined(MF_HEAP)
	#define MF_HEAP NULL
#endif
#if !defined(MF_FILESYSTEM)
	#define MF_FILESYSTEM NULL
#endif
#if !defined(MF_THREAD)
	#define MF_THREAD NULL
#endif
#if !defined(MF_SOCKETS)
	#define MF_SOCKETS NULL
#endif
#if !defined(MF_INPUT)
	#define MF_INPUT NULL
#endif
#if !defined(MF_AUXILLARYDISPLAY)
	#define MF_AUXILLARYDISPLAY NULL
#endif
#if !defined(MF_SYSTEM)
	#define MF_SYSTEM NULL
#endif
#if !defined(MF_TRANSLATION)
	#define MF_TRANSLATION NULL
#endif
#if !defined(MF_DEBUG)
	#define MF_DEBUG NULL
#endif
#if !defined(MF_PARTICLESYSTEM)
	#define MF_PARTICLESYSTEM NULL
#endif

// enable this define to allow the NULL drivers to operate using the standard CRT where appropriate
#define _USE_CRT_FOR_NULL_DRIVERS

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
#define MFMAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) | ((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))


/*** Additional includes ***/

#include "MFString.h"
#include "MFMath.h"
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
