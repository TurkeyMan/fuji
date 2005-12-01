/**** Includes ****/

#include "Fuji.h"
#include "MFCallstack_Internal.h"
#include "DebugMenu.h"
#include "MFSystem.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "Display_Internal.h"

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

		// profile colours
		MFVector gProfileColours[] =
		{
			MakeVector( 1,		0,		0,		1 ),
			MakeVector( 0,		1,		0,		1 ),
			MakeVector( 0,		0,		1,		1 ),
			MakeVector( 1,		1,		0,		1 ),
			MakeVector( 1,		0,		1,		1 ),
			MakeVector( 0.5f,	0,		0,		1 ),
			MakeVector( 0,		0.5f,	0,		1 ),
			MakeVector( 0,		0,		0.5f,	1 ),
			MakeVector( 0.5f,	0.5f,	0,		1 ),
			MakeVector( 0,		0.5f,	0.5f,	1 ),
			MakeVector( 0.5f,	0,		0.5f,	1 )
		};

		static const int gProfileColourCount = sizeof(gProfileColours) / sizeof(gProfileColours[0]);

		// debug menu items
		MenuItemBool drawCallstack(false);
		MenuItemBool drawCallstackMeter(true);

	#endif // _MFCALLSTACK_PROFILING

#endif // _MFCALLSTACK

void MFCallstack_InitModule()
{
#if defined(_MFCALLSTACK_PROFILING)
	DebugMenu_AddItem("Draw Callstack Profile", "Fuji Options", &drawCallstack);
	DebugMenu_AddItem("Draw Profile Meter", "Fuji Options", &drawCallstackMeter);
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
		int tempLen = (int)strlen(pTemp);
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

void MFCallstackInternal_DrawMeterBlock(const MFVector &meterPos, const MFVector &meterDimensions, float start, float end, const MFVector &colour)
{
	MFPrimitive_DrawUntexturedQuad(meterPos.x + meterDimensions.x*start, meterPos.y, meterPos.x + meterDimensions.x*end, meterPos.y + meterDimensions.y, colour);
}

void MFCallstackInternal_DrawMeterLabel(const MFVector &listPos, const MFVector &colour, const char *pString, const char *pStats)
{
	MFPrimitive_DrawUntexturedQuad(listPos.x, listPos.y + 1.0f, listPos.x + 14.0f, listPos.y + 15.0f, MFVector::one);
	MFPrimitive_DrawUntexturedQuad(listPos.x + 2, listPos.y + 3.0f, listPos.x + 12.0f, listPos.y + 13.0f, colour);
	MFFont_DrawTextf(MFFont_GetDebugFont(), listPos + MakeVector(16.0f, 0.0f, 0.0f), 16.0f, MFVector::one, pString);

	if(pStats)
	{
		float width = (float)gDisplay.width;
		MFFont_DrawTextf(MFFont_GetDebugFont(), listPos + MakeVector((width - listPos.x) - 250.0f, 0.0f, 0.0f), 16.0f, MFVector::one, pStats);
	}
}

int MFCallstackInternal_GetTotalIndex(int profileIndex)
{
	for(int a=0; a<gNumProfileTotals; a++)
	{
		if(gProfileResults[profileIndex].pFunction == gProfileTotals[a].pFunctionName)
			return a;
	}

	MFDebug_Assert(false, "Profile does not have a total?!!");
	return -1;
}

void MFCallstack_Draw()
{
	MFCALLSTACK;

	if(drawCallstack)
	{
		float width = (float)gDisplay.width;
		float height = (float)gDisplay.height;
		float xoffset = width / 10.0f;
		float yoffset = height / 6.0f;

		float rtcFreq = (float)MFSystem_GetRTCFrequency();

		uint32 frameTime = uint32(gBeginTime - gLastBeginTime);

		MFVector callstackPos = MakeVector(xoffset, yoffset, 0.f, 0.f);

		if(!drawCallstackMeter)
		{
			// just draw the callstack profiling information..

			MFPrimitive_DrawUntexturedQuad(callstackPos.x - 10.f, callstackPos.y - 10.f, (width - callstackPos.x) + 10, callstackPos.y + (float)gNumProfileTotals*16.0f + 10, MakeVector(0, 0, 0, 0.8f));

			for(int a=0; a<gNumProfileTotals; a++)
			{
				uint32 microseconds = uint32((float)gProfileTotals[a].totalFunctionTime / (rtcFreq / 1000000.0f));
				uint32 percent = uint32(((float)microseconds / (float)frameTime) * 100.0f);

				MFFont_DrawTextf(MFFont_GetDebugFont(), callstackPos, 16.0f, MFVector::one, "%s", gProfileTotals[a].pFunctionName);
				MFFont_DrawTextf(MFFont_GetDebugFont(), callstackPos + MakeVector((width - xoffset) - 250.0f, 0, 0), 16.0f, MFVector::one, "- %dus (%d%%) %d calls", microseconds, percent, gProfileTotals[a].numCalls);
				callstackPos.y += 16.0f;
			}
		}
		else
		{
			// draw a meter up the top of the screen representing the frame..

			MFPrimitive_DrawUntexturedQuad(callstackPos.x - 10.f, callstackPos.y - 10.f, width - callstackPos.x + 10, callstackPos.y + (float)(gNumProfileTotals+1)*16.0f + 18, MakeVector(0, 0, 0, 0.8f));

			MFVector meterPos = MakeVector(xoffset - 8.f, yoffset*0.5f - 15.0f, 0.0f);
			MFVector meterDimensions = MakeVector(width - (xoffset-8.0f)*2.0f, 20.0f, 0.0f);

			// draw the meter container
			MFPrimitive_DrawUntexturedQuad(meterPos.x - 2.0f, meterPos.y - 2.0f, meterPos.x + meterDimensions.x + 2.0f, meterPos.y + meterDimensions.y + 2.0f, MakeVector(1.0f, 1.0f, 1.0f, 0.8f));
			MFPrimitive_DrawUntexturedQuad(meterPos.x, meterPos.y, meterPos.x + meterDimensions.x, meterPos.y + meterDimensions.y, MakeVector(0.0f, 0.0f, 0.0f, 1.0f));

			// draw the overhead counter
			MFCallstackInternal_DrawMeterBlock(meterPos, meterDimensions, (float)(gEndTime - gLastBeginTime) / (float)frameTime, 1.0f, MakeVector(0.0f, 1.0f, 1.0f, 1.0f));
			MFCallstackInternal_DrawMeterLabel(callstackPos, MakeVector(0.0f, 1.0f, 1.0f, 1.0f), "Frame Overhead/VSync", NULL);
			callstackPos.y += 24.0f;

			// draw all the totals
			for(int a=0; a<gNumProfileTotals; a++)
			{
				uint32 microseconds = uint32((float)gProfileTotals[a].totalFunctionTime / (rtcFreq / 1000000.0f));
				uint32 percent = uint32(((float)microseconds / (float)frameTime) * 100.0f);

				MFCallstackInternal_DrawMeterLabel(callstackPos, gProfileColours[a % gProfileColourCount], gProfileTotals[a].pFunctionName, MFStr("- %dus (%d%%) %d calls", microseconds, percent, gProfileTotals[a].numCalls));
				callstackPos.y += 16.0f;
			}

			// draw all the function profiles
			float totalTime = (float)(gBeginTime - gLastBeginTime);
			for(int a=0; a<gNumProfileResults; a++)
			{
				int colour = MFCallstackInternal_GetTotalIndex(a);
				MFCallstackInternal_DrawMeterBlock(meterPos, meterDimensions, (float)(gProfileResults[a].startTime - gLastBeginTime) / totalTime, (float)(gProfileResults[a].endTime - gLastBeginTime) / totalTime, gProfileColours[colour % gProfileColourCount]);
			}
		}
	}
}

#endif // defined(_MFCALLSTACK_PROFILING)
