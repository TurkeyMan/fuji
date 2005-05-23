#if !defined(_SYSTEM_H)
#define _SYSTEM_H

#include "Timer.h"

struct FujiDefaults
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
		uint32 otrhoWidth;
		uint32 otrhoHeight;
	} display;

	struct ViewDefaults
	{
		int maxViewsOnStack;
	} view;

	struct TextureDefaults
	{
		uint32 maxTextures;
	} texture;

	struct MaterialDefaults
	{
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
};

extern FujiDefaults gDefaults;

// Timer Related

// Read Time Stamp Counter
uint64 RDTSC();
uint64 GetTSCFrequency();

// System Timer
extern Timer gSystemTimer;
extern float gSystemTimeDelta;

#define GetFPS() gSystemTimer.GetFPS()
#define TIMEDELTA gSystemTimeDelta

extern uint32 gFrameCount;

// these MUST be implemented by the game
void Game_InitSystem();
void Game_Init();
void Game_Update();
void Game_Draw();
void Game_Deinit();

#endif // _SYSTEM_H
