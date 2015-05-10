#if !defined(_MFDEVICE_INTERNAL_H)
#define _MFDEVICE_INTERNAL_H

#include "MFDevice.h"

typedef void (MFDevice_DestroyFunc)(MFDevice*);

struct MFDevice
{
	MFDeviceType type;
	MFDeviceState state;

	// TODO: change these to use a string-pool in a single allocation instead of massive static reservations...
	char strings[MFDS_Max][128];

	MFDevice *pParent;
	MFDevice **ppChildren;
	size_t numChildren;

	MFDevice_DestroyFunc *pDestroyFunc;

	void *pInternal;
};

MFInitStatus MFDevice_InitModule(int moduleId, bool bPerformInitialisation);
void MFDevice_DeinitModule();

MFDevice* MFDevice_AllocDevice(MFDeviceType type, MFDevice_DestroyFunc *pDestroyFunc);
void MFDevice_SetDefaultDevice(MFDeviceType type, MFDefaultDeviceType defaultType, MFDevice *pDevice);

#endif
