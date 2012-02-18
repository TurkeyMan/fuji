#if !defined(_MFNETWORK_INTERNAL_H)
#define _MFNETWORK_INTERNAL_H

#include "MFNetwork.h"
#include "MFInput_Internal.h"

MFInitStatus MFNetwork_InitModule();
void MFNetwork_DeinitModule();
void MFNetwork_Update();

// exposed to MFInput to read the Network state..
// this is kinda ugly.. it breaks link optimisation.
void MFNetwork_GetRemoteGamepadState(int id, MFGamepadState *pGamepadState);
const char* MFNetwork_GetRemoteGamepadName(int id);
const char* MFNetwork_GetRemoteGamepadButtonName(int id, int button);
void MFNetwork_LockInputMutex();
void MFNetwork_ReleaseInputMutex();

#endif // _MFNETWORK_INTERNAL_H
