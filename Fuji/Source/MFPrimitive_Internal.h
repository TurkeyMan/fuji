#if !defined(_MFPRIMITIVE_INTENAL_H)
#define _MFPRIMITIVE_INTENAL_H

#include "MFPrimitive.h"

MFInitStatus MFPrimitive_InitModule(int moduleId, bool bPerformInitialisation);
void MFPrimitive_DeinitModule();

void MFPrimitive_DrawStats();

#endif // _MFPRIMITIVE_INTENAL_H
