#if !defined(_MFSCRIPT_INTERNAL_H)
#define _MFSCRIPT_INTERNAL_H

#include "MFScript.h"

MFInitStatus MFScript_InitModule(int moduleId, bool bPerformInitialisation);
void MFScript_DeinitModule();

void RegisterInternalTypes();
void RegisterInternalFunctions();

#endif
