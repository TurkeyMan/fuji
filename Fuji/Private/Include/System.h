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

	struct TextureDefaults
	{
		uint32 maxTextures;
	} texture;

	struct MaterialDefaults
	{
		uint32 maxMaterials;
	} material;
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

int System_GameLoop();

void System_UpdateTimeDelta();

void System_Init();
void System_Update();
void System_PostUpdate();
void System_Draw();
void System_Deinit();

void Game_InitSystem();
void Game_Init();
void Game_Update();
void Game_Draw();
void Game_Deinit();

#if defined(_WINDOWS) || defined(_DC)
	void DoMessageLoop();
#elif defined(_LINUX)
	void CheckEvents();
#endif

#if defined(_LINUX)
	void System_MessageBox(char *title, char *message);
#endif

extern uint32 gFrameCount;

#endif // _SYSTEM_H
