#if !defined(_MFDISPLAY_H)
#define _MFDISPLAY_H

//#define _NEW_DISPLAY

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

#if defined(_NEW_DISPLAY)

#define MFASPECT_1x1	(1.f)
#define MFASPECT_4x3	(4.f/3.f)
#define MFASPECT_16x9	(16.f/9.f)
#define MFASPECT_16x10	(16.f/10.f)

enum MFDisplayModeFlags
{
	MFDM_Fullscreen = MFBIT(0)
};

struct MFDisplay;

struct MFDisplayMode
{
	int width;
	int height;
	float aspectRatio;
	uint32 createFlags;
};

MF_API MFDisplay *MFDisplay_Create(const char *pName, MFDisplayMode *pDisplayMode);
MF_API void MFDisplay_Destroy(MFDisplay *pDisplay);

MF_API int MFDisplay_GetDisplayModeCount();
MF_API void MFDisplay_GetDisplayMode(MFDisplayMode *pDisplayMode);

MF_API float MFDisplay_GetNativeAspectRatio();
MF_API bool MFDisplay_IsWidescreen();

MF_API void MFDisplay_HandleSystemMessages();

MF_API MFDisplayOrientation MFDisplay_GetDisplayOrientation();

// these functions are reserved for use by the renderer, and may not be called if the renderer chooses to implement them its self
MF_API void *MFDisplay_GetRenderBuffer(MFDisplay *pDisplay, int *pWidth, int *pHeight);
MF_API void *MFDisplay_GetDisplayBuffer(MFDisplay *pDisplay, int *pWidth, int *pHeight);
MF_API void MFDisplay_BlitToScreen(MFDisplay *pDisplay);

/////////////////////////////////////////////////////////////////////////////////
#else
/////////////////////////////////////////////////////////////////////////////////

const float MFAspect_1x1 = 1.f;
const float MFAspect_4x3 = 4.f/3.f;
const float MFAspect_16x9 = 16.f/9.f;
const float MFAspect_16x10 = 16.f/10.f;

// interface functions
MF_API bool MFDisplay_SupportsFullscreen();
MF_API void MFDisplay_GetNumDisplayModes(bool window = false, float aspectConstraint = 0.f);
MF_API void MFDisplay_GetDisplayMode(int index, bool window = false, float aspectConstraint = 0.f);

MF_API void MFDisplay_GetNativeRes(MFRect *pRect);
MF_API void MFDisplay_GetDefaultRes(MFRect *pRect);

MF_API void MFDisplay_GetDisplayRect(MFRect *pRect);

MF_API float MFDisplay_GetNativeAspectRatio();
MF_API bool MFDisplay_IsWidescreen();

MF_API bool MFDisplay_HasFocus();

MF_API inline MFDisplayOrientation MFDisplay_GetDisplayOrientation()
{
	return MFDO_Normal;
}

#endif

#endif // _MFDISPLAY_H
