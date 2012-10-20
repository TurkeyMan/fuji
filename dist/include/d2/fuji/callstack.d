module fuji.callstack;

/**
 * Log the callback to the console.
 * Logs the callback to the console.
 * @return None. 
 */
extern (C) void MFCallstack_Log();

/**
 * Get a string containing the current callstack.
 * Get2 a string containing the current callstack.
 * @return Pointer to a string containing the current callstack.
 * @remarks The string is allocated in the circular temporary string buffer, results should not be stored long term.
 */
extern (C) const(char*) MFCallstack_GetCallstackString();

