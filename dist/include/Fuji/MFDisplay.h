#if !defined(_MFDISPLAY_H)
#define _MFDISPLAY_H

#include "MFImage.h"

struct MFWindow;
struct MFSwapChain;

struct MFDisplay;

enum MFDisplayOrientation
{
	MFDO_Normal,
	MFDO_90CW,
	MFDO_90CCW,
	MFDO_180,
	MFDO_HFlip,
	MFDO_VFlip,

	MFDO_Max,
	MFDO_ForceInt = 0x7FFFFFFF
};

enum MFDisplayCable
{
	MFDC_Unknown = -1,

	// TV standards
	MFDC_RF,
	MFDC_Composite,
	MFDC_SVideo,

	// RGB standards
	MFDC_SCART,
	MFDC_Component,
	MFDC_VGA,
	MFDC_DVI,
	MFDC_HDMI,
	MFDC_DisplayPort,

	MFDC_Max,
	MFDC_ForceInt = 0x7FFFFFFF
};

enum MFDisplayMode
{
	MFDM_Unknown = -1,

	MFDM_Progressive,
	MFDM_Interlace,
	MFDM_NTSC,
	MFDM_PAL,

	MFDM_Max,
	MFDM_ForceInt = 0x7FFFFFFF
};

enum MFDisplayAspect
{
	MFDA_Unknown = -1,

	MFDA_Default,
	MFDA_Widescreen,
	MFDA_Letterbox,

	MFDA_Max,
	MFDA_ForceInt = 0x7FFFFFFF
};

enum MFDisplayFlags
{
	MFDF_HideMenuBar = MFBIT(0),
	MFDF_HideStatusBar = MFBIT(1),
	MFDF_DisableRotation = MFBIT(2),
	MFDF_EnableRotation = MFBIT(3),
	MFDF_CanResizeWindow = MFBIT(4)
};

struct MFMonitorDesc
{
	const char *pName;

	int displayAdaptor;

	MFDisplayMode defaultMode;
	MFDisplayAspect defaultAspect;
	MFDisplayCable cable;

	MFRect defaultResolution;
	MFRect nativeResolution;	// may not be set for variable-rate monitors (TV's)

	float nativeAspectRatio;
};

struct MFDisplayAdaptorDesc
{
	const char *pDeviceId;
	const char *pName;

	bool bSupportsFullscreen;
	bool bSupportsWindowed;
};

struct MFDisplayModeDesc
{
	int width;
	int height;
	int refreshRate;
	MFDisplayMode mode;
	uint32 cableBits;	// set a bit for each supported cable type
};

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

	MFWindow *pWindow;

	uint32 flags;	// MFDisplayFlags
};


// query hardware
MF_API int MFDisplay_GetNumMonitors();
MF_API const MFMonitorDesc* MFDisplay_GetMonitorDesc(int monitor);

MF_API int MFDisplay_GetNumDisplayAdaptors();
MF_API const MFDisplayAdaptorDesc *MFDisplay_GetDisplayAdaptorDesc(int adaptor);

MF_API int MFDisplay_GetDisplayModeCount(int monitor);
MF_API const MFDisplayModeDesc *MFDisplay_GetDisplayMode(int monitor, int index);

MF_API void MFDisplay_GetDefaults(MFDisplaySettings *pDisplaySettings);

// manage displays
MF_API MFDisplay* MFDisplay_Create(const char *pName, const MFDisplaySettings *pDisplaySettings);
MF_API MFDisplay* MFDisplay_CreateDefault(const char *pName);

MF_API bool MFDisplay_Reset(MFDisplay *pDisplay = NULL, const MFDisplaySettings *pSettings = NULL);

MF_API MFDisplay* MFDisplay_SetCurrent(MFDisplay *pDisplay);
MF_API MFDisplay* MFDisplay_GetCurrent();

MF_API void MFDisplay_Destroy(MFDisplay *pDisplay = NULL);

// query current settings
MF_API const MFDisplaySettings* MFDisplay_GetDisplaySettings(const MFDisplay *pDisplay = NULL);
MF_API MFDisplayOrientation MFDisplay_GetDisplayOrientation(const MFDisplay *pDisplay = NULL);

MF_API void MFDisplay_GetDisplayRect(MFRect *pRect, const MFDisplay *pDisplay = NULL);

MF_API float MFDisplay_GetAspectRatio(const MFDisplay *pDisplay = NULL);

MF_API bool MFDisplay_IsVisible(const MFDisplay *pDisplay = NULL);
MF_API bool MFDisplay_HasFocus(const MFDisplay *pDisplay = NULL);

#endif // _MFDISPLAY_H
