#include <stdio.h>
#include "Common.h"
#include "Util.h"
#include "Vector3.h"

char stringBuffer[1024*128];
uint32 stringOffset;

// Return only the last portion of the module name (exclude the path)
char *ModuleName(char *pSourceFileName)
{
  char *pTemp = strrchr(pSourceFileName,'/');
  if (!pTemp)
  {
    pTemp = strrchr(pSourceFileName,'\\');
  }
  return (pTemp) ? pTemp+1 : pSourceFileName;
}

#if defined(_XBOX)

void dbgAssert(char *pReason, char *pMessage, char *pFile, int line)
{
//  TimerInfo time,diff;
//  Timer_GetSystemTime(&time);
//  Timer_GetDifference(&diff,&startTime,&time);

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
    default: pDayStr = "th"; break;
  }
  LOGD(STR("Time : (%d%s):%02d:%02d:%02d (%02d:%02d:%02d from start)",time.day,pDayStr,time.hours,time.minutes,time.seconds,diff.day*24+diff.hours,diff.minutes,diff.seconds));
*/
}

#elif defined(_WINDOWS)

bool Debug_MsgBox(char *pMessageText, char *pTitle, char *pTime)
{
  bool bResult = false;

  if(IDYES == MessageBox(NULL, pMessageText, pTitle, MB_YESNO | MB_ICONSTOP | MB_TOPMOST))
  {
    bResult = true;
  }

  return bResult;
}

void dbgAssert(char *pReason, char *pMessage, char *pFile, int line)
{
  LOGD(STR("Failed Condition: (%s)\n%s", pReason, pMessage));
  LOGD(STR("Assertion Failure: File '%s', Line %d",pFile,line));
//  Callstack_Log();

  // build callstack log string for message box
#if defined(_DEBUG)
  char callstack[2048] = "";

  for(Callstack *pFunc = Callstack::pCallstack; pFunc; pFunc = pFunc->pParent)
  {
    char *pTemp = STR("  %-32s\t(%s)%s\n",pFunc->pStats->pFunctionName,ModuleName(pFunc->pStats->pModuleName),pFunc->pComment ? STR(" [%s]",pFunc->pComment) : "");
    if(strlen(callstack) + strlen(pTemp) < sizeof(callstack) - 1)
      strcat(callstack, pTemp);
  }

#else
  char callstack[] = "Not available in _RETAIL builds";
#endif

  // query for debug or exit of process
  if(!Debug_MsgBox(STR("Failed Condition: (%s)\n%s\nFile: %s\nLine: %d\n\nCallstack:\n%s", pReason, pMessage, pFile, line, callstack),
                              "Assertion Failure, do you wish to debug?",
                              "timewouldgohere"))
  {
    ExitProcess(0);
  }
}
#endif

//
// This just does OutputDebugString using a format string.
//
int dprintf(char *format, ...)
{
	char *buffer[2048];
	va_list arglist;
	int nRes = 0;

	va_start(arglist, format);

	nRes = vsprintf((char*)buffer, format, arglist);
	OutputDebugString((LPCTSTR)buffer);

	va_end(arglist);
	return nRes;
}

void LOGD(char *string)
{
	OutputDebugString(string);
	OutputDebugString("\n");
}

char* STR(char *format, ...)
{
	va_list arglist;
	char *buffer = &stringBuffer[stringOffset];
	int nRes = 0;

	va_start(arglist, format);

	nRes = vsprintf(buffer, format, arglist);
	stringOffset += nRes+1;

	if(stringOffset >= sizeof(stringBuffer) - 1024) stringOffset = 0;

	va_end(arglist);

	return buffer;
}

uint32 Rand()
{
	return rand();
}

float RandomUnit()
{
	return (float)rand() / RAND_MAX;
}

float RandomRange(float min , float max)
{
	return ((float)rand()/RAND_MAX)*(max-min) + min;
}

Vector3 RandomVector()
{
	Vector3 t;

	t.x = RandomUnit();
	t.y = RandomUnit();
	t.z = RandomUnit();

	return t;
}

