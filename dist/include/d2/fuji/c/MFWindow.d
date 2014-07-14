module fuji.c.MFWindow;

import fuji.c.Fuji;
import fuji.c.MFDisplay;

nothrow:
@nogc:

struct MFWindow;

enum MFWindowFlags : int
{
	WindowFrame = MFBit!0,
	CanResize = MFBit!1,
	AlwaysOnTop = MFBit!2,

	Max
};

struct MFWindowParams
{
	const(char)* pWindowTitle;

	int x;
	int y;
	int width;
	int height;

	uint flags;	// MFWindowFlags

	int monitor;

	bool bFullscreen;

	@property const(char)[] windowTitle() const pure nothrow @nogc { return pWindowTitle.toDStr; }
};

extern (C) MFWindow* MFWindow_Create(MFWindowParams* pParams);
extern (C) MFWindow* MFWindow_BindExisting(void* pWindowHandle);
extern (C) void MFWindow_Destroy(MFWindow* pWindow);

extern (C) void MFWindow_Update(MFWindow* pWindow, const(MFWindowParams)* pParams);

extern (C) const(MFWindowParams)* MFWindow_GetWindowParameters(const(MFWindow)* pWindow) pure;
extern (C) MFDisplay* MFWindow_GetDisplay(MFWindow* pWindow) pure;
extern (C) void* MFWindow_GetSystemWindowHandle(MFWindow* pWindow) pure;
