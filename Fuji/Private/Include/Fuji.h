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

/*** Compiler definitions ***/

#if defined(__GNUC__)
#define __cdecl __attribute__((__cdecl__))
#define _cdecl __attribute__((__cdecl__))
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4201)
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

#define _ENABLE_SCRIPTING

/*** Platform specific defines and includes ***/

#if defined(_WINDOWS)

	// Stop it bitching about the string functions not being secure
	#define _CRT_SECURE_NO_DEPRECATE

	#include <stdlib.h>

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

	// disable 'unreferenced formal parameter'
	#pragma warning(disable:4100)

	// Disable depreciated bullshit
	#pragma warning(disable:4996)

#elif (defined(_XBOX) && _XBOX_VER < 200)

	#define _MFXBOX

	#define DEBUG_KEYBOARD
	#define DEBUG_MOUSE
	#include <xtl.h>

	char*  FixXBoxFilename(const char *pFilename);

#elif (defined(_XBOX) && _XBOX_VER >= 200)

	#define _MFX360

//	#error Not Supported...
	#include <xtl.h>

	#define MFBIG_ENDIAN
	#define MF64BIT

#elif defined(_LINUX)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

	#if defined(SOME_LINUX_64BIT_FLAG)
		#define MF64BIT
		#define MF64BITPOINTERS
	#endif

	#define _OPENGL_CLIP_SPACE

#elif defined(_OSX)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

	#define _OPENGL_CLIP_SPACE

#elif defined(_PSP)

	#include <stdarg.h>
	#include <stdlib.h>

//	#define _USER_MODE
//	#define _PSP_GDB

	#if !defined(_USER_MODE)
//		#define _ENABLE_USB
	#endif

//	#define _OPENGL_CLIP_SPACE

#elif defined(_PS2)

	#include <stdarg.h>
	#include <stdlib.h>

#elif defined(_DC)

	#define _arch_dreamcast
	#include <kos.h>
	#include <stdarg.h> // For varargs
	#include <stdlib.h>

#elif defined(_GC)

	#include <stdarg.h>
	#include <stdlib.h>

	#define MFBIG_ENDIAN

	// angel script needs max portability...
	#define MAX_PORTABILITY

#endif

#if defined(_FUJI_UTIL)

	#include <stdio.h>
	#include <stdlib.h>
	#include <new>

#endif


/*** Compiler compatibility macros ***/

#if defined(_MSC_VER)
	#define MFALIGN_BEGIN(n) __declspec(align(n))
	#define MFALIGN_END(n)
#elif defined(__GNUC__)
	#define MFALIGN_BEGIN(n)
	#define MFALIGN_END(n) __attribute__((aligned(n)))
#else
	#define MFALIGN_BEGIN(n)
	#define MFALIGN_END(n)
#endif

#if defined(__GNUC__)
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
#define MFALIGN(x, bytes) ((((unsigned int)(x))+((bytes)-1)) & ~((bytes)-1))
#define MFALIGN16(x) MFALIGN(x, 16)
#define MFUNFLAG(x, y) ((x)&=~(y))
#define MFFLAG(x, y) ((x)|=(y))
#define MFBIT(x) (1<<(x))
#define MFMAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) | ((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))


/*** Additional includes ***/

#include "MFMath.h"
#include "MFString.h"
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
 * @subsection step1 What platforms are currently supported by the Mount Fuji Engine?
 *
 * The Mount Fuji Engine has been successfully compiled and tested on the following platforms: Windows, Linux, PSP and XBox.\n
 * Other platforms in the works at various degrees of functionality include OSX, PS2, Gamecube and Dreamcast. (I'd appreciate more people to help me test these system)
 *
 * @section credits_sec Special Thanks
 *
 * Special thanks go to all the console homebrew communities who have produced fantastic tools for many platforms and found numerous exploits to make any of it even possible!\n
 * The zlib team, yes i use zlib :)\n
 * Andreas Jönsson from AngelCode (http://www.angelcode.com) for providing AngelScript, an awesome cross platform scripting language and his BMFont tool which is used to generate Fuji Fonts.
 *
 */
