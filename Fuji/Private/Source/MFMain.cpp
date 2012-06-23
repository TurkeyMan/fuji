#include "Fuji.h"
#include "MFSystem.h"

int MFSystem_GameLoop();

MFInitParams gInitParams;
extern bool gDrawSystemInfo;

MF_API int MFMain(MFInitParams *pInitParams)
{
	MFCALLSTACK;

	gInitParams = *pInitParams;
	gDrawSystemInfo = !pInitParams->hideSystemInfo;

	// assign the app title to the window title it it was set
	if(gInitParams.pAppTitle)
		gDefaults.display.pWindowTitle = pInitParams->pAppTitle;

	// process command line
	//...

#if defined(MF_IPHONE)
	int StartIPhone(MFInitParams *);
	return StartIPhone(pInitParams);
#else
	MFSystem_GameLoop();

	return 0;
#endif
}
