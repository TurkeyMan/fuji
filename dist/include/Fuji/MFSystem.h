/**
 * @file MFSystem.h
 * @brief System related functions.
 * @author Manu Evans
 * @defgroup MFSystem System Related
 * @{
 */

#if !defined(_MFSYSTEM_H)
#define _MFSYSTEM_H

#include "Timer.h"

class MFIniLine;

/**
 * Fuji Defaults structure.
 * Stores default values for most aspects of the engine.
 */
struct MFDefaults
{
	/** Heap Defaults. */
	struct HeapDefaults
	{
		uint32 maxAllocations;			/**< Maximum allocations */
		uint32 maxStaticHeapMarkers;	/**< Maximum static heap markers */
	} heap;

	/** System Defaults. */
	struct SystemDefaults
	{
		int threadPriority;				/**< Main thread priority */
		uint32 maxThreads;				/**< Maximum number of threads */
		uint32 maxTlsSlots;				/**< Maximum number of MFTls slots */
	} system;

	/** Display Defaults. */
	struct DisplayDefaults
	{
		const char *pWindowTitle;		/**< Game window title */
		const char *pIcon;				/**< Resource name of the window icon */
		bool hideMouseCursor;			/**< Hide the mouse cursor when it hovers over the application window */
	} display;

	/** Render Defaults. */
	struct RenderDefaults
	{
		size_t renderHeapSize;			/**< Size of the renderer temp heap */
	} render;

	/** View Defaults. */
	struct ViewDefaults
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
	} view;

	/** Material Defaults. */
	struct MaterialDefaults
	{
		uint32 maxMaterialTypes;		/**< Maximum material types */
		uint32 maxMaterialDefs;			/**< Maximum material definitions */
		uint32 maxMaterials;			/**< Maximum materials */
	} material;

	/** Model Defaults. */
	struct ModelDefaults
	{
		uint32 maxModels;				/**< Maximum models */
	} model;

	/** Animation Defaults. */
	struct AnimationDefaults
	{
		uint32 maxAnimations;			/**< Maximum animations */
	} animation;

	/** FileSystem Defaults. */
	struct FileSystemDefaults
	{
		uint32 maxOpenFiles;			/**< Maximum open files */
		uint32 maxFinds;				/**< Maximum finds */
		uint32 maxFileSystems;			/**< Maximum file systems */
		uint32 maxFileSystemStackSize;	/**< Maximum file system stack size */
		uint32 maxHTTPFiles;			/**< Maximum number of concurrent HTTP files */
		uint32 maxHTTPFileCache;		/**< Maximum size of HTTP file cache */
	} filesys;

	/** Sound Defaults. */
	struct SoundDefaults
	{
		int maxSounds;					/**< Maximum sounds */
		int maxVoices;					/**< Maximum voices */
		int maxMusicTracks;				/**< Maximum music tracks */
		bool useGlobalFocus;			/**< Use global focus on PC */
	} sound;

	/** Input Defaults. */
	struct InputDefaults
	{
		bool allowMultipleMice;				/**< Allow multiple mice? */
		bool mouseZeroIsSystemMouse;		/**< Mouse 0 is System mouse? */
		bool systemMouseUseWindowsCursor;	/**< System mouse uses windows cursor? */
		bool useDirectInputKeyboard;		/**< Use DirectInput to read the keyboard */
		bool useXInput;						/**< Use XInput when available on PC */
	} input;

	/** Miscellaneous Defaults. */
	struct MiscellaneousDefaults
	{
		bool enableUSBOnStartup;		/**< Unable USB on startup (for PSP) */
	} misc;

	/** Plugin Selection Defaults. */
	struct PluginDefaults
	{
		int renderPlugin;
		int soundPlugin;
		int inputPlugin;
	} plugin;
};

/**
 * Fuji Defaults.
 * Extern to Fuji default values registry.
 */
extern "C" MFDefaults gDefaults;

/**
 * System callbacks.
 * System callback function idenifiers.
 */
enum MFCallback
{
	MFCB_Unknown = -1,			/**< Unknown callback. */

	MFCB_RegisterModules = 0,	/**< Register modules callback. This is called after the Fuji modules are registered, allowing registration of custom modules. */
	MFCB_FileSystemInit,		/**< File system init callback. This is called after the filesystem initialises, before Fuji attempts to load any debug resources. */
	MFCB_InitDone,				/**< Post engine init callback. Called after Fuji has completed initialisation. */
	MFCB_Deinit,				/**< Deinit callback. */

	MFCB_Update,				/**< Update callback. */
	MFCB_Draw,					/**< Draw callback. */

	MFCB_HandleSystemMessages,	/**< System Message Handler callback. */

	MFCB_GainedFocus,			/**< Focus Gained callback. */
	MFCB_LostFocus,				/**< Focus Lost callback. */

	MFCB_DisplayLost,			/**< Display Lost callback. */
	MFCB_DisplayReset,			/**< Reset Display callback. */

	MFCB_DisplayResize,			/**< Display resize callback. */

	MFCB_VerticalBlank,			/**< Vertical Blank callback. */

	MFCB_Max,					/**< Max callback. */
	MFCB_ForceInt = 0x7FFFFFFF	/**< Force enum to int type. */
};

/**
 * Fuji system callback function prototype.
 */
typedef void (*MFSystemCallbackFunction)(void);

/**
 * Parameters supplied when initialising Fuji.
 */
struct MFInitParams
{
	const char *pAppTitle;		/**< A title used to represent the application */

	void *hInstance;			/**< The WIN32 hInstance paramater supplied to WinMain() */
	void *hWnd;					/**< An optional hWnd to a WIN32 window that will contain the viewport */

	const char *pCommandLine;	/**< Pointer to the command line string */

	int argc;					/**< The argc parameter supplied to main() */
	const char **argv;			/**< The argv paramater supplied to main() */

	/**
	 * Describe the default display.
	 */
	struct DisplayMode
	{
		MFRect displayRect;		/**< Display rect. If not set, the value returned from MFDisplay_GetDefaultRes() is used. */
		bool bFullscreen;		/**< Fullscreen flag. */
	} display;					/**< Display settings. */

	bool hideSystemInfo;		/**< Hide the frame rate and system logo() */
};

/**
 * Begin the fuji main loop.
 * Begin initialisation of Fuji, and continue with the Fuji main loop.
 * @return An error code that can be returned to the OS.
 */
MF_API int MFMain(MFInitParams *pInitPrams);

/**
 * Quit the Fuji application.
 * Quits the Fuji application.
 * @return None.
 */
MF_API void MFSystem_Quit();

/**
 * Set a Fuji system callback.
 * Sets a Fuji system callback function.
 * @param callback The callback to set from the MFCallback enum.
 * @param pCallbackFunction Pointer to the callback function.
 * @return A pointer to the old callback function which MUST be called in the new registered callback.
 */
MF_API MFSystemCallbackFunction MFSystem_RegisterSystemCallback(MFCallback callback, MFSystemCallbackFunction pCallbackFunction);

/**
 * Get a Fuji system callback function.
 * Gets a registered Fuji system callback function pointer.
 * @param callback The callback to get from the MFCallback enum.
 * @return The currently registered callback function for the requested callback.
 */
MF_API MFSystemCallbackFunction MFSystem_GetSystemCallback(MFCallback callback);

/**
 * Get the engine defaults.
 * Gets a pointer to the global engine default settings.
 * @return Pointer to the global MFDefaults structure.
 */
MF_API MFDefaults* MFSystem_GetDefaults();

/**
 * Generate engine initialisation data.
 * Generates a string containing the current engine initialisation options.
 * @param tabDepth Initial tab depth for ini generation.
 * @return A string containing initialisation parameters for the engine in MFIni '.ini' format.
 */
MF_API const char * MFSystem_GetSettingString(int tabDepth);

/**
 * Initialise Fuji from saved settings.
 * Initialise Fuji from settings data stored in an MFIni File.
 * @param pSettings Pointer to the first MFiniLine in an options group containing engine initialisation options.
 * @return None.
 */
MF_API void MFSystem_InitFromSettings(const MFIniLine *pSettings);

/**
 * Gets the current platform.
 * Gets the current running platform.
 * @return The current running platform.
 * @see MFSystem_GetPlatformString()
 * @see MFSystem_GetPlatformName()
 */
MF_API MFPlatform MFSystem_GetCurrentPlatform();

/**
 * Gets a platform identifier string.
 * Gets an identifier string for a specified platform.
 * @param platform Platform to get the identifier string for.
 * @return The platform identifier string.
 * @see MFSystem_GetCurrentPlatform()
 * @see MFSystem_GetPlatformName()
 */
MF_API const char * MFSystem_GetPlatformString(int platform);

/**
 * Gets a platform name.
 * Gets the name string for a specified platform.
 * @param platform Platform identifier to get the name of.
 * @return The platform name string.
 * @see MFSystem_GetCurrentPlatform()
 * @see MFSystem_GetPlatformString()
 */
MF_API const char * MFSystem_GetPlatformName(int platform);

/**
 * Gets the name of this system.
 * Gets a user friendly name for this system. Typically a network identification name.
 * @return The system name string.
 */
MF_API const char * MFSystem_GetSystemName();

/**
 * Gets a platforms native endian.
 * Gets the native endian for the specifid platform.
 * @param platform Platform identifier to get the endian of.
 * @return The hardware endian of the specified platform.
 * @see MFSystem_GetCurrentPlatform()
 */
MF_API MFEndian MFSystem_GetPlatformEndian(int platform);

/**
 * Read the time stamp counter.
 * Gets the time stamp counters current clock tick.
 * @return The time stamp counters current tick.
 * @see MFSystem_GetRTCFrequency()
 */
MF_API uint64 MFSystem_ReadRTC();

/**
 * Read the time stamp counter frequency.
 * Gets the time stamp counters clock frequency.
 * @return The time stamp counters clock frequency.
 * @see MFSystem_ReadRTC()
 */
MF_API uint64 MFSystem_GetRTCFrequency();

/**
 * Gets the current time delta.
 * Gets the current time delta.
 * @return The current time delta.
 * @see MFSystem_GetFPS()
 */
float MFSystem_TimeDelta();

/**
 * Gets the current time delta.
 * Gets the current time delta.
 * @return The current time delta.
 * @see MFSystem_GetFPS()
 */
MF_API float MFSystem_GetTimeDelta();

/**
 * Gets the current frame rate.
 * Gets the current frame rate.
 * @return The current frame rate.
 * @see MFSystem_TimeDelta()
 */
MF_API float MFSystem_GetFPS();

/**
 * Gets the current frame.
 * Gets the current frame count.
 * @return The current frame count.
 * @see MFSystem_GetFPS()
 */
uint32 MFSystem_FrameCounter();

/**
 * Gets the current frame.
 * Gets the current frame count.
 * @return The current frame count.
 * @see MFSystem_GetFPS()
 */
MF_API uint32 MFSystem_GetFrameCounter();

#include "MFSystem.inl"

#endif // _MFSYSTEM_H

/** @} */
