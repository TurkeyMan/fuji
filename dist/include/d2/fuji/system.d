module fuji.system;

public import fuji.fuji;

// TODO: REMOVE ME!
struct MFIniLine;

/**
* System callbacks.
* System callback function idenifiers.
*/
enum MFCallback
{
	Unknown = -1,			/**< Unknown callback. */

	RegisterModules = 0,	/**< Register modules callback. This is called after the Fuji modules are registered, allowing registration of custom modules. */
	FileSystemInit,			/**< File system init callback. This is called after the filesystem initialises, before Fuji attempts to load any debug resources. */
	InitDone,				/**< Post engine init callback. Called after Fuji has completed initialisation. */
	Deinit,					/**< Deinit callback. */

	Update,					/**< Update callback. */
	Draw,					/**< Draw callback. */

	HandleSystemMessages,	/**< System Message Handler callback. */

	GainedFocus,			/**< Focus Gained callback. */
	LostFocus,				/**< Focus Lost callback. */

	DisplayLost,			/**< Display Lost callback. */
	DisplayReset,			/**< Reset Display callback. */

	DisplayResize,			/**< Display resize callback. */

	VerticalBlank			/**< Vertical Blank callback. */
}

/**
* Fuji system callback function prototype.
*/
alias void function() MFSystemCallbackFunction;

/**
* Parameters supplied when initialising Fuji.
*/
struct MFInitParams
{
	/**
	* Describe the default display.
	*/
	struct DisplayMode
	{
		MFRect displayRect = MFRect(0, 0, 1280, 720);	//*< Display rect. If not set, the value returned from MFDisplay_GetDefaultRes() is used. */
		bool bFullscreen = false;						//*< Fullscreen flag. */
	}

	const(char)* pAppTitle = "Fuji Window".ptr;			//*< A title used to represent the application */

	void* hInstance;									//*< The WIN32 hInstance paramater supplied to WinMain() */
	void* hWnd;											//*< An optional hWnd to a WIN32 window that will contain the viewport */

	const(char)* pCommandLine;							//*< Pointer to the command line string */

	int argc;											//*< The argc parameter supplied to main() */
	const(char*)*argv;									//*< The argv paramater supplied to main() */

	DisplayMode display;								//*< Display settings. */

	bool hideSystemInfo;								//*< Hide the frame rate and system logo() */
}


/**
* Begin the fuji main loop.
* Begin initialisation of Fuji, and continue with the Fuji main loop.
* @return An error code that can be returned to the OS.
*/
extern (C) int MFMain(const ref MFInitParams initParams);

/**
* Quit the Fuji application.
* Quits the Fuji application.
* @return None.
*/
extern (C) void MFSystem_Quit();

/**
* Set a Fuji system callback.
* Sets a Fuji system callback function.
* @param callback The callback to set from the MFCallback enum.
* @param pCallbackFunction Pointer to the callback function.
* @return A pointer to the old callback function which MUST be called in the new registered callback.
*/
extern (C) MFSystemCallbackFunction MFSystem_RegisterSystemCallback(MFCallback callback, MFSystemCallbackFunction pCallbackFunction);

/**
* Get a Fuji system callback function.
* Gets a registered Fuji system callback function pointer.
* @param callback The callback to get from the MFCallback enum.
* @return The currently registered callback function for the requested callback.
*/
extern (C) MFSystemCallbackFunction MFSystem_GetSystemCallback(MFCallback callback);

/**
* Generate engine initialisation data.
* Generates a string containing the current engine initialisation options.
* @param tabDepth Initial tab depth for ini generation.
* @return A string containing initialisation parameters for the engine in MFIni '.ini' format.
*/
extern (C) const(char)* MFSystem_GetSettingString(int tabDepth);

/**
* Initialise Fuji from saved settings.
* Initialise Fuji from settings data stored in an MFIni File.
* @param pSettings Pointer to the first MFiniLine in an options group containing engine initialisation options.
* @return None.
*/
extern (C) void MFSystem_InitFromSettings(const MFIniLine *pSettings);

/**
* Gets the current platform.
* Gets the current running platform.
* @return The current running platform.
* @see MFSystem_GetPlatformString()
* @see MFSystem_GetPlatformName()
*/
extern (C) MFPlatform MFSystem_GetCurrentPlatform();

/**
* Gets a platform identifier string.
* Gets an identifier string for a specified platform.
* @param platform Platform to get the identifier string for.
* @return The platform identifier string.
* @see MFSystem_GetCurrentPlatform()
* @see MFSystem_GetPlatformName()
*/
extern (C) const(char)* MFSystem_GetPlatformString(int platform);

/**
* Gets a platform name.
* Gets the name string for a specified platform.
* @param platform Platform identifier to get the name of.
* @return The platform name string.
* @see MFSystem_GetCurrentPlatform()
* @see MFSystem_GetPlatformString()
*/
extern (C) const(char)* MFSystem_GetPlatformName(int platform);

/**
* Gets the name of this system.
* Gets a user friendly name for this system. Typically a network identification name.
* @return The system name string.
*/
extern (C) const(char)* MFSystem_GetSystemName();

/**
* Gets a platforms native endian.
* Gets the native endian for the specifid platform.
* @param platform Platform identifier to get the endian of.
* @return The hardware endian of the specified platform.
* @see MFSystem_GetCurrentPlatform()
*/
extern (C) MFEndian MFSystem_GetPlatformEndian(int platform);

/**
* Read the time stamp counter.
* Gets the time stamp counters current clock tick.
* @return The time stamp counters current tick.
* @see MFSystem_GetRTCFrequency()
*/
extern (C) ulong MFSystem_ReadRTC();

/**
* Read the time stamp counter frequency.
* Gets the time stamp counters clock frequency.
* @return The time stamp counters clock frequency.
* @see MFSystem_ReadRTC()
*/
extern (C) ulong MFSystem_GetRTCFrequency();

/**
* Gets the current time delta.
* Gets the current time delta.
* @return The current time delta.
* @see MFSystem_GetFPS()
*/
extern (C) float MFSystem_GetTimeDelta();

/**
* Gets the current frame rate.
* Gets the current frame rate.
* @return The current frame rate.
* @see MFSystem_TimeDelta()
*/
extern (C) float MFSystem_GetFPS();

/**
* Gets the current frame.
* Gets the current frame count.
* @return The current frame count.
* @see MFSystem_GetFPS()
*/
extern (C) uint MFSystem_GetFrameCounter();

