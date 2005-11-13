#include "Fuji.h"
#include "MFInput_Internal.h"

#include <pspdebug.h>

extern int gQuit;

void MFDebug_Message(const char *pMessage)
{
	fprintf(stderr, pMessage);
}

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
	LOGD(STR("%s(%d) : Assertion Failure.",pFile,line));
	LOGD(STR("Failed Condition: %s\n%s", pReason, pMessage));
	Callstack_Log();

	const char *pString;

	pspDebugScreenSetXY(35 - 9, 13);
	pspDebugScreenPrintf("Assertion Failure!");

	pString = STR("%s(%d)", pFile, line);
	pspDebugScreenSetXY(35 - (strlen(pString)>>1), 15);
	pspDebugScreenPrintf(pString);

	pString = STR("Failed Condition: %s", pReason);
	pspDebugScreenSetXY(35 - (strlen(pString)>>1), 17);
	pspDebugScreenPrintf(pString);

	pspDebugScreenSetXY(35 - (strlen(pMessage)>>1), 18);
	pspDebugScreenPrintf(pMessage);

	while(!Input_WasPressed(IDD_Gamepad, 0, Button_P2_Start))
	{
		Input_Update();

		if(gQuit)
			sceKernelExitGame();
	}
}
