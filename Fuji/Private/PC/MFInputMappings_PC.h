#if !defined(_MFINPUTMAPPINGS_PC_H)
#define _MFINPUTMAPPINGS_PC_H

#define MFAXIS(x) ((x)<<6)
#define MFGETAXIS(x) (((x)&AID_AxisMask)>>6)

/*** Enums ***/

// Button masks for the controller button map's
enum MFButtonMasks
{
	// masks for the button field
	AID_ButtonMask = 0x003F, // max 64 buttons (6 bits)
	AID_AxisMask   = 0x0FC0, // axis offset (must be shifted down 6 bits)
	AID_Analog     = 0x4000, // use the analog axis, if not present on the controller, the button will be read
	AID_Negative   = 0x8000, // if this flag is present, the axis will be inverted
	AID_Clamp      = 0x2000, // clamps to only allow the positive range
	AID_Full       = 0x1000, // read the value across the full analog range

	// use these enum's to reference analog axii's
	AID_X        = MFAXIS(0)  | AID_Analog,
	AID_Y        = MFAXIS(1)  | AID_Analog,
	AID_Z        = MFAXIS(2)  | AID_Analog,
	AID_Rx       = MFAXIS(3)  | AID_Analog,
	AID_Ry       = MFAXIS(4)  | AID_Analog,
	AID_Rz       = MFAXIS(5)  | AID_Analog,
	AID_Slider1  = MFAXIS(6)  | AID_Analog,
	AID_Slider2  = MFAXIS(7)  | AID_Analog,
	AID_Vx       = MFAXIS(43) | AID_Analog,
	AID_Vy       = MFAXIS(44) | AID_Analog,
	AID_Vz       = MFAXIS(45) | AID_Analog,

	POV_Up       = MFAXIS(60),
	POV_Down     = MFAXIS(61),
	POV_Left     = MFAXIS(62),
	POV_Right    = MFAXIS(63),
};

// gamepad info structure
struct MFGamepadInfo
{
	const char *pName;
	const char *pIdentifier;
	uint32 vendorID, productID;
	const int *pButtonMap;
	const char **ppButtonNameStrings;

	MFGamepadInfo *pNext;
};

#endif
