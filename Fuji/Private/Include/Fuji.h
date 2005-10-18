#if !defined(_FUJI_H)
#define _FUJI_H

// Compiler definitions
#if defined(__GNUC__)
#define __cdecl __attribute__((__cdecl__))
#define _cdecl __attribute__((__cdecl__))
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4201)
#endif


// Fuji system wide enum's

enum MFPlatform
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
	FP_XBox360,
	FP_PS3,

	FP_Max
};

enum MFEndian
{
	MFEndian_Unknown = -1,

	MFEndian_LittleEndian,
	MFEndian_BigEndian,

	MFEndian_ForceInt = 0x7FFFFFFF,
};


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

//	#define _FUJI_SSE
#elif defined(_XBOX)
	#define DEBUG_KEYBOARD
	#define DEBUG_MOUSE
	#include <xtl.h>

	char*  FixXBoxFilename(const char *pFilename);

//	#define _FUJI_SSE
#elif defined(_LINUX)
	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc
	#include <string.h> // For strcpy
#elif defined(_PSP)
	#include <pspkernel.h>

	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>
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
#endif

#if defined(_FUJI_UTIL)
	#include <Windows.h>
	#include <stdio.h>
#endif


// Compiler compatibility macros

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


// Fuji types

#include "MFTypes.h"


// Defines and macros

#if !defined(NULL) /* In case stdlib.h hasn't been included */
#define NULL 0
#endif

#define MFPI 3.141592653589f
#define MFALMOST_ZERO 0.000001f
#define MFDEGREES(a) (0.017453292519943295769236907684886f * (a))
#define MFALIGN(x, bytes) (((x)+((bytes)-1)) & ~((bytes)-1))
#define MFALIGN16(x) MFALIGN(x, 16)
#define MFUNFLAG(x, y) ((x)&=~(y))
#define MFFLAG(x, y) ((x)|=(y))
#define MFBIT(x) (1<<(x))

#if !defined(MAKEFOURCC)
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) | ((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))
#endif


// Useful templates

// Min/Max
template <class T>
inline T MFMin(T a, T b) { return a < b ? a : b; }
template <class T>
inline T MFMax(T a, T b) { return a > b ? a : b; }

// clamps so that: x <= y <= z
template <class T>
inline T MFClamp(T x, T y, T z) { return MFMax(x, MFMin(y, z)); }


// Fuji defines

// callstack profiling
#if !defined(_RETAIL) && !defined(_DEBUG)
	#define _CALLSTACK_PROFILING

	#if defined(_CALLSTACK_PROFILING) && !defined(_DEBUG)
//		#define _CALLSTACK_MONITORING
	#endif
#endif


// Additional includes

#include <new>

#include "FujiMath.h"
#include "Util.h"

#include "Callstack.h"
#include "Heap.h"

#endif // _FUJI_H
