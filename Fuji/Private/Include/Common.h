#if !defined(_COMMON_H)
#define _COMMON_H

// Compiler definitions
#if defined(__GNUC__)
#define __cdecl __attribute__((__cdecl__))
#define _cdecl __attribute__((__cdecl__))
#endif

// an enum to define all platforms supported by fuji
enum FujiPlatforms
{
	FP_Unknown = -1,

	FP_PC = 0,
	FP_XBox,
	FP_Linux,
	FP_PSP,
	FP_PS2,
	FP_DC,
	FP_GC,
	FP_OSX,
	FP_Amiga,

	FP_Max
};

// extern to platform name strings
extern const char * const gPlatformStrings[FP_Max];

// Standard platform includes
#if defined(_WINDOWS)
	#include <Windows.h>
	#include <d3d9.h>
	#include <d3dx9.h>

	// this defines weather to take mouse[0]'s coords from the windows cursor or from
	// DirectInput's accumulated input events
	#define USE_WINDOWS_MOUSE_COORDS

	// this allows DirectInupt to manage the system mouse
	#define ALLOW_DI_MOUSE

	// this defines that rawinput will be used to recognise more than one mouse connected to the PC
	#define ALLOW_RAW_INPUT

	#define gCurrentPlatform FP_PC
#elif defined(_XBOX)
	#define DEBUG_KEYBOARD
	#define DEBUG_MOUSE
	#include <xtl.h>

	char*  FixXBoxFilename(const char *pFilename);

	#define gCurrentPlatform FP_XBox
#elif defined(_LINUX)
	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc
	#include <string.h> // For strcpy

	#define gCurrentPlatform FP_Linux
#elif defined(_PSP)
	#include <pspkernel.h>

	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>

	#define gCurrentPlatform FP_PSP
#elif defined(_PS2)
	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>

	#define gCurrentPlatform FP_PS2
#elif defined(_DC)
	#define _arch_dreamcast
	#include <kos.h>
	#include <stdarg.h> // For varargs
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>

	#define gCurrentPlatform FP_DC
#elif defined(_GC)
	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>

	#define gCurrentPlatform FP_GC
#endif

#if defined(_FUJI_UTIL)
	#include <Windows.h>
	#include <stdio.h>
#endif

// SSE optimisations for xbox and PC?
#if defined(_XBOX) || defined(_WINDOWS)
//	#define _FUJI_SSE
#endif

// if SSE optimisations are enabled, include SSE intrinsics header
#if defined(_FUJI_SSE)
	#include "xmmintrin.h"
	#define _ALIGN16 _MM_ALIGN16
#elif defined(_MSC_VER)
	#define _ALIGN16 __declspec(align(16))
#elif defined(__GNUC__)
	#define _ALIGN16 __attribute__((aligned(16)))
#else
	#define _ALIGN16
#endif

#if defined(__GNUC__)
	#define _PACKED __attribute__((packed))
#else
	#define _PACKED
#endif

// Data Type Definition
#if defined(_FUJI_SSE)
typedef __m128				uint128;
#else
typedef unsigned int		uint128[4];
#endif

#if !defined(_DC) // Bloody Kallistos defines all these
#if defined(__GNUC__)
typedef unsigned long long	uint64;
typedef long long 			int64;
#else
typedef unsigned __int64	uint64;
typedef __int64				int64;
#endif

typedef unsigned int		uint32;
typedef int					int32;
typedef unsigned short		uint16;
typedef short				int16;
typedef unsigned char		uint8;
typedef char				int8;
#endif

// defined values
#if !defined(PI)
#define PI 3.141592653589f
#endif

#define ALMOST_ZERO 0.000001f

#if !defined(NULL) /* In case stdlib.h hasn't been included */
#define NULL 0
#endif

#define DEGREES(a) (0.017453292519943295769236907684886f * a)

// callstack profiling
#if !defined(_RETAIL) && !defined(_DEBUG)
	#define _CALLSTACK_PROFILING

	#if defined(_CALLSTACK_PROFILING) && !defined(_DEBUG)
//		#define _CALLSTACK_MONITORING
	#endif
#endif

// Min/Max
template <class T>
inline T Min(T a, T b) { return a < b ? a : b; }
template <class T>
inline T Max(T a, T b) { return a > b ? a : b; }

// clamps so that: x <= y <= z
template <class T>
inline T Clamp(T x, T y, T z) { return Max(x, Min(y, z)); }

// useful macros
#define ALIGN16(x) (((x)+15) & 0xFFFFFFF0)

#define UNFLAG(x, y) (x&=~y)
#define FLAG(x, y) (x|=y)

#define BIT(x) (1<<(x))

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) |       \
                ((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

// additional includes
#include <new>

#include "FujiMath.h"
#include "Util.h"

#include "Callstack.h"
#include "Heap.h"

#endif // _COMMON_H
