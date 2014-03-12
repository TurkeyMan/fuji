#include "Fuji_Internal.h"

#if MF_DISPLAY == MF_DRIVER_XBOX

#include "MFRenderer_Internal.h"
#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

static bool gbWidescreen;
bool gbLetterBox;

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	DWORD avPack = XGetAVPack();
	DWORD vidMode = XGetVideoStandard();
	DWORD vidFlags = XGetVideoFlags();

	// log the AV pack (just an FYI)
	switch(avPack)
	{
		case XC_AV_PACK_SCART:		MFDebug_Log(2, "MFDisplay: Video output using Scart AV Pack"); break;
		case XC_AV_PACK_HDTV:		MFDebug_Log(2, "MFDisplay: Video output using High Def Pack"); break;
		case XC_AV_PACK_RFU:		MFDebug_Log(2, "MFDisplay: Video output using RF Unit"); break;
		case XC_AV_PACK_SVIDEO:		MFDebug_Log(2, "MFDisplay: Video output using Advanced AV Pack"); break;
		case XC_AV_PACK_STANDARD:	MFDebug_Log(2, "MFDisplay: Video output using Standard AV Pack"); break;
		default:					MFDebug_Log(2, "MFDisplay: Video output using Unknown AV Pack"); break;
	}

	// calculate the frame buffer size and display mode from the dashboard info
	gbWidescreen = !!(vidFlags & (XC_VIDEO_FLAGS_WIDESCREEN));
	gbLetterBox = !!(vidFlags & (XC_VIDEO_FLAGS_LETTERBOX));
	wide = gbWidescreen;

	if(vidMode == XC_VIDEO_STANDARD_PAL_I)
	{
		// PAL modes (poor PAL users dont get high def) :(
		if(vidFlags & XC_VIDEO_FLAGS_PAL_60Hz)
		{
			width = 720;
			height = 480;
			rate = 60;
			progressive = false;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to PAL60%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
		else
		{
			width = 720;
			height = 576;
			rate = 50;
			progressive = false;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to PAL (576i)%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
	}
	else
	{
		// NTSC modes
		if(vidFlags & XC_VIDEO_FLAGS_HDTV_1080i)
		{
			width = 1920;
			height = 1080;
			rate = 60;
			progressive = false;
			MFDebug_Log(2, "MFDisplay: Video Mode set to 1080i");
		}
		else if(vidFlags & XC_VIDEO_FLAGS_HDTV_720p)
		{
			width = 1280;
			height = 720;
			rate = 60;
			progressive = true;
			MFDebug_Log(2, "MFDisplay: Video Mode set to 720p");
		}
		else if(vidFlags & XC_VIDEO_FLAGS_HDTV_480p)
		{
			width = 720;
			height = 480;
			rate = 60;
			progressive = true;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to 480p%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
		else
		{
			width = 720;
			height = 480;
			rate = 60;
			progressive = false;
			MFDebug_Log(2, MFStr("MFDisplay: Video Mode set to NTSC (480i)%s", wide ? " (Widescreen)" : (gbLetterBox ? " (Letterbox)" : "")));
		}
	}

	// update display parameters
	gDisplay.fullscreenWidth = gDisplay.width = width;
	gDisplay.fullscreenHeight = gDisplay.height = height;
	gDisplay.progressive = progressive;
	gDisplay.refreshRate = rate;
	gDisplay.wide = wide;
	gDisplay.colourDepth = bpp;
	gDisplay.windowed = false;

	MFRenderer_CreateDisplay();

	// setup the initial viewport (letterbox)
	MFRenderer_ResetViewport();

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFCALLSTACK;

	MFRenderer_ResetDisplay();
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;

	MFRenderer_DestroyDisplay();
}

float MFDisplay_GetNativeAspectRatio()
{
	return (gbWidescreen || gbLetterBox) ? MFAspect_16x9 : MFAspect_4x3;
}

bool MFDisplay_IsWidescreen()
{
	return gbWidescreen || gbLetterBox;
}

#endif // MF_DISPLAY
