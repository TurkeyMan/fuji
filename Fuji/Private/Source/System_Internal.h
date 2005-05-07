#if !defined(_SYSTEM_INTERNAL_H)
#define _SYSTEM_INTERNAL_H

#include "System.h"

// system functions
int System_GameLoop();

void System_UpdateTimeDelta();

void System_Init();
void System_Update();
void System_PostUpdate();
void System_Draw();
void System_Deinit();

#if defined(_WINDOWS) || defined(_DC)
	void DoMessageLoop();
#elif defined(_LINUX)
	void CheckEvents();
#endif

#if defined(_LINUX)
	void System_MessageBox(char *title, char *message);
#endif

#endif