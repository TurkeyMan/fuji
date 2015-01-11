#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_OpenGL
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_OpenGL
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_OpenGL
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_OpenGL
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_OpenGL
	#define MFRenderer_LostFocus MFRenderer_LostFocus_OpenGL
	#define MFRenderer_GainedFocus MFRenderer_GainedFocus_OpenGL
	#define MFRenderer_BeginFramePlatformSpecific MFRenderer_BeginFramePlatformSpecific_OpenGL
	#define MFRenderer_EndFramePlatformSpecific MFRenderer_EndFramePlatformSpecific_OpenGL
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_OpenGL
	#define MFRenderer_SetViewport MFRenderer_SetViewport_OpenGL
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_OpenGL
	#define MFRenderer_GetDeviceRenderTarget MFRenderer_GetDeviceRenderTarget_OpenGL
	#define MFRenderer_GetDeviceDepthStencil MFRenderer_GetDeviceDepthStencil_OpenGL
	#define MFRenderer_SetRenderTargetPlatformSpecific MFRenderer_SetRenderTargetPlatformSpecific_OpenGL
	#define MFRenderer_GetTexelCenterOffset MFRenderer_GetTexelCenterOffset_OpenGL
	#define MFRendererInternal_SortElements MFRendererInternal_SortElements_OpenGL
#endif

#include "MFSystem_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFView_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFRenderTarget_Internal.h"
#include "MFWindow.h"

#include "MFOpenGL.h"

// TODO: REMOVE ME!! MFString needs MFString_SubStr()
#include <string.h>

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
			return NULL;
		}

		int glXMajor, glXMinor;
		if(!glXQueryVersion(xdisplay, &glXMajor, &glXMinor) || (glXMajor == 1 && glXMinor < 3))
		{
			MFDebug_Error(MFStr("Unable to open display, need GLX V1, and at least version 1.3 (Have version %d.%d)", glXMajor, glXMinor));
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
				return NULL;
			}
		}

		if((visualInfo = glXGetVisualFromFBConfig(xdisplay, fbConfigs[0])) == NULL)
		{
			MFDebug_Error("Unable to obtain a visualInfo structure for the associated FBConfig");
			return NULL;
		}

		if(visualInfo->depth < 16)
		{
			MFDebug_Error("Need at least a 16 bit screen!");
			return NULL;
		}

		return visualInfo;
	}

#elif MF_DISPLAY == MF_DRIVER_WIN32

	#pragma comment(lib, "Opengl32")
	#pragma comment(lib, "Glu32")

	extern HINSTANCE apphInstance;
	HGLRC hRC = NULL; // Permanent Rendering Context
	HDC hDC = NULL; // Private GDI Device Context

#elif MF_DISPLAY == MF_DRIVER_SDL2

	#include "SDL2/SDL.h"

	SDL_GLContext glContext;

#elif MF_DISPLAY == MF_DRIVER_IPHONE

	extern "C" int MFRendererIPhone_MakeCurrent();
	extern "C" void MFRendererIPhone_SetBackBuffer();
	extern "C" int MFRendererIPhone_SwapBuffers();

#endif

static GLuint gDefaultRenderTarget = 0;
static MFRenderTarget *gpDeviceRenderTarget = NULL;

static MFTexture *gpDeviceColourTarget = NULL;
static MFTexture *gpDeviceZTarget = NULL;

static MFRect gCurrentViewport;

void MFRenderer_InitModulePlatformSpecific()
{
#if MF_DISPLAY == MF_DRIVER_X11
	glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glXGetProcAddress((const GLubyte*)"glXChooseFBConfig");
	glXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC)glXGetProcAddress((const GLubyte*)"glXGetVisualFromFBConfig");
	glXCreateWindow = (PFNGLXCREATEWINDOWPROC)glXGetProcAddress((const GLubyte*)"glXCreateWindow");
	glXCreateNewContext = (PFNGLXCREATENEWCONTEXTPROC)glXGetProcAddress((const GLubyte*)"glXCreateNewContext");
	glXMakeContextCurrent = (PFNGLXMAKECONTEXTCURRENTPROC)glXGetProcAddress((const GLubyte*)"glXMakeContextCurrent");
#endif
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay(MFDisplay *pDisplay)
{
#if MF_DISPLAY == MF_DRIVER_X11
	Window window = (Window)MFWindow_GetSystemWindowHandle(pDisplay->settings.pWindow);

	glXWindow = glXCreateWindow(xdisplay, fbConfigs[0], window, NULL);
	if(!glXWindow)
	{
		MFDebug_Error("Unable to associate window with a GLXWindow");
		MFRenderer_DestroyDisplay(pDisplay);
		return 1;
	}

	glXContext = glXCreateNewContext(xdisplay, fbConfigs[0], GLX_RGBA_TYPE, NULL, true);
	if(!glXContext)
	{
		MFDebug_Error("Unable to create GLXContext");
		MFRenderer_DestroyDisplay(pDisplay);
		return 1;
	}

	XFree(fbConfigs);
	fbConfigs = NULL;

	if(!glXMakeContextCurrent(xdisplay, glXWindow, glXWindow, glXContext))
	{
		MFDebug_Error("glXMakeContextCurrent failed");
		MFRenderer_DestroyDisplay(pDisplay);
		return 1;
	}
#elif MF_DISPLAY == MF_DRIVER_WIN32
	HWND hWnd = (HWND)MFWindow_GetSystemWindowHandle(pDisplay->settings.pWindow);

	GLuint pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA,
		32, // colour depth
		0, 0, 0, 0, 0, 0,
		0, // No Alpha Buffer
		0, // Shift Bit Ignored
		0, // No Accumulation Buffer
		0, 0, 0, 0, // Accumulation Bits Ignored
		24, // 16Bit Z-Buffer (Depth Buffer)
		8, // No Stencil Buffer
		0, // No Auxiliary Buffer
		PFD_MAIN_PLANE, // Main Drawing Layer
		0, // Reserved
		0, 0, 0 // Layer Masks Ignored
	};

	pfd.dwFlags |= pDisplay->settings.numBuffers > 1 ? PFD_DOUBLEBUFFER : 0;

	hDC = GetDC(hWnd);
	if(!hDC)
	{
		MFRenderer_DestroyDisplay(pDisplay);
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 1;
	}

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(!pixelFormat)
	{
		MFRenderer_DestroyDisplay(pDisplay);
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 2;
	}

	if(!SetPixelFormat(hDC, pixelFormat, &pfd))
	{
		MFRenderer_DestroyDisplay(pDisplay);
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 3;
	}

	hRC = wglCreateContext(hDC);
	if(!hRC)
	{
		// *** driver bug ***
		// HACK: do it again...
		SetPixelFormat(hDC, pixelFormat, &pfd);
		hRC = wglCreateContext(hDC);
	}

	if(!hRC)
	{
		MessageBox(NULL, MFStr("Failed to create OpenGL context: %s", MFSystemPC_GetLastError()), "ERROR", MB_OK|MB_ICONEXCLAMATION);

		MFRenderer_DestroyDisplay(pDisplay);
		return 4;
	}

	if(!wglMakeCurrent(hDC, hRC))
	{
		MFRenderer_DestroyDisplay(pDisplay);
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return 5;
	}
#elif MF_DISPLAY == MF_DRIVER_SDL2
	glContext = SDL_GL_CreateContext((SDL_Window*)MFWindow_GetSystemWindowHandle(pDisplay->settings.pWindow));
#elif MF_DISPLAY == MF_DRIVER_IPHONE
	MFRendererIPhone_MakeCurrent();
#elif MF_DISPLAY == MF_DRIVER_NACL
	// do we need to do anything?
#endif

    // get the opengl version
	const char *pVersion = (const char *)glGetString(GL_VERSION);
	while(pVersion && *pVersion && !MFIsNumeric(*pVersion))
		++pVersion;
	float ver = MFString_AsciiToFloat(pVersion);
	gOpenGLVersion = (int)(ver * 100);

#if !defined(MF_OPENGL_ES)
	// glew wrangles all the horrid extensions...
	GLenum r = glewInit();
	MFDebug_Assert(r == GLEW_OK, "Error loading extensions!");
#endif

#if !defined(MF_OPENGL_ES)
	glEnable(GL_LINE_SMOOTH);

//	glFrontFace(GL_CW);
//	glCullFace(GL_BACK);

	glDisable(GL_LIGHTING);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

#if defined(MF_OPENGL_ES)
	// we need the EGL display apparently...
//	eglSwapInterval(, 1);
#else
#if MF_DISPLAY == MF_DRIVER_X11
//	GLXDrawable drawable = glXGetCurrentDrawable();
//	glXSwapIntervalEXT(xdisplay, drawable, 1);
#elif MF_DISPLAY == MF_DRIVER_WIN32
//	wglSwapInterval(1);
#endif
#endif

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&gDefaultRenderTarget);

	MFTextureDesc texDesc = { MFTexType_2D, ImgFmt_A8R8G8B8, pDisplay->settings.width, pDisplay->settings.height, 0, 0, 1, MFTCF_RenderTarget };
	gpDeviceColourTarget = MFTexture_InitTexture(&texDesc, MFRD_OpenGL, 0);
	gpDeviceColourTarget->pName = "Device Colour Target";
	gpDeviceColourTarget->pSurfaces[0].platformData = (uint64)gDefaultRenderTarget;

	texDesc.format = ImgFmt_D24S8;
	gpDeviceZTarget = MFTexture_InitTexture(&texDesc, MFRD_OpenGL, 0);
	gpDeviceZTarget->pName = "Device Depth Stencil";
	gpDeviceZTarget->pSurfaces[0].platformData = 0;

	MFRenderTargetDesc desc;
	desc.pName = "Device Render Target";
	desc.width = pDisplay->settings.width;
	desc.height = pDisplay->settings.height;
	desc.colourTargets[0].pSurface = gpDeviceColourTarget;
	desc.depthStencil.pSurface = gpDeviceZTarget;
	gpDeviceRenderTarget = MFRenderTarget_Create(&desc);

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)pDisplay->settings.width;
	gCurrentViewport.height = (float)pDisplay->settings.height;
	glViewport(0, 0, pDisplay->settings.width, pDisplay->settings.height);

	return 0;
}

void MFRenderer_DestroyDisplay(MFDisplay *pDisplay)
{
	if(gpDeviceRenderTarget)
		MFRenderTarget_Release(gpDeviceRenderTarget);

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
	HWND hWnd = (HWND)MFWindow_GetSystemWindowHandle(pDisplay->settings.pWindow);

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
		ReleaseDC(hWnd, hDC);
		hDC = NULL;
	}
#elif MF_DISPLAY == MF_DRIVER_SDL2
	SDL_GL_DeleteContext(glContext);
#endif
}

bool MFRenderer_ResetDisplay(MFDisplay *pDisplay, const MFDisplaySettings *pSettings)
{
	MFDebug_Warn(4, MFStr("MFRenderer_ResetDisplay(%d, %d, %s)", pSettings->width, pSettings->height, pSettings->bFullscreen ? "true" : "false"));

	bool bFullscreenChanged = pDisplay->settings.bFullscreen != pSettings->bFullscreen;

	// change display mode...
	if(bFullscreenChanged || (pSettings->bFullscreen && (pDisplay->settings.width != pSettings->width || pDisplay->settings.height != pSettings->height)))
	{
#if MF_DISPLAY == MF_DRIVER_WIN32
		if(pSettings->bFullscreen)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = pSettings->width;
			dmScreenSettings.dmPelsHeight = pSettings->height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				MFDebug_Warn(2, "The requested fullscreen mode is not supported by your video card.");
				return false;
			}

			if(bFullscreenChanged)
				ShowCursor(FALSE);
		}
		else
		{
			if(ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
			{
				MFDebug_Warn(2, "Unable to return to windowed mode!");
				return false;
			}

			if(bFullscreenChanged)
				ShowCursor(TRUE);
		}
#endif
	}

	gpDeviceColourTarget->imageFormat = ImgFmt_A8R8G8B8;
	gpDeviceColourTarget->pSurfaces[0].width = pSettings->width;
	gpDeviceColourTarget->pSurfaces[0].height = pSettings->height;
	gpDeviceColourTarget->pSurfaces[0].platformData = (uint64)gDefaultRenderTarget;

	gpDeviceZTarget->imageFormat = ImgFmt_D24S8;
	gpDeviceZTarget->pSurfaces[0].width = pSettings->width;
	gpDeviceZTarget->pSurfaces[0].height = pSettings->height;
	gpDeviceZTarget->pSurfaces[0].platformData = 0;

	gpDeviceRenderTarget->width = pSettings->width;
	gpDeviceRenderTarget->height = pSettings->height;

	MFRenderer_ResetViewport();

	return true;
}

void MFRenderer_LostFocus(MFDisplay *pDisplay)
{
#if MF_DISPLAY == MF_DRIVER_WIN32
	if(pDisplay->settings.bFullscreen)
	{
		if(ChangeDisplaySettings(NULL, 0) == DISP_CHANGE_SUCCESSFUL)
			ShowCursor(TRUE);
		else
			MFDebug_Warn(2, "Unable to return to windowed mode!");
	}
#endif
}

void MFRenderer_GainedFocus(MFDisplay *pDisplay)
{
#if MF_DISPLAY == MF_DRIVER_WIN32
	if(pDisplay->settings.bFullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = pDisplay->settings.width;
		dmScreenSettings.dmPelsHeight = pDisplay->settings.height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
			ShowCursor(FALSE);
		else
			MFDebug_Warn(2, "Couldn't change display mode!");
	}
#endif
}

bool MFRenderer_BeginFramePlatformSpecific()
{
	if(MFCheckForOpenGLError())
		return false;

#if defined(MF_IPHONE)
	MFRendererIPhone_MakeCurrent();
#endif
	MFRenderer_SetDeviceRenderTarget();
	MFRenderer_ResetViewport();

	if(MFCheckForOpenGLError())
		return false;

	return true;
}

void MFRenderer_EndFramePlatformSpecific()
{
	MFCALLSTACK;

#if MF_DISPLAY == MF_DRIVER_X11
	glXSwapBuffers(xdisplay, glXWindow);
#elif MF_DISPLAY == MF_DRIVER_WIN32
	SwapBuffers(hDC);
#elif MF_DISPLAY == MF_DRIVER_SDL2
	MFDisplay *pDisplay = MFDisplay_GetCurrent();
	if(pDisplay)
		SDL_GL_SwapWindow((SDL_Window*)MFWindow_GetSystemWindowHandle(pDisplay->settings.pWindow));
#elif MF_DISPLAY == MF_DRIVER_IPHONE
	MFRendererIPhone_SwapBuffers();
#endif

	MFCheckForOpenGLError(true);
}

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags, const MFVector &colour, float z, int stencil)
{
	MFCALLSTACK;

	int mask = ((flags & MFRCF_Colour) ? GL_COLOR_BUFFER_BIT : 0) |
				((flags & MFRCF_Depth) ? GL_DEPTH_BUFFER_BIT : 0) |
				((flags & MFRCF_Stencil) ? GL_STENCIL_BUFFER_BIT : 0);

	glClearColor(colour.x, colour.y, colour.z, colour.w);
	glClearDepth(z);
	glClearStencil(stencil);
	glClear(mask);

	MFCheckForOpenGLError();
}

MF_API void MFRenderer_SetViewport(MFRect *pRect)
{
	MFCALLSTACK;

	gCurrentViewport = *pRect;
	glViewport((GLint)pRect->x, (GLint)pRect->y, (GLint)pRect->width, (GLint)pRect->height);
	MFCheckForOpenGLError();
}

MF_API void MFRenderer_ResetViewport()
{
	MFCALLSTACK;

	MFDisplay *pDisplay = MFDisplay_GetCurrent();

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)pDisplay->settings.width;
	gCurrentViewport.height = (float)pDisplay->settings.height;

	glViewport(0, 0, pDisplay->settings.width, pDisplay->settings.height);
	MFCheckForOpenGLError();
}

MF_API MFRenderTarget* MFRenderer_GetDeviceRenderTarget()
{
	return gpDeviceRenderTarget;
}

void MFRenderer_SetRenderTargetPlatformSpecific(MFRenderTarget *pRenderTarget)
{
	MFCheckForOpenGLError();

	GLuint buffer = (GLuint)(uintp)pRenderTarget->pPlatformData;
#if defined(MF_IPHONE)
	if(buffer == gDefaultRenderTarget)
		MFRendererIPhone_SetBackBuffer();
	else
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);

	if(pRenderTarget->pDepthStencil)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	MFCheckForOpenGLError();

	MFRect viewport = { 0.f, 0.f, (float)pRenderTarget->width, (float)pRenderTarget->height };
	MFRenderer_SetViewport(&viewport);
}

MF_API float MFRenderer_GetTexelCenterOffset()
{
	return 0.f;
}


/*
opengl state change costs (descending cost):
  render target
  program
  rop
  texture binding
  vertex format
  UBO binding
  vertex binding
  uniform update
*/

static int SortDefault(const void *p1, const void *p2)
{
	MFRenderElement *pE1 = (MFRenderElement*)p1;
	MFRenderElement *pE2 = (MFRenderElement*)p2;

	int pred = pE1->primarySortKey - pE2->primarySortKey;
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterial - (char*)pE1->pMaterial);
	if(pred) return pred;
	pred = (int)((char*)pE2->pViewState - (char*)pE1->pViewState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pEntityState - (char*)pE1->pEntityState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialOverrideState - (char*)pE1->pMaterialOverrideState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialState - (char*)pE1->pMaterialState);
	return pred;
}

static int SortBackToFront(const void *p1, const void *p2)
{
	MFRenderElement *pE1 = (MFRenderElement*)p1;
	MFRenderElement *pE2 = (MFRenderElement*)p2;

	int pred = pE1->primarySortKey - pE2->primarySortKey;
	if(pred) return pred;
	pred = pE2->zSort - pE1->zSort;
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterial - (char*)pE1->pMaterial);
	if(pred) return pred;
	pred = (int)((char*)pE2->pViewState - (char*)pE1->pViewState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pEntityState - (char*)pE1->pEntityState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialOverrideState - (char*)pE1->pMaterialOverrideState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialState - (char*)pE1->pMaterialState);
	return pred;
}

static int SortFrontToBack(const void *p1, const void *p2)
{
	MFRenderElement *pE1 = (MFRenderElement*)p1;
	MFRenderElement *pE2 = (MFRenderElement*)p2;

	int pred = pE1->primarySortKey - pE2->primarySortKey;
	if(pred) return pred;
	pred = pE1->zSort - pE2->zSort;
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterial - (char*)pE1->pMaterial);
	if(pred) return pred;
	pred = (int)((char*)pE2->pViewState - (char*)pE1->pViewState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pEntityState - (char*)pE1->pEntityState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialOverrideState - (char*)pE1->pMaterialOverrideState);
	if(pred) return pred;
	pred = (int)((char*)pE2->pMaterialState - (char*)pE1->pMaterialState);
	return pred;
}

static MFRenderSortFunction gSortFunctions[MFRL_SM_Max] =
{
	SortDefault,
	SortFrontToBack,
	SortBackToFront
};

void MFRendererInternal_SortElements(MFRenderLayer &layer)
{
	qsort(layer.elements.getPointer(), layer.elements.size(), sizeof(MFRenderElement), gSortFunctions[layer.sortMode]);
}

#if defined(_DEBUG)
bool MFCheckForOpenGLError(bool bBreakOnError)
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
	{
#if !defined(MF_OPENGL_ES)
		const GLubyte *errorString = gluErrorString(err);
		if(bBreakOnError)
		{
			MFDebug_Assert(err == GL_NO_ERROR, MFStr("OpenGL Error %04X: %s", err, errorString));
		}
		else
		{
			MFDebug_Warn(1, MFStr("OpenGL Error %04X: %s", err, errorString));
		}
#else
		if(bBreakOnError)
		{
			MFDebug_Assert(err == GL_NO_ERROR, MFStr("OpenGL Error: %04X", err));
		}
		else
		{
			MFDebug_Warn(1, MFStr("OpenGL Error: %04X", err));
		}
#endif
		return true;
	}
	return false;
}
#endif

#endif
