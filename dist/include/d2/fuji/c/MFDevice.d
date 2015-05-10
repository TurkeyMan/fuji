module fuji.c.MFDevice;

nothrow:
@nogc:

struct MFDevice;

enum MFDeviceType
{
	// Displays
	Display,
	AuxiliaryDisplay,

	// Video devices
	VideoRender,

	// Audio devices
	AudioRender,
	AudioCapture,
	MIDI,

	// Input devices
	Keyboard,
	Mouse,
	GameController,
	TouchPanel,
	Accelerometer,
	Compass,
	Gyro,

	Max
}

enum MFDefaultDeviceType
{
	Default,
	Communication,
	Multimedia,

	Max
}

enum MFDeviceString
{
	ID,
	DeviceName,
	Description,
	InterfaceName,
	Manufacturer,

	Max
}

enum MFDeviceState
{
	Unknown = -1,

	Unavailable = 0,
	Ready,
	Disconnected,

	Max
}

enum MFDeviceEventType
{
	Unknown = -1,

	Connect = 0,
	Disconnect,
	Change,

	Max
}


extern (C) size_t MFDevice_GetNumDevices(MFDeviceType type);
extern (C) MFDevice* MFDevice_GetDeviceByIndex(MFDeviceType type, size_t index);
extern (C) MFDevice* MFDevice_GetDeviceById(const(char)* pId);
extern (C) MFDevice* MFDevice_GetDefaultDevice(MFDeviceType type, MFDefaultDeviceType defaultType);

extern (C) MFDeviceType MFDevice_GetDeviceType(const(MFDevice)* pDevice) pure;
extern (C) MFDeviceState MFDevice_GetDeviceState(const(MFDevice)* pDevice) pure;
extern (C) const(char)* MFDevice_GetDeviceString(const(MFDevice)* pDevice, MFDeviceString stringType) pure;

extern (C) inout(MFDevice)* MFDevice_GetParent(inout(MFDevice)* pDevice) pure;
extern (C) size_t MFDevice_GetNumChildren(const(MFDevice)* pDevice) pure;
extern (C) inout(MFDevice)* MFDevice_GetChild(inout(MFDevice)* pDevice, size_t index) pure;
