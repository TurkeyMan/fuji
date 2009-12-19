#if !defined(_MFOPENGL_H)

#if defined(MF_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
#endif
#if defined(MF_OPENGL_ES)
	#if MF_OPENGL_ES_VER == 1
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
	#elif MF_OPENGL_ES_VER == 2
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
	#endif
#else
	#if defined(MF_LINUX) || defined(MF_OSX)
		#include <GL/glx.h>
		#include <GL/glxext.h>
	#endif
	#include <GL/gl.h>
#endif

// define missing opengl constants
#if !defined(cGL_TEXTURE_MAX_LEVEL)
	#define GL_TEXTURE_MAX_LEVEL				0x813D
#endif
#if !defined(cGL_BGRA)
	#define GL_BGRA								0x80E1
#endif
#if !defined(cGL_ABGR_EXT)
	#define GL_ABGR_EXT							0x8000
#endif
#if !defined(cGL_RGBA16F_ARB)
	#define GL_RGBA16F_ARB						0x881A
#endif
#if !defined(cGL_RGBA32F_ARB)
	#define GL_RGBA32F_ARB						0x8814
#endif
#if !defined(cGL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
	#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT	0x83F1
#endif
#if !defined(cGL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT	0x83F2
#endif
#if !defined(cGL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT	0x83F3
#endif
#if !defined(cGL_HALF_FLOAT_ARB)
	#define GL_HALF_FLOAT_ARB					0x140B
#endif
#if !defined(cGL_UNSIGNED_INT_8_8_8_8)
	#define GL_UNSIGNED_INT_8_8_8_8				0x8035
#endif
#if !defined(cGL_UNSIGNED_INT_10_10_10_2)
	#define GL_UNSIGNED_INT_10_10_10_2			0x8036
#endif
#if !defined(cGL_UNSIGNED_SHORT_5_6_5)
	#define GL_UNSIGNED_SHORT_5_6_5				0x8363
#endif
#if !defined(cGL_UNSIGNED_SHORT_5_5_5_1)
	#define GL_UNSIGNED_SHORT_5_5_5_1			0x8034
#endif
#if !defined(cGL_UNSIGNED_SHORT_4_4_4_4)
	#define GL_UNSIGNED_SHORT_4_4_4_4			0x8033
#endif
#if !defined(cGL_UNSIGNED_INT_8_8_8_8_REV)
	#define GL_UNSIGNED_INT_8_8_8_8_REV			0x8367
#endif
#if !defined(cGL_UNSIGNED_INT_2_10_10_10_REV)
	#define GL_UNSIGNED_INT_2_10_10_10_REV		0x8368
#endif
#if !defined(cGL_UNSIGNED_SHORT_5_6_5_REV)
	#define GL_UNSIGNED_SHORT_5_6_5_REV			0x8364
#endif
#if !defined(cGL_UNSIGNED_SHORT_1_5_5_5_REV)
	#define GL_UNSIGNED_SHORT_1_5_5_5_REV		0x8366
#endif
#if !defined(cGL_UNSIGNED_SHORT_4_4_4_4_REV)
	#define GL_UNSIGNED_SHORT_4_4_4_4_REV		0x8365
#endif

#endif
