#pragma once
#if !defined(_MFRESOURCE_INTERNAL_H)
#define _MFRESOURCE_INTERNAL_H

#include "MFResource.h"

// functions
MFInitStatus MFResource_InitModule(int moduleId, bool bPerformInitialisation);
void MFResource_DeinitModule();

#endif
