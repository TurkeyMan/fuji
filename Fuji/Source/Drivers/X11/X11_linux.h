#ifndef X11_LINUX_H
#define X11_LINUX_H
#include <X11/Xutil.h>
#if defined(__CYGWIN__)
	#include <extensions/xf86vmode.h>
#else
	#include <X11/extensions/xf86vmode.h>
#endif

extern Display *xdisplay;
extern int screen;
extern Window rootWindow;

extern XF86VidModeModeInfo *originalVidMode;
extern XF86VidModeModeInfo **vidModes;

struct XMouseState
{
	int x, y;
	int prevX, prevY;
	int wheel;
	uint8 buttons[16];
};

#endif
