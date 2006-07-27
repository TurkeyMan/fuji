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
#if defined(_WINDOWS) || defined(_MFXBOX) || (defined(_FUJI_UTIL) && !defined(_LINUX))
	#define MFDebug_Breakpoint() { __asm { int 3 }; }
#elif defined(_LINUX) || (defined(_FUJI_UTIL) && !defined(_WINDOWS))
	#define MFDebug_Breakpoint() { asm("int $3"); }
#elif defined(_PSP) || defined(_PS2)
	#define MFDebug_Breakpoint() { asm("break"); }
//#elif defined(_GC)
//	#include <debug.h>
//	#define MFDebug_Breakpoint() { _break(); }
#else
	#define MFDebug_Breakpoint()
#endif


/*** Useful debuging functions ***/

#if !defined(_RETAIL)

// external debug assert functions
void MFDebug_DebugAssert(const char *pReason, const char *pMessage, const char *pFile, int line);

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
#define MFDebug_AssertAlignment(pPointer, alignment) MFDebug_Assert(!((uint32)((void*)(pPointer))&((alignment)-1)), MFStr("Pointer not %d byte alligned: 0x%08X", (alignment), (void*)(x)))

/**
 * Logs a message to the debug output.
 * Writes a message to the debug output.
 * @param pMessage Message to be written to the debug output.
 * @return None.
 */
void MFDebug_Message(const char *pMessage);

/**
 * Notifies the user of a critical error.
 * Notifies the user of a critical error and logs it to the debugger.
 * @param pErrorMessage Error message.
 * @return None.
 */
void MFDebug_Error(const char *pErrorMessage);

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
void MFDebug_Warn(int level, const char *pWarningMessage);

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
 * - 4 - Very trivial message.
 */
void MFDebug_Log(int level, const char *pMessage);

/**
 * Sets the maximum warning level.
 * Sets the maximum warning level to be written to the debug output.
 * @param maxLevel Maximum warning level (0-4).
 * @return None.
 */
void MFDebug_SetMaximumWarningLevel(int maxLevel);

/**
 * Sets the maximum log level.
 * Sets the maximum log level to be written to the debug output.
 * @param maxLevel Maximum log level (0-4).
 * @return None.
 */
void MFDebug_SetMaximumLogLevel(int maxLevel);

#else

// debug functions define to nothing in retail builds.
#define MFDebug_Assert(condition, pMessage)
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
