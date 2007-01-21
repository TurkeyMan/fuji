#if !defined(_DISPLAY_H)
#define _DISPLAY_H

extern const float MFAspect_1x1;
extern const float MFAspect_4x3;
extern const float MFAspect_16x9;
extern const float MFAspect_16x10;

enum MFClearScreenFlags
{
	CS_Colour	= 1,
	CS_ZBuffer	= 2,
	CS_Stencil	= 4,

	CS_All		= CS_Colour | CS_ZBuffer | CS_Stencil,
	CS_ForceInt	= 0x7FFFFFFF
};

// interface functions
void MFDisplay_SetClearColour(float r, float g, float b, float a);
void MFDisplay_ClearScreen(uint32 flags = CS_All);

void MFDisplay_SetViewport(float x, float y, float width, float height);
void MFDisplay_ResetViewport();

void MFDisplay_GetDisplayRect(MFRect *pRect);

float MFDisplay_GetNativeAspectRatio();
bool MFDisplay_IsWidescreen();

#endif // _DISPLAY_H

