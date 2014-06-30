#pragma once
#if !defined(_MFOPENGL_H)
#define _MFOPENGL_H

#if MF_OPENGL_ES_VER == 1
	#error GLES 1.x no longer supported!
#endif
#if !defined(MF_OPENGL_ES) || MF_OPENGL_ES_VER >= 2
	#define MF_OPENGL_SUPPORT_SHADERS
#endif

#if defined(MF_OPENGL_ES)
	#if defined(MF_IPHONE)
		#if MF_OPENGL_ES_VER == 1
			#include <OpenGLES/ES1/gl.h>
			#include <OpenGLES/ES1/glext.h>
		#elif MF_OPENGL_ES_VER == 2
			#include <OpenGLES/ES2/gl.h>
			#include <OpenGLES/ES2/glext.h>
		#endif
	#else
		#if MF_OPENGL_ES_VER == 1
			#include <GLES/gl.h>
			#include <GLES/glext.h>
		#elif MF_OPENGL_ES_VER == 2
			#include <GLES2/gl2.h>
			#include <GLES2/gl2ext.h>
		#endif
	#endif
	#if defined(MF_NACL)
		#include <ppapi/gles2/gl2ext_ppapi.h>
	#endif
#elif defined(MF_WINDOWS)
	#include "glew/glew.h"
	#include "glew/wglew.h"
#else
	#if defined(MF_LINUX) || defined(MF_OSX)
		#include "glew/glxew.h"
		#include <GL/glx.h>
		#include <GL/glxext.h>
	#else
		#include "glew/glew.h"
	#endif

	#include <GL/gl.h>
#endif

//  OpenGL ES seems to have renamed a couple of functions
#if defined(MF_OPENGL_ES)
	#define glClearDepth glClearDepthf
	#define glDepthRange glDepthRangef

	#if !defined(GL_CLAMP_TO_BORDER)
		#define GL_CLAMP_TO_BORDER 0x812D
	#endif
	#if !defined(GL_MIRROR_CLAMP_TO_EDGE_EXT)
		#define GL_MIRROR_CLAMP_TO_EDGE_EXT 0x8743
	#endif
	#if !defined(GL_GEOMETRY_SHADER)
		#define GL_GEOMETRY_SHADER 0x8DD9
	#endif
	#if !defined(GL_BGRA)
		#define GL_BGRA 0x80E1
	#endif
	#if !defined(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
		#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
	#endif
	#if !defined(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
		#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
	#endif
	#if !defined(GL_HALF_FLOAT)
		#define GL_HALF_FLOAT 0x140B
	#endif
	#if !defined(GL_UNSIGNED_INT_2_10_10_10_REV)
		#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
	#endif
	#if !defined(GL_INT_2_10_10_10_REV)
		#define GL_INT_2_10_10_10_REV 0x8D9F
	#endif
	#if !defined(GL_MIN)
		#define GL_MIN 0x8007
	#endif
	#if !defined(GL_MAX)
		#define GL_MAX 0x8008
	#endif
	#if !defined(GL_SRC1_COLOR)
		#define GL_SRC1_COLOR 0x88F9
	#endif
	#if !defined(GL_ONE_MINUS_SRC1_COLOR)
		#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
	#endif
	#if !defined(GL_SRC1_ALPHA)
		#define GL_SRC1_ALPHA 0x8589
	#endif
	#if !defined(GL_ONE_MINUS_SRC1_ALPHA)
		#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
	#endif
#endif

struct MFEffectData_OpenGL
{
	GLint program;
	uint8 uniformLocation[4][32];
};

#if defined(_DEBUG)
bool MFCheckForOpenGLError(bool bBreakOnError = false);
#else
#define MFCheckForOpenGLError(...) false
#endif

#endif
