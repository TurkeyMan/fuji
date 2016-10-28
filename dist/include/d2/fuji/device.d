module fuji.device;

public import fuji.c.MFDevice;
import fuji.string;

nothrow:
@nogc:

MFDevice* MFDevice_GetDeviceById(const(char)[] id)
{
	auto s = Stringz!128(id);
	return fuji.c.MFDevice.MFDevice_GetDeviceById(s);
}

const(char)[] MFDevice_GetDeviceString(const(MFDevice)* pDevice, MFDeviceString stringType) pure
{
	return fuji.c.MFDevice.MFDevice_GetDeviceString(pDevice, stringType).toDStr;
}


alias getNumDevices = MFDevice_GetNumDevices;

struct Device
{
nothrow @nogc:
	this(MFDeviceType type, size_t index)
	{
		pDevice = MFDevice_GetDeviceByIndex(type, index);
	}
	this(const(char)[] id)
	{
		pDevice = MFDevice_GetDeviceById(id);
	}
	this(MFDeviceType type, MFDefaultDeviceType defaultType)
	{
		pDevice = MFDevice_GetDefaultDevice(type, defaultType);
	}

pure:
	this(inout(MFDevice)* pDevice) inout
	{
		this.pDevice = pDevice;
	}

	@property inout(MFDevice)* handle() inout { return pDevice; }

	@property MFDeviceType type() { return MFDevice_GetDeviceType(pDevice); }
	@property MFDeviceState state() { return MFDevice_GetDeviceState(pDevice); }

	@property const(char)[] id() { return MFDevice_GetDeviceString(pDevice, MFDeviceString.ID); }
	@property const(char)[] name() { return MFDevice_GetDeviceString(pDevice, MFDeviceString.DeviceName); }
	@property const(char)[] description() { return MFDevice_GetDeviceString(pDevice, MFDeviceString.Description); }
	@property const(char)[] interfaceName() { return MFDevice_GetDeviceString(pDevice, MFDeviceString.InterfaceName); }
	@property const(char)[] manufacturer() { return MFDevice_GetDeviceString(pDevice, MFDeviceString.Manufacturer); }

	@property inout(Device) parent() inout { return inout(Device)(MFDevice_GetParent(pDevice)); }
	@property inout(Children) children() inout { return inout(Children)(MFDevice_GetNumChildren(pDevice), pDevice, 0); }

private:
	MFDevice* pDevice;

	// implement a range to access children
	struct Children
	{
	pure:
	nothrow:
	@nogc:
		size_t length;
		@property bool empty() const { return length == 0; }
		inout(Device) front() inout { return inout(Device)(MFDevice_GetChild(pDevice, index)); }
		inout(Device) back() inout { return inout(Device)(MFDevice_GetChild(pDevice, index+length-1)); }
		void popFront() { ++index; --length; }
		void popBack() { --length; }
		@property auto save() { return this; }
		inout(Device) opIndex(size_t n) inout { return inout(Device)(MFDevice_GetChild(pDevice, index+n)); }
		auto opSlice() inout { return this; }
		inout(Children) opSlice(size_t lower, size_t upper) inout { assert(lower <= upper && upper <= length, "Invalid range"); return inout(Children)(upper-lower, pDevice, index+lower); }
		alias opDollar = length;
	private:
		MFDevice* pDevice;
		size_t index;
	}
}
