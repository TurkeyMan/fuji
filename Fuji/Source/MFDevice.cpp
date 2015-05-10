#include "Fuji_Internal.h"
#include "MFDevice_Internal.h"
#include "MFObjectPool.h"
#include "Util.h"


// TODO: remove hard-coded 16 in favour of dynamic arrays...
MFDevice *gpDevices[MFDT_Max][16];
size_t gNumDevices[MFDT_Max];

MFDevice *gpDefaultDevices[MFDT_Max][MFDDT_Max];

MFObjectPool gDevicePool;


MFInitStatus MFDevice_InitModule(int moduleId, bool bPerformInitialisation)
{
	gDevicePool.Init(sizeof(MFDevice), 128, 128);

	return MFIS_Succeeded;
}

void MFDevice_DeinitModule()
{
	for(size_t i=0; i<MFDT_Max; ++i)
	{
		for(size_t j=0; j<gNumDevices[i]; ++j)
		{
			if(gpDevices[i][j]->pDestroyFunc)
				gpDevices[i][j]->pDestroyFunc(gpDevices[i][j]);
		}
	}

	gDevicePool.Deinit();
}

MFDevice* MFDevice_AllocDevice(MFDeviceType type, MFDevice_DestroyFunc *pDestroyFunc)
{
	MFDebug_Assert(gNumDevices[type] < 16, "Too many devices!");
	gpDevices[type][gNumDevices[type]] = (MFDevice*)gDevicePool.AllocAndZero();
	MFDevice *pDevice = gpDevices[type][gNumDevices[type]++];
	pDevice->type = type;
	pDevice->state = MFDevState_Unknown;
	pDevice->pDestroyFunc = pDestroyFunc;
	return pDevice;
}

void MFDevice_SetDefaultDevice(MFDeviceType type, MFDefaultDeviceType defaultType, MFDevice *pDevice)
{
	if(defaultType == MFDDT_All)
	{
		for(int i=0; i<MFDT_Max; ++i)
			gpDefaultDevices[type][i] = pDevice;
	}
	else
		gpDefaultDevices[type][defaultType] = pDevice;
}

MF_API size_t MFDevice_GetNumDevices(MFDeviceType type)
{
	return gNumDevices[type];
}

MF_API MFDevice* MFDevice_GetDeviceByIndex(MFDeviceType type, size_t index)
{
	MFDebug_Assert(index < gNumDevices[type], "Invalid device index!");
	return gpDevices[type][index];
}

MF_API MFDevice* MFDevice_GetDeviceById(const char *pId)
{
	for(size_t i=0; i<MFDT_Max; ++i)
	{
		for(size_t j=0; j<gNumDevices[i]; ++j)
		{
			if(!MFString_Compare(pId, gpDevices[i][j]->strings[MFDS_ID]))
				return gpDevices[i][j];
		}
	}
	return NULL;
}

MF_API MFDevice* MFDevice_GetDefaultDevice(MFDeviceType type, MFDefaultDeviceType defaultType)
{
	return gpDefaultDevices[type][defaultType];
}

MF_API MFDeviceType MFDevice_GetDeviceType(const MFDevice *pDevice)
{
	return pDevice->type;
}

MF_API MFDeviceState MFDevice_GetDeviceState(const MFDevice *pDevice)
{
	return pDevice->state;
}

MF_API const char *MFDevice_GetDeviceString(const MFDevice *pDevice, MFDeviceString string)
{
	return pDevice->strings[string];
}

MF_API MFDevice *MFDevice_GetParent(const MFDevice *pDevice)
{
	return pDevice->pParent;
}

MF_API size_t MFDevice_GetNumChildren(const MFDevice *pDevice)
{
	return pDevice->numChildren;
}

MF_API MFDevice *MFDevice_GetChild(const MFDevice *pDevice, size_t index)
{
	return pDevice->ppChildren[index];
}
