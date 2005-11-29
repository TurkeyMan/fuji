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
	#if defined(_ENABLE_CALLSTACK_PROFILING)
		#define _MFCALLSTACK_PROFILING
	#endif
#endif

// all the macros and fun stuff
#if defined(_MFCALLSTACK)

#if defined(_MFCALLSTACK_PROFILING)

struct MFFunctionProfileTotals
{
	const char *pFunctionName;
	int numCalls;
	uint64 totalFunctionTime;
};

struct MFFunctionProfile
{
	const char *pFunction;
	uint64 startTime;
	uint64 endTime;
};

#endif // defined(_MFCALLSTACK_PROFILING)

class MFCall
{
public:
	MFCall(const char *pFunction, bool profile);
	~MFCall();

#if defined(_MFCALLSTACK_PROFILING)
	MFFunctionProfile *pProfile;
#endif
};

#define MFCALLSTACK MFCall _call(__FUNCTION__, 0);
#define MFCALLSTACKc MFCall _call(__FUNCTION__, 1);
#define MFCALLSTACKg MFCall _call(__FUNCTION__, 3);
#define MFCALLSTACKs(s) MFCall _call(__FUNCTION__, 0);
#define MFCALLSTACKcs(s) MFCall _call(__FUNCTION__, 1);
#define MFCALLSTACKgs(s) MFCall _call(__FUNCTION__, 3);

#else // defined(_MFCALLSTACK)

#define MFCALLSTACK
#define MFCALLSTACKc
#define MFCALLSTACKg
#define MFCALLSTACKs(s)
#define MFCALLSTACKcs(s)
#define MFCALLSTACKgs(s)

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
