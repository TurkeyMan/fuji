#if !defined(_SYSTEM_H)
#define _SYSTEM_H

uint64 ReadPerformanceCounter();
uint64 GetPerfprmanceFrequency();

float GetFPS();

int System_GameLoop();

void System_UpdateTimeDelta();

void System_Init();
void System_Update();
void System_PostUpdate();
void System_Draw();
void System_Deinit();

void Game_Init();
void Game_Update();
void Game_Draw();
void Game_Deinit();

extern float TIMEDELTA;

#endif // _SYSTEM_H
