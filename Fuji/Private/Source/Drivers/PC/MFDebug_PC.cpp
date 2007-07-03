#include "Fuji.h"

#if MF_DEBUG == MF_DRIVER_PC

#if !defined(_RETAIL)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Output a string to the debugger.
void MFDebug_Message(const char *pMessage)
{
	OutputDebugString((LPCTSTR)MFStr("%s\n", pMessage));
}

bool MFDebugPC_MsgBox(const char *pMessageText, const char *pTitle)
{
	bool bResult = false;

	if(IDYES == MessageBox(NULL, pMessageText, pTitle, MB_YESNO | MB_ICONSTOP | MB_TOPMOST))
	{
		bResult = true;
	}

	return bResult;
}

#if defined(_FUJI_UTIL)

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
	MFDebug_Message(MFStr("%s(%d) : Assertion Failure.",pFile,line));
	MFDebug_Message(MFStr("Failed Condition: (%s)\n%s", pReason, pMessage));

	// query for debug or exit of process
	if(!MFDebugPC_MsgBox(MFStr("Failed Condition: (%s)\n%s\nFile: %s\nLine: %d", pReason, pMessage, pFile, line), "Assertion Failure, do you wish to debug?"))
	{
		ExitProcess(0);
	}
}

#else

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
	MFDebug_Message(MFStr("%s(%d) : Assertion Failure.",pFile,line));
	MFDebug_Message(MFStr("Failed Condition: (%s)\n%s", pReason, pMessage));

	// build callstack log string for message box
#if !defined(_RETAIL)
	MFCallstack_Log();
	const char *pCallstack = MFCallstack_GetCallstackString();
#else
	const char *pCallstack = "Not available in _RETAIL builds";
#endif

	// query for debug or exit of process
	if(!MFDebugPC_MsgBox(MFStr("Failed Condition: (%s)\n%s\nFile: %s\nLine: %d\n\nCallstack:\n%s", pReason, pMessage, pFile, line, pCallstack), "Assertion Failure, do you wish to debug?"))
	{
		ExitProcess(0);
	}
}

#endif

#endif // !defined(_RETAIL)

#endif
