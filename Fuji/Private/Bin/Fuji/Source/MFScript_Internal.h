#if !defined(_MFSCRIPT_INTERNAL_H)
#define _MFSCRIPT_INTERNAL_H

#include "MFScript.h"

void MFScript_InitModule();
void MFScript_DeinitModule();

void RegisterInternalTypes();
void RegisterInternalFunctions();

#endif
