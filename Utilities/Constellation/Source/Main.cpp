#include "Global.h"
#include "Exporter.h"
#include "StatusWindow.h"

// *** Globals ***
HINSTANCE hInstance;
bool dllInitialized = false;

class ConstellationClassDesc : public ClassDesc {
public:
	int IsPublic()
	{
		return(TRUE);
	}

	void * Create(BOOL loading=FALSE)
	{
		return(new Exporter());
	}

	const TCHAR * ClassName()
	{
		return(PLUGIN_CLASS_NAME);
	}

	SClass_ID SuperClassID()
	{
		return(SCENE_EXPORT_CLASS_ID);
	}

	Class_ID ClassID()
	{
		return(PLUGIN_CLASS_ID);
	}

	const TCHAR* Category()
	{
		return(SCENEEXPORT_CATEGORY);
	}
};

static ConstellationClassDesc ConstellationCD;


BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	if(!dllInitialized) {
		hInstance = hinstDLL;
	
		InitCommonControls();
		InitCustomControls(hInstance);
	
		dllInitialized = true;
	}
	
	return(TRUE);
}

__declspec(dllexport) int LibNumberClasses()
{
	return(1);
}

__declspec(dllexport) ClassDesc *LibClassDesc(int i)
{
	switch(i) {
		case 0:
			return(&ConstellationCD);
		default:
			return(0);
	}
}

__declspec(dllexport) const TCHAR *LibDescription()
{
	return(PLUGIN_DESCRIPTION);
}

__declspec(dllexport) ULONG LibVersion()
{
	return(VERSION_3DSMAX);
}

__declspec(dllexport) ULONG CanAutoDefer()
{
	return(1);
}
