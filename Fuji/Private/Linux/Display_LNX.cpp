#include <stdio.h>

#include "Common.h"
#include "Display.h"
#include "View.h"
#include "DebugMenu.h"
#include "Heap.h"

// Typedefs
struct Resolution {
	uint32 width, height;
};

// Data definition
Resolution defaultModes[] = {{320, 240}, {640, 480}, {800, 600}, {1024, 768}, {1280, 1024}, {0, 0}};

// Variables
static char **modeStrings = NULL;
static Resolution *modes = NULL;
MenuItemIntString *modesMenu;

SDL_Surface *screen;

bool isortho = false;
float fieldOfView;


bool Display_GetModes(Resolution **_modes, char ***_modeStrings, bool fullscreen);
void Display_FreeModes();
void Display_ResetDisplay();


bool Display_GetModes(Resolution **_modes, char ***_modeStrings, bool fullscreen)
{
	CALLSTACK;
	
	SDL_Rect **rects;
	uint32 flags = SDL_OPENGL;
	uint32 numModes = 0;

	Display_FreeModes();

	flags |= (fullscreen? SDL_FULLSCREEN : 0);
	rects = SDL_ListModes(NULL, flags);
	
	if(rects == NULL) { /* No modes available */
		return(false);
	}

	if(rects == (SDL_Rect **)-1) { /* Resolution is unrestricted, use the defaults */
		modes = defaultModes;
	
		for(uint32 i = 0; modes[i].width != 0; ++i) {
			++numModes;
		}
	} else {
		for(uint32 i = 0; rects[i] != NULL; ++i) {
			++numModes;
		}
	
		modes = (Resolution *)Heap_Alloc(sizeof(Resolution) * (numModes + 1));
		for(uint32 i = 0; i < numModes; i++) {
			modes[i].width = rects[i]->w;
			modes[i].height = rects[i]->h;
		}
		
		modes[numModes].width = 0;
		modes[numModes].height = 0;
	}


	/* Allocate one contiguous string for all the modes */
	/* Assume each mode isn't going to be longer than 14 characters */
	/* (WWWWWWxHHHHHH\0) */
	modeStrings = (char **)Heap_Alloc(sizeof(char *) * (numModes + 1));

	modeStrings[0] = NULL;
	modeStrings[numModes] = NULL;

	char *stringBuffer = (char *)Heap_Alloc(sizeof(char) * 14 * numModes);
	char *p = stringBuffer;
	
	for(uint32 i = 0; i < numModes; ++i) {
		uint32 amountWritten;

		amountWritten = snprintf(p, 14, "%dx%d", modes[i].width, modes[i].height);
		++amountWritten; // It doesn't include the \0 in this

		// It returns the amount that _would_ have been written if it truncates
		amountWritten = Min<size_t>(14, amountWritten);
		modeStrings[i] = p;
		p += amountWritten;
	}

	if(_modes != NULL)
		*_modes = modes;
	
	if(_modeStrings != NULL)
		*_modeStrings = modeStrings;

	return(true);
}

void Display_FreeModes()
{
	CALLSTACK;
	
	if(modes != NULL) {
		if(modes != defaultModes) {
			Heap_Free(modes);
		}
		
		modes = NULL;
		
		if(modeStrings[0] != NULL) {
			Heap_Free(modeStrings[0]);
		}

		Heap_Free(modeStrings);
		modeStrings = NULL;
	}
}

void ChangeResCallback(MenuObject *pMenu, void *pData)
{
	MenuItemIntString *pRes = static_cast<MenuItemIntString *>(pMenu);
	
	display.width = modes[pRes->data - 1].width;
	display.height = modes[pRes->data - 1].height;

	display.fullscreenWidth = display.width;
	display.fullscreenHeight = display.height;

	Display_ResetDisplay();
}

int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	CALLSTACK;

	Display_FreeModes(); /* Just in case */

	display.fullscreenWidth = display.width = width;
	display.fullscreenHeight = display.height = height;
	display.refreshRate = 0; /* SDL doesn't know */
	display.colourDepth = 0; /* Use default.  Chances are, it's something sane */
	display.windowed = true;
	display.wide = false;
	display.progressive = false;

	/* These are minimums */
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	uint32 flags = SDL_OPENGL;
	flags |= (display.windowed? 0 : SDL_FULLSCREEN);

	if((screen = SDL_SetVideoMode(display.width, display.height, display.colourDepth, flags)) == NULL) {
		
		fprintf(stderr, "Unable to create display\n");
		return(1);
	}

#if !defined(_DEBUG) // Really messes with the debugger
	SDL_WM_GrabInput(SDL_GRAB_ON); // FIXME: Should provide a way to ungrab the input, a menu item perhaps?
	SDL_ShowCursor(SDL_DISABLE);
#endif

	SDL_WM_SetCaption("Fuji Window", NULL);

	Display_GetModes(NULL, NULL, !display.windowed);


	/* Can this be freed?  If so, when? */
	modesMenu = new MenuItemIntString(modeStrings, 2);
	DebugMenu_AddItem("Resolution", "Display Options", modesMenu, ChangeResCallback);

	
	glEnable(GL_LINE_SMOOTH);
	
//	glFrontFace(GL_CW);
//	glCullFace(GL_BACK);

	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	
	return(0);
}

void Display_ResetDisplay()
{
	CALLSTACK;
}

void Display_DestroyDisplay()
{
	CALLSTACK;

	Display_FreeModes(); /* It doesn't really get closed until SDL_Quit is called */
}

void Display_BeginFrame()
{
	CALLSTACK;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Display_EndFrame()
{
	CALLSTACK;

	SDL_GL_SwapBuffers();
}

void Display_ClearScreen(uint32 flags)
{
	CALLSTACK;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
