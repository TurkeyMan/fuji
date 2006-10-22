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

	/** Display Defaults. */
	struct DisplayDefaults
	{
		uint32 displayWidth;			/**< Display width */
		uint32 displayHeight;			/**< Display height */
	} display;

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

	/** Texture Defaults. */
	struct TextureDefaults
	{
		uint32 maxTextures;				/**< Maximum loaded textures */
	} texture;

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
		uint32 maxFileSystems;			/**< Maximum file systems */
		uint32 maxFileSystemStackSize;	/**< Maximum file system stack size */
		uint32 maxHTTPFiles;			/**< Maximum number of concurrent HTTP files */
		uint32 maxHTTPFileCache;		/**< Maximum size of HTTP file cache */
	} filesys;

	/** Sound Defaults. */
	struct SoundDefaults
	{
		int maxMusicTracks;				/**< Maximum music tracks */
	} sound;

	/** Input Defaults. */
	struct InputDefaults
	{
		bool allowMultipleMice;				/**< Allow multiple mice? */
		bool mouseZeroIsSystemMouse;		/**< Mouse 0 is System mouse? */
		bool systemMouseUseWindowsCursor;	/**< System mouse uses windows cursor? */
	} input;

	/** Miscellaneous Defaults. */
	struct MiscellaneousDefaults
	{
		bool enableUSBOnStartup;		/**< Unable USB on startup (for PSP) */
	} misc;
};

/**
 * Fuji Defaults.
 * Extern to Fuji default values registry.
 */
extern MFDefaults gDefaults;

/**
 * System callbacks.
 * System callback function idenifiers.
 */
enum MFCallbacks
{
	MFCB_Unknown = -1,			/**< Unknown callback */

	MFCB_Update = 0,			/**< Update callback */
	MFCB_Draw,					/**< Draw callback */
	MFCB_Init,					/**< Init callback */
	MFCB_Deinit,				/**< Deinit callback */

	MFCB_HandleSystemMessages,	/**< System Message Handler callback */

	MFCB_DisplayLost,			/**< Display Lost callback */
	MFCB_DisplayReset,			/**< Reset Display callback */

	MFCB_VerticalBlank,			/**< Vertical Blank callback */

	MFCB_Max,					/**< Max callback */
	MFCB_ForceInt = 0x7FFFFFFF	/**< Force enum to int type */
};


// Fuji system functions

/**
 * Gets the current platform.
 * Gets the current running platform.
 * @return Returns the current running platform.
 * @see MFSystem_GetPlatformString()
 * @see MFSystem_GetPlatformName()
 */
MFPlatform MFSystem_GetCurrentPlatform();

/**
 * Gets a platform identifier string.
 * Gets an identifier string for a specified platform.
 * @param platform Platform to get the identifier string for.
 * @return Returns the platform identifier string.
 * @see MFSystem_GetCurrentPlatform()
 * @see MFSystem_GetPlatformName()
 */
const char * MFSystem_GetPlatformString(int platform);

/**
 * Gets a platform name.
 * Gets the name string for a specified platform.
 * @param platform Platform identifier to get the name of.
 * @return Returns the platform name string.
 * @see MFSystem_GetCurrentPlatform()
 * @see MFSystem_GetPlatformString()
 */
const char * MFSystem_GetPlatformName(int platform);

/**
 * Gets the name of this system.
 * Gets a user friendly name for this system. Typically a network identification name.
 * @return Returns the system name string.
 */
const char * MFSystem_GetSystemName();

/**
 * Gets a platforms native endian.
 * Gets the native endian for the specifid platform.
 * @param platform Platform identifier to get the endian of.
 * @return The hardware endian of the specified platform.
 * @see MFSystem_GetCurrentPlatform()
 */
MFEndian MFSystem_GetPlatformEndian(int platform);

/**
 * Read the time stamp counter.
 * Gets the time stamp counters current clock tick.
 * @return The time stamp counters current tick.
 * @see MFSystem_GetRTCFrequency()
 */
uint64 MFSystem_ReadRTC();

/**
 * Read the time stamp counter frequency.
 * Gets the time stamp counters clock frequency.
 * @return The time stamp counters clock frequency.
 * @see MFSystem_ReadRTC()
 */
uint64 MFSystem_GetRTCFrequency();

/**
 * Gets the current time delta.
 * Gets the current time delta.
 * @return The current time delta.
 * @see MFSystem_GetFPS()
 */
float MFSystem_TimeDelta();

/**
 * Gets the current frame rate.
 * Gets the current frame rate.
 * @return The current frame rate.
 * @see MFSystem_TimeDelta()
 */
float MFSystem_GetFPS();

/**
 * Gets the current frame.
 * Gets the current frame count.
 * @return The current frame count.
 * @see MFSystem_GetFPS()
 */
uint32 MFSystem_GetFrameCounter();

#include "MFSystem.inl"


// these MUST be implemented by the game

/**
 * The games pre-engine init hook.
 * The games pre-engine init hook. The game can use this function to modify engine defaults before the engine initialises.
 * @return None.
 */
void Game_InitSystem();

/**
 * The games initialisation function.
 * This function is called after engine initialisation to initialise the game before begining the main loop.
 * @return None.
 */
void Game_Init();

/**
 * The games update function.
 * This is called by the engine to update the game.
 * @return None.
 */
void Game_Update();

/**
 * The games draw function.
 * This is called by the engine to draw the game.
 * @return None.
 */
void Game_Draw();

/**
 * The games deinitialisation function.
 * This function is called before the engine deinitialisation to clean up the game before app termination.
 * @return None.
 */
void Game_Deinit();

#endif // _MFSYSTEM_H

/** @} */
