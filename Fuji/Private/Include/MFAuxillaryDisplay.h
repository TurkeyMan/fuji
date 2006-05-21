/**
 * @file MFAuxillaryDisplay.h
 * @brief Provides an interface to any available auxillary displays.
 * @author Manu Evans
 * @defgroup MFAuxillaryDisplay Auxillary Display Interface
 * @{
 */

#if !defined(_MFAUXILLARYDISPLAY_H)
#define _MFAUXILLARYDISPLAY_H

#include "MFInput.h"

enum MFAuxInputTypes
{
	MFAuxType_Unknown = -1,

	MFAuxType_G15 = 0,
	MFAuxType_DreamcastVMU,

	MFAuxType_Max,
	MFAuxType_ForceInt = 0x7FFFFFFF
};

enum MFAuxPriority
{
	MFAuxPriority_High = 2,
	MFAuxPriority_Normal = 5,
	MFAuxPriority_Low = 8,

	MFAuxPriority_Max,
	MFAuxPriority_ForceInt = 0x7FFFFFFF
};

struct MFAuxDisplayProperties
{
	MFAuxInputTypes type;
	uint32 numButtons;

	int screenWidth;
	int screenHeight;
	int colourDepth;

	MFInputDevice relatedInputDevice;
	int relatedInputDeviceID;
};

void MFAuxDisplay_Init(const char *pAppName);
void MFAuxDisplay_Deinit();

void MFAuxDisplay_Update();

int MFAuxDisplay_GetNumberOfDisplays();
void MFAuxDisplay_GetDisplayProperties(int device, MFAuxDisplayProperties *pProperties);

void MFAuxDisplay_LockScreen(int device, bool lock);

void MFAuxDisplay_UploadImage(int device, const char *pImageBuffer, int priorioty);

int MFAuxDisplay_ReadButton(int device, int button);
bool MFAuxDisplay_WasPressed(int device, int button);
bool MFAuxDisplay_WasReleased(int device, int button);

#endif // _MFAUXILLARYDISPLAY_H
