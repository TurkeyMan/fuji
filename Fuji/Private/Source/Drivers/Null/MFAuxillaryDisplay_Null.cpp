#include "Fuji.h"

#if MF_AUXILLARYDISPLAY == NULL

#include "MFAuxillaryDisplay.h"

void MFAuxDisplay_Init(const char *pAppName)
{
}

void MFAuxDisplay_Deinit()
{
}

void MFAuxDisplay_Update()
{
}

int MFAuxDisplay_GetNumberOfDisplays()
{
	return 0;
}

void MFAuxDisplay_GetDisplayProperties(int device, MFAuxDisplayProperties &pProperties)
{
	MFDebug_Assert(false, "Invalid auxillary display.");
}

void MFAuxDisplay_LockScreen(int device, bool lock)
{
}

void MFAuxDisplay_UploadImage(int device, const char *pImageBuffer, int priority)
{
}

int MFAuxDisplay_ReadButton(int device, int button)
{
	return 0;
}

bool MFAuxDisplay_WasPressed(int device, int button)
{
	return false;
}

bool MFAuxDisplay_WasReleased(int device, int button)
{
	return false;
}

#endif
