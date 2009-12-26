#if !defined(_MFSYSTEM_INTERNAL_H)
#define _MFSYSTEM_INTERNAL_H

#include "MFSystem.h"

// system functions
int MFSystem_GameLoop();
void MFSystem_RunFrame();

void MFSystem_UpdateTimeDelta();

void MFSystem_Init();
void MFSystem_Update();
void MFSystem_PostUpdate();
void MFSystem_Draw();
void MFSystem_Deinit();

void MFSystem_InitModulePlatformSpecific();
void MFSystem_DeinitModulePlatformSpecific();

void MFSystem_UpdatePlatformSpecific();
void MFSystem_DrawPlatformSpecific();

#if defined(MF_WINDOWS) || defined(_DC)
	void DoMessageLoop();
#elif defined(_LINUX)
	void CheckEvents();
#endif

#if defined(_LINUX)
	void MFSystem_MessageBox(char *title, char *message);
#endif

#endif
