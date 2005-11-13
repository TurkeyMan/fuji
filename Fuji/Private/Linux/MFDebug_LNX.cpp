#include "Fuji.h"

// Output a string to the debugger.
void MFDebug_Message(const char *pMessage)
{
	fprintf(stderr, pMessage);
}

void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line)
{
	LOGD(STR("%s(%d) : Assertion Failure.",pFile,line));
	LOGD(STR("Failed Condition: (%s)\n%s", pReason, pMessage));

	// build callstack log string for message box

#if !defined(_RETAIL)
	Callstack_Log();
#endif
}
