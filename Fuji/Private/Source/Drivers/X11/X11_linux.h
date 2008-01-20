#ifndef X11_LINUX_H
#define X11_LINUX_H
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#if defined(__CYGWIN__)
	#include <extensions/xf86vmode.h>
#else
	#include <X11/extensions/xf86vmode.h>
#endif

extern Display *xdisplay;
extern int screen;
extern Window window;
extern Window rootWindow;
extern XF86VidModeModeInfo *originalVidMode;
extern XF86VidModeModeInfo **vidModes;
extern Colormap colorMap;
extern Atom wm_delete_window;

#endif
