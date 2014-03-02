#include "Fuji.h"

MF_API bool *MFAsset_ConvertAsset(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes = 0);

MF_API bool *MFAsset_ConvertAsset(const char *pFilename, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes)
{
	// get asset type from filename and convert it...

	return false;
}

#if defined(MF_WINDOWS) && defined(MF_SHAREDLIB)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "MFModule.h"

bool MFModule_InitModules();

BOOL WINAPI DllMain(HANDLE hModule, DWORD ulReasonForCall, LPVOID lpReserved)
{
    switch (ulReasonForCall)
    {
		case DLL_PROCESS_ATTACH:
			MFModule_RegisterCoreModules();
			MFModule_InitModules();
			break;
		case DLL_THREAD_ATTACH:  break;
		case DLL_THREAD_DETACH:  break;
		case DLL_PROCESS_DETACH:
			// TODO: clean up!
			break;
    }

    return TRUE;
}
#endif
