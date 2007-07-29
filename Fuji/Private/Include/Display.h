#if !defined(_MFDISPLAY_H)
#define _MFDISPLAY_H

//#define _NEW_DISPLAY

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

MFDisplay *MFDisplay_Create(const char *pName, MFDisplayMode *pDisplayMode);
void MFDisplay_Destroy(MFDisplay *pDisplay);

int MFDisplay_GetDisplayModeCount();
void MFDisplay_GetDisplayMode(MFDisplayMode *pDisplayMode);

float MFDisplay_GetNativeAspectRatio();
bool MFDisplay_IsWidescreen();

void MFDisplay_HandleSystemMessages();

// these functions are reserved for use by the renderer, and may not be called if the renderer chooses to implement them its self
void *MFDisplay_GetRenderBuffer(MFDisplay *pDisplay, int *pWidth, int *pHeight);
void *MFDisplay_GetDisplayBuffer(MFDisplay *pDisplay, int *pWidth, int *pHeight);
void MFDisplay_BlitToScreen(MFDisplay *pDisplay);

/////////////////////////////////////////////////////////////////////////////////
#else
/////////////////////////////////////////////////////////////////////////////////

extern const float MFAspect_1x1;
extern const float MFAspect_4x3;
extern const float MFAspect_16x9;
extern const float MFAspect_16x10;

// interface functions
void MFDisplay_GetDisplayRect(MFRect *pRect);

float MFDisplay_GetNativeAspectRatio();
bool MFDisplay_IsWidescreen();

#endif

#endif // _MFDISPLAY_H
