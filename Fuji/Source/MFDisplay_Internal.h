#if !defined(_MFDISPLAY_INTERNAL_H)
#define _MFDISPLAY_INTERNAL_H

#include "MFDisplay.h"

struct MFDisplay
{
	MFDisplaySettings settings;

	MFDisplayOrientation orientation;
	float aspectRatio;

	int fullscreenWidth;
	int fullscreenHeight;
	int windowWidth;
	int windowHeight;

	bool bHasFocus;
	bool bIsVisible;
};

// internal functions
MFInitStatus MFDisplay_InitModule(int moduleId, bool bPerformInitialisation);
void MFDisplay_DeinitModule();

void MFDisplay_InitModulePlatformSpecific();
void MFDisplay_DeinitModulePlatformSpecific();

void MFDisplay_LostFocus(MFDisplay *pDisplay);
void MFDisplay_GainedFocus(MFDisplay *pDisplay);

#endif
