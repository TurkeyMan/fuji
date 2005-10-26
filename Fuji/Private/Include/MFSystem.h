#if !defined(_SYSTEM_H)
#define _SYSTEM_H

#include "Timer.h"

struct MFDefaults
{
	struct HeapDefaults
	{
		uint32 dynamicHeapSize;
		uint32 staticHeapSize;
		uint32 dynamicHeapCount;
		uint32 staticHeapCount;

		uint32 maxResources;
		uint32 maxStaticMarkers;
	} heap;

	struct DisplayDefaults
	{
		uint32 displayWidth;
		uint32 displayHeight;
	} display;

	struct ViewDefaults
	{
		int maxViewsOnStack;
		float defaultFOV;
		float defaultAspect;
		float defaultNearPlane;
		float defaultFarPlane;
		float orthoMinX;
		float orthoMinY;
		float orthoMaxX;
		float orthoMaxY;
	} view;

	struct TextureDefaults
	{
		uint32 maxTextures;
	} texture;

	struct MaterialDefaults
	{
		uint32 maxMaterialTypes;
		uint32 maxMaterialDefs;
		uint32 maxMaterials;
	} material;

	struct ModelDefaults
	{
		uint32 maxModels;
	} model;

	struct FileSystemDefaults
	{
		uint32 maxOpenFiles;
		uint32 maxFileSystems;
		uint32 maxFileSystemStackSize;
	} filesys;

	struct SoundDefaults
	{
		int maxMusicTracks;
	} sound;

	struct InputDefaults
	{
		bool allowMultipleMice;
		bool mouseZeroIsSystemMouse;
		bool systemMouseUseWindowsCursor;
	} input;

	struct MiscellaneousDefaults
	{
		bool enableUSBOnStartup;
	} misc;
};

extern MFDefaults gDefaults;

enum MFCallbacks
{
	MFCB_Unknown = -1,

	MFCB_Update = 0,
	MFCB_Draw,
	MFCB_Init,
	MFCB_Deinit,

	MFCB_HandleSystemMessages,

	MFCB_DisplayLost,
	MFCB_DisplayReset,

	MFCB_VerticalBlank,

	MFCB_Max,
	MFCB_ForceInt = 0x7FFFFFFF
};

// Fuji system functions

MFPlatform MFSystem_GetCurrentPlatform();

const char * const MFSystem_GetPlatformString(int platform);
const char * const MFSystem_GetPlatformName(int platform);

MFEndian MFSystem_GetPlatformEndian(int platform);

// Timer Related

// Read Time Stamp Counter
uint64 MFSystem_ReadRTC();
uint64 MFSystem_GetRTCFrequency();

// System Timer
float MFSystem_TimeDelta();

float MFSystem_GetFPS();
uint32 MFSystem_GetFrameCounter();

// these MUST be implemented by the game
void Game_InitSystem();
void Game_Init();
void Game_Update();
void Game_Draw();
void Game_Deinit();

#include "MFSystem.inl"

#endif // _SYSTEM_H
