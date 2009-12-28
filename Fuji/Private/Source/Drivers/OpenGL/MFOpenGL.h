#if !defined(_MFOPENGL_H)

#if defined(MF_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>

	#define glGetProcAddress wglGetProcAddress
#elif defined(MF_OPENGL_ES)
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

	extern "C" void (*glXGetProcAddressARB(const char *))();
	#define glGetProcAddress glXGetProcAddressARB
#endif

// define missing opengl constants
#if !defined(GL_TEXTURE_MAX_LEVEL)
	#define GL_TEXTURE_MAX_LEVEL				0x813D
#endif
#if !defined(GL_BGRA)
	#define GL_BGRA								0x80E1
#endif
#if !defined(GL_ABGR_EXT)
	#define GL_ABGR_EXT							0x8000
#endif
#if !defined(GL_RGBA16F_ARB)
	#define GL_RGBA16F_ARB						0x881A
#endif
#if !defined(GL_RGBA32F_ARB)
	#define GL_RGBA32F_ARB						0x8814
#endif
#if !defined(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
	#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT	0x83F1
#endif
#if !defined(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT	0x83F2
#endif
#if !defined(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT	0x83F3
#endif
#if !defined(GL_HALF_FLOAT_ARB)
	#define GL_HALF_FLOAT_ARB					0x140B
#endif
#if !defined(GL_UNSIGNED_INT_8_8_8_8)
	#define GL_UNSIGNED_INT_8_8_8_8				0x8035
#endif
#if !defined(GL_UNSIGNED_INT_10_10_10_2)
	#define GL_UNSIGNED_INT_10_10_10_2			0x8036
#endif
#if !defined(GL_UNSIGNED_SHORT_5_6_5)
	#define GL_UNSIGNED_SHORT_5_6_5				0x8363
#endif
#if !defined(GL_UNSIGNED_SHORT_5_5_5_1)
	#define GL_UNSIGNED_SHORT_5_5_5_1			0x8034
#endif
#if !defined(GL_UNSIGNED_SHORT_4_4_4_4)
	#define GL_UNSIGNED_SHORT_4_4_4_4			0x8033
#endif
#if !defined(GL_UNSIGNED_INT_8_8_8_8_REV)
	#define GL_UNSIGNED_INT_8_8_8_8_REV			0x8367
#endif
#if !defined(GL_UNSIGNED_INT_2_10_10_10_REV)
	#define GL_UNSIGNED_INT_2_10_10_10_REV		0x8368
#endif
#if !defined(GL_UNSIGNED_SHORT_5_6_5_REV)
	#define GL_UNSIGNED_SHORT_5_6_5_REV			0x8364
#endif
#if !defined(GL_UNSIGNED_SHORT_1_5_5_5_REV)
	#define GL_UNSIGNED_SHORT_1_5_5_5_REV		0x8366
#endif
#if !defined(GL_UNSIGNED_SHORT_4_4_4_4_REV)
	#define GL_UNSIGNED_SHORT_4_4_4_4_REV		0x8365
#endif
#if !defined(GL_FRAMEBUFFER)
	#define GL_FRAMEBUFFER						0x8D40
#endif
#if !defined(GL_RENDERBUFFER)
	#define GL_RENDERBUFFER						0x8D41
#endif
#if !defined(GL_COLOR_ATTACHMENT0)
	#define GL_COLOR_ATTACHMENT0				0x8CE0
#endif
#if !defined(GL_DEPTH_ATTACHMENT)
	#define GL_DEPTH_ATTACHMENT					0x8D00
#endif
#if !defined(GL_PROXY_TEXTURE_2D)
	#define GL_PROXY_TEXTURE_2D					0x8064
#endif

// defines for working with buffer objects
#if !defined(GL_ARRAY_BUFFER)
	#if defined(GL_ARRAY_BUFFER_ARB)
		#define GL_ARRAY_BUFFER GL_ARRAY_BUFFER_ARB
	#else
		#define GL_ARRAY_BUFFER 0x8892
	#endif
#endif
#if !defined(GL_ELEMENT_ARRAY_BUFFER)
	#if defined(GL_ELEMENT_ARRAY_BUFFER_ARB)
		#define GL_ELEMENT_ARRAY_BUFFER GL_ELEMENT_ARRAY_BUFFER_ARB
	#else
		#define GL_ELEMENT_ARRAY_BUFFER 0x8893
	#endif
#endif
#if !defined(GL_STATIC_DRAW)
	#if defined(GL_STATIC_DRAW_ARB)
		#define GL_STATIC_DRAW GL_STATIC_DRAW_ARB
	#else
		#define GL_STATIC_DRAW 0x88E4
	#endif
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
	#endif
#endif

// APIENTRY seems to be missing on some platforms
#if !defined(APIENTRY)
	#define APIENTRY
#endif

// some implementations may pre-load the extensions
#if !defined(MF_OPENGL_ES)
	#define LOAD_EXTENSIONS
#endif

#if defined(LOAD_EXTENSIONS)
	// function pointers for opengl extensions
	typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC)(GLsizei n, GLuint *buffers);
	typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC)(GLsizei n, const GLuint *buffers);
	typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC)(GLenum target, GLuint buffer);
	typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC)(GLenum target, int size, const GLvoid *data, GLenum usage);

	typedef void (APIENTRY * PFNGLGENRENDERBUFFERSEXTPROC)(GLsizei n, GLuint* ids);
	typedef void (APIENTRY * PFNGLDELETERENDERBUFFERSEXTPROC)(GLsizei n, const GLuint* ids);
	typedef void (APIENTRY * PFNGLBINDRENDERBUFFEREXTPROC)(GLenum target, GLuint id);
	typedef void (APIENTRY * PFNGLRENDERBUFFERSTORAGEEXTPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

	typedef void (APIENTRY * PFNGLGENFRAMEBUFFERSEXTPROC)(GLsizei n, GLuint* ids);
	typedef void (APIENTRY * PFNGLDELETEFRAMEBUFFERSEXTPROC)(GLsizei n, const GLuint* ids);
	typedef void (APIENTRY * PFNGLBINDFRAMEBUFFEREXTPROC)(GLenum target, GLuint id);
	typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)(GLenum target, GLenum attachmentPoint, GLenum textureTarget, GLuint textureId, GLint  level);
	typedef void (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

	// VBO extension function pointers
	extern PFNGLGENBUFFERSARBPROC glGenBuffers;
	extern PFNGLBINDBUFFERARBPROC glBindBuffer;
	extern PFNGLBUFFERDATAARBPROC glBufferData;
	extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;

	// FBO extension function pointers
	extern PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffers;
	extern PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffers;
	extern PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbuffer;
	extern PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorage;

	extern PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffers;
	extern PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffers;
	extern PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer;
	extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2D;
	extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbuffer;
#endif

bool MFCheckForOpenGLError();

#endif
