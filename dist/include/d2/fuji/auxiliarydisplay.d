module fuji.auxiliarydisplay;

import fuji.input;

/**
 * Auxillary display type.
 * Various types of known auxillary displays.
 */
enum MFAuxDisplayTypes
{
	Unknown = -1,	/**< Unknown auxillary display type. */

	G15 = 0,		/**< Logitech G15 gaming keyboard. */
	DreamcastVMU	/**< Sega Dreamcast VMU. */
}

/**
 * Auxillary display priority.
 * The priority of the image being displayed on the screen.
 */
enum MFAuxPriority
{
	High = 2,	/**< Image is high priority and should be displayed immediately if possible. */
	Normal = 5,	/**< Image is normal priority and should allow other applications with higher priority to take the display. */
	Low = 8		/**< Image is low priority and should always give priority to other applications. */
}

/**
 * Auxillary display properties.
 * Various properties related to an auxillary display.
 */
struct MFAuxDisplayProperties
{
	MFAuxDisplayTypes type;	/**< The type of the display. */
	uint numButtons;		/**< The number of soft buttons available on the device. */

	int screenWidth;		/**< The width, in pixels, of the screen. */
	int screenHeight;		/**< The height, in pixels, of the screen. */
	int colourDepth;		/**< The display colour depth. This will be 1 for monochrome displays. */

	MFInputDevice associatedInputDevice;	/**< The input device (if any) that is associated with this screen. */
	int assoiatedInputDeviceID;				/**< The input deviceID of this screen. */
}

/**
 * Initislise the auxillary display module.
 * Initislises the auxillary display module. The user must call this function before any other call to MFAuxillaryDisplay interfaces can be made.
 * @param pAppName The name of the app as may appear on the auxillary display.
 * @return None.
 */
extern (C) void MFAuxDisplay_Init(const(char*) pAppName);

/**
 * De-initislise the auxillary display module.
 * De-initislises the auxillary display module. Call this when you have finished with the auxillary display device.
 * @return None.
 */
extern (C) void MFAuxDisplay_Deinit();

/**
 * Update the auxillary display module.
 * Updates the auxillary display module.
 * @return None.
 * @remarks This function will perform various update functionality such as read any available input from the device.
 */
extern (C) void MFAuxDisplay_Update();

/**
 * Get the number of auxillary display devices.
 * Gets the number of auxillary display devices.
 * @return Returns the number of available auxillary display devices.
 */
extern (C) int MFAuxDisplay_GetNumberOfDisplays();

/**
 * Get the device properties for the specified auxillary display device.
 * Gets the device properties for the specified auxillary display device.
 * @param device The target device.
 * @param pProperties Pointer to an \a MFAuxDisplayProperties structure to receive the device properties.
 * @return None.
 */
extern (C) void MFAuxDisplay_GetDisplayProperties(int device, MFAuxDisplayProperties* pProperties);

/**
 * Lock the focus of the auxillary display.
 * Locks the focus of the auxillary display.
 * @param device The target device.
 * @param lock If true, the device will attempt to lock focus of the screen. If false, focus lock will be released.
 * @return None.
 */
extern (C) void MFAuxDisplay_LockScreen(int device, bool lock);

/**
 * Upload an image to the auxillary display device.
 * Uploads an image to the auxillary display device.
 * @param device The target device.
 * @param pImageBuffer Pointer to an array of bytes whose length matches the dimensions of the screen as returned from \a MFAuxDisplay_GetDisplayProperties().
 * @param priorioty Priority of the image being displayed If the priority is high and the application does not have focus on the device, the focus may be granted for the image to be displayed immediately.
 * @return None.
 */
extern (C) void MFAuxDisplay_UploadImage(int device, const(char*) pImageBuffer, int priorioty);

/**
 * Read the button state of an auxillary display device key.
 * Reads the button state of an auxillary display device key.
 * @param device The target device.
 * @param button The button ID to read.
 * @return Returns the state of the button.
 */
extern (C) int MFAuxDisplay_ReadButton(int device, int button);

/**
 * Read the button pressed state from the auxillary display device.
 * Reads the button pressed state from the auxillary display device.
 * @param device The target device.
 * @param button The button ID to read.
 * @return Returns true if the button was pressed this frame.
 */
extern (C) bool MFAuxDisplay_WasPressed(int device, int button);

/**
 * Read the button released state from the auxillary display device.
 * Reads the button released state from the auxillary display device.
 * @param device The target device.
 * @param button The button ID to read.
 * @return Returns true if the button was released this frame.
 */
extern (C) bool MFAuxDisplay_WasReleased(int device, int button);

