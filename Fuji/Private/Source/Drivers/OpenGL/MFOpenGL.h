#pragma once
#if !defined(_MFOPENGL_H)
#define _MFOPENGL_H

#if defined(MF_WINDOWS)
	#include "glew/glew.h"
	#include "glew/wglew.h"

	#define MF_OPENGL_SUPPORT_SHADERS
#elif defined(MF_OPENGL_ES)
	#if defined(MF_IPHONE)
		#if MF_OPENGL_ES_VER == 1
			#include <OpenGLES/ES1/gl.h>
			#include <OpenGLES/ES1/glext.h>
		#elif MF_OPENGL_ES_VER == 2
			#include <OpenGLES/ES2/gl.h>
			#include <OpenGLES/ES2/glext.h>
		#endif
	#elif defined(MF_ANDROID)
		#define GL_GLEXT_PROTOTYPES
		#if MF_OPENGL_ES_VER == 1
			#include <GLES/gl.h>
			#include <GLES/glext.h>
		#elif MF_OPENGL_ES_VER == 2
			#include <GLES2/gl2.h>
			#include <GLES2/gl2ext.h>
		#endif
	#elif defined(MF_NACL)
//		#define GL_GLEXT_PROTOTYPES
		#if MF_OPENGL_ES_VER == 1
			#error GLES 1.x not supported in Native Client!
		#elif MF_OPENGL_ES_VER == 2
			#include <GLES2/gl2.h>
			#include <GLES2/gl2ext.h>
			#include <ppapi/gles2/gl2ext_ppapi.h>
		#endif
	#else
		#error Unknown OpenGL ES include paths...
	#endif

	#if MF_OPENGL_ES_VER >= 2
		#define MF_OPENGL_SUPPORT_SHADERS
	#endif
#else
	#if defined(MF_LINUX) || defined(MF_OSX)
		#include "glew/glxew.h"
		#include <GL/glx.h>
		#include <GL/glxext.h>
	#else
		#include "glew/glew.h"
	#endif

	#include <GL/gl.h>

	#define MF_OPENGL_SUPPORT_SHADERS
#endif

//  OpenGL ES seems to have renamed a couple of functions
#if defined(MF_OPENGL_ES)
	#define glClearDepth glClearDepthf
	#define glDepthRange glDepthRangef

	#if MF_OPENGL_ES_VER == 1
		#define glGenRenderbuffers glGenRenderbuffersOES
		#define glDeleterenderbuffers glDeleteRenderbuffersOES
		#define glBindRenderbuffer glBindRenderbufferOES
		#define glRenderbufferStorage glRenderbufferStorageOES

		#define glGenFramebuffers glGenFramebuffersOES
		#define glDeleteFramebuffers glDeleteFramebuffersOES
		#define glBindFramebuffer glBindFramebufferOES
		#define glFramebufferTexture2D glFramebufferTexture2DOES
		#define glFramebufferRenderbuffer glFramebufferRenderbufferOES

		#define glCheckFramebufferStatus glCheckFramebufferStatusOES
	#endif

	#if !defined GL_CLAMP_TO_BORDER
		#define GL_CLAMP_TO_BORDER 0x812D
	#endif
	#if !defined GL_MIRROR_CLAMP_TO_EDGE_EXT
		#define GL_MIRROR_CLAMP_TO_EDGE_EXT 0x8743
	#endif

	#if !defined GL_GEOMETRY_SHADER
		#define GL_GEOMETRY_SHADER 0x8DD9
	#endif

	#if !defined GL_BGRA
		#define GL_BGRA 0x80E1
	#endif
#endif

enum MFOpenGL_ShaderType
{
	MFOGL_ShaderType_VertexShader = 0,
	MFOGL_ShaderType_FragmentShader,
	MFOGL_ShaderType_GeometryShader
};

enum MFOpenGL_MatrixType
{
	MFOGL_ShaderType_Projection = 0,
	MFOGL_ShaderType_WorldView,
	MFOGL_ShaderType_Texture
};

GLuint MFRenderer_OpenGL_CompileShader(const char *pShader, MFOpenGL_ShaderType shaderType);
GLuint MFRenderer_OpenGL_CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0);

void MFRenderer_OpenGL_SetMatrix(MFOpenGL_MatrixType type, const MFMatrix &mat);

void MFRenderer_OpenGL_SetShaderProgram(GLuint program);
bool MFRenderer_OpenGL_SetUniformV(const char *pName, const MFVector *pV, int numVectors = 1);
bool MFRenderer_OpenGL_SetUniformM(const char *pName, const MFMatrix *pM, int numMatrices = 1);
bool MFRenderer_OpenGL_SetUniformS(const char *pName, int sampler);

extern GLuint gCurrentShaderProgram;
inline GLuint MFRenderer_OpenGL_GetCurrentProgram() { return gCurrentShaderProgram; }

bool MFCheckForOpenGLError(bool bBreakOnError = false);

extern bool gOpenGLUseShaders;
inline bool MFOpenGL_UseShaders() { return gOpenGLUseShaders; }

#endif
