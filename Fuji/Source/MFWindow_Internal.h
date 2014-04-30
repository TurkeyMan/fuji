#if !defined(_MFWINDOWINTERNAL_H)
#define _MFWINDOWINTERNAL_H

#include "MFWindow.h"

struct MFDisplay;

enum MFWindowState
{
	MFWinState_Normal,
	MFWinState_Minimised,
	MFWinState_Maximised
};

struct MFWindow
{
	MFWindowParams params;

	MFDisplay *pDisplay;

	MFWindowState state;
	bool bHasFocus;
};


// internal functions
MFInitStatus MFWindow_InitModule(int moduleId, bool bPerformInitialisation);
void MFWindow_DeinitModule();

void MFWindow_InitModulePlatformSpecific();
void MFWindow_DeinitModulePlatformSpecific();

void MFWindow_AssociateDisplay(MFWindow *pWindow, MFDisplay *pDisplay);

#endif // _MFWINDOWINTERNAL_H
