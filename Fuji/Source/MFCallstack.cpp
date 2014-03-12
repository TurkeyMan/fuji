/**** Includes ****/

#include "Fuji_Internal.h"
#include "MFCallstack_Internal.h"
#include "DebugMenu.h"
#include "MFSystem.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "MFView.h"
#include "MFDisplay_Internal.h"

#if defined(_MFCALLSTACK)

	// maximum depth of the callstack
	static const int gMaxCallstackDepth = 256;

	// maintains the callstack
	const char *pCallstack[gMaxCallstackDepth];
	static int gCallDepth = 0;

	#if defined(_MFCALLSTACK_PROFILING)

		// maximum number of functions that can be profiled in a frame..
		static const int gMaxProfileFunctions = 128;

		// an array of pointers to functions added for profiling this frame.
		MFCallstack_Function *gpProfiles[gMaxProfileFunctions];
		static int gNumProfileFunctions = 0;

		// used to gather profiling information
		MFCallstack_FunctionCall gCalls[gMaxProfileFunctions];
		static int gNumFunctionCalls = 0;

		// frame begin/end times
		static uint64 gLastBeginTime = 0;
		static uint64 gBeginTime = 0;
		static uint64 gEndTime = 0;

		// profile colours
		static MFVector gProfileColours[] =
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
		static int gColourCounter = -1;

		int MFCallstack_GetNextColour()
		{
			return gColourCounter = (gColourCounter+1)%gProfileColourCount;
		}


		// debug menu items
		MenuItemBool drawCallstack(false);
		MenuItemBool drawCallstackMeter(true);

	#endif // _MFCALLSTACK_PROFILING

#endif // _MFCALLSTACK

MFInitStatus MFCallstack_InitModule(int moduleId, bool bPerformInitialisation)
{
#if defined(_MFCALLSTACK_PROFILING)
	DebugMenu_AddItem("Draw Callstack Profile", "Fuji Options", &drawCallstack);
	DebugMenu_AddItem("Draw Profile Meter", "Fuji Options", &drawCallstackMeter);
#endif

	return MFIS_Succeeded;
}

void MFCallstack_DeinitModule()
{

}

MF_API void MFCallstack_Log()
{
	MFDebug_Message(MFCallstack_GetCallstackString());
}

MF_API const char* MFCallstack_GetCallstackString()
{
#if defined(_MFCALLSTACK)
	char callstack[2048] = "";
	size_t bufferUsed = 0;

	for(int a = gCallDepth-1; a >= 0; a--)
	{
		const char *pTemp = MFStr("  %-32s\n",pCallstack[a]);
		size_t tempLen = MFString_Length(pTemp);
//		char *pTemp = MFStr("  %-32s\t(%s)%s\n",Callstack[a].c_str(),ModuleName(pFunc->pStats->pModuleName),pFunc->pComment ? MFStr(" [%s]",pFunc->pComment) : "");
		if(bufferUsed + tempLen < sizeof(callstack) - 1)
		{
			MFString_Cat(callstack, pTemp);
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

MFCall::MFCall(const char *pFunctionName, MFCallstack_Function *_pFunction)
{
	pCallstack[gCallDepth] = pFunctionName;
	++gCallDepth;

#if defined(_MFCALLSTACK_PROFILING)
	if(_pFunction && gNumProfileFunctions < gMaxProfileFunctions)
	{
		pFunction = _pFunction;

		if(!_pFunction->bAdded)
		{
			gpProfiles[gNumProfileFunctions] = _pFunction;
			++gNumProfileFunctions;
		}

		++_pFunction->numCalls;
		_pFunction->bAdded = true;

		startTime = MFSystem_ReadRTC();

		if(0)//gNumFunctionCalls < gMaxProfileFunctions)
		{
			pCall = &gCalls[gNumFunctionCalls];
			++gNumFunctionCalls;

			pCall->pFunction = _pFunction;
			pCall->startTime = startTime;
		}
		else
			pCall = NULL;
	}
	else
		pFunction = NULL;
#endif
}

MFCall::~MFCall()
{
#if defined(_MFCALLSTACK_PROFILING)
	if(pFunction)
	{
		uint64 endTime = MFSystem_ReadRTC();
		pFunction->totalTime += endTime - startTime;

		if(pCall)
			pCall->endTime = endTime;
	}
#endif

	--gCallDepth;
}

#endif // defined(_MFCALLSTACK)


/**** Profiling related functions ****/

#if defined(_MFCALLSTACK_PROFILING)

void MFCallstack_BeginFrame()
{
	MFCALLSTACK;

	gLastBeginTime = gBeginTime;
	gBeginTime = MFSystem_ReadRTC();
}

int MFCallstack_SortPred(const void *pF1, const void *pF2)
{
	MFCallstack_Function *pFunc1 = *(MFCallstack_Function**)pF1;
	MFCallstack_Function *pFunc2 = *(MFCallstack_Function**)pF2;

	return MFString_CaseCmp(pFunc1->pFunctionName, pFunc2->pFunctionName);
//	return pFunc1->totalTime < pFunc2->totalTime;
//	return pFunc1->numCalls < pFunc2->numCalls;
}

void MFCallstack_EndFrame()
{
	MFCALLSTACK;

	// mark the end of the frame
	gEndTime = MFSystem_ReadRTC();

	// take a copy and reset everything
	for(int a=0; a<gNumProfileFunctions; ++a)
	{
		// reset the function
		gpProfiles[a]->totalTime = 0;
		gpProfiles[a]->numCalls = 0;
		gpProfiles[a]->bAdded = false;
	}

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

void MFCallstack_Draw()
{
	MFCALLSTACK;

	if(drawCallstack)
	{
		float width = (float)gDisplay.width;
		float height = (float)gDisplay.height;
		float xoffset = width / 10.0f;
		float yoffset = height / 6.0f;

		MFView_Push();

		MFRect rect = { 0.f, 0.f, width, height };
		MFView_SetOrtho(&rect);

		float rtcFreq = 1000000.f / (float)MFSystem_GetRTCFrequency();

		MFVector callstackPos = MakeVector(xoffset, yoffset, 0.f, 0.f);

		// sort the calls
		qsort(gpProfiles, gNumProfileFunctions, sizeof(gpProfiles[0]), MFCallstack_SortPred);

//		if(!drawCallstackMeter)
		{
			// just draw the callstack profiling information..
			MFPrimitive_DrawUntexturedQuad(callstackPos.x - 10.f, callstackPos.y - 10.f, (width - callstackPos.x) + 10, callstackPos.y + (float)gNumProfileFunctions*16.0f + 10, MakeVector(0, 0, 0, 0.8f));

			for(int a=0; a<gNumProfileFunctions; a++)
			{
				uint32 microseconds = uint32((float)gpProfiles[a]->totalTime * rtcFreq);
				uint32 percent = uint32((float)microseconds / 166.66666f);

//				MFVector colour = gProfileColours[gpProfiles[a]->functionColour];
				MFVector colour = MFVector::one;
				MFFont_DrawTextf(MFFont_GetDebugFont(), callstackPos, 16.0f, colour, "%s", gpProfiles[a]->pFunctionName);
				MFFont_DrawTextf(MFFont_GetDebugFont(), callstackPos + MakeVector((width - xoffset) - 250.0f, 0, 0), 16.0f, colour, "- %dus (%d%%) %d calls", microseconds, percent, gpProfiles[a]->numCalls);
				callstackPos.y += 16.0f;
			}
		}
/*
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
			uint32 frameDuration = uint32(gBeginTime - gLastBeginTime);
			float frameTime = (float)frameDuration * rtcFreq;
			MFCallstackInternal_DrawMeterBlock(meterPos, meterDimensions, (float)(gEndTime - gLastBeginTime) / (float)frameTime, 1.0f, MakeVector(0.0f, 1.0f, 1.0f, 1.0f));
			MFCallstackInternal_DrawMeterLabel(callstackPos, MakeVector(0.0f, 1.0f, 1.0f, 1.0f), "Frame Overhead/VSync", NULL);
			callstackPos.y += 24.0f;

			// draw all the totals
			for(int a=0; a<gNumProfileTotals; a++)
			{
				uint32 microseconds = uint32((float)gpProfiles[a]->totalTime * rtcFreq);
				uint32 percent = uint32((float)microseconds / 166.66666f);

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
*/

		MFView_Pop();
	}
}

#endif // defined(_MFCALLSTACK_PROFILING)
