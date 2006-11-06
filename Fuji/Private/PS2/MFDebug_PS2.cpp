#include "Fuji.h"
#include "MFInput_Internal.h"

#include <stdio.h>

void MFDebug_Message(const char *pMessage)
{
	fprintf(stderr, "%s\n", pMessage);
}

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
	MFDebug_Message(MFStr("%s(%d) : Assertion Failure.",pFile,line));
	MFDebug_Message(MFStr("Failed Condition: %s\n%s", pReason, pMessage));
	MFCallstack_Log();

	// draw some shit on the screen..

	while(!MFInput_WasPressed(Button_P2_Start, IDD_Gamepad, 0))
	{
		MFInput_Update();
	}
}
