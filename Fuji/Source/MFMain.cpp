#include "Fuji_Internal.h"
#include "MFSystem.h"
#include "MFCommandLine.h"
#include "MFDebug.h"
#include "MFHeap_Internal.h"

int MFSystem_GameLoop();

MFInitParams gInitParams;
#include <stdio.h>
MF_API int MFMain(MFInitParams *pInitParams)
{
	MFCALLSTACK;

	gInitParams = *pInitParams;
	gpEngineInstance->bDrawSystemInfo = !pInitParams->hideSystemInfo;

	// assign the app title to the window title it it was set
	if(gInitParams.pAppTitle)
		gDefaults.display.pWindowTitle = pInitParams->pAppTitle;

	// process command line
	char *pFullCmdLine = NULL;
	if(gInitParams.argc > 1 && gInitParams.argv != NULL)
	{
		size_t cmdLineBytes = gInitParams.pCommandLine ? MFString_Length(gInitParams.pCommandLine) : 0;
		for(int a = 1; a < gInitParams.argc; ++a)
			cmdLineBytes += MFString_Length(gInitParams.argv[a]) + 1;

		pFullCmdLine = (char*)MFHeap_SystemMalloc(cmdLineBytes + 1);
		pFullCmdLine[0] = 0;

		if(gInitParams.pCommandLine)
			MFString_Copy(pFullCmdLine, gInitParams.pCommandLine);

		for(int a = 1; a < gInitParams.argc; ++a)
		{
			MFString_Cat(pFullCmdLine, " ");
			MFString_Cat(pFullCmdLine, gInitParams.argv[a]);
		}

		gInitParams.pCommandLine = pFullCmdLine;
	}

	int logLevel = MFCommandLine_GetInt("loglevel", -1);
	if(logLevel >= 0)
		MFDebug_SetMaximumLogLevel(logLevel);

	int warnLevel = MFCommandLine_GetInt("warnlevel", -1);
	if(warnLevel >= 0)
		MFDebug_SetMaximumWarningLevel(warnLevel);

	// kick it off...
	int r = 0;
#if defined(MF_IPHONE)
	int StartIPhone(MFInitParams *);
	r = StartIPhone(pInitParams);
#else
	r = MFSystem_GameLoop();
#endif

	if(pFullCmdLine)
		MFHeap_SystemFree(pFullCmdLine);

	return r;
}
