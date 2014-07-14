module fuji.window;

public import fuji.c.MFWindow;
import fuji.c.MFTypes;
import fuji.c.MFDisplay;

nothrow:
@nogc:

struct Window
{
	MFWindow *pWindow;
	alias pWindow this;

	// TODO: methods, assignment, setter's.

nothrow:
@nogc:
	@property ref const(MFWindowParams) params() const pure { return *MFWindow_GetWindowParameters(pWindow); }

	@property MFRect rect() const pure { return MFRect(params.x, params.y, params.width, params.height); }

	@property const(char)[] title() const pure { return params.windowTitle; }
	@property bool isFullscreen() const pure { return params.bFullscreen; }

	@property MFDisplay* display() pure { return MFWindow_GetDisplay(pWindow); }
	@property void* systemHandle() pure { return MFWindow_GetSystemWindowHandle(pWindow); }
}
