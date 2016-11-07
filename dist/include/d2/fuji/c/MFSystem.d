module fuji.c.MFSystem;

import fuji.c.Fuji;
import fuji.c.MFFileSystem;

nothrow:

// TODO: REMOVE ME!
struct MFIniLine;

/**
* Fuji system callback function prototype.
*/
alias extern (C) void function(void*) MFSystemCallbackFunction;

@nogc:

enum MFMonth : ushort
{
	January = 1,
	February,
	March,
	April,
	May,
	June,
	July,
	August,
	September,
	October,
	November,
	December
}

enum MFDayOfWeek : ushort
{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
}

struct MFSystemTime
{
	ushort year;
	MFMonth month;
	MFDayOfWeek dayOfWeek;
	ushort day;
	ushort hour;
	ushort minute;
	ushort second;
	ushort tenthMillisecond; // ie, 100 microseconds

nothrow:
@nogc:
	bool opEquals(MFSystemTime other) const pure	{ return tenthMillisecond == other.tenthMillisecond && second == other.second && minute == other.minute && hour == other.hour && day == other.day && month == other.month && year == other.year; }
	int opCmp(MFSystemTime other) const pure
	{
		// TODO: should we combine these and compare as long's?
		// ...loading misaligned ulong's is probably worse than field comparison.
		int diff = year - other.year;
		if(diff) return diff;
		diff = month - other.month;
		if(diff) return diff;
		diff = day - other.day;
		if(diff) return diff;
		diff = hour - other.hour;
		if(diff) return diff;
		diff = minute - other.minute;
		if(diff) return diff;
		diff = second - other.second;
		if(diff) return diff;
		return tenthMillisecond - other.tenthMillisecond;
	}
}

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
* Parameters supplied when initialising Fuji.
*/
struct MFInitParams
{
	/**
	* Describe the default display.
	*/
	struct DisplayMode
	{
		MFRect displayRect = MFRect(0, 0, 1280, 720);	/**< Display rect. If not set, the value returned from MFDisplay_GetDefaultRes() is used. */
		bool bFullscreen = false;						/**< Fullscreen flag. */
	}

	const(char)* pAppTitle = "Fuji Window".ptr;			/**< A title used to represent the application */

	void* hInstance;									/**< The WIN32 hInstance paramater supplied to WinMain() */
	void* hWnd;											/**< An optional hWnd to a WIN32 window that will contain the viewport */

	const(char)* pCommandLine;							/**< Pointer to the command line string */

	int argc;											/**< The argc parameter supplied to main() */
	const(char*)*argv;									/**< The argv paramater supplied to main() */

	DisplayMode display;								/**< Display settings. */

	bool hideSystemInfo;								/**< Hide the frame rate and system logo() */
}


/**
* Fuji Defaults structure.
* Stores default values for most aspects of the engine.
*/
extern (C) struct MFDefaults
{
	/** Heap Defaults. */
	extern (C) struct HeapDefaults
	{
		uint maxAllocations;			/**< Maximum allocations */
		uint maxStaticHeapMarkers;	/**< Maximum static heap markers */
	}

	/** System Defaults. */
	extern (C) struct SystemDefaults
	{
		int threadPriority;				/**< Main thread priority */
		uint maxThreads;				/**< Maximum number of threads */
		uint maxTlsSlots;				/**< Maximum number of MFTls slots */
	}

	/** Display Defaults. */
	extern (C) struct DisplayDefaults
	{
		const(char)* pWindowTitle;		/**< Game window title */
		const(char)* pIcon;				/**< Resource name of the window icon */
		bool hideMouseCursor;			/**< Hide the mouse cursor when it hovers over the application window */
	}

	/** Render Defaults. */
	extern (C) struct RenderDefaults
	{
		size_t renderHeapSize;			/**< Size of the renderer temp heap */
	}

	/** View Defaults. */
	extern (C) struct ViewDefaults
	{
		int maxViewsOnStack;			/**< Maximum view stack height */
		float defaultFOV;				/**< Default field of view */
		float defaultAspect;			/**< Default aspect ratio */
		float defaultNearPlane;			/**< Default near plane */
		float defaultFarPlane;			/**< Default far plane */
		float orthoMinX;				/**< Ortho min X */
		float orthoMinY;				/**< Ortho min Y */
		float orthoMaxX;				/**< Ortho max X */
		float orthoMaxY;				/**< Ortho max Y */
	}

	/** Material Defaults. */
	extern (C) struct MaterialDefaults
	{
		uint maxMaterialTypes;			/**< Maximum material types */
		uint maxMaterialDefs;			/**< Maximum material definitions */
		uint maxMaterials;				/**< Maximum materials */
	}

	/** Model Defaults. */
	extern (C) struct ModelDefaults
	{
		uint maxModels;					/**< Maximum models */
	}

	/** Animation Defaults. */
	extern (C) struct AnimationDefaults
	{
		uint maxAnimations;				/**< Maximum animations */
	}

	/** FileSystem Defaults. */
	extern (C) struct FileSystemDefaults
	{
		uint maxOpenFiles;				/**< Maximum open files */
		uint maxFinds;					/**< Maximum finds */
		uint maxFileSystems;			/**< Maximum file systems */
		uint maxFileSystemStackSize;	/**< Maximum file system stack size */
		uint maxHTTPFiles;				/**< Maximum number of concurrent HTTP files */
		uint maxHTTPFileCache;			/**< Maximum size of HTTP file cache */
	}

	/** Sound Defaults. */
	extern (C) struct SoundDefaults
	{
		int maxSounds;					/**< Maximum sounds */
		int maxVoices;					/**< Maximum voices */
		int maxMusicTracks;				/**< Maximum music tracks */
		bool useGlobalFocus;			/**< Use global focus on PC */
	}

	/** Input Defaults. */
	extern (C) struct InputDefaults
	{
		bool allowMultipleMice;				/**< Allow multiple mice? */
		bool mouseZeroIsSystemMouse;		/**< Mouse 0 is System mouse? */
		bool systemMouseUseWindowsCursor;	/**< System mouse uses windows cursor? */
		bool useDirectInputKeyboard;		/**< Use DirectInput to read the keyboard */
		bool useXInput;						/**< Use XInput when available on PC */
	}

	/** Midi Defaults. */
	extern (C) struct MidiDefaults
	{
		bool useMidi;						/**< Use MIDI devices */
	}

	/** Miscellaneous Defaults. */
	extern (C) struct MiscellaneousDefaults
	{
		bool enableUSBOnStartup;		/**< Unable USB on startup (for PSP) */
	}

	/** Plugin Selection Defaults. */
	extern (C) struct PluginDefaults
	{
		int renderPlugin;
		int soundPlugin;
		int inputPlugin;
	}

	HeapDefaults heap;
	SystemDefaults system;
	DisplayDefaults display;
	RenderDefaults render;
	ViewDefaults view;
	MaterialDefaults material;
	ModelDefaults model;
	AnimationDefaults animation;
	FileSystemDefaults filesys;
	SoundDefaults sound;
	InputDefaults input;
	MidiDefaults midi;
	MiscellaneousDefaults misc;
	PluginDefaults plugin;
};

/**
* Fuji Defaults.
* Extern to Fuji default values registry.
*/
extern (C) extern __gshared MFDefaults gDefaults;


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
* @param pUserData Optional user-data pointer that will be passed to the handler each call.
* @param ppOldUserData Optional pointer that received the specified callback's old user-data.
* @return A pointer to the old callback function which MUST be called in the new registered callback.
*/
extern (C) MFSystemCallbackFunction MFSystem_RegisterSystemCallback(MFCallback callback, MFSystemCallbackFunction pCallbackFunction, void* pUserData = null, void** ppOldUserData = null);

/**
* Get a Fuji system callback function.
* Gets a registered Fuji system callback function pointer.
* @param callback The callback to get from the MFCallback enum.
* @param ppUserData Optional pointer that receives the specified callback's user-data.
* @return The currently registered callback function for the requested callback.
*/
extern (C) MFSystemCallbackFunction MFSystem_GetSystemCallback(MFCallback callback, void **ppUserData = null);

/**
* Get the engine defaults.
* Gets a pointer to the global engine default settings.
* @return Pointer to the global MFDefaults structure.
*/
extern (C) MFDefaults* MFSystem_GetDefaults();

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

extern (C) void MFSystem_SystemTime(MFSystemTime* pSystemTime);
extern (C) void MFSystem_SystemTimeToFileTime(const(MFSystemTime)* pSystemTime, MFFileTime* pFileTime);
extern (C) void MFSystem_FileTimeToSystemTime(const(MFFileTime)* pFileTime, MFSystemTime* pSystemTime);
extern (C) void MFSystem_SystemTimeToLocalTime(const(MFSystemTime)* pSystemTime, MFSystemTime* pLocalTime);
extern (C) void MFSystem_LocalTimeToSystemTime(const(MFSystemTime)* pLocalTime, MFSystemTime* pSystemTime);

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
* @see MFSystem_GetTimeDelta()
*/
extern (C) float MFSystem_GetFPS();

/**
* Gets the current frame.
* Gets the current frame count.
* @return The current frame count.
* @see MFSystem_GetFPS()
*/
extern (C) uint MFSystem_GetFrameCounter();

