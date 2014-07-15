#if !defined(_MFDRIVER_H)
#define _MFDRIVER_H

/*
	We'll define some #define's to tell us which drivers to use.
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
	MF_MIDI:
	MF_PARTICLESYSTEM:
		NULL
		MF_PLATFORM

	MF_COMPUTE:
		NULL
		OPENCL
*/

/*** Driver list ***/

#define MF_DRIVER_NULL			0
#define MF_DRIVER_WIN32			1
#define MF_DRIVER_X11			2
#define MF_DRIVER_COCOA			3
#define MF_DRIVER_D3D9			4
#define MF_DRIVER_D3D11			5
#define MF_DRIVER_OPENGL		6
#define MF_DRIVER_SDL			7
#define MF_DRIVER_SOFTWARE		8
#define MF_DRIVER_DSOUND		9
#define MF_DRIVER_FMOD			10
#define MF_DRIVER_ALSA			11
#define MF_DRIVER_CRT			12
#define MF_DRIVER_WINSOCK		13
#define MF_DRIVER_PC			14
#define MF_DRIVER_LINUX			15
#define MF_DRIVER_OSX			16
#define MF_DRIVER_XBOX			17
#define MF_DRIVER_X360			18
#define MF_DRIVER_PSP			19
#define MF_DRIVER_PS2			20
#define MF_DRIVER_PS3			21
#define MF_DRIVER_GC			22
#define MF_DRIVER_DC			23
#define MF_DRIVER_AMIGA			24
#define MF_DRIVER_WII			25
#define MF_DRIVER_SYMBIAN		26
#define MF_DRIVER_PORTAUDIO		27
#define MF_DRIVER_JACK			28
#define MF_DRIVER_OPENAL		29
#define MF_DRIVER_PULSEAUDIO	30
#define MF_DRIVER_PLUGIN		31
#define MF_DRIVER_IPHONE		32
#define MF_DRIVER_WINDOWSMOBILE	33
#define MF_DRIVER_OPENCL		34
#define MF_DRIVER_BSDSOCKETS	35
#define MF_DRIVER_ANDROID		36
#define MF_DRIVER_NACL			37
#define MF_DRIVER_WEB			38
#define MF_DRIVER_XAUDIO2		39
#define MF_DRIVER_ASIO			40
#define MF_DRIVER_XB1			41
#define MF_DRIVER_PS4			42
#define MF_DRIVER_WIIU			43
#define MF_DRIVER_SDL2			44


// we need a list of all available renderer drivers
enum MFRendererDrivers
{
	MFRD_Unknown = -1,

	MFRD_D3D9 = 0,
	MFRD_D3D11,
	MFRD_OpenGL,
	MFRD_X360,
	MFRD_XBox,
	MFRD_PSP,
	MFRD_PS2,

	MFRD_Max,
	MFRD_ForceInt = 0x7FFFFFFF
};

/*** Platform specific defines, includes and driver selection ***/

#if defined(MF_WINDOWS)

	#include <stdlib.h>

	// this defines weather to take mouse[0]'s coords from the windows cursor or from
	// DirectInput's accumulated input events
	#define USE_WINDOWS_MOUSE_COORDS

	#if defined(MF_COMPILER_VISUALC)
		// add support for DirectInput
		#define SUPPORT_DINPUT

		// add support for XInput (will reserve the first 4 controller slots and support hotswapping if any XInput devices are connected at startup)
		#define SUPPORT_XINPUT

		// add support for the G15 LCD screen
		#define SUPPORT_G15
	#endif

	// this allows DirectInupt to manage the system mouse
	#define ALLOW_DI_MOUSE

	// this defines that rawinput will be used to recognise more than one mouse connected to the PC
	#define ALLOW_RAW_INPUT

	// use assimp to load 3d assets
	#define USE_ASSIMP

	// shader features
	#define SUPPORT_HLSL
	#define SUPPORT_GLSL
//	#define SUPPORT_CG
	#define SUPPORT_HLSL2GLSL
	#define SUPPORT_GLSL_OPTIMIZER

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
	#define MF_MIDI				MF_DRIVER_PC
//	#define MF_COMPUTE			MF_DRIVER_OPENCL

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

	// use assimp to load 3d assets
	#define USE_ASSIMP

	// shader features
	#define SUPPORT_HLSL
	#define SUPPORT_GLSL
//	#define SUPPORT_CG
	#define SUPPORT_HLSL2GLSL
	#define SUPPORT_GLSL_OPTIMIZER

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_X11
	#define MF_RENDERER		MF_DRIVER_OPENGL
	#define MF_INPUT		MF_DRIVER_LINUX
	#define MF_SYSTEM		MF_DRIVER_LINUX
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX
	#define MF_SOCKETS		MF_DRIVER_BSDSOCKETS
	#define MF_FILESYSTEM	MF_DRIVER_LINUX
	#define MF_SOUND		MF_DRIVER_OPENAL

//	#define USE_MFSOUNDBUFFER
//	#define USE_MFMIXER

#elif defined(MF_OSX)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

//	#define _OPENGL_CLIP_SPACE

	// use assimp to load 3d assets
//	#define USE_ASSIMP

	// shader features
//	#define SUPPORT_HLSL
	#define SUPPORT_GLSL
//	#define SUPPORT_CG
//	#define SUPPORT_HLSL2GLSL
//	#define SUPPORT_GLSL_OPTIMIZER

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_OSX
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

	// shader features
//	#define SUPPORT_HLSL
	#define SUPPORT_GLSL
//	#define SUPPORT_CG
//	#define SUPPORT_HLSL2GLSL
//	#define SUPPORT_GLSL_OPTIMIZER

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

#elif defined(MF_ANDROID)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc

//	#define _OPENGL_CLIP_SPACE

	// shader features
//	#define SUPPORT_HLSL
	#define SUPPORT_GLSL
//	#define SUPPORT_CG
//	#define SUPPORT_HLSL2GLSL
//	#define SUPPORT_GLSL_OPTIMIZER

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

#elif defined(MF_SYMBIAN)

	#define MF_NO_CRT

	// specify drivers

#elif defined(MF_NACL)

	#include <stdarg.h> // For varargs
	#include <stdlib.h> // For realloc, malloc
	#include <sys/types.h> // expose ssize_t

//	#define _OPENGL_CLIP_SPACE

	// shader features
//	#define SUPPORT_HLSL
	#define SUPPORT_GLSL
//	#define SUPPORT_CG
//	#define SUPPORT_HLSL2GLSL
//	#define SUPPORT_GLSL_OPTIMIZER

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

#elif defined(MF_WEB)

//	#define _OPENGL_CLIP_SPACE

	// shader features
//	#define SUPPORT_HLSL
	#define SUPPORT_GLSL
//	#define SUPPORT_CG
//	#define SUPPORT_HLSL2GLSL
//	#define SUPPORT_GLSL_OPTIMIZER

	// specify drivers
	#define MF_DISPLAY		MF_DRIVER_WEB
	#define MF_RENDERER		MF_DRIVER_OPENGL
	#define MF_INPUT		MF_DRIVER_WEB
	#define MF_SYSTEM		MF_DRIVER_WEB
	#define MF_TRANSLATION	MF_DRIVER_LINUX
	#define MF_THREAD		MF_DRIVER_LINUX
	#define MF_FILESYSTEM	MF_DRIVER_WEB
	#define MF_SOCKETS		MF_DRIVER_NULL
	#define MF_SOUND		MF_DRIVER_NULL
	#define MF_DEBUG		MF_DRIVER_WEB

	#define USE_MFSOUNDBUFFER
	#define USE_MFMIXER

	#define MF_OPENGL_ES

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
#if !defined(MF_MIDI)
	#define MF_MIDI NULL
#endif
#if !defined(MF_COMPUTE)
	#define MF_COMPUTE NULL
#endif

// if no OpenGL ES version was defined, set the default
#if defined(MF_OPENGL_ES) && !defined(MF_OPENGL_ES_VER)
	// set default OpenGL ES version to 2
	#define MF_OPENGL_ES_VER 2
#endif

// if the plugin driver was selected, the available plugins need to be configured for various platforms
#include "Fuji/MFDriver_Plugin.h"

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
#if defined(MF_WINDOWS) || defined(MF_LINUX) || defined(MF_OSX) || defined(MF_ANDROID)
	// libMAD mp3 codec
	#define	MAD_STREAM
#endif
#if defined(MF_WINDOWS) && defined(MF_32BIT) && defined(MF_COMPILER_VISUALC)
	// MiniFMOD can decode old XM tracker format tunes
//	#define MINIFMOD_STREAM
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

#endif
