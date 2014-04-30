/**
 * @file MFDebug.h
 * @brief Mount Fuji Engine debugging tools.
 * This provides a set of debugging and profiling tools that can be used in games.
 * Typically these tools are wrapped in macros which define out to nothing in _RETAIL builds.
 * @author Manu Evans
 * @defgroup MFDebug Debugging Tools
 * @{
 */

#if !defined(_MFDEBUG_H)
#define _MFDEBUG_H

#if defined(_MSC_VER)
#pragma warning(disable:4127)
#endif

/**
 * @fn void MFDebug_Breakpoint()
 * Triggers a debugger breakpoint.
 * Triggers a debugger breakpoint.
 * @return None.
 */
#if defined(MF_NACL)
	#include <assert.h>
	#define MFDebug_Breakpoint()		assert(false)
#elif defined(MF_WEB)
	#include <assert.h>
	#define MFDebug_Breakpoint()		assert(false)
#elif defined(MF_ARCH_X86) || defined(MF_ARCH_X64)
	#if defined(MF_COMPILER_VISUALC)
		#define MFDebug_Breakpoint()	__debugbreak();
	#else
		#if defined(MF_ASM_INTEL)
			#define MFDebug_Breakpoint() { __asm { int 3 }; }
		#elif defined(MF_ASM_ATNT)
			#define MFDebug_Breakpoint() { asm("int $3"); }
		#endif
	#endif
#elif defined(MF_ARCH_PPC)
	#if defined(MF_ASM_INTEL)
		#define MFDebug_Breakpoint() { __asm { trap }; }
	#elif defined(MF_ASM_ATNT)
		#define MFDebug_Breakpoint() { asm("trap"); }
	#endif
#elif defined(MF_ARCH_MIPS)
	#if defined(MF_ASM_INTEL)
		#define MFDebug_Breakpoint() { __asm { break }; }
	#elif defined(MF_ASM_ATNT)
		#define MFDebug_Breakpoint() { asm("break"); }
	#endif
#elif defined(MF_ARCH_ARM)
	#if defined(MF_ASM_INTEL)
		#define MFDebug_Breakpoint() { __asm { bkpt }; }
	#elif defined(MF_ASM_ATNT)
		#define MFDebug_Breakpoint() { asm("bkpt"); }
	#endif
#else
	#define MFDebug_Breakpoint()
#endif

struct MFCodeLocation
{
	MFCodeLocation(const char *pComment = NULL, void *pUserData = NULL, const char *pFile = __FILE__, int line = __LINE__)
	: pFile(pFile)
	, line(line)
	, pComment(pComment)
	, pUserData(pUserData)
	{
	}

	const char *pFile;
	int line;
	const char *pComment;
	void *pUserData;
};

/*** Useful debuging functions ***/

#if !defined(_RETAIL)

// external debug assert functions
MF_API void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line);

/**
 * @fn void MFDebug_Assert(bool condition, const char *pMessage)
 * Asserts that a condition is true.
 * Asserts that a condition is true, if it fails, it throws an error and breaks the debugger.
 * @param condition Test condition.
 * @param pMessage Error message if condition fails.
 * @return None.
 */
#define MFDebug_Assert(condition, pMessage) { static bool ignore=false; if(!(condition) && !ignore) { MFDebug_DebugAssert(#condition, pMessage, __FILE__, __LINE__); MFDebug_Breakpoint(); ignore=true; } }

/**
 * @fn void MFDebug_AssertAlignment(void *pPointer, int alignment)
 * Asserts that a pointer is aligned to a specific number of bytes.
 * Asserts that a pointer is aligned to a specific number of bytes.
 * @param pPointer Pointer to check for alignment.
 * @param alignment Bytes to align pointer to.
 * @return None.
 */
#define MFDebug_AssertAlignment(pPointer, alignment) MFDebug_Assert(((size_t)(void*)(pPointer)&((alignment)-1)) == 0, MFStr("Pointer not %d byte alligned: 0x%p", (alignment), (void*)(x)))

/**
 * Logs a message to the debug output.
 * Writes a message to the debug output.
 * @param pMessage Message to be written to the debug output.
 * @return None.
 */
MF_API void MFDebug_Message(const char *pMessage);

/**
 * Notifies the user of a critical error.
 * Notifies the user of a critical error and logs it to the debugger.
 * @param pErrorMessage Error message.
 * @return None.
 */
MF_API void MFDebug_Error(const char *pErrorMessage);

/**
 * Notifies the user of a runtime warning.
 * Notifies the user of a runtime warning. The warning level can be controlled at runtime to restrict unwanted warnings.
 * @param level Warning level.
 * @param pWarningMessage Message to log to the debugger.
 * @return None.
 * @remarks The warning level output can be controlled at runtime.
 *
 * Valid Warning levels:
 * - 0 - Warning will be always be displayed. For critical warnings.
 * - 1 - Critical Warning. Application will probably not run correctly.
 * - 2 - Non-Critical Warning. Application will run, but may perform incorrectly.
 * - 3 - General Warning. For general information feedback.
 * - 4 - Low Warning. For small generally unimportant details.
 */
MF_API void MFDebug_Warn(int level, const char *pWarningMessage);

/**
 * Log a message to the debug output.
 * Logs a message to the debug output. The log level can be controlled at runtime to restrict unwanted log messages.
 * @param level Log level.
 * @param pMessage Message to log to the debugger.
 * @return None.
 * @remarks The log level output can be controlled at runtime.
 *
 * Valid Log levels:
 * - 0 - Messages will be always be displayed.
 * - 1 - Important message.
 * - 2 - Not so important message.
 * - 3 - Typically unwanted message.
 * - 4 - Very trivial and probably frequent spammy message.
 * - 5 - Output function calls to log program flow.
 */
MF_API void MFDebug_Log(int level, const char *pMessage);

/**
 * Sets the maximum warning level.
 * Sets the maximum warning level to be written to the debug output.
 * @param maxLevel Maximum warning level (0-4).
 * @return The previous warning level.
 */
MF_API int MFDebug_SetMaximumWarningLevel(int maxLevel);

/**
 * Sets the maximum log level.
 * Sets the maximum log level to be written to the debug output.
 * @param maxLevel Maximum log level (0-5).
 * @return The previous log level.
 */
MF_API int MFDebug_SetMaximumLogLevel(int maxLevel);

#else

// debug functions define to nothing in retail builds.
#define MFDebug_Assert(condition, pMessage) MFASSUME(condition)
#define MFDebug_AssertAlignment(pPointer, alignment)
#define MFDebug_Error(pErrorMessage)
#define MFDebug_Warn(level, pWarningMessage)
#define MFDebug_Log(level, pMessage)
#define MFDebug_Message(pMessage)
#define MFDebug_SetMaximumWarningLevel(maxLevel)
#define MFDebug_SetMaximumLogLevel(maxLevel)

#endif

#if 0
// this section is to make doxygen happy :)
void MFDebug_Breakpoint();
void MFDebug_Assert(bool condition, const char *pMessage);
void MFDebug_AssertAlignment(void *pPointer, int alignment);
#endif

#endif // _MFDEBUG_H

/** @} */
