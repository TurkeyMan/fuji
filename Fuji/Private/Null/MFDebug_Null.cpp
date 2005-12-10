#include "Fuji.h"
#include "MFSystem_Internal.h"
#include "Display_Internal.h"
#include "MFFont.h"
#include "MFView.h"
#include "MFPrimitive.h"

#if defined(_WINDOWS)
#include <d3d9.h>
extern IDirect3DDevice9 *pd3dDevice;
#endif
#if defined(_XBOX)
#include <stdio.h>
extern IDirect3DDevice8 *pd3dDevice;
#endif

void MFSystem_HandleEventsPlatformSpecific();

// Output a string to the debugger.
void MFDebug_Message(const char *pMessage)
{
	fprintf(stderr, MFStr("%s\n", pMessage));
}

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
	MFDebug_Message(MFStr("%s(%d) : Assertion Failure.",pFile,line));
	MFDebug_Message(MFStr("Failed Condition: %s\n%s", pReason, pMessage));
#if !defined(_RETAIL)
	MFCallstack_Log();
#endif

	while(1)
	{
		MFSystem_HandleEventsPlatformSpecific();

		MFSystem_UpdateTimeDelta();
		gFrameCount++;

		MFSystem_Update();

		MFDisplay_BeginFrame();

		MFView_SetDefault();
		MFView_SetOrtho();

		// Set some renderstates
#if defined(_WINDOWS) || defined(_XBOX)		
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTexture(0, NULL);
#endif

		MFPrimitive(PT_TriStrip);

		MFBegin(4);
		MFSetColour(0xFF000000);
		MFSetPosition(0, 0, 0);
		MFSetPosition(640, 0, 0);
		MFSetPosition(0, 480, 0);
		MFSetPosition(640, 480, 0);
		MFEnd();

		if(!(((uint32)gSystemTimer.GetSecondsF()) % 2))
		{
			MFBegin(4);
			MFSetColour(0xFFFF0000);
			MFSetPosition(50, 50, 0);
			MFSetPosition(590, 50, 0);
			MFSetPosition(50, 110, 0);
			MFSetPosition(590, 110, 0);
			MFEnd();

			MFBegin(4);
			MFSetColour(0xFF000000);
			MFSetPosition(55, 55, 0);
			MFSetPosition(585, 55, 0);
			MFSetPosition(55, 105, 0);
			MFSetPosition(585, 105, 0);
			MFEnd();
		}

		MFFont_DrawTextf(MFFont_GetDebugFont(), 110, 60, 20, MakeVector(1,0,0,1), "Software Failure. Press left mouse button to continue");
		MFFont_DrawTextf(MFFont_GetDebugFont(), 240, 80, 20, MakeVector(1,0,0,1), "Guru Meditation: ");

		MFFont_DrawTextf(MFFont_GetDebugFont(), 80, 120, 20, MakeVector(1,0,0,1), "Assertion Failure:");
		MFFont_DrawTextf(MFFont_GetDebugFont(), 80, 140, 20, MakeVector(1,0,0,1), MFStr("Failed Condition: %s", pReason));
		MFFont_DrawTextf(MFFont_GetDebugFont(), 80, 160, 20, MakeVector(1,0,0,1), MFStr("File: %s, Line: %d", pFile, line));
		MFFont_DrawTextf(MFFont_GetDebugFont(), 80, 190, 20, MakeVector(1,0,0,1), MFStr("Message: %s", pMessage));

#if !defined(_RETAIL)
		MFFont_DrawTextf(MFFont_GetDebugFont(), 80, 230, 20, MakeVector(1,0,0,1), "Callstack:");
		MFFont_DrawTextf(MFFont_GetDebugFont(), 100, 250.0f, 20, MakeVector(1,0,0,1), MFCallstack_GetCallstackString());
#else
		MFFont_DrawTextf(MFFont_GetDebugFont(), 80, 230, 20, MakeVector(1,0,0,1), "Callstack not available in _RETAIL builds");
#endif

		MFDisplay_EndFrame();
	}
}
