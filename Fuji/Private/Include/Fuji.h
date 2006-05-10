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
	#include <memory.h>
	#include <string.h>

	// this defines weather to take mouse[0]'s coords from the windows cursor or from
	// DirectInput's accumulated input events
	#define USE_WINDOWS_MOUSE_COORDS

	// this allows DirectInupt to manage the system mouse
	#define ALLOW_DI_MOUSE

	// this defines that rawinput will be used to recognise more than one mouse connected to the PC
	#define ALLOW_RAW_INPUT

	// add support for XInput (will reserve the first 4 controller slots and support hotswapping if any XInput devices are connected at startup)
	#define SUPPORT_XINPUT

	// disable 'unreferenced formal parameter'
	#pragma warning(disable:4100)
	
	// Disable depreciated bullshit
	#pragma warning(disable:4996)

#elif defined(_XBOX)

	#define DEBUG_KEYBOARD
	#define DEBUG_MOUSE
	#include <xtl.h>

	char*  FixXBoxFilename(const char *pFilename);

#elif defined(_LINUX)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc
	#include <string.h> // For strcpy

	#if defined(SOME_LINUX_64BIT_FLAG)
		#define MF64BIT
	#endif

#elif defined(_PSP)

	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>

//	#define _USER_MODE
//	#define _PSP_GDB

	#if !defined(_USER_MODE)
//		#define _ENABLE_USB
	#endif

#elif defined(_PS2)

	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>

#elif defined(_DC)

	#define _arch_dreamcast
	#include <kos.h>
	#include <stdarg.h> // For varargs
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>

#elif defined(_GC)

	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>

	#define MFBIG_ENDIAN

	// angel script needs max portability...
	#define MAX_PORTABILITY

#endif

#if defined(_FUJI_UTIL)

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
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
