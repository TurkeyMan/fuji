#if !defined(_DISPLAY_H)
#define _DISPLAY_H

enum DisplayAspect
{
	Aspect_1x1,
	Aspect_4x3,
	Aspect_16x9,
	Aspect_16x10,

	Aspect_Max,
	Aspect_ForceInt = 0x7FFFFFFF
};

extern float AspectRatio[Aspect_Max];

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

