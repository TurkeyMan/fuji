#include "Fuji.h"

#if defined(_WINDOWS) || defined(_XBOX)		
#include <d3d9.h>
extern IDirect3DDevice9 *pd3dDevice;
#endif

void MFSystem_HandleEventsPlatformSpecific();

// Output a string to the debugger.
void MFDebug_Message(const char *pMessage)
{
	fprintf(stderr, MFStr("%s\n", pMessage));
}

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
	LOGD(MFStr("%s(%d) : Assertion Failure.",pFile,line));
	LOGD(MFStr("Failed Condition: %s\n%s", pReason, pMessage));
	Callstack_Log();

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

		Font_DrawTextf(gpDebugFont, 110, 60, 20, MakeVector(1,0,0,1), "Software Failure. Press left mouse button to continue");
		Font_DrawTextf(gpDebugFont, 240, 80, 20, MakeVector(1,0,0,1), "Guru Meditation: ");

		Font_DrawTextf(gpDebugFont, 80, 120, 20, MakeVector(1,0,0,1), "Assertion Failure:");
		Font_DrawTextf(gpDebugFont, 80, 140, 20, MakeVector(1,0,0,1), MFStr("Failed Condition: %s", pReason));
		Font_DrawTextf(gpDebugFont, 80, 160, 20, MakeVector(1,0,0,1), MFStr("File: %s, Line: %d", pFile, line));
		Font_DrawTextf(gpDebugFont, 80, 190, 20, MakeVector(1,0,0,1), MFStr("Message: %s", pMessage));

#if !defined(_RETAIL)
		Font_DrawTextf(gpDebugFont, 80, 230, 20, MakeVector(1,0,0,1), "Callstack:");
		float y = 250.0f;
		for(int a=Callstack.size()-1; a>=0; a--)
		{
			Font_DrawTextf(gpDebugFont, 100, y, 20, MakeVector(1,0,0,1), Callstack[a]);
			y+=20.0f;
		}
#else
		Font_DrawTextf(gpDebugFont, 80, 230, 20, MakeVector(1,0,0,1), "Callstack not available in _RETAIL builds");
#endif

		MFDisplay_EndFrame();
	}
}
