#include "Fuji_Internal.h"
#include "MFDisplay_Internal.h"
#include "DebugMenu.h"
#include "MFView.h"
#include "MFSystem.h"

MFDisplay *gpCurrentDisplay = NULL;
extern MFInitParams gInitParams;

bool gAppHasFocus = true;

MFInitStatus MFDisplay_InitModule(int moduleId, bool bPerformInitialisation)
{
	DebugMenu_AddMenu("Display Options", "Fuji Options");

	MFDisplay_InitModulePlatformSpecific();

	if(!gpCurrentDisplay)
	{
		gpCurrentDisplay = MFDisplay_CreateDefault("Fuji Display");
		if(!gpCurrentDisplay)
			return MFIS_Failed;
	}

	return MFIS_Succeeded;
}

void MFDisplay_DeinitModule()
{
	MFDisplay_DeinitModulePlatformSpecific();
}

MF_API MFDisplay *MFDisplay_CreateDefault(const char *pName)
{
	MFDisplaySettings displaySettings;
	MFDisplay_GetDefaults(&displaySettings);
	return MFDisplay_Create(pName, &displaySettings);
}

MF_API MFDisplay *MFDisplay_SetCurrent(MFDisplay *pDisplay)
{
	MFDisplay *pOld = gpCurrentDisplay;
	gpCurrentDisplay = pDisplay;
	return pOld;
}

MF_API MFDisplay *MFDisplay_GetCurrent()
{
	return gpCurrentDisplay;
}

MF_API const MFDisplaySettings *MFDisplay_GetDisplaySettings(const MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	return &pDisplay->settings;
}

MF_API MFDisplayOrientation MFDisplay_GetDisplayOrientation(const MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	return pDisplay->orientation;
}

MF_API void MFDisplay_GetDisplayRect(MFRect *pRect, const MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	pRect->x = pRect->y = 0;
	pRect->width = (float)pDisplay->settings.width;
	pRect->height = (float)pDisplay->settings.height;
}

MF_API float MFDisplay_GetAspectRatio(const MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	return pDisplay->aspectRatio;
}

MF_API bool MFDisplay_IsVisible(const MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	return pDisplay->bIsVisible;
}

MF_API bool MFDisplay_HasFocus(const MFDisplay *pDisplay)
{
	if(!pDisplay)
		pDisplay = gpCurrentDisplay;

	return pDisplay->bHasFocus;
}
