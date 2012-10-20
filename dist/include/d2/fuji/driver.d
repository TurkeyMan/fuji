module fuji.driver;

enum MFDriver
{
	NULL			= 0,
	WIN32			= 1,
	X11				= 2,
	COCOA			= 3,
	D3D9			= 4,
	D3D11			= 5,
	OPENGL			= 6,
	SDL				= 7,
	SOFTWARE		= 8,
	DSOUND			= 9,
	FMOD			= 10,
	ALSA			= 11,
	CRT				= 12,
	WINSOCK			= 13,
	PC				= 14,
	LINUX			= 15,
	OSX				= 16,
	XBOX			= 17,
	X360			= 18,
	PSP				= 19,
	PS2				= 20,
	PS3				= 21,
	GC				= 22,
	DC				= 23,
	AMIGA			= 24,
	WII				= 25,
	SYMBIAN			= 26,
	PORTAUDIO		= 27,
	JACK			= 28,
	OPENAL			= 29,
	PULSEAUDIO		= 30,
	PLUGIN			= 31,
	IPHONE			= 32,
	WINDOWSMOBILE	= 33,
	OPENCL			= 34,
	BSDSOCKETS		= 35,
	ANDROID			= 36,
	NACL			= 37
}

/*** Platform specific defines, includes and driver selection ***/

/+
#if defined(MF_WINDOWS)

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

	// specify drivers
	#define MF_DISPLAY			MF_DRIVER_WIN32
	#define MF_RENDERER			MF_DRIVER_PLUGIN
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
	//#define MF_COMPUTE			MF_DRIVER_OPENCL

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
	#define MF_SYSTEM		MF_DRIVER_LINUX
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX
	#define MF_SOCKETS		MF_DRIVER_BSDSOCKETS
	#define MF_FILESYSTEM	MF_DRIVER_LINUX
	#define MF_SOUND		MF_DRIVER_PORTAUDIO

	#define USE_MFSOUNDBUFFER
	#define USE_MFMIXER

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
	#define MF_SOCKETS		MF_DRIVER_BSDSOCKETS
	#define MF_FILESYSTEM	MF_DRIVER_LINUX

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

	#define USE_MFSOUNDBUFFER
	#define USE_MFMIXER

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

#elif defined(MF_GC) || defined(MF_WII)

	#include <stdarg.h>
	#include <stdlib.h>

	// angel script needs max portability...
	#define MAX_PORTABILITY

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_GC
	#define MF_RENDERER		MF_DRIVER_GC
	#define MF_INPUT		MF_DRIVER_GC
	#define MF_SYSTEM		MF_DRIVER_GC

#elif defined(MF_IPHONE)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

//	#define _OPENGL_CLIP_SPACE

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_IPHONE
	#define MF_RENDERER		MF_DRIVER_OPENGL
	#define MF_INPUT		MF_DRIVER_IPHONE
	#define MF_SYSTEM		MF_DRIVER_IPHONE
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX
	#define MF_FILESYSTEM	MF_DRIVER_LINUX
	#define MF_SOUND		MF_DRIVER_IPHONE

	#define USE_MFSOUNDBUFFER
	#define USE_MFMIXER

	#define MF_OPENGL_ES
	#if !defined(MF_OPENGL_ES_VER)
		#define MF_OPENGL_ES_VER 1
	#endif

#elif defined(MF_ANDROID)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

//	#define _OPENGL_CLIP_SPACE

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_ANDROID
	#define MF_RENDERER		MF_DRIVER_OPENGL
	#define MF_INPUT		MF_DRIVER_ANDROID
	#define MF_SYSTEM		MF_DRIVER_ANDROID
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX
	#define MF_FILESYSTEM	MF_DRIVER_LINUX
	#define MF_SOUND		MF_DRIVER_NULL
	#define MF_DEBUG		MF_DRIVER_ANDROID

	#define USE_MFSOUNDBUFFER
	#define USE_MFMIXER

	#define MF_OPENGL_ES
	#if !defined(MF_OPENGL_ES_VER)
		#define MF_OPENGL_ES_VER 1
	#endif

#elif defined(MF_SYMBIAN)

	#define MF_NO_CRT

	// specify drivers

#elif defined(MF_NACL)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc
	#include <sys/types.h> // expose ssize_t

//	#define _OPENGL_CLIP_SPACE

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_NACL
	#define MF_RENDERER		MF_DRIVER_OPENGL
	#define MF_INPUT		MF_DRIVER_NACL
	#define MF_SYSTEM		MF_DRIVER_NACL
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX
	#define MF_FILESYSTEM	MF_DRIVER_NACL
	#define MF_SOCKETS		MF_DRIVER_NULL
	#define MF_SOUND		MF_DRIVER_NULL
	#define MF_DEBUG		MF_DRIVER_NACL

	#define USE_MFSOUNDBUFFER
	#define USE_MFMIXER

	#define MF_OPENGL_ES
	#if !defined(MF_OPENGL_ES_VER)
		#define MF_OPENGL_ES_VER 2
	#endif

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
#if !defined(MF_COMPUTE)
	#define MF_COMPUTE NULL
#endif

// if no OpenGL ES version was defined, set the default
#if defined(MF_OPENGL_ES) && !defined(MF_OPENGL_ES_VER)
	// set default OpenGL ES version to 1
	#define MF_OPENGL_ES_VER 1
#endif

// if the plugin driver was selected, the available plugins need to be configured for various platforms
#include "MFDriver_Plugin.h"

// enable this define to allow the NULL drivers to operate using the standard CRT where appropriate
#if !defined(MF_NO_CRT)
	#define _USE_CRT_FOR_NULL_DRIVERS
#endif

// define avalable audio codecs
#define WAV_STREAM	// every platform can load and stream wav files.
#if defined(MF_WINDOWS) || defined(MF_LINUX) || defined(MF_OSX) || defined(MF_PSP)
	// vorbis codec
	#define	VORBIS_STREAM
#endif
#if defined(MF_WINDOWS) || defined(MF_ANDROID)
	// libMAD mp3 codec
	#define	MAD_STREAM
#endif
#if defined(MF_WINDOWS) && !defined(MF_64BIT)
	// MiniFMOD can decode old XM tracker format tunes
	#define MINIFMOD_STREAM
#endif
#if defined(MF_PSP)
	// PSP hardware mp3 decoder
	#define PSPAUDIOCODEC_STREAM
//	#define VORBIS_TREMOR
#endif

// additional features
#if !defined(MF_PS2)
//	#define _ENABLE_SCRIPTING
#endif

// old rubbish that should disappear
#if defined(_FUJI_UTIL)
	#include <stdio.h>
	#include <stdlib.h>
	#include <new>
#endif
+/
