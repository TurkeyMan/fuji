#if !defined(_DISPLAY_H)
#define _DISPLAY_H

#define WIDE_ASPECT 1.7777777777777777777777777777778f
#define STANDARD_ASPECT 1.3333333333333333333333333333333f

enum ClearScreenFlags
{
	CS_Colour	= 1,
	CS_ZBuffer	= 2
};

// interface functions
void Display_SetClearColour(float r, float g, float b, float a);
void Display_ClearScreen(uint32 flags = CS_Colour|CS_ZBuffer);

void SetViewport(float x, float y, float width, float height);
void ResetViewport();

#endif // _DISPLAY_H

