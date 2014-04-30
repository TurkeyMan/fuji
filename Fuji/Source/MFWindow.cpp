#include "Fuji_Internal.h"
#include "MFWindow_Internal.h"

MFInitStatus MFWindow_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFWindow_InitModulePlatformSpecific();

	return MFIS_Succeeded;
}

void MFWindow_DeinitModule()
{
	MFWindow_DeinitModulePlatformSpecific();
}

void MFWindow_AssociateDisplay(MFWindow *pWindow, MFDisplay *pDisplay)
{
	pWindow->pDisplay = pDisplay;
}

MF_API const MFWindowParams *MFWindow_GetWindowParameters(const MFWindow *pWindow)
{
	return &pWindow->params;
}

MF_API MFDisplay *MFWindow_GetDisplay(MFWindow *pWindow)
{
	return pWindow->pDisplay;
}
