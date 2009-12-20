#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_OpenGL
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_OpenGL
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_OpenGL
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_OpenGL
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_OpenGL
	#define MFRenderer_BeginFrame MFRenderer_BeginFrame_OpenGL
	#define MFRenderer_EndFrame MFRenderer_EndFrame_OpenGL
	#define MFRenderer_SetClearColour MFRenderer_SetClearColour_OpenGL
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_OpenGL
	#define MFRenderer_GetViewport MFRenderer_GetViewport_OpenGL
	#define MFRenderer_SetViewport MFRenderer_SetViewport_OpenGL
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_OpenGL
	#define MFRenderer_SetRenderTarget MFRenderer_SetRenderTarget_OpenGL
	#define MFRenderer_SetDeviceRenderTarget MFRenderer_SetDeviceRenderTarget_OpenGL
#endif

#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"
#include "MFRenderer_Internal.h"

#include "MFOpenGL.h"

int gOpenGLVersion = 0;

#if MF_DISPLAY == MF_DRIVER_X11
	#include "../X11/X11_linux.h"
	#include <stdio.h>

	const int glAttrsSingle[] =
	{
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, /* Don't need to draw to anything else */
		GLX_DEPTH_SIZE, 16,
		GLX_STENCIL_SIZE, 8,
		None
	};

	const int glAttrsDouble[] =
	{
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_DEPTH_SIZE, 16,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, true,
		None
	};

	GLXWindow glXWindow;
	GLXContext glXContext = NULL;
	GLXFBConfig *fbConfigs = NULL;

	XVisualInfo *MFRenderer_GetVisualInfo()
	{
		XVisualInfo *visualInfo = NULL;

		if(!glXQueryExtension(xdisplay, NULL, NULL))
		{
			MFDebug_Error("GLX extension not available");
			MFDisplay_DestroyDisplay();
			return NULL;
		}

		int glXMajor, glXMinor;
		if(!glXQueryVersion(xdisplay, &glXMajor, &glXMinor) || (glXMajor == 1 && glXMinor < 3))
		{
			MFDebug_Error(MFStr("Unable to open display, need GLX V1, and at least version 1.3 (Have version %d.%d)", glXMajor, glXMinor));
			MFDisplay_DestroyDisplay();
			return NULL;
		}

		// Try and obtain a suitable FBconfig, try for double buffering first
		int numConfigs;
		fbConfigs = glXChooseFBConfig(xdisplay, screen, glAttrsDouble, &numConfigs);
		if(numConfigs == 0)
		{
			fbConfigs = glXChooseFBConfig(xdisplay, screen, glAttrsSingle, &numConfigs);
			if(numConfigs == 0)
			{
				MFDebug_Error("Unable to obtain a suitable glX FBConfig");
				MFDisplay_DestroyDisplay();
				return NULL;
			}
		}

		if((visualInfo = glXGetVisualFromFBConfig(xdisplay, fbConfigs[0])) == NULL)
		{
			MFDebug_Error("Unable to obtain a visualInfo structure for the associated FBConfig");
			MFDisplay_DestroyDisplay();
			return NULL;
		}

		if(visualInfo->depth < 16)
		{
			MFDebug_Error("Need at least a 16 bit screen!");
			MFDisplay_DestroyDisplay();
			return NULL;
		}

		return visualInfo;
	}
#elif MF_DISPLAY == MF_DRIVER_WIN32
	#pragma comment(lib, "Opengl32")
//	#pragma comment(lib, "Glu32")

	extern HINSTANCE apphInstance;
	extern HWND apphWnd;
	HGLRC hRC = NULL; // Permanent Rendering Context
	HDC hDC = NULL; // Private GDI Device Context
#endif

#if defined(LOAD_EXTENSIONS)
	PFNGLGENBUFFERSARBPROC glGenBuffers = NULL;
	PFNGLBINDBUFFERARBPROC glBindBuffer = NULL;
	PFNGLBUFFERDATAARBPROC glBufferData = NULL;
	PFNGLDELETEBUFFERSARBPROC glDeleteBuffers = NULL;
#endif

static MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);
static MFRect gCurrentViewport;

bool IsExtensionSupported(const char *extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	// Extension names should not have spaces.
	where = (GLubyte *) strchr(extension, ' ');
	if(where || *extension == '\0')
		return false;
	extensions = glGetString(GL_EXTENSIONS);

    // It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. Don't be fooled by sub-strings, etc.
	start = extensions;
	for(;;)
	{
		where = (GLubyte *)strstr((const char *)start, extension);
		if(!where)
			break;
		terminator = where + strlen(extension);
		if(where == start || *(where - 1) == ' ')
			if(*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}
	return false;
}

void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay()
{
#if MF_DISPLAY == MF_DRIVER_X11
	if(!(glXWindow = glXCreateWindow(xdisplay, fbConfigs[0], window, NULL)))
	{
		MFDebug_Error("Unable to associate window with a GLXWindow");
		MFDisplay_DestroyDisplay();
		return 1;
	}

	if(!(glXContext = glXCreateNewContext(xdisplay, fbConfigs[0], GLX_RGBA_TYPE, NULL, true)))
	{
		MFDebug_Error("Unable to create GLXContext");
		MFDisplay_DestroyDisplay();
		return 1;
	}

	XFree(fbConfigs);
	fbConfigs = NULL;

	if(!glXMakeContextCurrent(xdisplay, glXWindow, glXWindow, glXContext))
	{
		MFDebug_Error("Unable to bind GLXContext");
		MFDisplay_DestroyDisplay();
		return 1;
	}

	// Check OpenGL version
	const char *glVersionStr = (const char *)glGetString(GL_VERSION);
	int32 majorGLVersion, minorGLVersion;

	if(sscanf(glVersionStr, "%d.%d.%*d", &majorGLVersion, &minorGLVersion) != 2)
	{
		if(sscanf(glVersionStr, "%d.%d", &majorGLVersion, &minorGLVersion) != 2)
		{
			MFDebug_Error("Unable to determine OpenGl version");
			MFDisplay_DestroyDisplay();
			return 1;
		}
	}

	if(majorGLVersion == 1 && minorGLVersion < 4)
	{
		MFDebug_Error("Need at least OpenGL version 1.4");
		MFDisplay_DestroyDisplay();
		return 1;
	}

	// Might want to check for extensions here
	//...
#elif MF_DISPLAY == MF_DRIVER_WIN32
	GLuint pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32, // colour depth
		0, 0, 0, 0, 0, 0,
		0, // No Alpha Buffer
		0, // Shift Bit Ignored
		0, // No Accumulation Buffer
		0, 0, 0, 0, // Accumulation Bits Ignored
		16, // 16Bit Z-Buffer (Depth Buffer)
		0, // No Stencil Buffer
		0, // No Auxiliary Buffer
		PFD_MAIN_PLANE, // Main Drawing Layer
		0, // Reserved
		0, 0, 0 // Layer Masks Ignored
	};

	hDC = GetDC(apphWnd);
	if(!hDC)
	{
		MFDisplay_DestroyDisplay();
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 1;
	}

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(!pixelFormat)
	{
		MFDisplay_DestroyDisplay();
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 2;
	}

	if(!SetPixelFormat(hDC, pixelFormat, &pfd))
	{
		MFDisplay_DestroyDisplay();
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 3;
	}

	hRC = wglCreateContext(hDC);
	if(!hRC)
	{
		MFDisplay_DestroyDisplay();
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 4;
	}

	if(!wglMakeCurrent(hDC, hRC))
	{
		MFDisplay_DestroyDisplay();
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 5;
	}
#endif

    // get the opengl version
	const char *pVersion = (const char *)glGetString(GL_VERSION);
	float ver = MFString_AsciiToFloat(pVersion);
	gOpenGLVersion = (int)(ver * 100);

#if defined(LOAD_EXTENSIONS)
	// try and load some extensions
	// try and load the buffer object extensions
	if(gOpenGLVersion < 150 && !IsExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		MFDebug_Warn(1, "Neither OpenGL 1.5 nor GL_ARB_vertex_buffer_object extension is available!");
	}
	else
	{
		// Load the function pointers
		if(gOpenGLVersion >= 150)
		{
			glBindBuffer = (PFNGLBINDBUFFERARBPROC)glGetProcAddress("glBindBuffer");
			glBufferData = (PFNGLBUFFERDATAARBPROC)glGetProcAddress("glBufferData");
//			glBufferSubData = glGetProcAddress("glBufferSubData");
			glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)glGetProcAddress("glDeleteBuffers");
			glGenBuffers = (PFNGLGENBUFFERSARBPROC)glGetProcAddress("glGenBuffers");
//			glMapBuffer = glGetProcAddress("glMapBuffer");
//			glUnmapBuffer = glGetProcAddress("glUnmapBuffer");
		}
		else
		{
			glBindBuffer = (PFNGLBINDBUFFERARBPROC)glGetProcAddress("glBindBufferARB");
			glBufferData = (PFNGLBUFFERDATAARBPROC)glGetProcAddress("glBufferDataARB");
//			glBufferSubData = glGetProcAddress("glBufferSubDataARB");
			glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)glGetProcAddress("glDeleteBuffersARB");
			glGenBuffers = (PFNGLGENBUFFERSARBPROC)glGetProcAddress("glGenBuffersARB");
//			glMapBuffer = glGetProcAddress("glMapBufferARB");
//			glUnmapBuffer = glGetProcAddress("glUnmapBufferARB");
		}
	}
#endif

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

//	glFrontFace(GL_CW);
//	glCullFace(GL_BACK);

	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return 0;
}

void MFRenderer_DestroyDisplay()
{
#if MF_DISPLAY == MF_DRIVER_X11
	if(fbConfigs != NULL)
	{
		XFree(fbConfigs);
		fbConfigs = NULL;
	}

	if(glXContext != NULL)
	{
		glXDestroyContext(xdisplay, glXContext);
		glXContext = NULL;
	}

	if(glXWindow != 0)
	{
		glXDestroyWindow(xdisplay, glXWindow);
		glXWindow = 0;
	}
#elif MF_DISPLAY == MF_DRIVER_WIN32
	if(hRC)
	{
		if(wglMakeCurrent(NULL, NULL))
		{
			wglDeleteContext(hRC);
		}
		hRC = NULL;
	}

	if(hDC)
	{
		ReleaseDC(apphWnd, hDC);
		hDC = NULL;
	}
#endif
}

void MFRenderer_ResetDisplay()
{
	MFRenderer_ResetViewport();
}

void MFRenderer_BeginFrame()
{
}

void MFRenderer_EndFrame()
{
	MFCALLSTACK;

#if MF_DISPLAY == MF_DRIVER_X11
	glXSwapBuffers(xdisplay, glXWindow);
#elif MF_DISPLAY == MF_DRIVER_WIN32
	SwapBuffers(hDC);
#endif
}

void MFRenderer_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void MFRenderer_ClearScreen(uint32 flags)
{
	MFCALLSTACK;

	int mask = ((flags & CS_Colour) ? GL_COLOR_BUFFER_BIT : 0) | ((flags & CS_ZBuffer) ? GL_DEPTH_BUFFER_BIT : 0);

	glClearColor(gClearColour.x, gClearColour.y, gClearColour.z, gClearColour.w);
	glClear(mask);
}

void MFRenderer_GetViewport(MFRect *pRect)
{
	*pRect = gCurrentViewport;
}

void MFRenderer_SetViewport(MFRect *pRect)
{
	MFCALLSTACK;

	gCurrentViewport = *pRect;
	glViewport((GLint)pRect->x, (GLint)pRect->y, (GLint)pRect->width, (GLint)pRect->height);
}

void MFRenderer_ResetViewport()
{
	MFCALLSTACK;

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)gDisplay.width;
	gCurrentViewport.height = (float)gDisplay.height;

	glViewport(0, 0, gDisplay.width, gDisplay.height);
}

void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget)
{
	MFDebug_Assert(false, "!");
}

void MFRenderer_SetDeviceRenderTarget()
{
	MFDebug_Assert(false, "!");
}

#endif
