#if !defined(_MFSOCKETS_INTERNAL_H)
#define _MFSOCKETS_INTERNAL_H

#include "MFSockets.h"

MFInitStatus MFSockets_InitModule(int moduleId, bool bPerformInitialisation);
void MFSockets_DeinitModule();

int MFSockets_InitModulePlatformSpecific();
void MFSockets_DeinitModulePlatformSpecific();

#endif
