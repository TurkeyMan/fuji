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
	} heap;

	struct DisplayDefaults
	{
		uint32 otrhoWidth;
		uint32 otrhoHeight;
	} display;


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

#if defined(_WINDOWS)
	void DoMessageLoop();
#endif

extern uint32 gFrameCount;

#endif // _SYSTEM_H
