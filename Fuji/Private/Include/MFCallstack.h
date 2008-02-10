/**
 * @file MFCallstack.h
 * @brief Runtime callstack management.
 * @author Manu Evans
 * @defgroup MFCallstack Callstack Management
 * @{
 */

#if !defined(_MFCALLSTACK_H)
#define _MFCALLSTACK_H

// set up callstack profiling defines
#if !defined(_RETAIL)
	#define _MFCALLSTACK

//	#if !defined(_DEBUG) && defined(_ENABLE_CALLSTACK_PROFILING)
	#if defined(_ENABLE_CALLSTACK_PROFILING) && !defined(_FUJI_UTIL)
		#define _MFCALLSTACK_PROFILING
	#endif
#endif

// all the macros and fun stuff
#if defined(_MFCALLSTACK)

#if defined(_MFCALLSTACK_PROFILING)

int MFCallstack_GetNextColour();

struct MFCallstack_Function
{
	uint64 totalTime;
	const char *pFunctionName;
	int numCalls;
	uint16 functionColour;
	uint16 profileFlags;
	bool bAdded;
};

struct MFCallstack_FunctionCall
{
	MFCallstack_Function *pFunction;
	uint64 startTime;
	uint64 endTime;
};

#endif // defined(_MFCALLSTACK_PROFILING)

class MFCall
{
public:
	MFCall(const char *pFunctionName, MFCallstack_Function *pFunction);
	~MFCall();

#if defined(_MFCALLSTACK_PROFILING)
	uint64 startTime;
	MFCallstack_Function *pFunction;
	MFCallstack_FunctionCall *pCall;
#endif
};


#if defined(__GNUC__)
	#define MFCALLSTACK_FUNCTIONNAME __PRETTY_FUNCTION__
#else
	#define MFCALLSTACK_FUNCTIONNAME __FUNCTION__
#endif

#if defined(_MFCALLSTACK_PROFILING)
	#define MFCALLSTACK_PROFILE(name, mode) static MFCallstack_Function MFCallstack_FunctionDef = { 0LL, name, 0, MFCallstack_GetNextColour(), (mode), false }; MFCall _call(name, &MFCallstack_FunctionDef)
#else
	#define MFCALLSTACK_PROFILE(name, mode) MFCall _call(name, NULL)
#endif

#define MFCALLSTACK MFCall _call(MFCALLSTACK_FUNCTIONNAME, NULL);
#define MFCALLSTACKc MFCALLSTACK_PROFILE(MFCALLSTACK_FUNCTIONNAME, 1);
#define MFCALLSTACKg MFCALLSTACK_PROFILE(MFCALLSTACK_FUNCTIONNAME, 2);
#define MFCALLSTACKcg MFCALLSTACK_PROFILE(MFCALLSTACK_FUNCTIONNAME, 3);
#define MFCALLSTACKs(s) MFCall _call((s), NULL);
#define MFCALLSTACKcs(s) MFCALLSTACK_PROFILE((s), 1);
#define MFCALLSTACKgs(s) MFCALLSTACK_PROFILE((s), 2);
#define MFCALLSTACKcgs(s) MFCALLSTACK_PROFILE((s), 3);

#else // defined(_MFCALLSTACK)

#define MFCALLSTACK
#define MFCALLSTACKc
#define MFCALLSTACKg
#define MFCALLSTACKcg
#define MFCALLSTACKs(s)
#define MFCALLSTACKcs(s)
#define MFCALLSTACKgs(s)
#define MFCALLSTACKcgs(s)

#endif // defined(_MFCALLSTACK)

// functions

/**
 * Log the callback to the console.
 * Logs the callback to the console.
 * @return None. 
 */
void MFCallstack_Log();

/**
 * Get a string containing the current callstack.
 * Get2 a string containing the current callstack.
 * @return Pointer to a string containing the current callstack.
 * @remarks The string is allocated in the circular temporary string buffer, results should not be stored long term.
 */
const char* MFCallstack_GetCallstackString();

#endif // _MFCALLSTACK_H

/** @} */
