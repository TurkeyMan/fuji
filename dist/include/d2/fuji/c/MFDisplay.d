module fuji.c.MFDisplay;

import fuji.c.Fuji;
import fuji.c.MFWindow;
import fuji.c.MFImage;

nothrow:

struct MFDisplay;

enum MFDisplayOrientation : int
{
	Normal,
	_90CW,
	_90CCW,
	_180,
	HFlip,
	VFlip,

	Max
};

enum MFDisplayCable : int
{
	Unknown = -1,

	// TV standards
	RF,
	Composite,
	SVideo,

	// RGB standards
	SCART,
	Component,
	VGA,
	DVI,
	HDMI,
	DisplayPort,

	Max
};

enum MFDisplayMode : int
{
	Unknown = -1,

	Progressive,
	Interlace,
	NTSC,
	PAL,

	Max
};

enum MFDisplayAspect : int
{
	Unknown = -1,

	Default,
	Widescreen,
	Letterbox,

	Max
}

enum MFDisplayFlags
{
	HideMenuBar = MFBit!0,
	HideStatusBar = MFBit!1,
	DisableRotation = MFBit!2,
	EnableRotation = MFBit!3,
	CanResizeWindow = MFBit!4
}

struct MFMonitorDesc
{
	const(char)* pName;

	int displayAdaptor;

	MFDisplayMode defaultMode;
	MFDisplayAspect defaultAspect;
	MFDisplayCable cable;

	MFRect defaultResolution;
	MFRect nativeResolution;	// may not be set for variable-rate monitors (TV's)

	float nativeAspectRatio;

	@property const(char)[] name() const pure nothrow { return pName.toDStr; }
}

struct MFDisplayAdaptorDesc
{
	const(char)* pDeviceId;
	const(char)* pName;

	bool bSupportsFullscreen;
	bool bSupportsWindowed;

	@property const(char)[] deviceId() const pure nothrow { return pDeviceId.toDStr; }
	@property const(char)[] name() const pure nothrow { return pName.toDStr; }
}

struct MFDisplayModeDesc
{
	int width;
	int height;
	int refreshRate;
	MFDisplayMode mode;
	uint cableBits;	// set a bit for each supported cable type
}

struct MFDisplaySettings
{
	int monitor;
	int displayAdaptor;

	int width;
	int height;
	int refreshRate;

	bool bFullscreen;
	bool bVSync;

	int numBuffers;		// 1/2/3 (single, double, triple buffering

	MFImageFormat backBufferFormat;
	MFImageFormat depthStencilFormat;

	MFDisplayMode mode;
	MFDisplayAspect aspect;
	MFDisplayCable cable;

	MFWindow* pWindow;

	uint flags;	// MFDisplayFlags
}


// query hardware
extern (C) int MFDisplay_GetNumMonitors();
extern (C) const(MFMonitorDesc)* MFDisplay_GetMonitorDesc(int monitor);

extern (C) int MFDisplay_GetNumDisplayAdaptors();
extern (C) const(MFDisplayAdaptorDesc)* MFDisplay_GetDisplayAdaptorDesc(int adaptor);

extern (C) int MFDisplay_GetDisplayModeCount(int monitor);
extern (C) const(MFDisplayModeDesc)* MFDisplay_GetDisplayMode(int monitor, int index);

extern (C) void MFDisplay_GetDefaults(MFDisplaySettings* pDisplaySettings);

// manage displays
extern (C) MFDisplay* MFDisplay_Create(const(char)* pName, const(MFDisplaySettings)* pDisplaySettings);
extern (C) MFDisplay* MFDisplay_CreateDefault(const(char)* pName);

extern (C) bool MFDisplay_Reset(MFDisplay* pDisplay = null, const(MFDisplaySettings)* pSettings = null);

extern (C) MFDisplay* MFDisplay_SetCurrent(MFDisplay* pDisplay);
extern (C) MFDisplay* MFDisplay_GetCurrent();

extern (C) void MFDisplay_Destroy(MFDisplay* pDisplay = null);

// query current settings
extern (C) const(MFDisplaySettings)* MFDisplay_GetDisplaySettings(const(MFDisplay)* pDisplay = null) pure;
extern (C) MFDisplayOrientation MFDisplay_GetDisplayOrientation(const(MFDisplay)* pDisplay = null) pure;

extern (C) void MFDisplay_GetDisplayRect(MFRect* pRect, const(MFDisplay)* pDisplay = null) pure;

extern (C) float MFDisplay_GetAspectRatio(const(MFDisplay)* pDisplay = null) pure;

extern (C) bool MFDisplay_IsVisible(const(MFDisplay)* pDisplay = null) pure;
extern (C) bool MFDisplay_HasFocus(const(MFDisplay)* pDisplay = null) pure;
