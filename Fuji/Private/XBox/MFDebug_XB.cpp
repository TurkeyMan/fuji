#include "Fuji.h"

// Output a string to the debugger.
void MFDebug_Message(const char *pMessage)
{
	OutputDebugString((LPCTSTR)pMessage);
}

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
//	TimerInfo time,diff;
//	Timer_GetSystemTime(&time);
//	Timer_GetDifference(&diff,&startTime,&time);

	LOGD(STR("%s(%d) : Assertion Failure.",pFile,line));
	LOGD(STR("Failed Condition: (%s)\n%s", pReason, pMessage));
/*
	char *pDayStr = gNullStr;
	switch (time.day)
	{
		case 1: 
		case 21:
		case 31:
			pDayStr = "st"; 
			break;
		case 2: 
		case 22:
		case 32:
			pDayStr = "nd"; 
			break;
		case 3:
		case 23:
		case 33:
			pDayStr = "rd"; 
			break;
		default:
			pDayStr = "th";
			break;
	}
	LOGD(STR("Time : (%d%s):%02d:%02d:%02d (%02d:%02d:%02d from start)",time.day,pDayStr,time.hours,time.minutes,time.seconds,diff.day*24+diff.hours,diff.minutes,diff.seconds));
*/

	Callstack_Log();
}
