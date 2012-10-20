module fuji.commandline;

/**
 * Get the command line buffer.
 * Gets the command line buffer.
 * @return Returns a pointer to the internal command line buffer which can be used for custom parsing.
 */
extern (C) const(char*) MFCommandLine_GetCommandLineString();

/**
 * Get a bool argument from the command line.
 * Gets a bool argument from the command line.
 * @param pParameter Name of the parameter to fetch from the command line.
 * @param defaultValue Default value to use if parameter does not appear on the command line.
 * @return Returns the value of the parameter on the command line as a bool. If the parameter does not exist, \a defaultValue is returned.
 */
extern (C) bool MFCommandLine_GetBool(const(char*) pParameter, bool defaultValue = false);

/**
 * Get an int argument from the command line.
 * Gets a int argument from the command line.
 * @param pParameter Name of the parameter to fetch from the command line.
 * @param defaultValue Default value to use if parameter does not appear on the command line.
 * @return Returns the value of the parameter on the command line as an int. If the parameter does not exist, \a defaultValue is returned.
 */
extern (C) int MFCommandLine_GetInt(const(char*) pParameter, int defaultValue = 0);

/**
 * Get a float argument from the command line.
 * Gets a float argument from the command line.
 * @param pParameter Name of the parameter to fetch from the command line.
 * @param defaultValue Default value to use if parameter does not appear on the command line.
 * @return Returns the value of the parameter on the command line as a float. If the parameter does not exist, \a defaultValue is returned.
 */
extern (C) float MFCommandLine_GetFloat(const(char*) pParameter, float defaultValue = 0);

/**
 * Get a string argument from the command line.
 * Gets a string argument from the command line.
 * @param pParameter Name of the parameter to fetch from the command line.
 * @return Returns the value of the parameter on the command line as a string. If the parameter does not exist, NULL is returned.
 */
extern (C) const(char*) MFCommandLine_GetString(const(char*) pParameter);

