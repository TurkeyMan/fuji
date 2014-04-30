module fuji.window;

public import fuji.c.MFWindow;
import fuji.c.MFTypes;
import fuji.c.MFDisplay;

struct Window
{
	MFWindow *pWindow;
	alias pWindow this;

	// TODO: methods, assignment, setter's.

	@property ref const(MFWindowParams) params() const pure nothrow { return *MFWindow_GetWindowParameters(pWindow); }

	@property MFRect rect() const pure nothrow { return MFRect(params.x, params.y, params.width, params.height); }

	@property const(char)[] title() const pure nothrow { return params.windowTitle; }
	@property bool isFullscreen() const pure nothrow { return params.bFullscreen; }

	@property MFDisplay* display() pure nothrow { return MFWindow_GetDisplay(pWindow); }
	@property void* systemHandle() pure nothrow { return MFWindow_GetSystemWindowHandle(pWindow); }
}