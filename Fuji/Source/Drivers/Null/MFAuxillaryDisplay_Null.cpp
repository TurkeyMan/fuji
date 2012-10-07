#include "Fuji.h"

#if MF_AUXILLARYDISPLAY == MF_DRIVER_NULL

#include "MFAuxillaryDisplay.h"

MF_API void MFAuxDisplay_Init(const char *pAppName)
{
}

MF_API void MFAuxDisplay_Deinit()
{
}

MF_API void MFAuxDisplay_Update()
{
}

MF_API int MFAuxDisplay_GetNumberOfDisplays()
{
	return 0;
}

MF_API void MFAuxDisplay_GetDisplayProperties(int device, MFAuxDisplayProperties *pProperties)
{
	MFDebug_Assert(false, "Invalid auxillary display.");
}

MF_API void MFAuxDisplay_LockScreen(int device, bool lock)
{
}

MF_API void MFAuxDisplay_UploadImage(int device, const char *pImageBuffer, int priority)
{
}

MF_API int MFAuxDisplay_ReadButton(int device, int button)
{
	return 0;
}

MF_API bool MFAuxDisplay_WasPressed(int device, int button)
{
	return false;
}

MF_API bool MFAuxDisplay_WasReleased(int device, int button)
{
	return false;
}

#endif
