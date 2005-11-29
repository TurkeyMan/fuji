/**** Includes ****/

#include "Fuji.h"
#include "MFCallstack_Internal.h"
#include "DebugMenu.h"
#include "MFSystem.h"

#if defined(_MFCALLSTACK)

	// maximum depth of the callstack
	static const int gMaxCallstackDepth = 256;

	// maintains the callstack
	const char *pCallstack[gMaxCallstackDepth];
	static int gCallDepth = 0;

	#if defined(_MFCALLSTACK_PROFILING)

		// maximum number of functions that can be profiled in a frame..
		static const int gMaxProfileFunctions = 64;

		// used to gather profiling information
		MFFunctionProfile gProfiles[gMaxProfileFunctions];
		static int gNumProfileFunctions = 0;

		// stores results of the frames profiling
		MFFunctionProfile gProfileResults[gMaxProfileFunctions];
		static int gNumProfileResults = 0;

		MFFunctionProfileTotals gProfileTotals[gMaxProfileFunctions];
		static int gNumProfileTotals = 0;

		// frame begin/end times
		static uint64 gLastBeginTime = 0;
		static uint64 gBeginTime = 0;
		static uint64 gEndTime = 0;

		// debug menu items
		MenuItemBool drawCallstack(false);
		MenuItemBool drawCallstackMeter(true);

	#endif // _MFCALLSTACK_PROFILING

#endif // _MFCALLSTACK

void MFCallstack_InitModule()
{
#if defined(_MFCALLSTACK_PROFILING)
	DebugMenu_AddItem("Draw Callstack Profile", "Fuji Options", &drawCallstack);
	DebugMenu_AddItem("Draw Callstack Meter", "Fuji Options", &drawCallstackMeter);
#endif
}

void MFCallstack_DeinitModule()
{

}

void MFCallstack_Log()
{
	MFDebug_Message(MFCallstack_GetCallstackString());
}

const char* MFCallstack_GetCallstackString()
{
#if defined(_MFCALLSTACK)
	char callstack[2048] = "";
	int bufferUsed = 0;

	for(int a = gCallDepth-1; a >= 0; a--)
	{
		char *pTemp = MFStr("  %-32s\n",pCallstack[a]);
		int tempLen = strlen(pTemp);
//		char *pTemp = MFStr("  %-32s\t(%s)%s\n",Callstack[a].c_str(),ModuleName(pFunc->pStats->pModuleName),pFunc->pComment ? MFStr(" [%s]",pFunc->pComment) : "");
		if(bufferUsed + tempLen < sizeof(callstack) - 1)
		{
			strcat(callstack, pTemp);
			bufferUsed += tempLen;
		}
	}

	return MFStr(callstack);
#else
	return "Callstack not available in this build.";
#endif
}


/**** Callstack related functions ****/

#if defined(_MFCALLSTACK)

MFCall::MFCall(const char *pFunction, bool _profile)
{
	pCallstack[gCallDepth] = pFunction;
	++gCallDepth;

#if defined(_MFCALLSTACK_PROFILING)
	if(_profile && gNumProfileFunctions < gMaxProfileFunctions)
	{
		pProfile = &gProfiles[gNumProfileFunctions];
		++gNumProfileFunctions;

		pProfile->pFunction = pFunction;
		pProfile->startTime = MFSystem_ReadRTC();
	}
	else
	{
		pProfile = NULL;
	}
#endif
}

MFCall::~MFCall()
{
	--gCallDepth;

#if defined(_MFCALLSTACK_PROFILING)
	if(pProfile)
	{
		pProfile->endTime = MFSystem_ReadRTC();
	}
#endif
}

#endif // defined(_MFCALLSTACK)


/**** Profiling related functions ****/

#if defined(_MFCALLSTACK_PROFILING)

MFFunctionProfileTotals* MFCallstackInternal_GetProfileTotals(const char *pFunction)
{
	MFCALLSTACK;

	for(int a=0; a<gNumProfileTotals; a++)
	{
		if(gProfileTotals[a].pFunctionName == pFunction)
		{
			return &gProfileTotals[a];
		}
	}

	if(a == gNumProfileTotals && gNumProfileTotals < gMaxProfileFunctions)
	{
		++gNumProfileTotals;

		gProfileTotals[a].pFunctionName = pFunction;
		gProfileTotals[a].numCalls = 0;
		gProfileTotals[a].totalFunctionTime = 0;

		return &gProfileTotals[a];
	}

	return NULL;
}

void MFCallstack_BeginFrame()
{
	MFCALLSTACK;

	gLastBeginTime = gBeginTime;
	gBeginTime = MFSystem_ReadRTC();
}

void MFCallstack_EndFrame()
{
	MFCALLSTACK;

	// mark the end of the frame
	gEndTime = MFSystem_ReadRTC();

	// reset the profile totals (totals from this frame will exist until this time next frame)
	gNumProfileTotals = 0;

	// build profile stats here..

	// copy profile results
	memcpy(gProfileResults, gProfiles, sizeof(MFFunctionProfile)*gNumProfileFunctions);
	gNumProfileResults = gNumProfileFunctions;

	// build profile totals
	for(int a=0; a<gNumProfileFunctions; a++)
	{
		MFFunctionProfileTotals *pTotals = MFCallstackInternal_GetProfileTotals(gProfiles[a].pFunction);

		if(pTotals)
		{
			++pTotals->numCalls;
			pTotals->totalFunctionTime += gProfiles[a].endTime - gProfiles[a].startTime;
		}
	}

	// reset everything
	gNumProfileFunctions = 0;
}

void MFCallstack_Draw()
{
	MFCALLSTACK;

	
}

#endif // defined(_MFCALLSTACK_PROFILING)
