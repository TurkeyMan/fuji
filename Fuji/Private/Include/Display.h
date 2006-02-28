#if !defined(_DISPLAY_H)
#define _DISPLAY_H

extern const float MFAspect_1x1;
extern const float MFAspect_4x3;
extern const float MFAspect_16x9;
extern const float MFAspect_16x10;

enum MFClearScreenFlags
{
	CS_Colour	= 1,
	CS_ZBuffer	= 2
};

// interface functions
void MFDisplay_SetClearColour(float r, float g, float b, float a);
void MFDisplay_ClearScreen(uint32 flags = CS_Colour|CS_ZBuffer);

void MFDisplay_SetViewport(float x, float y, float width, float height);
void MFDisplay_ResetViewport();

void MFDisplay_GetDisplayRect(MFRect *pRect);

#endif // _DISPLAY_H

