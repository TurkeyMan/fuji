#include "Fuji.h"
#include "MFSystem.h"

int MFSystem_GameLoop();

MFInitParams gInitParams;
extern bool gDrawSystemInfo;

#if defined(USE_LIBJSON)
	#include <libjson/libjson.h>
	#include "MFHeap.h"

	static void *mf_json_malloc(unsigned long bytes)
	{
		return MFHeap_Alloc(bytes);
	}

	static void *mf_json_realloc(void *pMem, unsigned long bytes)
	{
		return MFHeap_Realloc(pMem, bytes);
	}

	static void mf_json_free(void *pMem)
	{
		MFHeap_Free(pMem);
	}
#endif

int MFMain(MFInitParams *pInitParams)
{
	MFCALLSTACK;

	gInitParams = *pInitParams;
	gDrawSystemInfo = !pInitParams->hideSystemInfo;

	// assign the app title to the window title it it was set
	if(gInitParams.pAppTitle)
		gDefaults.display.pWindowTitle = pInitParams->pAppTitle;

	// process command line
	//...

#if defined(USE_LIBJSON)
	json_register_memory_callbacks(mf_json_malloc, mf_json_realloc, mf_json_free);
#endif

#if defined(MF_IPHONE)
	int StartIPhone(MFInitParams *);
	return StartIPhone(pInitParams);
#else
	MFSystem_GameLoop();

	return 0;
#endif
}
