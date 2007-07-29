#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL

#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"

#include "MFRenderer_Internal.h"

#if MF_DISPLAY == MF_DRIVER_X11
	#include <GL/glx.h>

	const int glAttrsSingle[] =
	{
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, /* Don't need to draw to anything else */
		GLX_DEPTH_SIZE, 16,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, true,
		None
	};

	const int glAttrsDouble[] =
	{
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_DEPTH_SIZE, 16,
		GLX_STENCIL_SIZE, 8,
		None
	};

	GLXWindow glXWindow;
	GLXContext glXContext = NULL;

#elif MF_DISPLAY == MF_DRIVER_WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <gl\gl.h>

	extern HINSTANCE apphInstance;
	extern HWND apphWnd;
	HGLRC hRC = NULL; // Permanent Rendering Context
	HDC hDC = NULL; // Private GDI Device Context
#endif

MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);


void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay()
{
#if MF_DISPLAY == MF_DRIVER_X11

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
}

void MFRenderer_ResetDisplay()
{
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

void MFRenderer_SetViewport(float x, float y, float width, float height)
{
	MFCALLSTACK;

	glViewport((GLint)((x/640.0f) * (float)gDisplay.width), (GLint)((y/480.0f) * (float)gDisplay.height), (GLint)((width/640.0f) * (float)gDisplay.width), (GLint)((height/480.0f) * (float)gDisplay.height));
}

void MFRenderer_ResetViewport()
{
	MFCALLSTACK;

	MFRenderer_SetViewport(0.f, 0.f, (float)gDisplay.width, (float)gDisplay.height);
}

#endif
