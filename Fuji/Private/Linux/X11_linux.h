#ifndef X11_LINUX_H
#define X11_LINUX_H
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/Xutil.h>

Display *xdisplay = NULL;
int screen = 0;
Window window = 0;
Window rootWindow;
XF86VidModeModeInfo *originalVidMode = NULL;
XF86VidModeModeInfo **vidModes = NULL;
XSizeHints *sizeHints = NULL;
Colormap colorMap = 0;
Atom wm_delete_window;

#endif
