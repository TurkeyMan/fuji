module fuji.c.MFDisplay;

import fuji.c.MFTypes;

nothrow:

enum MFDisplayOrientation
{
	Normal,
	_90CW,
	_90CCW,
	_180,
	HFlip,
	VFlip,
}

version(_NEW_DISPLAY)
{
	enum float MFASPECT_1x1 =	1.0;
	enum float MFASPECT_4x3 =	4.0/3.0;
	enum float MFASPECT_16x9 =	16.0/9.0;
	enum float MFASPECT_16x10 =	16.0/10.0;

	enum MFDisplayModeFlags
	{
		MFDM_Fullscreen = MFBit!0
	}

	struct MFDisplay;

	struct MFDisplayMode
	{
		int width;
		int height;
		float aspectRatio;
		uint createFlags;
	};

	extern (C) MFDisplay* MFDisplay_Create(const char* pName, MFDisplayMode* pDisplayMode);
	extern (C) void MFDisplay_Destroy(MFDisplay* pDisplay);

	extern (C) int MFDisplay_GetDisplayModeCount();
	extern (C) void MFDisplay_GetDisplayMode(MFDisplayMode* pDisplayMode);

	extern (C) float MFDisplay_GetNativeAspectRatio();
	extern (C) bool MFDisplay_IsWidescreen();

	extern (C) void MFDisplay_HandleSystemMessages();

	extern (C) MFDisplayOrientation MFDisplay_GetDisplayOrientation();

	// these functions are reserved for use by the renderer, and may not be called if the renderer chooses to implement them its self
	extern (C) void* MFDisplay_GetRenderBuffer(MFDisplay* pDisplay, int* pWidth, int* pHeight);
	extern (C) void* MFDisplay_GetDisplayBuffer(MFDisplay* pDisplay, int* pWidth, int* pHeight);
	extern (C) void MFDisplay_BlitToScreen(MFDisplay* pDisplay);
}
else
{
	enum float MFAspect_1x1 = 1.0;
	enum float MFAspect_4x3 = 4.0/3.0;
	enum float MFAspect_16x9 = 16.0/9.0;
	enum float MFAspect_16x10 = 16.0/10.0;

	// interface functions
	extern (C) bool MFDisplay_SupportsFullscreen();
	extern (C) void MFDisplay_GetNumDisplayModes(bool window = false, float aspectConstraint = 0.0);
	extern (C) void MFDisplay_GetDisplayMode(int index, bool window = false, float aspectConstraint = 0.0);

	extern (C) void MFDisplay_GetNativeRes(MFRect* pRect);
	extern (C) void MFDisplay_GetDefaultRes(MFRect* pRect);

	extern (C) void MFDisplay_GetDisplayRect(MFRect* pRect);

	extern (C) float MFDisplay_GetNativeAspectRatio();
	extern (C) bool MFDisplay_IsWidescreen();

	extern (C) bool MFDisplay_HasFocus();

	extern (C) MFDisplayOrientation MFDisplay_GetDisplayOrientation();
}

