#include <stdio.h>

#include "Common.h"
#include "Display.h"
#include "View.h"
#include "DebugMenu.h"
#include "Heap.h"

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>

// Typedefs
struct Resolution {
	int width, height;
	float refresh;
};

// Data definition
Resolution defaultModes[] = {{320, 240, 0.0f}, {640, 480, 0.0f}, {800, 600, 0.0f}, {1024, 768, 0.0f}, {1280, 1024, 0.0f}, {0, 0, 0.0f}};
char modeString[24] = "";
char *resMenuItems[] = {"-", modeString, "+", NULL};

MenuItemIntString resSelect(resMenuItems, 1);
MenuItemStatic applyDisplayMode;

const int glAttrsSingle[] = {
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, /* Don't need to draw to anything else */
	GLX_DEPTH_SIZE, 16,
	GLX_STENCIL_SIZE, 8,
	GLX_DOUBLEBUFFER, true,
	None
};

const int glAttrsDouble[] = {
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_DEPTH_SIZE, 16,
	GLX_STENCIL_SIZE, 8,
	None
};

// Variables
Resolution *modes = NULL;
MenuItemIntString *modesMenu;
int32 currentMode = -1, selectedMode = -1;
int32 numModes;

Display *xdisplay = NULL;
int screen = 0;
Window window = 0;
Window rootWindow;
XF86VidModeModeInfo *originalVidMode = NULL;
XF86VidModeModeInfo **vidModes = NULL;
GLXWindow glXWindow;
GLXContext glXContext = NULL;
XSizeHints *sizeHints = NULL;
Colormap colorMap = 0;
Atom wm_delete_window;

bool isortho = false;
float fieldOfView;

extern MFVector gClearColour;

static bool GetModes(Resolution **_modes, bool fullscreen);
static void SetSingleMode(Resolution **modes);
static void FreeModes();
static bool FindMode(Resolution *modes, int width, int height);
static void Display_ResetDisplay();

static Bool WaitForNotify(Display *d, XEvent *e, char *arg)
{ 
	return((e->type == MapNotify) && (e->xmap.window == (Window)arg));
}

static bool GetModes(Resolution **_modes, bool fullscreen)
{
	CALLSTACK;

	int numModeLines;

	if(!fullscreen) {
		modes = defaultModes;

		for(uint32 i = 0; modes[i].width != 0; ++i) {
			++numModes;
		}
	} else {
		int throwaway;
		if(!XF86VidModeQueryExtension(xdisplay, &throwaway, &throwaway)) {
			SetSingleMode(_modes);
			return(true);
		}
		
		if((!XF86VidModeGetAllModeLines(xdisplay, screen, &numModeLines, &vidModes)) || numModeLines < 2) {
			SetSingleMode(_modes);
			return(true);
		}

		originalVidMode = vidModes[0];
		numModes = (uint32)numModeLines;
	
		modes = (Resolution *)Heap_Alloc(sizeof(Resolution) * (numModes + 1));

		for(int32 i = 0; i < numModes; i++) {
			modes[i].width = vidModes[i]->hdisplay;
			modes[i].height = vidModes[i]->vdisplay;
			modes[i].refresh = ((float)vidModes[i]->dotclock / (float)vidModes[i]->htotal) / (float)vidModes[i]->htotal;
		}
	
		modes[numModes].width = 0;
		modes[numModes].height = 0;
	}
	

	if(_modes != NULL)
		*_modes = modes;

	return(true);
}

static void SetSingleMode(Resolution **modes)
{
	*modes = (Resolution *)Heap_Alloc(sizeof(Resolution) * 2);

	(*modes)[0].width = DisplayWidth(xdisplay, screen);
	(*modes)[0].height = DisplayHeight(xdisplay, screen);
	(*modes)[0].refresh = 0;

	(*modes)[1].width = 0;
	(*modes)[1].height = 0;
	(*modes)[1].refresh = 0;

	originalVidMode = vidModes[0];

	numModes = 1;
}

static void FreeModes()
{
	CALLSTACK;
	
	if(modes != NULL) {
		if(modes != defaultModes) {
			Heap_Free(modes);
		}
		
		modes = NULL;
	}

	if(vidModes != NULL) {
		XFree(vidModes);
		vidModes = NULL;
	}
}

static bool FindMode(Resolution *modes, int width, int height)
{
	currentMode = -1;
	for(int32 i=0; i < numModes; i++) {
		if(width == modes[i].width && height == modes[i].height) {
			currentMode = i;
			break;
		}
	}

	return((currentMode != -1));
}

void ChangeResCallback(MenuObject *pMenu, void *pData)
{
	MenuItemIntString *pRes = (MenuItemIntString *)pMenu;

	if(pRes->data == 0) {
		selectedMode--;
		if(selectedMode < 0) {
			selectedMode = numModes - 1;
		}
	} else if(pRes->data == 2) {
		selectedMode++;
		if(selectedMode == numModes) {
			selectedMode = 0;
		}
	}

	pRes->data = 1;
	snprintf(modeString, 24, "%dx%d (%.02fMhz)", modes[selectedMode].width, modes[selectedMode].height, modes[selectedMode].refresh);
}

void ApplyDisplayModeCallback(MenuObject *pMenu, void *pData)
{
	currentMode = selectedMode;
	display.width = modes[currentMode].width;
	display.height = modes[currentMode].height;

	display.fullscreenWidth = display.width;
	display.fullscreenHeight = display.height;


	Display_ResetDisplay();	
}

int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	CALLSTACK;

	XVisualInfo *visualInfo = NULL;

	display.fullscreenWidth = display.width = width;
	display.fullscreenHeight = display.height = height;
	display.refreshRate = 0;
	display.colourDepth = 0; /* Use default.  Chances are, it's something sane */
	display.windowed = true;
	display.wide = false;
	display.progressive = false;



	if(!(xdisplay = XOpenDisplay(NULL))) {
		LOGD("Unable to open display");
		Display_DestroyDisplay();
		return(1);
	}

	screen = DefaultScreen(xdisplay);
	rootWindow = RootWindow(xdisplay, screen);

	GetModes(&modes, !display.windowed);
	if(!FindMode(modes, width, height)) {
		if(!display.windowed) { // Try windowed mode
			LOGD("No suitable modes for fullscreen mode, trying windowed mode");
			
			display.windowed = true;

			FreeModes();
			GetModes(&modes, false);
			if(!FindMode(modes, width, height)) {
				LOGD("No suitable modes found");
				Display_DestroyDisplay();
				return(1);
			}
		} else {
			LOGD("No suitable modes found");
			Display_DestroyDisplay();
			return(1);
		}
	}


	DebugMenu_AddItem("Resolution", "Display Options", &resSelect, ChangeResCallback);
	DebugMenu_AddItem("Apply", "Display Options", &applyDisplayMode, ApplyDisplayModeCallback);

	if(!glXQueryExtension(xdisplay, NULL, NULL)) {
		LOGD("GLX extension not available");
		Display_DestroyDisplay();
		return(1);
	}
	
	int glXMajor, glXMinor;
	if(!glXQueryVersion(xdisplay, &glXMajor, &glXMinor) || (glXMajor == 1 && glXMinor < 3)) {
		LOGD(STR("Unable to open display, need GLX V1, and at least version 1.3 (Have version %d.%d)", glXMajor, glXMinor));
		Display_DestroyDisplay();
		return(1);
	}

	// Check OpenGL version




	// Set full screen mode, if necessary
	if(!display.windowed && numModes > 1) {
		if(!XF86VidModeSwitchToMode(xdisplay, screen, vidModes[currentMode])) {
			LOGD("Unable to switch screenmodes, defaulting to windowed mode");
			Display_DestroyDisplay();
			return(1);
		}
	}

	// Try and obtain a suitable FBconfig, try for double buffering first
	GLXFBConfig *fbConfigs;
	int numConfigs;
	fbConfigs = glXChooseFBConfig(xdisplay, screen, glAttrsDouble, &numConfigs);
	if(numConfigs == 0) {
		fbConfigs = glXChooseFBConfig(xdisplay, screen, glAttrsDouble, &numConfigs);
		if(numConfigs == 0) {
			LOGD("Unable to obtain a suitable glX FBConfig");
			Display_DestroyDisplay();
			return(1);
		}
	}

	if((visualInfo = glXGetVisualFromFBConfig(xdisplay, fbConfigs[0])) == NULL) {
		LOGD("Unable to obtain a visualInfo structure for the associated FBConfig");
		XFree(fbConfigs);
		Display_DestroyDisplay();		
		return(1);
	}

	if(visualInfo->depth < 16) {
		LOGD("Need at least a 16 bit screen!");
		XFree(fbConfigs);
		Display_DestroyDisplay();		
		return(1);
	}

	
	if(!(colorMap = XCreateColormap(xdisplay, rootWindow, visualInfo->visual, AllocNone))) {
		LOGD("Unable to create colourmap");
		XFree(fbConfigs);
		XFree(visualInfo);
		Display_DestroyDisplay();		
		return(1);
	}

	XSetWindowAttributes windowAttrs;
	windowAttrs.colormap = colorMap;
	windowAttrs.cursor = None;
	windowAttrs.event_mask = StructureNotifyMask;
	windowAttrs.border_pixel = BlackPixel(xdisplay, screen);
	windowAttrs.background_pixel = BlackPixel(xdisplay, screen);
	
	if(!(window = XCreateWindow(xdisplay, rootWindow, 0, 0, width, height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWBackPixel | CWBorderPixel | CWCursor | CWColormap | CWEventMask, &windowAttrs))) {
		LOGD("Unable to create X Window");
		XFree(fbConfigs);
		XFree(visualInfo);
		Display_DestroyDisplay();		
		return(1);
	}


	// Tell the window manager not to allow our window to be resized.  But some window managers can ignore me and do it anyway.  Typical X-Windows.

	if((sizeHints = XAllocSizeHints()) == NULL) {
		LOGD("Unable to alloc XSizeHints structure, out of memory?");
		XFree(fbConfigs);
		XFree(visualInfo);
		Display_DestroyDisplay();		
		return(1);
	}
	
	sizeHints->flags = PSize | PMinSize | PMaxSize;
    sizeHints->min_width = sizeHints->max_width = sizeHints->base_width = width;
    sizeHints->min_height = sizeHints->max_height = sizeHints->base_height = height;	
	
	XSetWMNormalHints(xdisplay, window, sizeHints);


	// Window title
	XStoreName(xdisplay, window, "Fuji Window");
	
	
	XWMHints *wmHints;
	if((wmHints = XAllocWMHints()) == NULL) {
		LOGD("Unable to alloc XWMHints structure, out of memory?");
		XFree(fbConfigs);
		XFree(visualInfo);
		Display_DestroyDisplay();		
		return(1);
	}


	wmHints->flags = InputHint | StateHint;
	wmHints->input = true;
	wmHints->initial_state = NormalState;
	if(!XSetWMHints(xdisplay, window, wmHints)) {
		LOGD("Unable to set WM hints for window");
		XFree(fbConfigs);
		XFree(visualInfo);
		Display_DestroyDisplay();
		return(1);
	}

	XFree(wmHints);	

	// Tell the window manager that I want to be notified if the window's closed
	wm_delete_window = XInternAtom(xdisplay, "WM_DELETE_WINDOW", false);
	if(!XSetWMProtocols(xdisplay, window, &wm_delete_window, 1)) {
		LOGD("Unable to set Window Manager protocols");
		XFree(fbConfigs);
		Display_DestroyDisplay();		
		return(1);
	}


	if(!XMapRaised(xdisplay, window)) {
		LOGD("Unable to map new window");
		XFree(fbConfigs);
		Display_DestroyDisplay();		
		return(1);
	}

	// Wait for the window to be mapped, etc. The documentation doesn't indicate that this is necessary, but every GLX program I've ever seen does it, so I assume it is.
	XEvent event;
	XIfEvent(xdisplay, &event, WaitForNotify, (char *)window);

	XFree(visualInfo);

	if(!(glXWindow = glXCreateWindow(xdisplay, fbConfigs[0], window, NULL))) {
		LOGD("Unable to associate window with a GLXWindow");
		XFree(fbConfigs);
		Display_DestroyDisplay();
		return(1);
	}

	if(!(glXContext = glXCreateNewContext(xdisplay, fbConfigs[0], GLX_RGBA_TYPE, NULL, true))) {
		LOGD("Unable to create GLXContext");
		XFree(fbConfigs);
		Display_DestroyDisplay();
		return(1);
	}

	XFree(fbConfigs);

	if(!glXMakeContextCurrent(xdisplay, glXWindow, glXWindow, glXContext)) {
		LOGD("Unable to bind GLXContext");
		Display_DestroyDisplay();
		return(1);
	}

	// Can't find this out until the context has been created
	const char *glVersionStr = (const char *)glGetString(GL_VERSION);
	int32 majorGLVersion, minorGLVersion;

	if(sscanf(glVersionStr, "%d.%d.%*d", &majorGLVersion, &minorGLVersion) != 2) {
		if(sscanf(glVersionStr, "%d.%d", &majorGLVersion, &minorGLVersion) != 2) {
			LOGD("Unable to determine OpenGl version");
			Display_DestroyDisplay();
			return(1);
		}
	}

	if(majorGLVersion == 1 && minorGLVersion < 4) {
		LOGD("Need at least OpenGL version 1.4");
		Display_DestroyDisplay();
		return(1);
	}

	// Might want to check for extensions here


	if(!display.windowed && numModes > 1) {
		if(!XF86VidModeSwitchToMode(xdisplay, screen, vidModes[currentMode])) {
			LOGD("Unable to set screen mode");
			Display_DestroyDisplay();
			return(1);
		}

		XGrabPointer(xdisplay, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);


		XFlush(xdisplay);
		// A little trick to make sure the entire window is on the screen
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, width - 1, height - 1);
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, 0, 0);
		XFlush(xdisplay);
	}

	glEnable(GL_LINE_SMOOTH);
	
//	glFrontFace(GL_CW);
//	glCullFace(GL_BACK);

	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	return(0);
}

static void Display_ResetDisplay()
{
	CALLSTACK;

	sizeHints->flags = PSize | PMinSize | PMaxSize;
	sizeHints->min_width = sizeHints->max_width = sizeHints->base_width = display.width;
	sizeHints->min_height = sizeHints->max_height = sizeHints->base_height = display.height;

	XSetWMNormalHints(xdisplay, window, sizeHints);
	XResizeWindow(xdisplay, window, display.width, display.height);

	if(!display.windowed && numModes > 1) {
		XF86VidModeSwitchToMode(xdisplay, screen, vidModes[currentMode]);
	
		XGrabPointer(xdisplay, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
		XFlush(xdisplay);

		// A little trick to make sure the entire window is on the screen
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, display.width - 1, display.height - 1);
		XWarpPointer(xdisplay, None, window, 0, 0, 0, 0, 0, 0);	
		XFlush(xdisplay);
	}

	ResetViewport();
}

void Display_DestroyDisplay()
{
	CALLSTACK;

	if((!display.windowed) && (originalVidMode != NULL) && xdisplay != NULL && numModes > 1) {
		XF86VidModeSwitchToMode(xdisplay, screen, originalVidMode);
	}

	if(glXContext != NULL) {
		glXDestroyContext(xdisplay, glXContext);
		glXContext = NULL;
	}

	if(glXWindow != 0) {
		glXDestroyWindow(xdisplay, glXWindow);
		glXWindow = 0;
	}

	if(sizeHints != NULL) {
		XFree(sizeHints);
		sizeHints = NULL;
	}

	if(window != 0) {
		XDestroyWindow(xdisplay, window);
		window = 0;
	}

	if(colorMap != 0) {
		XFreeColormap(xdisplay, colorMap);
		colorMap = 0L;
	}

	if(xdisplay != NULL) {
		XCloseDisplay(xdisplay);
		xdisplay = NULL;
	}

	FreeModes();
}

void Display_BeginFrame()
{
	CALLSTACK;
}

void Display_EndFrame()
{
	CALLSTACK;

	glXSwapBuffers(xdisplay, glXWindow);
}

void Display_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void Display_ClearScreen(uint32 flags)
{
	CALLSTACK;

	glClearColor(gClearColour.x, gClearColour.y, gClearColour.z, gClearColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void SetViewport(float x, float y, float width, float height)
{
	CALLSTACK;

	glViewport((GLint)((x/640.0f) * (float)display.width), (GLint)((y/480.0f) * (float)display.height), (GLint)((width/640.0f) * (float)display.width), (GLint)((height/480.0f) * (float)display.height));
}

void ResetViewport()
{
	CALLSTACK;

	SetViewport(0, 0, display.width, display.height);
}
