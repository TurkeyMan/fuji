#include "Fuji.h"

#if MF_DEBUG == MF_DRIVER_NACL

#if !defined(_RETAIL)

#include "MFSystem_NaCl.h"

// Output a string to the debugger.
void MFDebug_Message(const char *pMessage)
{
	FujiModule *pModule = (FujiModule*)pp::Module::Get();
	Fuji *pInstance = pModule->GetInstance();
	if(pInstance)
		pInstance->PostMessage(pp::Var(pMessage));
}

bool MFDebugPC_MsgBox(const char *pMessageText, const char *pTitle)
{
	// show an android request box somehow...
	MFDebug_Assert(false, "Not done!");
	return false;
}

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

	MFDebug_Breakpoint();

	exit(0);

	// TODO: show the android message box...
/*
	// query for debug or exit of process
	if(!MFDebugPC_MsgBox(MFStr("Failed Condition: (%s)\n%s\nFile: %s\nLine: %d\n\nCallstack:\n%s", pReason, pMessage, pFile, line, pCallstack), "Assertion Failure, do you wish to debug?"))
	{
		exit(0);
	}
*/
}

#endif // !defined(_RETAIL)

#endif