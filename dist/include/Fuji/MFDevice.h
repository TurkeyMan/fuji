/**
 * @file MFDevice.h
 * @brief Provides access to hardware devices.
 * @author Manu Evans
 * @defgroup MFDevice Hardware Device Access
 * @{
 */

#if !defined(_MFDEVICE_H)
#define _MFDEVICE_H

struct MFDevice;

enum MFDeviceType
{
	// Displays
	MFDT_Display,
	MFDT_AuxiliaryDisplay,

	// Video devices
	MFDT_VideoRender,

	// Audio devices
	MFDT_AudioRender,
	MFDT_AudioCapture,
	MFDT_MidiInput,
	MFDT_MidiOutput,

	// Input devices
	MFDT_Keyboard,
	MFDT_Mouse,
	MFDT_GameController,
	MFDT_TouchPanel,
	MFDT_Accelerometer,
	MFDT_Compass,
	MFDT_Gyro,

	MFDT_Max,
	MFDT_ForceInt = 0x7FFFFFFF
};

enum MFDefaultDeviceType
{
	MFDDT_All = -1,

	MFDDT_Default = 0,
	MFDDT_Communication,
	MFDDT_Multimedia,

	MFDDT_Max,
	MFDDT_ForceInt = 0x7FFFFFFF
};

enum MFDeviceString
{
	MFDS_ID,
	MFDS_DeviceName,
	MFDS_Description,
	MFDS_InterfaceName,
	MFDS_Manufacturer,

	MFDS_Max,
	MFDS_ForceInt = 0x7FFFFFFF
};

enum MFDeviceState
{
	MFDevState_Unknown = -1,

	MFDevState_Available = 0,	/**< Device is available. */
	MFDevState_Unavailable,		/**< Device is unavailable. */
	MFDevState_Disconnected,	/**< Device is disconnected. */
	MFDevState_Ready,			/**< Device is ready for use. */
	MFDevState_Active,			/**< Device is in use. */

	MFDevState_Max,
	MFDevState_ForceInt = 0x7FFFFFFF
};

enum MFDeviceEventType
{
	MFDET_Unknown = -1,

	MFDET_Connect = 0,
	MFDET_Disconnect,
	MFDET_Change,

	MFDET_Max,
	MFDET_ForceInt = 0x7FFFFFFF
};


MF_API size_t MFDevice_GetNumDevices(MFDeviceType type);
MF_API MFDevice* MFDevice_GetDeviceByIndex(MFDeviceType type, size_t index);
MF_API MFDevice* MFDevice_GetDeviceById(const char *pId);
MF_API MFDevice* MFDevice_GetDefaultDevice(MFDeviceType type, MFDefaultDeviceType defaultType);

MF_API MFDeviceType MFDevice_GetDeviceType(const MFDevice *pDevice);
MF_API MFDeviceState MFDevice_GetDeviceState(const MFDevice *pDevice);
MF_API const char *MFDevice_GetDeviceString(const MFDevice *pDevice, MFDeviceString string);

MF_API MFDevice *MFDevice_GetParent(const MFDevice *pDevice);
MF_API size_t MFDevice_GetNumChildren(const MFDevice *pDevice);
MF_API MFDevice *MFDevice_GetChild(const MFDevice *pDevice, size_t index);

#endif // _MFDEVICE_H

/** @} */
